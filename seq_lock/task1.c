#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/seqlock.h>
struct task_struct* MyThread = NULL;
struct task_struct* MyThread1 = NULL;
int MyVar = 1;
int count = 0; 
seqlock_t lock;//声明顺序锁
void setMyVar(int num){
	write_seqlock(&lock);//写加锁
	 if(count){
		printk("busy setmyvar\n");
	}
	MyVar = num;
	count++;
	write_sequnlock(&lock);//解锁
	count--;
	printk("setMyVar num is: %d\n",MyVar);;
}

int getMyVar(void){
	unsigned long seq;//顺序锁中计数
	int temp = 0;
	do{
		seq = read_seqbegin(&lock);//读取顺序号，如果是奇数，说明是在写操作，处理器等待，如果不是奇数的话，就返回读到的顺序号
		temp = MyVar;
	}while(read_seqretry(&lock,seq));//检测读到的数据有没有效，如果顺序号跟一开始的不一致，就返回1，说明修改了临界区，需要重新读数据
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
	return 0;
}
int Threadtwo(void *date){//线程2
	int j = 100;
	while(!kthread_should_stop()){
		printk("In threadtwo \n");
		setMyVar(j++);
		getMyVar();
		ssleep(3);
	}
	return 0;
}
static int __init
hello_init(void)
{
	seqlock_init(&lock);//锁初始化
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
