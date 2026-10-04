# Threads in Zephyr RTOS

Practice notes on multi-threading with Zephyr RTOS, tested on the STM32 Nucleo-F401RE board.

## What is a thread?

A thread is a function that runs on its own, at the same time as other functions. Zephyr switches between threads very quickly, so one program can do several jobs together, for example reading two sensors while also handling a button press.

Every thread has three things:

- An entry function: the code the thread runs.
- A stack: private memory the thread uses for its variables and function calls.
- A priority: decides which thread runs first when several are ready. A lower number means a higher priority.

## Creating a thread

The easiest way is K_THREAD_DEFINE. It creates the stack and the thread, and starts the thread when the board boots.

```c
K_THREAD_DEFINE(my_tid, STACKSIZE, my_thread, NULL, NULL, NULL, PRIORITY, 0, 0);
```

What each part means:

- my_tid: the name of the thread.
- STACKSIZE: how much memory the thread gets, in bytes.
- my_thread: the function the thread runs.
- NULL, NULL, NULL: optional values passed to the function. We pass nothing.
- PRIORITY: the thread's priority.
- 0, 0: normal options, start immediately.

## Example

```c
void my_thread(void)
{
	while (1) {
		printk("Hello from my thread\n");
		k_msleep(1000);
	}
}

K_THREAD_DEFINE(my_tid, 1024, my_thread, NULL, NULL, NULL, 7, 0, 0);
```

This thread prints a message once every second, forever.

## Hardware and software

- Board: STM32 Nucleo-F401RE
- RTOS: Zephyr v4.4.2
- Serial console speed: 115200 baud