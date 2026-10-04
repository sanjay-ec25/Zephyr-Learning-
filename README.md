# Zephyr Learning

A repository of practice projects and notes from learning Zephyr RTOS. Everything here is built and tested on the STM32 Nucleo-F401RE board.

## About

I am learning Zephyr step by step, starting with the basics of multi-threading and moving on to working with real sensors. Each topic has its own folder with working code, a short explanation, and the output I got on the board.

## What is Zephyr?

Zephyr is a small open-source real-time operating system (RTOS) for microcontrollers. It lets a program run several tasks at the same time using threads, and it provides ready-made drivers for hardware such as I2C, SPI, GPIO and sensors. Hardware is described in a devicetree file, so the same code can run on different boards.

## Topics

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

Each project folder has its own `CMakeLists.txt`, `prj.conf` and `src/main.c`. Build and flash it from the Zephyr workspace with:

```bash
west build -p always -b nucleo_f401re <path-to-project-folder>
west flash
```

Then open the serial console at 115200 baud and press the reset button on the board to see the output.

## Author

- Sanjay Senthil Kumar
- II Year, Electronics and Communication Engineering (ECE)
- Bannari Amman Institute of Technology




