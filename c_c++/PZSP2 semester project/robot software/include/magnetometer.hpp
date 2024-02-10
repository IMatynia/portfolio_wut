/* PZPS2 2023Z */
/* Mateusz Okulus */

#ifndef MAGNETOMETER_HPP
#define MAGNETOMETER_HPP

#define USE_CALIBRATION 1

#include <array>

#include "periph/i2c.h"
#include "errno.h"
#include "log.h"

namespace {
class i2c_guard {
public:
    i2c_guard(i2c_t device) : device(device) {
        i2c_acquire(device);
    }
    ~i2c_guard() {
        i2c_release(device);
    }
private:
    i2c_t device;
};
}

class Magnetometer {
public:
    Magnetometer() {
        i2c_init(DEVICE);
        i2c_guard g(DEVICE);
        writeReg(REG_IC0, 0);
        writeReg(REG_IC1, 0);
        writeReg(REG_IC2, 0);
    }

    uint8_t readProductID() {
        i2c_guard g(DEVICE);
        return readReg(REG_PRODUCT_ID);
    }

    void accurateMeasure() {
        i2c_guard g(DEVICE);
        doSet();
        rawMeasure();
        auto sx = x;
        auto sy = y;
        auto sz = z;
        doReset();
        rawMeasure();
        auto rx = x;
        auto ry = y;
        auto rz = z;
        x = (sx - rx) / 2;
        y = (sy - ry) / 2;
        z = (sz - rz) / 2;
    }

    double getX() {
        #if USE_CALIBRATION
        auto sx = 0.0625 * x - HARD_IRON[0];
        auto sy = 0.0625 * y - HARD_IRON[1];
        auto sz = 0.0625 * z - HARD_IRON[2];
        return SOFT_IRON[0][0] * sx + SOFT_IRON[0][1] * sy + SOFT_IRON[0][2] * sz;
        #else
        return 0.0625 * x;
        #endif
    }

    int32_t getRawX() {
        return x;
    }

    double getY() {
        #if USE_CALIBRATION
        auto sx = 0.0625 * x - HARD_IRON[0];
        auto sy = 0.0625 * y - HARD_IRON[1];
        auto sz = 0.0625 * z - HARD_IRON[2];
        return SOFT_IRON[1][0] * sx + SOFT_IRON[1][1] * sy + SOFT_IRON[1][2] * sz;
        #else
        return 0.0625 * y;
        #endif
    }

    int32_t getRawY() {
        return y;
    }

    double getZ() {
        #if USE_CALIBRATION
        auto sx = 0.0625 * x - HARD_IRON[0];
        auto sy = 0.0625 * y - HARD_IRON[1];
        auto sz = 0.0625 * z - HARD_IRON[2];
        return SOFT_IRON[2][0] * sx + SOFT_IRON[2][1] * sy + SOFT_IRON[2][2] * sz;
        #else
        return 0.0625 * z;
        #endif
    }

    int32_t getRawZ() {
        return z;
    }

    std::array<double, 3> getXYZ() {
        #if USE_CALIBRATION
        return getCalibrated(getRawXYZ());
        #else
        return {0.0625 * x, 0.0625 * y, 0.0625 * z};
        #endif
    }

    std::array<int32_t, 3> getRawXYZ() {
        return {x, y, z};
    }

    std::array<double, 3> getCalibrated(const std::array<int32_t, 3> &v) {
        std::array<double, 3> s;
        std::array<double, 3> r;

        s[0] = 0.0625 * v[0] - HARD_IRON[0];
        s[1] = 0.0625 * v[1] - HARD_IRON[1];
        s[2] = 0.0625 * v[2] - HARD_IRON[2];

        r[0] = SOFT_IRON[0][0] * s[0] + SOFT_IRON[0][1] * s[1] + SOFT_IRON[0][2] * s[2];
        r[1] = SOFT_IRON[1][0] * s[0] + SOFT_IRON[1][1] * s[1] + SOFT_IRON[1][2] * s[2];
        r[2] = SOFT_IRON[2][0] * s[0] + SOFT_IRON[2][1] * s[1] + SOFT_IRON[2][2] * s[2];

        return r;
    }

    void temperatureMeasure() {
        i2c_guard g(DEVICE);
        writeReg(REG_IC0, MASK_IC0_MEAS_TEMP);
        waitForMask(REG_STATUS1, MASK_STATUS_TEMP_DONE);
        temp = readReg(REG_TEMP_OUT);
    }

    double getTemperature() {
        double t = temp * 200 / 255;
        t -= 75;
        return t;
    }

