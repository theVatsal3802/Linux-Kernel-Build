#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/stat.h>
#include <linux/printk.h>
#include <linux/version.h>
#include <linux/seq_file.h>
#include <linux/slab.h> // for kmalloc and kfree
#include <linux/sched.h> // for current macro

MODULE_LICENSE("GPL");

#define MAX_SET_SIZE 100
#define BUFFER_LEN 256

static int set_size = 0;
static int set_curr_size = 0;
static int *set_container = NULL;
static int set_elem_found = 0;
static pid_t last_pid = 0;

static struct proc_dir_entry *set_file;

static void reset_set(void) {
    set_size = 0;
    set_curr_size = 0;
    if (set_container) {
        kfree(set_container);
        set_container = NULL;
    }
    printk(KERN_ALERT "Set has been reset\n");
}

static ssize_t set_file_read(struct file *file,
                             char __user *user_buff,
                             size_t count,
                             loff_t *pos) {
    char buf[BUFFER_LEN];
    int len = 0, i;

    // Return 0 (EOF) if the position is greater than 0
    if (*pos > 0)
        return 0;

    // Add all elements from the set to the buffer, separated by spaces
    for (i = 0; i < set_curr_size; i++) {
        // set_size * 4 = total size of set elements
        // one byte of space after each number
        len += snprintf(buf + len, (set_size-i)*5, "%d ", set_container[i]);
    }

    len += snprintf(buf + len, 2, "\n");

    // Ensure that len is within count limits
    if (len > count)
        len = count;

    // Copy data to user space
    if (copy_to_user(user_buff, buf, len))
        return -EFAULT;

    // Update the position
    *pos = len;

    return len;
}

static ssize_t set_file_write(struct file *file, const char __user *user_buff, size_t count, loff_t *pos) {
    char buf[BUFFER_LEN];
    int num_chars = -1, i, idx;
    
    // Reset the set if accessed by a different process
    if (set_size != 0 && last_pid != current->pid) {
        reset_set();
        last_pid = current->pid;
    }
    
    // Ensure the input size is within buffer limits
    if (count >= BUFFER_LEN)
        return -EINVAL;

    if (copy_from_user(buf, user_buff, count))
        return -EFAULT;

    buf[count] = '\0'; // Null-terminate the string
    printk(KERN_ALERT "Got this from user: %s", buf);
    num_chars = sscanf(buf, "%d", &i);
    // sscanf fails if buff is not a valid number
    if (num_chars != 1)
        return -EINVAL;

    // Ensuring set size is between 1 and MAX_SET_SIZE (both inclusive)
    if (set_size == 0 && (i < 1 || i > MAX_SET_SIZE)) {
        printk(KERN_ALERT "The set size passed (%d) is out of range\n", i);
        return -EINVAL;
    }

    if(set_size == 0){
        // Allocate memory for the set based on the size
        set_container = kmalloc(i * sizeof(int), GFP_KERNEL);
        if (!set_container)
            return -ENOMEM;
        // Initialize the set
        last_pid = current->pid;
        set_size = i;
        set_curr_size = 0;
        printk(KERN_ALERT "Set size set: %d\n", set_size);
        return count; // return now. No need for remaining steps
    }

    // Check if set is full
    if (set_curr_size >= set_size) {
        printk(KERN_ALERT "Set is full. Cannot add more elements.\n");
        return -EACCES;
    }
    // inefficient implementation but we need the set to work first
    set_elem_found = 0;
    for(idx=0;idx<set_curr_size;idx++){
        if(set_container[idx] == i){
            set_elem_found = 1;
            break;
        }
    }
    // Add the element to the set
    if(set_elem_found == 0){
        set_container[set_curr_size++] = i;
    } else {
        return 0;
    }
    printk(KERN_ALERT "This is the value of count we are returning: %ld", count);
    return count;
}

static const struct proc_ops myops = {
    .proc_read = set_file_read,
    .proc_write = set_file_write
};

static int __init set_init(void) {
    set_file = proc_create("partb_24CS60R43_24CS60R26", 0666, NULL, &myops);
    if (!set_file) {
        printk(KERN_ALERT "File create unsuccessful\n");
        return -ENOMEM;
    }
    printk(KERN_ALERT "set module loaded\n");
    return 0;
}

static void __exit set_exit(void) {
    if (set_container)
        kfree(set_container); // Free the dynamically allocated memory for the set
    if (set_file)
        proc_remove(set_file);
    printk(KERN_WARNING "Removing set module. Bye...\n");
}

module_init(set_init);
module_exit(set_exit);
