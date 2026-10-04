# Zephyr Learning

A repository of practice projects and notes from learning Zephyr RTOS. Everything here is built and tested on the STM32 Nucleo-F401RE board.

## About

I am learning Zephyr step by step. I started with connecting sensors to the board and reading their data, then moved on to multi-threading and synchronization. Each topic has its own folder with working code and a short explanation.

## What is Zephyr?

Zephyr is a small open-source real-time operating system (RTOS) for microcontrollers. It lets a program run several tasks at the same time using threads, and it provides ready-made drivers for hardware such as I2C, SPI, GPIO and sensors. Hardware is described in a devicetree file, so the same code can run on different boards.

## Topics

Sensor_Integration: how to connect an I2C sensor to a Zephyr project and read its data. It covers the devicetree (nodes, labels, aliases, compatible strings), the overlay file, the driver settings in prj.conf, and reading the sensor in main.c. Projects: ADXL345, MPU6050 and both sensors together.

Threads: how to create and run multiple threads in Zephyr.

- Semaphore: how to make threads take turns. It includes a basic example with two threads and a project that reads two accelerometers (ADXL345 and MPU6050) from two threads.

More topics will be added as I learn them.

## Hardware

- Board: STM32 Nucleo-F401RE
- Sensors: ADXL345 accelerometer (I2C3) and MPU6050 accelerometer and gyroscope (I2C1)

## Software

- RTOS: Zephyr v4.4.2
- Toolchain: Zephyr SDK 1.0.1
- Build tool: west
- Serial console: 115200 baud

## How to build a project

Each project folder has its own `CMakeLists.txt`, `prj.conf`, `app.overlay` and `src/main.c`. Build and flash it from inside the Zephyr workspace with:

```bash
cd ~/zephyr-work/zephyrproject
west build -p always -b nucleo_f401re <path-to-project-folder>
west flash
```

Then open the serial console at 115200 baud and press the reset button on the board to see the output.

## Author

Sanjay Senthil Kumar<br>
II Year, Electronics and Communication Engineering (ECE)<br>
Bannari Amman Institute of Technology
