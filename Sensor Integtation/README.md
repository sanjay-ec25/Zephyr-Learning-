# Sensor Integration in Zephyr RTOS

This folder shows how to connect an I2C sensor to a Zephyr project and read its data. The sensors used are the ADXL345 and the MPU6050, tested on the STM32 Nucleo-F401RE board.

## What does integrating a sensor mean?

A sensor does not work just because it is wired up. Zephyr must be told three things:

1. Where the sensor is: which bus it is on and what its address is.
2. Which driver controls it.
3. How the program should find it and read it.

Each of these is handled by a different file, and all three are needed.

## Step 1: Tell Zephyr where the sensor is (devicetree)

Zephyr does not scan the hardware. You describe it in the devicetree. For an I2C sensor you describe:

- The bus it is connected to, such as I2C1 or I2C3, and the pins that bus uses.
- The sensor's I2C address.
- The compatible string, which names the driver that should control the sensor.

The sensor is added as a node under its bus. In this project the nodes are placed in `app.overlay`, so Zephyr's own board files are not changed.

Each node is given a label, such as `axl_node` or `mpu_node`, and an alias, such as `adxl` or `mpu`. The alias is the short name that the C code uses to find the sensor.

## Step 2: Turn on the driver (prj.conf)

Describing the sensor in the devicetree is not enough. The driver must also be built into the program, and `prj.conf` is where that is switched on.

- `CONFIG_I2C` enables the I2C bus.
- `CONFIG_SENSOR` enables Zephyr's sensor system.
- `CONFIG_ADXL345` or `CONFIG_MPU6050` enables the driver for that particular sensor.
- `CONFIG_CBPRINTF_FP_SUPPORT` lets `printk` print decimal values.

If the driver line is missing, the sensor exists in the devicetree but no driver controls it, and the build fails with an undefined reference to `__device_dts_ord_N`.

## Step 3: Read the sensor (main.c)

In the code, the sensor is found by its alias:

```c
static const struct device *const adxl = DEVICE_DT_GET(DT_ALIAS(adxl));
```

The program then does the following:

1. Checks that the device is ready with `device_is_ready()`. If the driver could not start the sensor, this fails.
2. Reads the sensor with `sensor_sample_fetch()`. This talks to the sensor over I2C and stores the latest measurement.
3. Gets the X, Y and Z values with `sensor_channel_get()`.
4. Prints them with `printk`.

These steps are placed in a function called `print_accels(dev)`. Its input `dev` is a pointer to the sensor, so the same function can read any sensor. Only the pointer passed in changes.

## How the three files connect

```
app.overlay  defines the sensor, its address and its alias
prj.conf     enables the driver that controls it
main.c       finds the sensor by alias and reads it
```

The alias is what links the devicetree to the code. The driver is what turns the devicetree node into a working device. If the alias name in `main.c` does not match the one in `app.overlay`, the build fails. If the driver is not enabled, the build fails or the sensor is not ready.

## Integrating a new sensor

To add any other I2C sensor, follow the same order:

1. Find the sensor's Zephyr driver and its compatible string.
2. Add a node for it under the correct I2C bus, with its address, and give it a label and an alias.
3. Enable the driver in `prj.conf`.
4. Get the device in `main.c` using the alias, then read it with the sensor API.

## Common problems

- Link error `undefined reference to __device_dts_ord_N`: the driver is not enabled, the alias is missing, or `compatible` is misspelled.
- `sensor_sample_fetch() failed: -5`: the I2C connection failed. Check the wiring and power.
- Sensor shows "not ready": the driver failed to start. Check the address and the driver line in `prj.conf`.
- No decimal values in the output: `CONFIG_CBPRINTF_FP_SUPPORT` is missing.
- Duplicate label error: the same node is defined in both the board file and the overlay. Keep it in only one place.

## Author

Sanjay Senthil Kumar<br>
II Year, Electronics and Communication Engineering (ECE)<br>
Bannari Amman Institute of Technology
