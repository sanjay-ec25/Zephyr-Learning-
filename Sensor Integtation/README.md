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

## Devicetree terms explained

**Node**

A node is one block in the devicetree that describes one piece of hardware. The I2C bus is a node, and the sensor is a node inside it. A node looks like this:

```dts
mpu6050@68 {
	compatible = "invensense,mpu6050";
	reg = <0x68>;
	status = "okay";
};
```

The name before the `@` is a readable name for the device (`mpu6050`). The number after the `@` is the I2C address in hexadecimal (`68`). Nodes can sit inside other nodes, which is how the sensor sits inside its bus.

**Property**

A property is one line inside a node that gives a detail about the hardware. In the node above, `compatible`, `reg` and `status` are properties.

- `compatible`: the name of the driver that should control this device. It must be spelled exactly right, or no driver is attached.
- `reg`: the address of the device on its bus. For I2C this is the I2C address.
- `status`: `"okay"` turns the node on. `"disabled"` turns it off.

**Label**

A label is a name written before the node, followed by a colon:

```dts
mpu_node: mpu6050@68 { ... };
```

Here `mpu_node` is the label. It is a handle for that node inside the devicetree files, so other parts of the devicetree can point to it with `&mpu_node`. Labels are used by Zephyr's devicetree system and by the alias line. The `&` means "refer to the node with this label".

**Alias**

An alias is a short, fixed name for a node, written in the `aliases` block:

```dts
/ {
	aliases {
		mpu = &mpu_node;
	};
};
```

This says: the name `mpu` refers to the node labelled `mpu_node`. The C code then finds the sensor by the alias:

```c
DEVICE_DT_GET(DT_ALIAS(mpu))
```

The advantage of an alias is that the code never needs to know the real node name or address. If the sensor moves to another bus, you change the devicetree and the alias still points to it, so `main.c` stays the same. Alias names use only lowercase letters, digits and dashes.

**Compatible string**

The compatible string connects a node to a driver. Each driver in Zephyr lists the compatible strings it handles. When the build finds a node whose compatible matches a driver, and that driver is enabled in `prj.conf`, Zephyr creates a device for it. Examples: `"adi,adxl345"` for the ADXL345 and `"invensense,mpu6050"` for the MPU6050.

**Overlay**

An overlay is an extra devicetree file that adds to or changes the board's devicetree, without editing the board file itself. `app.overlay` is picked up automatically when it is in the project folder.

**Summary of the terms**

| Term | What it is | Example |
|---|---|---|
| Node | A block describing one piece of hardware | `mpu6050@68 { ... };` |
| Property | One detail inside a node | `reg = <0x68>;` |
| Label | A name used to refer to a node in the devicetree | `mpu_node:` |
| Alias | A short name the C code uses to find a node | `mpu = &mpu_node;` |
| Compatible | The string that selects the driver | `"invensense,mpu6050"` |
| Overlay | An extra file that adds nodes without editing the board file | `app.overlay` |

**How they fit together**

```
alias  mpu  -->  label  mpu_node  -->  node  mpu6050@68  -->  driver (from compatible)
```

The C code asks for the alias. The alias points to the label. The label identifies the node. The compatible string in the node selects the driver, and the driver makes the working device.

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
