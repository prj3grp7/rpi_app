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
#include <linux/platform_device.h>
#include <linux/of_gpio.h>
#include <linux/err.h>
#include <linux/semaphore.h>
#include <linux/delay.h>


//Protocol definitions
#define side1_ADDRESS 0x10          // I2C Address for side1 : Player side
#define side2_ADDRESS 0x11          // I2C Address for side2 : Player side
#define ballDispenser_ADDRESS 0x12  // I2C Address for : Ball dispenser

//Driver definitions
#define NBR_MINOR 3           // Number of minor numbers
#define BASE_MINOR 0          // The first minor number to be allocated
#define BUFFER_SIZE 255         // Size of buffer read for reading and writing from and to the driver
                              // Can't read or write more than BUFFER_SIZE

//Module info
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Gruppe 7");
MODULE_DESCRIPTION("i2c-interruptDriver");

// Character driver functions
int i2c_interrupt_open(struct inode * inode, struct file * filep);
int i2c_interrupt_release(struct inode * inode, struct file * filep);
ssize_t i2c_interrupt_read(struct file * filep, char __user * buf, size_t count, loff_t *f_pos);
ssize_t i2c_interrupt_write(struct file *filep, const char __user *buf, size_t count, loff_t *f_pos);

//struct to be able to create platform driver
static const struct of_device_id i2c_platform_device[];
static struct platform_driver i2c_platform_driver;
struct class* i2c_platform_class;

// Structures necessary to create a character device.
static struct cdev i2c_interrupt_cdev;
static struct device *i2c_devices[10];

struct file_operations i2c_interrupt_fops = {
  .owner = THIS_MODULE,
  .open = i2c_interrupt_open,
  .release = i2c_interrupt_release,
  .read = i2c_interrupt_read,
  .write = i2c_interrupt_write,
};

//global gpio count
static int gpio_count;
// Device number. To contain major and minor number.
static int devno;

//Handle Interrupt and sleeping
static wait_queue_head_t queue[3];
static irqreturn_t isr(int irq, void *dev_id);

// Declere pointer to a i2c_adapter, which is representing the i2c hardware on the RPi
static struct i2c_adapter * adap;

// Custom structure for each I2C device
struct i2c_interrupt_device {
  struct i2c_client * client;       // Representing a device on the I2C bus
  struct i2c_board_info clientInfo; // Info about a device on the I2C bus. Used to create a i2c_client.
  char name[15];                    //name of device
  int minor;                         // The minor number for the given device
  int nr;                              //Gpio nr. for the given device
  int dir;                              //  direction of gpio
  int open_already;        //Used in function open to make sure interruot i only requested once
  int flag;                //Used for wait_event_interruptible conditional argument
};

//3 interrupt devices created one for every
static struct i2c_interrupt_device devices[3]={
  {.clientInfo = {I2C_BOARD_INFO("PlayerSide 1", side1_ADDRESS),},.name = "playerside1", .open_already = 0, .flag=0},
  {  .clientInfo = {I2C_BOARD_INFO("PlayerSide 2", side2_ADDRESS),},.name = "playerside2",  .open_already = 0, .flag=0},
  {.clientInfo = {I2C_BOARD_INFO("ballDispenser", ballDispenser_ADDRESS),},.name = "BallDispenser",  .open_already = 0, .flag=0},
};

