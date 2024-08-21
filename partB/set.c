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
#define BUFFER_LEN 1024

struct set_node{
    int val;
    struct set_node *left;
    struct set_node *right;
};

struct set_pid_map{
    int set_size;
    int set_curr_size;
    struct set_node *set_container;
    pid_t set_curr_pid;
};

static DEFINE_MUTEX(set_mutex);
static struct set_pid_map **set_proc_map = NULL;


static struct proc_dir_entry *set_file;

// Core Set functions start
static int set_val_max(int a, int b){
    if(a > b)
        return a;
    return b;
}

/*
    returns a new set_node
*/
static struct set_node *set_new_node(int val){
    struct set_node *temp = (struct set_node*)kmalloc(sizeof(struct set_node), GFP_KERNEL);
    temp->val = val;
    temp->left = NULL;
    temp->right = NULL;
    return temp;
}

/*
    search if the element exists in the set
*/
static int set_search_node(struct set_node *root , int val){
    if(!root)
        return 0;
    if(root->val == val)
        return 1;
    if(root->val > val)
        return set_search_node(root->left, val);
    return set_search_node(root->right, val);
}

/*
    returns the height of root
*/
static int set_node_height(struct set_node *root){
    if(!root)
        return 0;
    return set_val_max(1+set_node_height(root->left), 1+set_node_height(root->right));
}

/*
    right rotate functionality of AVL tree
*/
static struct set_node *set_rotate_right(struct set_node *root){
    struct set_node *T1, *T2;
    if(!root)
        return NULL;
    T1 = root->left;
    T2 = T1->right;

    T1->right = root;
    root->left = T2;

    return T1;
}

/*
    left rotate functionality of AVL tree
*/
static struct set_node *set_rotate_left(struct set_node *root){
    struct set_node *T1, *T2;
    if(!root)
        return NULL;
    T1 = root->right;
    T2 = T1->left;

    T1->left = root;
    root->right = T2;

    return T1;
}

/*
    inserts a new val in the set
*/
static struct set_node *set_insert_node(struct set_node *root, int val){
    int bal;
    if(!root)
        return set_new_node(val);
    if(root->val < val)
        root->right = set_insert_node(root->right, val);
    else if(root->val > val)
        root->left = set_insert_node(root->left, val);
    else
        return root;
    bal = set_node_height(root->left) - set_node_height(root->right);
    if(bal > 1){
        if(val < root->left->val)
            return set_rotate_right(root);
        else if(val > root->left->val){
            root->left = set_rotate_left(root->left);
            return set_rotate_right(root);
        }
    }
    else if(bal < -1){
        if(val > root->right->val)
            return set_rotate_left(root);
        else if(val < root->right->val) {
            root->right = set_rotate_right(root->right);
            return set_rotate_left(root);
        }
    }
    return root;
}

/*
    get all data from the set
*/
static void set_get_elem(struct set_node *root, char *buf, int *len){
    if(root) {
        int temp_len;
        set_get_elem(root->left, buf, len);
        temp_len = *len;
        *len = temp_len + snprintf(buf + temp_len, 12, "%d ", root->val);
        printk(KERN_ALERT "buf: %s\n", buf);
        set_get_elem(root->right, buf, len);
    }
}
// Core Set functions end 

// Core LKM Functions start

/* 
    Cleanup for a particular PID.
*/
static void set_reset_pid(struct set_pid_map *temp) {
    printk(KERN_ALERT "PID: %d is releasing the file. Set has been reset\n", temp->set_curr_pid);
    temp->set_size = 0;
    temp->set_curr_size = 0;
    temp->set_container = NULL;
    temp->set_curr_pid = -1;
}

/* 
    Tells if the array of sets is full
    Return value: struct set_pid_map* whose pid = current->pid
                  NULL = current->pid does not exist
*/
static struct set_pid_map* set_get_pid_exists(void){
    int i;
    for(i=0;i<MAX_SET_SIZE;i++){
        if(set_proc_map[i] && set_proc_map[i]->set_curr_pid == current->pid){
            return set_proc_map[i];
        }
    }
    return NULL;
}

