#include <linux/gpio.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/i2c.h>

#define BUFFER_SIZE 256
#define I2C_MAJOR 50
#define I2C_MINOR 0
#define NBR_MINOR 1 //Number of minor numbers

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Gruppe 7");
MODULE_DESCRIPTION("ic2_test Driver");

int i2c_test_open(struct inode * inode, struct file * filep);
int i2c_test_release(struct inode * inode, struct file * filep);
ssize_t i2c_test_read(struct file * filep, char __user * buf, size_t count, loff_t *f_pos);
ssize_t i2c_test_write(struct file *filep, const char __user *buf, size_t count, loff_t *f_pos);



static struct cdev i2c_test_cdev;

struct file_operations i2c_test_fops = {
  .owner = THIS_MODULE,
  .open = i2c_test_open,
  .release = i2c_test_release,
  .read = i2c_test_read,
  .write = i2c_test_write,
};


static int devno;

static struct i2c_board_info tempInfo = {
	I2C_BOARD_INFO("Temp sensor", 0x48),
};

static struct i2c_client * tempClient;

static struct i2c_adapter * adap;


static const struct i2c_board_info EmptyInfo;

static int __init i2c_test_init(void)
{
  printk(KERN_INFO "Getting I2C adapter\n");
  adap = i2c_get_adapter(1);
  if (!adap){
    printk(KERN_ALERT "Error when getting I2C adapter\n");
    goto request_i2c_error;
  }

  printk(KERN_INFO "Creating I2C device for temp\n");
  tempClient = i2c_new_device(adap, &tempInfo);
  if (!tempClient) {
    printk(KERN_ALERT "Error when creating I2C device for temp\n");
    goto new_i2c_device_error;
  }

  devno = MKDEV(I2C_MAJOR, I2C_MINOR);

  printk(KERN_INFO "Register devno\n");
  if(register_chrdev_region(devno,NBR_MINOR,"i2c_test Driver"))
  {
    printk(KERN_ALERT "Devno registration error\n");
    goto register_devno_error;
  }

  cdev_init(&i2c_test_cdev, &i2c_test_fops);

  printk(KERN_INFO "Adding cdev to system\n");
  if(cdev_add(&i2c_test_cdev, devno, NBR_MINOR))
  {
    printk(KERN_ALERT "Error when adding cdev to system\n");
    goto add_cdev_error;
  }

return 0;

add_cdev_error:
  printk(KERN_INFO "Unregister devno\n");
  unregister_chrdev_region(devno,NBR_MINOR);
register_devno_error:
//request_pin_error:
  //TODO: free I2C
new_i2c_device_error:
    i2c_put_adapter(adap);
request_i2c_error:
return -1;

}


void __exit i2c_test_exit(void)
{


  printk(KERN_INFO "Deleting cdev");
  cdev_del(&i2c_test_cdev);

  printk(KERN_INFO "Unregister devno");
  unregister_chrdev_region(devno,NBR_MINOR);

  //TODO: free I2C
  i2c_put_adapter(tempClient->adapter);
  i2c_unregister_device(tempClient);
}


int i2c_test_open(struct inode * inode, struct file * filep)
{
  int major, minor;
  major = imajor(inode);
  minor = iminor(inode);
  printk(KERN_INFO "Opening i2c_test Device [major], [minor]: %d, %d\n", major,minor);

  switch (minor) {
    case 0:
    filep->private_data = tempClient;
  }
  return 0;
}

int i2c_test_release(struct inode * inode, struct file * filep)
{
  int major, minor;
  major = imajor(inode);
  minor = iminor(inode);
  printk(KERN_INFO "Closing/Releasing i2c_test Device [major], [minor]: %d, %d\n", major,minor);


  return 0;
}

ssize_t i2c_test_read(struct file * filep, char __user * buf, size_t count, loff_t *f_pos)
{

  if(*f_pos != 0)
  {
    *f_pos = 0;
    return 0;
  }
  int major, minor;
  major = imajor(filep->f_inode);
  minor = iminor(filep->f_inode);
  printk(KERN_INFO "Reading i2c_test Device [major], [minor]: %i, %i", major, minor);

  struct i2c_client *client = filep->private_data;

  char kernelBuf[2];

  count = (count > 2)? 2 : count;

  /*int bytesRead =  */i2c_master_recv(client, kernelBuf,
          count);


  char returnBuffer[256];
  int len = snprintf(returnBuffer,256, "%d.%d", kernelBuf[0], kernelBuf[1]);
  *f_pos += len;
  if(copy_to_user(buf, returnBuffer, len))
  {
    printk(KERN_ALERT "copy_to_user failed\n");
    return -1;
  }


  return len;
}


ssize_t i2c_test_write(struct file *filep, const char __user *buf, size_t count, loff_t *f_pos)
{

  int major, minor;
  major = imajor(filep->f_inode);
  minor = iminor(filep->f_inode);
  printk(KERN_INFO "Writing i2c_test Device [major], [minor]: %i, %i", major, minor);

  struct i2c_client *client = filep->private_data;

  char kernelBuf[BUFFER_SIZE];

  count = (count > BUFFER_SIZE)? BUFFER_SIZE : count;

  if(copy_from_user(kernelBuf, buf, count))
  {
    printk(KERN_ALERT "copy_from_user failed\n");
    return -1;
  }

  int bytesWritten =  i2c_master_send(client, kernelBuf,
  			  count);
  *f_pos += bytesWritten;

  return bytesWritten;
}

module_init(i2c_test_init);
module_exit(i2c_test_exit);
