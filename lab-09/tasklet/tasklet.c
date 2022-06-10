#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>

#define IRQ_NUM 1

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tatiana K.");

static int my_dev_id;
const unsigned long my_tasklet_data = 123456;

struct tasklet_struct my_tasklet;

static int __init my_tasklet_init(void);
static void __exit my_tasklet_exit(void); 

#ifndef DECLARE_TASKLET_OLD
#define DECLARE_TASKLET_OLD(arg1, arg2) DECLARE_TASKLET(arg1, arg2, 0L)
#endif

void my_tasklet_handler(unsigned long data);
static irqreturn_t my_interrupt_handler(int irq, void *dev_id);

DECLARE_TASKLET_OLD(my_tasklet, my_tasklet_handler);

static inline void printk_tasklet_info(const char *prefix)
{
    printk(KERN_INFO "== %s -state: %ld, count: %d, data: %ld\n", prefix, my_tasklet.state, my_tasklet.count, my_tasklet.data);
}

void my_tasklet_handler(unsigned long data)
{
    printk(KERN_INFO "== Called my_tasklet_handler\n");
    printk_tasklet_info("");
}

static irqreturn_t my_interrupt_handler(int irq, void *dev_id)
{
    printk(KERN_INFO "== Called my_interrupt_handler\n");
    if (irq == IRQ_NUM)
    {
        tasklet_schedule(&my_tasklet);
        printk_tasklet_info("In interrupt handler");
        printk(KERN_INFO "== Tasklet scheduled\n");
        return IRQ_HANDLED;
    }
    else
        return IRQ_NONE;
}

static int __init my_tasklet_init(void)
{
    if (request_irq(IRQ_NUM, my_interrupt_handler, IRQF_SHARED, "my_interrupt_tasklet", &my_dev_id))
    {
        printk(KERN_ERR "== Error: can't register irq handler\n");
        return -1;
    }
    printk(KERN_INFO "== Module loaded\n");
    return 0;
}

static void __exit my_tasklet_exit(void)
{
    tasklet_kill(&my_tasklet);
    free_irq(IRQ_NUM, &my_dev_id);

    printk(KERN_INFO "== Module unloaded\n");
}

module_init(my_tasklet_init);
module_exit(my_tasklet_exit);