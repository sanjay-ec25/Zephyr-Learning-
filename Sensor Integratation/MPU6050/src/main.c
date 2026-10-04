#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/sensor.h>

static const struct device *const mpu = DEVICE_DT_GET(DT_ALIAS(mpu));

static const enum sensor_channel channels[] = {
	SENSOR_CHAN_ACCEL_X,
	SENSOR_CHAN_ACCEL_Y,
	SENSOR_CHAN_ACCEL_Z,
};

static int print_accels(const struct device *dev)
{
	int ret;
	struct sensor_value accel[3];

	ret = sensor_sample_fetch(dev);
	if (ret < 0) {
		printk("%s: sensor_sample_fetch() failed: %d\n", dev->name, ret);
		return ret;
	}

	for (size_t i = 0; i < ARRAY_SIZE(channels); i++) {
		ret = sensor_channel_get(dev, channels[i], &accel[i]);
		if (ret < 0) {
			printk("%s: sensor_channel_get(%c) failed: %d\n",
			       dev->name, 'X' + (int)i, ret);
			return ret;
		}
	}

	printk("%16s [m/s^2]:    (%12.6f, %12.6f, %12.6f)\n", dev->name,
	       sensor_value_to_double(&accel[0]),
	       sensor_value_to_double(&accel[1]),
	       sensor_value_to_double(&accel[2]));

	return 0;
}

int main(void)
{
	if (!device_is_ready(adxl)) {
		printk("%s is not ready\n", adxl->name);
		return 0;
	}

	while (1) {
		print_accels(adxl);
		k_msleep(1000);
	}

	return 0;
}
