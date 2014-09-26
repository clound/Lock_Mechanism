#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
struct task_struct* MyThread = NULL;
struct task_struct* MyThread1 = NULL;
int MyVar = 1;
spinlock_t lock;//声明自旋锁
void setMyVar(int num){
	spin_lock(&lock);//自旋加锁
	 MyVar = num;
	printk("setMyVar num is: %d\n",MyVar);;
}

int getMyVar(void){
	int temp = 0;
	spin_unlock(&lock);//解锁
	temp = MyVar;
	printk("getMyVar num is: %d\n",MyVar);
	return temp;
}
int ThreadOne(void *num){//线程1
	int i = 0;	
	while(!kthread_should_stop()){
		printk("in threadone \n");		
		setMyVar(i++);
		getMyVar();
		ssleep(3);
	}
}
int Threadtwo(void *date){//线程2
	int j = 100;
	while(!kthread_should_stop()){
		printk("in threadtwo \n");
		setMyVar(j++);
		getMyVar();
		ssleep(3);
	}
}
static int __init
hello_init(void)
{
	spin_lock_init(&lock);//锁初始化
	MyThread =  kthread_run(ThreadOne,NULL,"mythread");//线程开始
	MyThread1 = kthread_run(Threadtwo,NULL,"mythread1");	
	return 0;
}
static void __exit
hello_exit(void)//停止线程
{
	if(MyThread){
		kthread_stop(MyThread);
		MyThread = NULL;
	}
	if(MyThread1){
		kthread_stop(MyThread1);
		MyThread1 = NULL;
	}
}
module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Valerie Henson <val@nmt.edu>");
MODULE_DESCRIPTION("\"Hello, world!\" minimal module");
MODULE_VERSION("printk");