    uint8_t getRawTemperature() {
        return temp;
    }

private:
    static inline void checkError(int error) {
        if (error == 0)
            return;
        switch (-error) {
        case EIO:
            LOG_ERROR("magnetometer: slave device doesn't ACK the byte\n");
            break;
        case ENXIO:
            LOG_ERROR("magnetometer: no devices respond on the address sent on the bus\n");
            break;
        case ETIMEDOUT:
            LOG_ERROR("magnetometer: timeout occurs before device's response\n");
            break;
        case EINVAL:
            LOG_ERROR("magnetometer: invalid argument\n");
            break;
        case EOPNOTSUPP:
            LOG_ERROR("magnetometer: MCU driver doesn't support the flag operation\n");
            break;
        case EAGAIN:
            LOG_ERROR("magnetometer: lost bus arbitration occured\n");
            break;
        }
        // crash when asserts enabled
        assert(error == 0);
    }

    static inline void doSet() {
        writeReg(REG_IC0, MASK_IC0_DO_SET);
        waitForMaskUnset(REG_IC0, MASK_IC0_DO_SET);
    }

    static inline void doReset() {
        writeReg(REG_IC0, MASK_IC0_DO_RESET);
        waitForMaskUnset(REG_IC0, MASK_IC0_DO_RESET);
    }

    void rawMeasure() {
        writeReg(REG_IC0, MASK_IC0_MEAS_MAG);
        waitForMask(REG_STATUS1, MASK_STATUS_MAG_DONE);

        uint8_t buf[9];
        i2c_write_byte(DEVICE, MAG_ADDR, 0, I2C_NOSTOP);
        i2c_read_bytes(DEVICE, MAG_ADDR, buf, sizeof(buf), 0);

        x = combineReadings(buf[0], buf[1], buf[6]);
        y = combineReadings(buf[2], buf[3], buf[7]);
        z = combineReadings(buf[4], buf[5], buf[8]);
    }

    static inline int32_t combineReadings(uint8_t out0, uint8_t out1, uint8_t out2) {
        int32_t v = (uint32_t)out0 << 12 | (uint32_t)out1 << 4 | (uint32_t)out2 >> 4;
        v -= (uint32_t)1 << 19;
        return v;
    }

    static inline void waitForMask(uint16_t reg, uint8_t mask) {
        uint8_t r = 0;
        do {
            r = readReg(reg);
        } while (0 == (r & mask));
    }

    static inline void waitForMaskUnset(uint16_t reg, uint8_t mask) {
        uint8_t r = 0;
        do {
            r = readReg(reg);
        } while (r & mask);
    }

    static inline uint8_t readReg(uint16_t reg) {
        uint8_t r = 0;
        auto err = i2c_read_reg(DEVICE, MAG_ADDR, reg, &r, 0);
        checkError(err);
        return r;
    }

    static inline void writeReg(uint16_t reg, uint8_t value) {
        auto err = i2c_write_reg(DEVICE, MAG_ADDR, reg, value, 0);
        checkError(err);
    }

    int32_t x = 0;
    int32_t y = 0;
    int32_t z = 0;

    uint8_t temp = 0;

    static const i2c_t DEVICE = I2C_DEV(0);
    static const uint16_t MAG_ADDR = 0x30;
    static const uint16_t REG_PRODUCT_ID = 0x39;
    static const uint16_t REG_IC0 = 0x1B;
    static const uint16_t REG_IC1 = 0x1C;
    static const uint16_t REG_IC2 = 0x1D;
    static const uint16_t REG_TEMP_OUT = 0x09;
    static const uint16_t REG_STATUS1 = 0x18;

    static const uint16_t REG_XOUT0 = 0x00;
    static const uint16_t REG_XOUT1 = 0x01;
    static const uint16_t REG_XOUT2 = 0x06;

    static const uint16_t REG_YOUT0 = 0x02;
    static const uint16_t REG_YOUT1 = 0x03;
    static const uint16_t REG_YOUT2 = 0x07;

    static const uint16_t REG_ZOUT0 = 0x04;
    static const uint16_t REG_ZOUT1 = 0x05;
    static const uint16_t REG_ZOUT2 = 0x08;

    static const uint8_t MASK_IC0_MEAS_MAG = 1 << 0;
    static const uint8_t MASK_IC0_MEAS_TEMP = 1 << 1;
    static const uint8_t MASK_IC0_DO_SET = 1 << 3;
    static const uint8_t MASK_IC0_DO_RESET = 1 << 4;
    static const uint8_t MASK_IC0_AUTO_SR = 1 << 5;

    static const uint8_t MASK_STATUS_MAG_DONE = 1 << 6;
    static const uint8_t MASK_STATUS_TEMP_DONE = 1 << 7;

    #if USE_CALIBRATION
    static constexpr double HARD_IRON[3] =
        {-934.46199691, -233.41477681, 709.08990503};
    static constexpr double SOFT_IRON[3][3] =
        {{  1.09325076, -0.00135596, 0.04528635 },
         { -0.00135596,  1.11884699, 0.0041804  },
         {  0.04528635,  0.00418040, 1.22673542 },};
    #endif
};

#endif /* MAGNETOMETER_HPP */
