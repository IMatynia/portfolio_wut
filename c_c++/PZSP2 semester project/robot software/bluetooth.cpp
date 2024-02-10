/*
 * Code developed during the PZSP2 course during semester 23L by
 * M. Dabrawski, M. Lojek, M. Kindeusz, R. Ishchuk
 * Development during 23Z semester by
 * I. Matynia
 */
#include "include/bluetooth.hpp"

#define CONNECTION_ESTABLISHED_SIGNITURE "BTSTATE:4\r"
#define CES_LENGTH sizeof CONNECTION_ESTABLISHED_SIGNITURE

BluetoothIO::~BluetoothIO(){
}

void BluetoothIO::open() {
    int res = this->initUartConnection();
    if(res!=0)
    {
        printf("Failed to initialize UART");
        return;
    }
    this->startCallbackNotifierThread();
    this->setUpBluetooth();
};

void BluetoothIO::close() {
    this->stopCallbackNotifierThread();
    this->currentStatus = BT_DISCONNECTED;
}

void BluetoothIO::send(const std::string& buffer) {
    riot::lock_guard<riot::mutex> guard(this->send_mtx);
    if (this->currentStatus == BT_CONNECTED) {
        forceSend(buffer);
    } else {
        printf("You cant send data before connecting!\n");
    }
}


bool compare_string_section(std::string::iterator str_a, const char* str_b, size_t length)
{
    size_t position=0;
    while (*(str_b + position) != 0 && *(str_a + position) == *(str_b + position) && position < length)
        position++;

    return (position == length-1);
}

void BluetoothIO::handleRawData(char byte) {
    // packet splitting
    if(byte == this->packetSeparator)
    {
        this->passBufferToCallbacks(this->buffer);
        this->buffer.clear();
    }
    else
    {
        this->buffer += byte;
    }

    // check for connection
    if(this->currentStatus == BT_WAITING_FOR_CONNECTION && this->buffer.size() >= CES_LENGTH)
    {
        bool match = compare_string_section(this->buffer.end()-CES_LENGTH, CONNECTION_ESTABLISHED_SIGNITURE, CES_LENGTH);
        if(match)
        {
            printf("Bluetooth connection established!\n");
            this->currentStatus = BT_CONNECTED;
            this->buffer.clear();
        }
    }
}

void BluetoothIO::forceSend(std::string buffer) {
    uart_write(this->uart_device, (const uint8_t*)buffer.c_str(), buffer.size());
}

int BluetoothIO::initUartConnection() {
    printf("Initializing serial connection to bluetooth module\n");
    int res = uart_init(UART_DEV(this->uart_device), this->uart_baud, uart_rx_callback, this);
    return res;
}

void BluetoothIO::setUpBluetooth() {
    printf("Initializing bluetooth parameters\n");
    this->forceSend("");
    this->forceSend("\r\n+STWMOD=0\r\n");
    this->forceSend("\r\n+STNA=Robot2.0\r\n");
    this->forceSend("\r\n+STOAUT=1\r\n");
    this->forceSend("\r\n+STAUTO=0\r\n");
    xtimer_msleep(2000);
    this->forceSend("\r\n+INQ=1\r\n");
    xtimer_msleep(2000);
    printf("The robot should now be visible\n");
    this->currentStatus = BT_WAITING_FOR_CONNECTION;
}

void uart_rx_callback(void *bluetoothIO_ptr, uint8_t byte)
{
    BluetoothIO* bluetoothIO = (BluetoothIO*)bluetoothIO_ptr;
    msg_t msg;
    msg.content.value = byte;
    msg_send(&msg, bluetoothIO->callbackNotifierThread->native_handle());
}
