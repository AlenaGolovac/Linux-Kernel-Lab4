

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/errno.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Student");
MODULE_DESCRIPTION("Linux kernel module - Lab 4");


#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/errno.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/ktime.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Student");
MODULE_DESCRIPTION("Linux kernel module - Lab 4");

static unsigned int count = 1;
module_param(count, uint, 0444);
MODULE_PARM_DESC(count, "Number of Hello world messages");

struct hello_event {
    struct list_head list;
    ktime_t time;
};

static LIST_HEAD(event_list);

static int __init hello_init(void)
{
    unsigned int i;
    struct hello_event *event;

    if (count > 10) {
        pr_err("Invalid count: %u\n", count);
        return -EINVAL;
    }

    if (count == 0 || count >= 5)
        pr_warn("Warning: count = %u\n", count);

    for (i = 0; i < count; i++) {
        event = kmalloc(sizeof(*event), GFP_KERNEL);
        if (!event) {
            struct hello_event *item, *tmp;

            list_for_each_entry_safe(item, tmp,
                                     &event_list, list) {
                list_del(&item->list);
                kfree(item);
            }
            return -ENOMEM;
        }

        event->time = ktime_get();
        list_add_tail(&event->list, &event_list);

        pr_info("Hello, world!\n");
    }

    return 0;
}

static void __exit hello_exit(void)
{
    struct hello_event *item, *tmp;

    list_for_each_entry_safe(item, tmp,
                             &event_list, list) {
        pr_info("Event time: %lld ns\n",
                (long long)ktime_to_ns(item->time));

        list_del(&item->list);
        kfree(item);
    }

    pr_info("Goodbye, world!\n");
}

module_init(hello_init);
module_exit(hello_exit);


