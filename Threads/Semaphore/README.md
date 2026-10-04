# Semaphores in Zephyr RTOS

This folder shows how semaphores make threads take turns. It has two projects: a basic example that only prints messages, and a real example that reads two sensors.

## What is a semaphore?

A semaphore is a counter that decides whether a thread is allowed to continue.

- k_sem_take: if the count is above 0, it subtracts 1 and the thread continues. If the count is 0, the thread sleeps until another thread gives.
- k_sem_give: adds 1 to the count and wakes a waiting thread.

A semaphore is created with:

```c
K_SEM_DEFINE(name, initial_count, limit);
```

- initial_count: the count at boot. 1 means the thread may go now, 0 means it must wait.
- limit: the maximum count. It must be at least 1.

K_FOREVER as the timeout means the thread waits with no time limit.

## Turn-taking pattern

Two threads and two semaphores. Each thread takes its own semaphore and gives the other thread's semaphore.

```c
K_SEM_DEFINE(a_sem, 1, 1);
K_SEM_DEFINE(b_sem, 0, 1);

void thread_a(void)
{
	while (1) {
		k_sem_take(&a_sem, K_FOREVER);
		/* work */
		k_sem_give(&b_sem);
	}
}

void thread_b(void)
{
	while (1) {
		k_sem_take(&b_sem, K_FOREVER);
		/* work */
		k_sem_give(&a_sem);
	}
}
```

How the counts change:

```
Start:          a_sem = 1, b_sem = 0
A takes a_sem   a_sem becomes 0, A runs
A gives b_sem   b_sem becomes 1, B wakes
B takes b_sem   b_sem becomes 0, B runs
B gives a_sem   a_sem becomes 1, A can run again
```

## Project 1: Semaphore_Basic

Two threads print three messages each.

Without a semaphore, both threads run at the same time, so their lines alternate one by one:

```
THREAD 1 IS RUNNING
THREAD 2 IS RUNNING
THREAD 1 IS RUNNING
THREAD 2 IS RUNNING
...
```

With a semaphore, thread 1 finishes its group of three before thread 2 starts:

```
THREAD 1 IS RUNNING
THREAD 1 IS RUNNING
THREAD 1 IS RUNNING
THREAD 2 IS RUNNING
THREAD 2 IS RUNNING
THREAD 2 IS RUNNING
...
```

Screenshots of both outputs:

![Without semaphore](Semaphore_Basic/without_semaphore.png)

![With semaphore](Semaphore_Basic/with_semaphore.png)

## Project 2: Semaphore_Sensor

Two accelerometers are read by two threads that take turns using semaphores.

Sensors used:

- ADXL345 on I2C3, address 0x53, devicetree alias adxl
- MPU6050 on I2C1, address 0x68, devicetree alias mpu

How it works:

1. The ADXL thread takes a_sem, prints ADXL STARTING, reads the ADXL345 five times, prints ADXL COMPLETED, then gives b_sem.
2. The MPU thread takes b_sem, prints MPU STARTING, reads the MPU6050 five times, prints MPU COMPLETED, then gives a_sem.
3. Both threads loop forever, so the cycle repeats.

The same print_accels function reads either sensor. It receives the device pointer as an argument, so the sensor name and values come from whichever device is passed.

Output with semaphore:

```
ADXL STARTING
   axl345@53 [m/s^2]:    (    9.500136,    -0.459684,     0.153228)
   ... (5 lines)
ADXL COMPLETED
MPU STARTING
  mpu6050@68 [m/s^2]:    (   -0.239421,    -0.201113,     9.564835)
  ... (5 lines)
MPU COMPLETED
ADXL STARTING
...
```

The two boards are mounted in different orientations, so gravity (about 9.8 m/s^2) appears on a different axis for each sensor.

Without semaphore, both threads sleep for the same time and print together, so the sensor lines alternate one by one instead of in groups of five.

![Sensor output with semaphore](Semaphore_Sensor/with_semaphore.png)

![Sensor output without semaphore](Semaphore_Sensor/without_semaphore.png)

### Configuration

prj.conf:

```
CONFIG_I2C=y
CONFIG_SENSOR=y
CONFIG_ADXL345=y
CONFIG_MPU6050=y
CONFIG_CBPRINTF_FP_SUPPORT=y
```

CONFIG_CBPRINTF_FP_SUPPORT is needed so printk can print floating point values.

### Build and run

```bash
west build -p always -b nucleo_f401re <path-to-Semaphore_Sensor>
west flash
```

Open the serial console at 115200 baud and press the reset button to see the output.

## Common problems

- undefined reference to __device_dts_ord_N: the devicetree alias is missing, compatible is misspelled, or the driver is not enabled in prj.conf.
- sensor_sample_fetch() failed: -5: the I2C transfer failed. Check wiring, pull-ups and power.
- No float values printed: add CONFIG_CBPRINTF_FP_SUPPORT=y.
- Output stops after one round: a thread is not giving the other thread's semaphore.
- A thread waits forever: the other thread returned early without giving its semaphore.

## Key points

- A thread takes its own semaphore and gives the other thread's semaphore.
- Exactly one semaphore starts at 1, the others start at 0.
- Semaphores control the order. k_msleep only controls the speed.
- Every thread must give the next semaphore, or the cycle stops.

## Author

Sanjay Senthil Kumar<br>
II Year, Electronics and Communication Engineering (ECE)<br>
Bannari Amman Institute of Technology
