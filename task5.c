#include <linux/fs.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/stat.h>
#include <linux/slab.h>

struct myfile{
	struct file * f;
	mm_segment_t fs;
	loff_t pos;
};

struct myfile * open_file_for_read (char * filename){
	
	struct myfile * menna;
	
	menna = kmalloc(sizeof(struct myfile), GFP_KERNEL);

	menna -> f = NULL;

	int err=0;
	menna -> fs = get_fs();
	set_fs(get_ds());
	menna -> pos=14;
	menna -> f=filp_open(filename, O_RDONLY, 0);
	set_fs(menna -> fs);
	if (IS_ERR(menna -> f)){
			err=PTR_ERR(menna -> f);
			return NULL;
	}
	return menna;
}
volatile int read_from_file_until (struct myfile * mf, char * buf, unsigned long vlen, char c){

	int sol;
	char* delimiter = kmalloc (sizeof (char)*2, 0);
        

	mf -> fs = get_fs();
	set_fs(get_ds());

	sol = vfs_read (mf -> f, buf, vlen, &(mf -> pos)); 
	mf->pos  -= vlen; 
	mf-> pos += strchr (buf, c) - buf +1; 
	buf [strchr (buf, c) - buf] = 0; 
/*	printk ("%s \n", buf); */
	set_fs(mf -> fs);
	return sol;

}
void close_file (struct myfile * mf){

	filp_close(mf -> f, NULL);
}


int init_module(void){

	char * bufff;
	bufff= kmalloc(150,GFP_KERNEL);
	
	printk(KERN_INFO "Hello World CSCE-3402 :) \n");
	struct myfile * menna =  open_file_for_read ("/proc/version");

	int solution = read_from_file_until(menna, bufff,16,' ');
	printk(KERN_INFO "%s\n",bufff);
	close_file(menna);


	char * sys;
	sys = kmalloc(sizeof(char*)*34,GFP_KERNEL);
	strcpy(sys,"/boot/System.map-");
	bufff[16]=0;
	strcat(sys,bufff);
	printk("%s \n", sys);

	sys[32] = NULL;
	struct myfile * systemMap = open_file_for_read(sys);
 
	char * final;
	final = kmalloc(16, GFP_KERNEL);
	final[16] = NULL;
	int i;
	for (i=0;i<16; i++){
	       final[i]=0;
	}	       


	while(strstr(bufff,"sys_call_table")==NULL){
		
		
	read_from_file_until(systemMap, bufff,150,'\n'); 

	}
	/*printk ("%s \n", bufff);*/ 

	strncat(final,bufff,16);
	printk ("Address: %s \n", final);


	long long temp;
	void**fork;
	sscanf(final, "%11x", &temp);
	forkk = temp;
	printk("Fork Address: %p\n", forkk[__NR_fork]);
	
	close_file(systemMap);

	kfree(bufff);
	kfree(menna);
	kfree(sys);
	kfree(systemMap);
	kfree(final);
	
	return 0;
}
void cleanup_module(void){
	printk(KERN_INFO "Bye Bye CSCE-3402 :)\n");
}

MODULE_LICENSE("GPL");
