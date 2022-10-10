#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/slab.h>


#define PROC_TEST_DIR "proc_test_d"
#define PROC_TEST_FILE "proc_test_f"

static int proc_test_show(struct seq_file *sf, void *v)
{
	seq_printf(sf, "hello from %s\n", __FUNCTION__);
	return 0;
}

static int proc_test_open(struct inode *p_inode, struct file *p_file)
{
	return single_open(p_file, proc_test_show, NULL);
}

static ssize_t proc_test_read(struct file *p_file, char __user *buff, size_t len, loff_t *off)
{
	char str[] = "hello from proc_test_read\n";
	const int str_len = strlen(str);

	int len_copy = 0;
	if (p_file->f_pos + len > str_len)
	{
		len_copy = str_len - p_file->f_pos;
	}
	else
	{
		len_copy = len;
	}

	int ret = copy_to_user(buff, str + p_file->f_pos, len_copy);
	*off += len_copy - ret;
	return *off;
}

static ssize_t proc_test_write(struct file *p_file, const char __user *buff, size_t count, loff_t *off)
{
	char *tmp_buff = kzalloc(count + 1, GFP_KERNEL);
	if (tmp_buff == NULL)
	{
		printk(KERN_ERR"in %s, alloc %lu bytes fail\n", __FUNCTION__, count);
		return -ENOMEM;
	}

	if (copy_from_user(tmp_buff, buff, count))
	{
		kfree(tmp_buff);
		printk(KERN_ERR"in %s, copy from user fail\n", __FUNCTION__);
		return -EFAULT;
	}

	printk("from %s, get user data:%s\n", __FUNCTION__, tmp_buff);
	kfree(tmp_buff);
	return count;
}

static struct proc_ops proc_test_file_op = {  // 这个结构体似乎只能放在这里定义成static，否则驱动加载后访问文件时会出现异常
	.proc_open = proc_test_open,
	.proc_read = proc_test_read,
	.proc_write = proc_test_write,
	.proc_lseek = seq_lseek,
	.proc_release = single_release
};

struct proc_dir_entry *proc_test_dir = NULL;

static int proc_test_init(void)
{
	printk(KERN_INFO"probing proc_test.ko\n");
	
	proc_test_dir = proc_mkdir(PROC_TEST_DIR, NULL);
	if (proc_test_dir == NULL)
	{
		printk(KERN_ERR"in %s, create proc dir %s fail\n", __FUNCTION__, PROC_TEST_DIR);
		return -EINVAL;
	}

	struct proc_dir_entry *proc_test_file = proc_create(PROC_TEST_FILE, 0777, proc_test_dir, &proc_test_file_op);
	if (proc_test_file == NULL)
	{
		printk(KERN_ERR"in %s, create proc file %s fail\n", __FUNCTION__, PROC_TEST_FILE);
		return -ENOMEM;
	}

	return 0;
}

static void proc_test_exit(void)
{
	remove_proc_entry(PROC_TEST_FILE, proc_test_dir);
	remove_proc_entry(PROC_TEST_DIR, NULL);
	printk(KERN_INFO"removing proc_test.ko\n");
}

module_init(proc_test_init);
module_exit(proc_test_exit);
MODULE_LICENSE("GPL");
