#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/stat.h>
#include <linux/fs.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>
#include <linux/slab.h>
MODULE_LICENSE("GPL");


struct myfile{
struct file* f; 
mm_segment_t fs; 
loff_t pos;


};



struct myfile* openfile (const char *path) 
{
    struct myfile *myfileptr;
    myfileptr= kmalloc(sizeof(struct myfile), GFP_KERNEL);
   // mm_segment_t oldfs;
    int err = 0;

    myfileptr->fs = get_fs();
    set_fs(get_ds());
   myfileptr-> f = filp_open(path,0, 0);
    set_fs(myfileptr->fs);
    if (IS_ERR(myfileptr->f)) {
        err = PTR_ERR(myfileptr->f);
        return NULL;
    }
    return myfileptr;
}


int read_from_file_until(struct myfile *myfileptr, unsigned char* buf,unsigned long  vlen, char c) 
{
   // mm_segment_t oldfs;
    int ret;

   myfileptr-> fs = get_fs();
    set_fs(get_ds());
int i;    
    for (i=0;i<vlen;i++){
	
   	 ret = vfs_read(myfileptr->f, buf+i ,1 , &(myfileptr->pos));
	 if (buf[i]==c){
	 buf[i]=NULL;
	 break;
	 }
	}
    set_fs(myfileptr->fs);
    return ret;
}   

void file_close(struct myfile *myfileptr) 
{
    filp_close(myfileptr->f, NULL);
}








int init_module(void){
struct myfile *myfileptr;

myfileptr= kmalloc(sizeof(struct myfile), GFP_KERNEL);


const char* filepath="/proc/version";
myfileptr= openfile(filepath);
printk(KERN_ALERT "PROGRAM Started\n");
if (myfileptr==NULL)
	printk(KERN_ALERT "NULL PTR RETURNED\n");
else {
	printk(KERN_ALERT "FILE OPENED\n");
	char* mybuffer;
	mybuffer= kmalloc(sizeof(char),GFP_KERNEL);
	myfileptr->pos=0;
	read_from_file_until(myfileptr,mybuffer,1000, ' ');
	read_from_file_until(myfileptr,mybuffer,1000, ' ');
	read_from_file_until(myfileptr,mybuffer,1000, ' ');
	printk(KERN_ALERT "Kernel_Version: %s\n", mybuffer);
	file_close(myfileptr);
	printk(KERN_ALERT "FILE CLOSED\n");
	char systemmap [100]="/boot/System.map-";
	strncat(systemmap, mybuffer, 20);
	printk(KERN_ALERT "Name of the file: %s\n", systemmap);
	myfileptr=openfile(systemmap);
	if (myfileptr== NULL)
		printk(KERN_ALERT "NULL ptr returned\n");
	else {
		printk(KERN_ALERT "FILE2 OPENED\n");
		char* sysbuffer;
		sysbuffer= kmalloc(sizeof(char),GFP_KERNEL);
		myfileptr->pos=0;
		int i; int flag=1;
		while (flag){
		
		read_from_file_until(myfileptr,sysbuffer,1000, '\n');
		if (strstr (sysbuffer, "sys_call_table")!= NULL)
			flag=0;
		
		
		}
		char addr [17];
		strncpy(addr,sysbuffer,16);
		addr[16] ='\0';
		printk(KERN_ALERT "FirstLineinsysmap: %s\n", sysbuffer);
		printk(KERN_ALERT "FirstLineAddress: %s\n", addr);	
		sys_call_ptr_t* sys_call;
		unsigned long ladd; 
		sscanf(addr,"%lx", &ladd);
		sys_call= (sys_call_ptr_t *) ladd;
		printk(KERN_ALERT "NR_FORK_ADDRESS: %px \n", sys_call[2]);
		kfree(sysbuffer);
	
	}

	kfree(myfileptr);
	kfree(mybuffer);
	
}

	
return 0; 
}



void cleanup_module(void){
int i;
for (i=0; i< 3; i++)
	printk(KERN_INFO "%d. hi hi\n",i+1);

}



