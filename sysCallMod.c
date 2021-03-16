#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/buffer_head.h>
#include <asm/segment.h>
#include <linux/kthread.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/moduleparam.h>
#include <linux/stat.h>
MODULE_LICENSE("GPL");

struct myfile
{
struct file *f;
mm_segment_t fs;
loff_t pos;
};


static char *FILEPATH= "/proc/version";
static char MAPPATH[32]= "/boot/System.map-";
static char *reader= "'\n'";



struct myfile * open_file_for_read (char * filename){

	struct myfile *fp= kmalloc(sizeof(struct myfile), GFP_KERNEL);
	fp->fs= get_fs();
	set_fs(get_ds());
	fp->f= filp_open(filename,0,0);
	set_fs(fp->fs);
	fp->pos=0;
	
if(!(fp->f)){
	fp->pos= PTR_ERR(fp->f);
	return 0;
	printk(KERN_INFO "ERROR");
}

	return fp;
}

volatile int read_from_file_until (struct myfile * mf, char * buf, unsigned long vlen, char c){

int ret;
set_fs(get_ds());
mf-> fs= get_fs();

int i;
for(i=0; i<vlen;i++){
ret= vfs_read(mf->f,buf,vlen,&mf->pos);

	if(buf[i]==c){
	buf[i]= NULL;
	break;
	}
}

set_fs(mf->fs);
/*printk(KERN_INFO "%s\n", buf);*/
return ret;
}	

void close_file (struct myfile * mf){

	filp_close(mf->f, NULL);
}



static int syscallTableAddressFetch(void){

struct myfile *AF= kmalloc(sizeof(struct myfile), GFP_KERNEL); /*Adress Fetcher */


char *rbuffer= kmalloc(sizeof(char)*1024, GFP_KERNEL); /* reading buffer*/
char *rbuffer2= kmalloc(sizeof(char)*1024, GFP_KERNEL);

AF= open_file_for_read(FILEPATH);
AF-> pos=0;
read_from_file_until(AF, rbuffer, 1024,' ');
read_from_file_until(AF, rbuffer, 1024,' ');
read_from_file_until(AF, rbuffer, 1024,' ');
printk(KERN_ALERT "Version: %s \n", rbuffer);
close_file(AF);
strncat(MAPPATH, rbuffer, 20);
printk(KERN_ALERT "File Name: %s \n", MAPPATH);
AF=open_file_for_read(MAPPATH);
AF->pos=0;
int i;

while(true){
	read_from_file_until(AF, rbuffer2, 1024, '\n');
	if(strstr(rbuffer2,"sys_call_table")!=NULL)
	break;
	}
char Address[17];
strncpy(Address,rbuffer2,16);
Address[16]='\0';

printk(KERN_ALERT "syscall map address: %s \n", rbuffer);
printk(KERN_ALERT "syscall map address bardo: %s \n", Address);

sys_call_ptr_t* sysCall;
long holder;
sscanf(Address, "%lx", &holder);
printk(KERN_ALERT "Fork Address: %px \n", sysCall[2]);

close_file(AF);
kfree(rbuffer);
kfree(rbuffer2);
kfree (AF);
return 0;
}



static void cleanup(void){
printk(KERN_INFO "\ntmam keda\n");
}

module_init(syscallTableAddressFetch);
module_exit(cleanup);