// Init function for kernel module
static int __init i2c_interrupt_init(void)
{


  // Registering device numbers
  printk(KERN_INFO"Register devno\n");
  if(alloc_chrdev_region(&devno,BASE_MINOR, NBR_MINOR,"i2c_driver"))
  {
    printk(KERN_ALERT "Devno registration error\n");
    goto register_devno_error;
  }

  // Initializing cdev
  cdev_init(&i2c_interrupt_cdev, &i2c_interrupt_fops);

  // Adding character device to system
  printk(KERN_INFO"Adding cdev to system\n");
  if(cdev_add(&i2c_interrupt_cdev, devno, NBR_MINOR))
  {
    printk(KERN_ALERT "Error when adding cdev to system\n");
    goto add_cdev_error;
  }

//platform driver class created in /sys/class
  i2c_platform_class = class_create(THIS_MODULE, "i2c_platform_class");
  if(IS_ERR(i2c_platform_class)){
    printk(KERN_ALERT "Error when creating platform class");
    goto create_plat_class_error;
  }
  else
    printk(KERN_INFO"platform class '%s' created",i2c_platform_class->name);

//driver registered as platform driver
  if(platform_driver_register(&i2c_platform_driver)){
    printk(KERN_ALERT"ERROR registering as platform driver\n");
    goto platform_register_error;
  }
  else
    printk(KERN_INFO "platform driver registered");

  return 0; // Return 0 for success


// Error handling
// Unregister things in opposite order as registration
platform_register_error:
  class_destroy(i2c_platform_class);
create_plat_class_error:
add_cdev_error:
  printk(KERN_INFO "Unregister devno\n");
  unregister_chrdev_region(devno,NBR_MINOR);
register_devno_error:

return -1; // Return -1 for error
}


void __exit i2c_interrupt_exit(void)
{
  //unregister platform driver
   platform_driver_unregister(&i2c_platform_driver);
  //destroy the class created in sys/class
   class_destroy(i2c_platform_class);

  // Removing character device from system
  printk(KERN_INFO "Deleting cdev\n");
  cdev_del(&i2c_interrupt_cdev);

  // Unregister device numbers
  printk(KERN_INFO "Unregister devno\n");
  unregister_chrdev_region(devno,NBR_MINOR);



}

// Open function for character device
int i2c_interrupt_open(struct inode * inode, struct file * filep)
{
  //Get major and minor number
  int major, minor;
  major = imajor(inode);
  minor = iminor(inode);
  printk(KERN_INFO "Opening I2C-interruptDriver [major], [minor]: %d, %d\n", major,minor);

  // Set private data according to minor number
  // Each minor number represent each device connected to I2C bus
  // Setting private data to the correct i2c_interrupt_device (I2C Device)
  filep->private_data = &devices[minor];

  //requesting interrupt
  devices[minor].open_already++;
  if(devices[minor].open_already==1){
  int irq_line=0;
		init_waitqueue_head(&queue[minor]);
		irq_line = gpio_to_irq(devices[minor].nr);
		devices[minor].minor=minor;
    	if (request_irq(irq_line,isr,IRQF_TRIGGER_FALLING,"IRQ",&devices[minor])) {
   	 		printk("error requesting irq with minor nr.%d",minor);
    		goto irq_request_error;
  		}

}
  return 0;

irq_request_error:
return -1;
}

// Release function for character device
int i2c_interrupt_release(struct inode * inode, struct file * filep)
{
  //Get major and minor number
  int major, minor;
  major = imajor(inode);
  minor = iminor(inode);
  printk(KERN_INFO "Closing/Releasing I2C-interruptDriver [major], [minor]: %d, %d\n", major,minor);
  if(devices[minor].open_already==1)
    free_irq(gpio_to_irq(devices[minor].nr),&devices[minor]);
  devices[minor].open_already--;

return 0;
}

