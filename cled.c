/**************************************************************************************************
***************************************************************************************************
**  文件名称 : rk3399的led程序
**  作    者 ：chenhao
**  设计时间 ：2020/10/16
**  更改时间 ：2020/10/16
**  版本说明 ：V1.0
**  硬件说明 ：
**************************************************************************************************/


/**************************************************************************************************
**  功能说明 ：头文件包含
**************************************************************************************************/

#include <linux/fs.h>
#include <linux/poll.h>
#include <linux/irq.h>
#include <asm/irq.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/time.h>

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <asm/io.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
//#include <asm/mach/map.h>
#include <asm/uaccess.h>         //该头文件包涵copy_to_user(buf, RxBuffer, count);copy_from_user(TxBuffer, buf, count);
#include <asm/io.h>

/**************************************************************************************************
**  功能说明 ：数据定义说明
**************************************************************************************************/
#define     uint32_t        unsigned int
#define     uint16_t        unsigned short
#define     uint8_t         unsigned char
#define     _bool           unsigned char

#define     WORD            unsigned short
#define     BYTE            unsigned char
#define     bool            unsigned char

#define     true            1
#define     false           0
#define     OK              1
#define     FAIL            0
#define     TRUE            1
#define     FALSE           0

/*************************************************************************************************/


#define     DEVICE_NAME     "cled"
//------------------------------------------------------


/* gpioled 设备结构体 */
struct gpioled_dev
{
    dev_t devid;        /* 设备号 */
    struct cdev cdev;   /* cdev */
    struct class *class; /* 类 */
    struct device *device; /* 设备 */
    int major;          /* 主设备号 */
    int minor;          /* 次设备号 */
    struct device_node *nd; /* 设备节点 */
    int LED_gpios[2];   /* led 所使用的 GPIO 编号 */
    int lcd_gpio;
    int buz_gpio;

};
struct gpioled_dev gpioled;
/**************************************************************************************************
**  功能说明 ：函数声明
**************************************************************************************************/
void init_sigaction(void);

struct	timer_list	polling_timer;
struct	timer_list	buzzing_timer;

static void polling_handler(unsigned long data)			//告诉用户时间到了,定时处理响应事件,0.3
{
    static BYTE cnt=0;
    static int i = 0;
    mod_timer(&polling_timer,jiffies + 300);
    cnt++;
    if(i==1)
    {
	gpio_set_value(gpioled.LED_gpios[1], 0);
	i = 0;
    }
    else
    {
	gpio_set_value(gpioled.LED_gpios[1], 1);
	i = 1;
    }
/*
    for (i=0;i<2;i++)
    {
        if( cnt & (1<<i))
        {
            gpio_set_value(gpioled.LED_gpios[i], 0);
        }
        else
        {
            gpio_set_value(gpioled.LED_gpios[i], 1);
        }
    }
*/
}


/*************************************************************************************************/
void init_sigaction(void) 					//建立信号处理机制,程序初始化需调用该函数
{
	init_timer(&polling_timer);
	polling_timer.data = (unsigned long)0;
	polling_timer.function = &polling_handler;
	polling_timer.expires = jiffies +100 ;
	add_timer(&polling_timer);


}

/*************************************************************************************************/
static inline uint32_t get_ms(void)
{
    struct timeval tv;
    do_gettimeofday(&tv);
    return tv.tv_sec*1000 + tv.tv_usec/1000;
}

void delay(uint32_t time)
{
   // uint32_t temp=0;
    uint32_t time1 =0;
    time1= get_ms();
    while((uint32_t)(get_ms()-time1) < time);
}



//*************************************************************************************************
void LED_PIN_init(void)
{
        printk(DEVICE_NAME"\tLED_PIN_initialized\n");
}






/**************************************************************************************************
**  函数功能:   CAN设备的配置和驱动程序
**  入口参数:   无
**  出口参数:   无
**************************************************************************************************/
static long LED_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    long rtn_getposition = 0;
    switch(cmd)
    {
        case 0:
            //return tx_cpu_count;
            break;
        case 1:
            break;

        case 2:
            break;

        case 3:
            break;

        case 6:
            gpio_set_value(gpioled.buz_gpio, 0);
            mod_timer(&buzzing_timer,jiffies + 15);
            break;
        case 8:
            break;
        case 9:

            break;

        default:
            return -EINVAL;
    }

    return -EINVAL;
}

/**************************************************************************************************
**  函数功能:   CAN设备的读操作程序
**  入口参数:   用户缓冲区：RxBuffer；系统缓冲区：buf；长度：count
**  出口参数:   无
**************************************************************************************************/
ssize_t LED_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    return 0;
}

/**************************************************************************************************
**  函数功能:   CAN设备的写操作程序
**  入口参数:   无
**  出口参数:   无
**************************************************************************************************/
ssize_t LED_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
    return 0;
}

/**************************************************************************************************
**  函数名称:   void    LED_StartInit();
**  函数功能:   初始化与CAN有关寄存器和设备
**  入口参数:   无
**  出口参数:   无
**************************************************************************************************/
void    LED_StartInit(void)
{
    LED_PIN_init();                              //对触摸屏PIN初始化
	init_sigaction();
}

/*------------------------------------------------------------------------------------------------------------*/
static struct file_operations LED_fops = {
    .owner          =   THIS_MODULE,
    .unlocked_ioctl =   LED_ioctl,
    .write          =   LED_write,
    .read           =   LED_read,
};

static struct miscdevice misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &LED_fops,
};

/*------------------------------------------------------------------------------------------------------------*/
static int __init LED_init(void)
{
    int ret;
    /* 1、获取设备节点： gpioled */
    gpioled.nd = of_find_node_by_path("/gpio-dled");
    if(gpioled.nd == NULL)
    {
        printk("gpioled node cant not found!\r\n");
         return -EINVAL;
    }
    else
    {
        printk("gpioled node has been found!\r\n");
    }

    gpioled.LED_gpios[0] = of_get_named_gpio(gpioled.nd, "led1_gpios", 0);
    gpio_request(gpioled.LED_gpios[0], "led1");
    gpio_direction_output(gpioled.LED_gpios[0], 0);
    gpio_set_value(gpioled.LED_gpios[0], 0);

    gpioled.LED_gpios[1] = of_get_named_gpio(gpioled.nd, "led2_gpios", 0);
    gpio_request(gpioled.LED_gpios[1], "led2");
    gpio_direction_output(gpioled.LED_gpios[1], 1);
    gpio_set_value(gpioled.LED_gpios[1], 1);

    ret = misc_register(&misc);
    if (ret < 0)
    {
      printk(DEVICE_NAME "is failed!\n");
      return ret;
    }
    printk(DEVICE_NAME "dev-tree rk3399 dLED ver1.00 -GPIO1_C7<1 23> GPIO1_D0 <1 24>-2020-10-16 \n");

    LED_StartInit();

    return ret;
}

/*------------------------------------------------------------------------------------------------------------*/
static void __exit LED_exit(void)
{
    misc_deregister(&misc);
    del_timer(&polling_timer);
    printk("Good-bye, rk3399_LED was removed by chenhao!\n");
}

/*------------------------------------------------------------------------------------------------------------*/

module_init(LED_init);
module_exit(LED_exit);

MODULE_AUTHOR("chest");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("led interface for rk3399 MPU");

/**************************************************************************************************
**************************************************************************************************/

