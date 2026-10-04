# Semaphores in Zephyr RTOS

## What is a semaphore?

A semaphore is a counter that decides whether a thread is allowed to continue. Threads use it to wait for each other, so they can take turns instead of running all at once.

A semaphore supports two operations:

- Take (`k_sem_take`): if the count is above 0, the count goes down by 1 and the thread continues. If the count is 0, the thread goes to sleep and waits.
- Give (`k_sem_give`): the count goes up by 1, and a waiting thread wakes up.

Think of it as a key on a hook. Taking the semaphore is grabbing the key. If the hook is empty, you wait. Giving is hanging the key back so the next thread can take it.

## Creating a semaphore

```c
K_SEM_DEFINE(name, initial_count, limit);
```

- name: the name of the semaphore.
- initial_count: the count at boot. 1 means the thread may go immediately, 0 means it must wait.
- limit: the maximum the count can reach. It must be at least 1.

Example:

```c
K_SEM_DEFINE(a_sem, 1, 1);
K_SEM_DEFINE(b_sem, 0, 1);
```

## Waiting time

The second argument of `k_sem_take` is how long the thread is willing to wait:

- K_FOREVER: wait as long as it takes.
- K_NO_WAIT: do not wait. If the count is 0, return an error at once.
- K_MSEC(100): wait up to 100 milliseconds, then give up with an error.

## Taking turns with two threads

To make two threads alternate, use two semaphores. Each thread takes its own semaphore and gives the other thread's semaphore.

```c
K_SEM_DEFINE(a_sem, 1, 1);
K_SEM_DEFINE(b_sem, 0, 1);

void thread_a(void)
{
	while (1) {
		k_sem_take(&a_sem, K_FOREVER);
		printk("Thread A\n");
		k_sem_give(&b_sem);
	}
}

void thread_b(void)
{
	while (1) {
		k_sem_take(&b_sem, K_FOREVER);
		printk("Thread B\n");
		k_sem_give(&a_sem);
	}
}
```

How the counts change:

```
Start:          a_sem = 1, b_sem = 0
A takes a_sem   a_sem becomes 0, A runs
A gives b_sem   b_sem becomes 1, B wakes up
B takes b_sem   b_sem becomes 0, B runs
B gives a_sem   a_sem becomes 1, A can run again
```

Output:

```
Thread A
Thread B
Thread A
Thread B
...
```

## Without a semaphore

If the semaphore calls are removed, both threads run freely at the same time. Their output mixes together and there is no guaranteed order.

## Rules to remember

- A thread takes its own semaphore and gives the other thread's semaphore.
- Exactly one semaphore starts at 1. The others start at 0.
- Semaphores control the order. `k_msleep` only controls the speed.
- Every thread must give the next semaphore, or the cycle stops.
- If a thread waits on a semaphore that nobody gives, it sleeps forever.

## Where semaphores are used

- Making threads take turns, such as printing or reading sensors one after another.
- Waking a thread when an interrupt happens, such as a button press or received data.
- Telling one thread that data from another thread is ready.
- Limiting how many threads can use a resource at the same time.

## Author

Sanjay Senthil Kumar<br>
II Year, Electronics and Communication Engineering (ECE)<br>
Bannari Amman Institute of Technology