/* 
    Tells if the array of sets is full
    Return value: pointer to a newly allocated struct set_pid_map
                  NULL = Empty slot does not exist
*/
static struct set_pid_map* set_alloc_proc_map_entry(void){
    int i;
    for(i=0;i<MAX_SET_SIZE;i++){
        if(set_proc_map[i] == NULL || set_proc_map[i]->set_curr_pid == -1){
            set_proc_map[i] = (struct set_pid_map *)kmalloc(sizeof(struct set_pid_map), GFP_KERNEL);
            set_proc_map[i]->set_size = 0;
            set_proc_map[i]->set_curr_size = 0;
            set_proc_map[i]->set_container = NULL;
            set_proc_map[i]->set_curr_pid = current->pid;
            return set_proc_map[i];
        }
    }
    return NULL;
}

/* 
    Tells if set_elem exists in set represented by temp->set_container
    Return value: 1 = set_elem exists
                  0 = does not exist
*/
static int set_elem_exists(struct set_node *root, int set_elem){
    return set_search_node(root, set_elem);
}

/* 
    Insert set_elem in the set
*/
static void set_insert_elem(struct set_pid_map *temp, int set_elem){
    temp->set_container = set_insert_node(temp->set_container, set_elem);
}

// Core LKM Functions end

// Function to log when a file is opened
static int set_file_open(struct inode *inode, struct file *file) {
    struct set_pid_map *temp;
    // Acquiring the lock
    while(!mutex_trylock(&set_mutex));
    // cannot open the file more than once
    temp = set_get_pid_exists();
    if(temp){
        printk(KERN_ALERT "Cannot open a file more than once. PID %d alread exists\n", current->pid);
        mutex_unlock(&set_mutex);
        return -EACCES;
    }
    temp = set_alloc_proc_map_entry();
    // now check if the array is full

    mutex_unlock(&set_mutex);
    if(temp == NULL){
        printk(KERN_ALERT "MAX Limit reached\n");
        return -ENOMEM;
    }
    printk(KERN_INFO "PID %d opened the file\n", current->pid);
    return 0;
}

// Function to log when a file is released (closed)
static int set_file_release(struct inode *inode, struct file *file) {
    struct set_pid_map *temp;
    while(!mutex_trylock(&set_mutex));
    
    temp = set_get_pid_exists();
    set_reset_pid(temp);
    
    mutex_unlock(&set_mutex);
    printk(KERN_INFO "PID %d closed the file\n", current->pid);
    return 0;
}

static ssize_t set_file_read(struct file *file,
                             char __user *user_buff,
                             size_t count,
                             loff_t *pos) {
    char *buf = (char *)kmalloc(sizeof(char) * BUFFER_LEN, GFP_KERNEL);
    int len = 0;
    
    struct set_pid_map *temp;
    // check if the current pid exists
    while(!mutex_trylock(&set_mutex));
    printk(KERN_ALERT "Reading!\n");
    temp = set_get_pid_exists();
    if(!temp){
            mutex_unlock(&set_mutex);
            printk(KERN_ALERT "Something is wrong. PID %d not found\n", current->pid);
            return -EACCES;
    }
    // Return 0 (EOF) if the position is greater than 0
    if (*pos > 0)
        return 0;
    // Acquiring the lock
    
    // Add all elements from the set to the buffer, separated by spaces
    set_get_elem(temp->set_container, buf, &len);
    // we are using puts in userspace program hence we do not need this
    // len += snprintf(buf + len, 2, "\n");

    // Ensure that len is within count limits
    if (len > count)
        len = count;

    // Copy data to user space
    if (copy_to_user(user_buff, buf, len))
        return -EFAULT;
        
    mutex_unlock(&set_mutex);
    // Update the position

    return len;
}

