# 2023L PZSP2
# Mateusz Okulus

import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import serial

xs = []
ys = []
zs = []

matplotlib.interactive(True)

fig = plt.figure()
ax = fig.add_subplot(projection='3d')
ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_zlabel('Z')

print("CTRL-C to stop and append")

ser = serial.Serial("/dev/ttyACM0", baudrate=115200, timeout=None)
while True:
    try:
        while ser.in_waiting >= 100:
            line = ser.readline().decode()
            row = line.split(',')
            x = 0.0625 * float(row[0])
            y = 0.0625 * float(row[1])
            z = 0.0625 * float(row[2])
            if abs(x) > 10000 or abs(y) > 10000 or abs(z) > 10000:
                continue
            if np.allclose([x, y, z], np.zeros(3)):
                continue
            xs.append(x)
            ys.append(y)
            zs.append(z)
        ax.scatter(xs, ys, zs)
        plt.draw()
        plt.pause(0.01)
        ax.cla()
    except KeyboardInterrupt:
        break
    except:
        continue

print(f"N = {len(xs)}")

print("saving...")
with open('data.csv', 'a') as file:
    for x, y, z in zip(xs, ys, zs):
        file.write(f"{x:.6f},{y:.6f},{z:.6f}\n")
print("saved")
