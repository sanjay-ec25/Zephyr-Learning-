#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/sensor.h>

#define STACKSIZE 2048
#define PRIORITY 7

K_SEM_DEFINE(a_sem, 1, 1);
K_SEM_DEFINE(b_sem, 0, 1);

static const struct device *const adxl = DEVICE_DT_GET(DT_ALIAS(adxl));
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
			       dev->name, 'X' + i, ret);
			return ret;
		}
	}

	printk("%16s [m/s^2]:    (%12.6f, %12.6f, %12.6f)\n", dev->name,
	       sensor_value_to_double(&accel[0]),
	       sensor_value_to_double(&accel[1]),
	       sensor_value_to_double(&accel[2]));

	return 0;
}

void adxl_sensor(void)
{
	while (1) {
		k_sem_take(&a_sem, K_FOREVER);
		printk("ADXL STARTING\n");

		for (int i = 1; i <= 5; i++) {
			print_accels(adxl);
			k_msleep(1000);
		}

		printk("ADXL COMPLETED\n");
		k_sem_give(&b_sem);
	}
}

void mpu_sensor(void)
{
	while (1) {
		k_sem_take(&b_sem, K_FOREVER);
		printk("MPU STARTING\n");

		for (int a = 1; a <= 5; a++) {
			print_accels(mpu);
			k_msleep(1000);
		}

		printk("MPU COMPLETED\n");
		k_sem_give(&a_sem);
	}
}

K_THREAD_DEFINE(adxl_tid, STACKSIZE, adxl_sensor, NULL, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(mpu_tid, STACKSIZE, mpu_sensor, NULL, NULL, NULL, PRIORITY, 0, 0);