ssize_t i2c_interrupt_read(struct file * filep, char __user * buf, size_t count, loff_t *f_pos)
{
  // Get major and minor number
  int major, minor;
  major = imajor(filep->f_inode);
  minor = iminor(filep->f_inode);
  printk(KERN_INFO "Reading I2C-interruptDriver [major], [minor]: %i, %i", major, minor);

  // Getting private data and from that the I2C client
  struct i2c_interrupt_device *side = filep->private_data;
  struct i2c_client *client = side->client;

  // Declaring buffer (in kernelspace) to read to from I2C bus
  char kernelBuf[BUFFER_SIZE];

  // Prevent reading more data than buffer can hold
  count = (count > BUFFER_SIZE)? BUFFER_SIZE : count;
  // Sleep process
  printk(KERN_INFO "sleeping proces with flag: %d",devices[minor].flag);
  devices[minor].flag=0;
  wait_event_interruptible(queue[minor],devices[minor].flag==1);
  printk(KERN_INFO "process woken with flag: %d",devices[minor].flag);

  // Read data and copy to user
  int bytesRead;

//receiving data from client
  bytesRead =  i2c_master_recv(client, kernelBuf,1);
  if(bytesRead<0)
  {
    printk(KERN_ALERT "FEJL NR: %d\n",bytesRead);
    return 1;
  }
    *f_pos += bytesRead;
  //copy sent data to userspace
  if(copy_to_user(buf, kernelBuf, count))
  {
    printk(KERN_ALERT "copy_to_user failed\n");
    return 1;
  }

  return bytesRead;

}


ssize_t i2c_interrupt_write(struct file *filep, const char __user *buf, size_t count, loff_t *f_pos)
{


  // Get major and minor number
  int major, minor;
  major = imajor(filep->f_inode);
  minor = iminor(filep->f_inode);
  printk(KERN_INFO "Writing I2C-interruptDriver [major], [minor]: %i, %i", major, minor);

  // Getting private data and from that the I2C client
  struct i2c_interrupt_device *side = filep->private_data;
  struct i2c_client *client = side->client;

  // Declaring buffer (in kernelspace) to wrtie to from user.
  // Buffer is used when writing to I2C device
  char kernelBuf[BUFFER_SIZE];

  // Prevent writing more data than buffer can hold
  count = (count > BUFFER_SIZE)? BUFFER_SIZE : count;

  // Get data from userspace
  if(copy_from_user(kernelBuf, buf, count))
  {
    printk(KERN_ALERT "copy_from_user failed\n");
    return -1;
  }

  // Write to I2C device
  int bytesWritten;
  bytesWritten =  i2c_master_send(client, kernelBuf,
  			  count);
  if(bytesWritten<0){
  printk(KERN_ALERT "FEJL NR: %d i write",bytesWritten);
  }

  *f_pos += bytesWritten;
  return bytesWritten;
}


// Bottom half of interrupt handler
static irqreturn_t isr(int irq, void *dev_id)
{
  printk(KERN_INFO "IRS PROC started");

  // Get i2c_interrupt_device data
  struct i2c_interrupt_device *dev=dev_id;
  int minor=dev->minor;     //find minor number from device

  if(dev->flag == 0){
  dev->flag = 1;            //setting flag
  wake_up_interruptible(&queue[minor]);
}
  return IRQ_HANDLED;
}