static ssize_t set_file_write(struct file *file, const char __user *user_buff, size_t count, loff_t *pos) {
    char *buf = kmalloc(sizeof(char) * BUFFER_LEN, GFP_KERNEL);
    int num_chars = -1, i,j;
    struct set_pid_map *temp;
    
    // Get the set for current PID 
    temp = set_get_pid_exists();
    if(temp == NULL){
        printk(KERN_ALERT "Something is wrong. Set for PID %d not found\n", current->pid);
        return -EACCES;
    }
    printk(KERN_ALERT "Set size is: %d and pid is: %d\n", temp->set_curr_size, temp->set_curr_pid);

    // Ensure the input size is within buffer limits
    if (count >= BUFFER_LEN)
        return -EINVAL;

    if (copy_from_user(buf, user_buff, count))
        return -EFAULT;
    
    buf[count] = '\0'; // Null-terminate the string
    printk(KERN_ALERT "Got this from pid %d, set-pid: %d: %s",current->pid, temp->set_curr_pid, buf);
    for(j=0;j<count;j++){
        if((buf[j]>='0' && buf[j]<='9') || buf[j] == '\0')
            continue;
        else{
            printk(KERN_ALERT "Coming here for %c\n", buf[j]);
            return -EINVAL;
        }
    }
    num_chars = sscanf(buf, "%d", &i);
    // sscanf fails if buff is not a valid number
    if (num_chars != 1)
        return -EINVAL;
    // Ensuring set size is between 1 and MAX_SET_SIZE (both inclusive)
    if (temp->set_size == 0 && (i < 1 || i > MAX_SET_SIZE)) {
        printk(KERN_ALERT "The set size passed (%d) is out of range\n", i);
        return -EINVAL;
    }

    if(temp->set_size == 0){
        // Initialize set size and current size. Root should still be NULL hence not
        // initializing temp->set_container
        temp->set_size = i;
        temp->set_curr_size = 0;
        printk(KERN_ALERT "Set size set: %d", temp->set_size);
        return count; // return now. No need for remaining steps
    }
    // Check if set is full
    if (temp->set_curr_size >= temp->set_size) {
        printk(KERN_ALERT "Set is full. Cannot add more elements.\n");
        return -EACCES;
    }
    // check if element is in set
    // if yes, return 0, ie, we are not inserting it.
    // else insert the element in the set
    if(set_elem_exists(temp->set_container, i)){
        return 0;
    } else {
        if(temp->set_container)
            printk(KERN_ALERT "Root is: %d for pid: %d\n", temp->set_container->val, temp->set_curr_pid);
        // inc the size
        temp->set_curr_size += 1;
        set_insert_elem(temp, i);
    }

    printk(KERN_ALERT "This is the value of count we are returning: %ld\n", count);
    return count;
}

static const struct proc_ops myops = {
    .proc_read = set_file_read,
    .proc_write = set_file_write,
    .proc_release = set_file_release,
    .proc_open = set_file_open
};

static int __init set_init(void) {
    int i;
    set_file = proc_create("partb_24CS60R43_24CS60R26", 0666, NULL, &myops);
    if (!set_file) {
        printk(KERN_ALERT "File create unsuccessful\n");
        return -ENOMEM;
    }
    // Memory allocation for array of set_pid_map array
    set_proc_map = (struct set_pid_map **)kmalloc(sizeof(struct set_pid_map*) * MAX_SET_SIZE, GFP_KERNEL);
    for(i=0;i<MAX_SET_SIZE;i++)
        set_proc_map[i]= NULL;
    if(!set_proc_map){
        printk(KERN_ALERT "Memory allocation failed\n");
        return -ENOMEM;
    }
    mutex_init(&set_mutex);
    printk(KERN_ALERT "Set module loaded\n");
    return 0;
}

static void __exit set_exit(void) {
    if (set_proc_map){
        int i;
        for(i=0;i<MAX_SET_SIZE;i++)
            kfree(set_proc_map[i]); // Free the dynamically allocated memory for the set
    }
    if (set_file)
        proc_remove(set_file);
    printk(KERN_WARNING "Removing set module. Bye...\n");
}

module_init(set_init);
module_exit(set_exit);