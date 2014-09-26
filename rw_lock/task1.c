#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/rwlock.h>
struct task_struct* MyThread = NULL;
struct task_struct* MyThread1 = NULL;
int MyVar = 1;
int count = 0;
rwlock_t lock;//声明读写锁
void setMyVar(int num){
	write_lock(&lock);//写加锁
	if(count){//计数判断是否在使用
		printk("busy setmyvar\n");
	}
	MyVar = num;
	count++;
	write_unlock(&lock);//解锁
	count--;
	printk("setMyVar num is: %d\n",MyVar);;
}
int getMyVar(void){
	int temp = 0;
	read_lock(&lock);//读加锁
	if(count){
		printk("busy getmyvar\n");
	}
	temp = MyVar;
	count++;
	read_unlock(&lock);//解锁
	count--;
	printk("getMyVar num is: %d\n",MyVar);
	return temp;
}
int ThreadOne(void *num){//线程1
	int i = 0;	
	while(!kthread_should_stop()){
		printk("In threadone \n");		
		setMyVar(i++);
		getMyVar();
		ssleep(3);
	}
}
int Threadtwo(void *date){//线程2
	int j = 100;
	while(!kthread_should_stop()){
		printk("In threadtwo \n");
		setMyVar(j++);
		getMyVar();
		ssleep(3);
	}
}
static int __init
hello_init(void)
{
	rwlock_init(&lock);//锁初始化
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