static int i2c_interrupt_probe(struct platform_device *pdev)
{
	unsigned int minor=0;
  printk(KERN_ALERT"New Platform device: %s\n", pdev->name);
  // Getting I2C adapter for bus nr 1.
  printk(KERN_INFO"Getting I2C adapter\n");
  adap = i2c_get_adapter(1);
  if (!adap){
    printk(KERN_ALERT "Error when getting I2C adapter\n");
    goto get_adapter_error;
  }

//get gpio count
  gpio_count=of_gpio_count(pdev->dev.of_node);
  if(gpio_count<0){
    printk(KERN_ALERT "Error getting count\n");
    goto gpio_count_error;
  }

//getting gpio nr and driection and requesting them
  enum of_gpio_flags flag;
  for(minor=0;(minor<gpio_count) && (minor<NBR_MINOR);minor++)
  {
    devices[minor].nr=of_get_gpio(pdev->dev.of_node,minor);
    if(devices[minor].nr<0){
      printk(KERN_ALERT "Error when getting gpios\n");
      goto get_gpio_error;
    }
  }
//getting gpio direction
  for(minor=0;(minor<gpio_count) && (minor<NBR_MINOR);minor++)
  {
    if(of_get_gpio_flags(pdev->dev.of_node, minor, &flag)<0){
      printk(KERN_ALERT "Error getting gpio direction");
      goto get_gpio_dir_error;
    }
    devices[minor].dir=(int)flag;
  }

//requesting gpios
  for(minor=0;(minor<gpio_count) && (minor<NBR_MINOR);minor++)
  {
    if(gpio_request(devices[minor].nr,"INT_PIN_nr_%d\n"))
    {
      printk(KERN_ALERT "INT_PIN (%d) request error\n", devices[minor].nr);
      goto request_pin_error;
    }
  }

//setting directing for gpio
  for(minor=0;(minor<gpio_count) && (minor<NBR_MINOR);minor++)
  {
    if(gpio_direction_input(devices[minor].nr))
    {
      printk(KERN_ALERT "INT_PIN (%d) direction error\n", devices[minor].nr);
      goto direction_pin_error;
    }
  }

//creating i2c_client structs
  for(minor = 0; (minor<gpio_count) && (minor<NBR_MINOR);minor++)
  {
    devices[minor].client = i2c_new_device(adap, &devices[minor].clientInfo);
    if (!devices[minor].client) {
      printk(KERN_ALERT "Error when creating I2C device for side1\n");
      goto client_device_error;
    }
  }

//creating nodes for the i2c devices
 for(minor=0;(minor<gpio_count) && (minor<NBR_MINOR);minor++){
  i2c_devices[minor]=device_create(i2c_platform_class,NULL,MKDEV(MAJOR(devno),minor),NULL,"%s",devices[minor].name);
  if(IS_ERR(i2c_devices[minor])){
    printk(KERN_ALERT "Failed to create device with minor nr. %d\n",minor);
    goto create_device_error;
  }

}
    return 0;


  create_device_error:
  for(size_t i=0;i<minor;i++){
    printk(KERN_INFO "Error: Freeing device with minor nr.(%d)\n",i);
      device_destroy(i2c_platform_class,MKDEV(MAJOR(devno),i));
  }
  client_device_error:
  for(unsigned int i=0;i<minor;i++)
    i2c_unregister_device(devices[i].client);
  direction_pin_error:
  request_pin_error:
  for(size_t i=0;i<minor;i++){
    printk(KERN_INFO "Error: Freeing INT_gpio_nr_(%d)\n",devices[i].nr);
    gpio_free(devices[i].nr);
  }
  get_gpio_dir_error:
  get_gpio_error:
  gpio_count_error:

  i2c_put_adapter(adap);
  get_adapter_error:
  return -1;
}


static int i2c_interrupt_remove(struct platform_device *pdev)
{
  printk(KERN_ALERT"Removing devices: %s\n", pdev->name);


// Unregister all devices on bus
  for(size_t i=0;(i<gpio_count) && (i<NBR_MINOR);i++)
  {
    printk(KERN_INFO "Unregister i2c client %s\n", devices[i].name);
    i2c_unregister_device(devices[i].client);
  }

  printk(KERN_INFO "Putting i2c adapter\n");
  i2c_put_adapter(adap);

  //destroy all devices created in /dev
  for(size_t i=0;(i<gpio_count) && (i<NBR_MINOR);i++)
    device_destroy(i2c_platform_class,MKDEV(MAJOR(devno),i));

 // Releasing GPIO
  for(size_t i=0;(i<gpio_count) && (i<NBR_MINOR);i++){
    printk(KERN_INFO "Freeing INT_gpio_nr_(%d)\n",devices[i].nr);
    gpio_free(devices[i].nr);
  }
  return 0;
}

//device id struct
static const struct of_device_id i2c_platform_device[]=
{
  {.compatible = "i2c_plat_drv",},{},
};

//platform driver struct
static struct platform_driver i2c_platform_driver={
  .probe=i2c_interrupt_probe,
  .remove=i2c_interrupt_remove,
  .driver={
      .name = "my_p_dev",
      .of_match_table = i2c_platform_device,
      .owner = THIS_MODULE,
  },
};

// Declare init and exit function
module_init(i2c_interrupt_init);
module_exit(i2c_interrupt_exit);
