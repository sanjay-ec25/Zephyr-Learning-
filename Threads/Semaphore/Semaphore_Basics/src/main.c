#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define PRIORITY   7
#define STACK_SIZE 1024

K_SEM_DEFINE(a_sem, 1, 1);
K_SEM_DEFINE(b_sem, 0, 1);

void thread_1(void)
{
	k_sem_take(&a_sem, K_FOREVER);

	for (int a = 1; a <= 3; a++) {
		printk("THREAD 1 IS RUNNING\n");
		k_msleep(500);
	}

	k_sem_give(&b_sem);
}

void thread_2(void)
{
	k_sem_take(&b_sem, K_FOREVER);

	for (int b = 1; b <= 3; b++) {
		printk("THREAD 2 IS RUNNING\n");
		k_msleep(500);
	}
}

K_THREAD_DEFINE(t1_id, STACK_SIZE, thread_1, NULL, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(t2_id, STACK_SIZE, thread_2, NULL, NULL, NULL, PRIORITY, 0, 0);
