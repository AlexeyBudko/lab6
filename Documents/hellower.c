#include <linux/kernel.h> /* ��� printk() � �.�. */
#include <linux/module.h> /* ��� �������� ������� �����, ������� �������� ������ */
#include <linux/init.h> /* ����������� �������� */
#include <linux/fs.h>
#include <asm/uaccess.h> /* put_user */
#include <linux/timer.h>

// ���� �� ����� ���������� � ������, ������� ����� ����� ������� � ������� Modinfo
MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Isachenko Andrew" );
MODULE_DESCRIPTION( "print hello in log every x second" );
MODULE_SUPPORTED_DEVICE("hellower");

#define SUCCESS 0
#define DEVICE_NAME "hellower" /* ��� ������ ���������� */

// �������������� ����� ����������� ��������
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

// ���������� ����������, ��������� ��� static, ����������� ���������� ����.
static int major_number; /* ������� ����� ���������� ������ �������� */
static int is_device_open = 0; /* ������������ �� ������ ? */
static char text[] = "hello"; /* �����, ������� �� ����� �������� ��� ��������� � ������ ���������� */
static char* text_ptr = text; /* ��������� �� ������� ������� � ������ */

static int tick_time;
static int hello_counter;

// ����������� ����������� �������� �� �����������
static struct file_operations fops =
{
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};

//������
static struct timer_list my_timer;

void my_timer_callback(unsigned long data)
{
    if (tick_time != 0) {
        printk("%s: %d\n", text, hello_counter++);
        mod_timer(&my_timer, jiffies + msecs_to_jiffies(tick_time));
    }
}

// ������� �������� ������. ������� �����. ����� ������� ��� ��� ��� main()
static int __init test_init(void)
{
tick_time = 0;
hello_counter = 0;
    printk( KERN_ALERT "TEST driver loaded!\n" );

 // ������������ ��������� � �������� ������� ����� ����������
    major_number = register_chrdev(0, DEVICE_NAME, &fops);

    if (major_number < 0) {
        printk("Registering the character device failed with %d\n", major_number);
        return major_number;
    }
    //��������� �������
    setup_timer(&my_timer, my_timer_callback, 0);

 // �������� ����������� ��� ������� ����� ����������
    printk("Test module is loaded!\n");

    printk("Please, create a dev file with 'mknod /dev/hellower c %d 0'.\n", major_number);

    return SUCCESS;
}

// ������� �������� ������
static void __exit test_exit(void)
{
//�������� �������
del_timer(&my_timer);
  // ����������� ����������
    unregister_chrdev(major_number, DEVICE_NAME);

    printk(KERN_ALERT "Test module is unloaded!\n");
}

// ��������� ���� ������� �������� � ��������
module_init(test_init);
module_exit(test_exit);
