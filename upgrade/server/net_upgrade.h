/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : net_upgrade.h
* Version      : v1.0.0
* Author       : i.sshe
* Github       : github.com/isshe
* Date         : 2016/10/23
* Description  :
* Function list: 1.
*                2.
*                3.
* History      :
\*=========================================================================*/

#ifndef _NET_UPGRADE_H_
#define _NET_UPGRADE_H_

#ifdef __cplusplus
extern "C"{
#endif

/*=========================================================================*\
 * #include#                                                               *
\*=========================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "net_api.h"
#include "timer_list.h"
#include "flash.h"

/*=========================================================================*\
 * #define#                                                                *
\*=========================================================================*/
typedef int (*net_upgrade_cb)(int nPercent, void *usrArg);

#define UPGRADE_FILE_NAME       "/tmp/upgrade"      //暂时放到tmp中
#define UPGRADE_STATUS_FILE     "./upgrade.status"  //保存升级的状态文件

#define UPGRADE_MACHINE_AT91    0x926e
#define UPGRADE_MACHINE_2440    0x2440
#define UPGRADE_MAGIC           0x30404891          //???
#define UPGRADE_FILE_LEN        128

#define UPGRADE_PER_WRITE_LEN   (32 * 1024)
#define UPGRADE_WRITE_BLOCK_LEN (128 * 1024)

#define UPGRADE_UBOOT_MTD       "/dev/mtd1"
#define UPGRADE_KERNEL_MTD      "/dev/mtd6"
#define UPGRADE_ROOTFS_MTD      "/dev/mtd7"
#define UPGRADE_USRFS_MTD       "/dev/mtd4"

#define UPGRADE_FILE_NAME_LEN 	128

/*=========================================================================*\
 * #enum#                                                                  *
\*=========================================================================*/
typedef enum _UPGRADE_STATUS_E_
{
    UPGRADE_IDLE        = 0,        //idle
    UPGRADE_BUSY        = 1,        //recving upgrade file to /tmp
    UPGRADE_WRITE       = 2,        //write /tmp file to nand flash
    UPGRADE_FINISH      = 3,        //upgrade finish
}upgrade_status_e;

typedef enum _UPGRADE_PART_E_
{
     UPGRADE_UBOOT_PART     = 0xF0F0F0F0,   //升级uboot
     UPGRADE_KERNEL_PART    = 0xF1F1F1F1,   //
     UPGRADE_ROOTFS_PART    = 0xF2F2F2F2,
     UPGRADE_USRFS_PART     = 0xF3F3F3F3,   //升级用户程序
     UPGRADE_ALL_PART       = 0xFEFEFEFE,   //升级所有内容
     UPGRADE_FILE           = 0,            //升级rootfs上的某个文件
}upgrade_part_e;

/*=========================================================================*\
 * #struct#                                                                *
\*=========================================================================*/

/***********************************************************************
 * upgrade_file
 * -------------------------------------------------------
 * | hea1der | file1 | file2 |......................|filen|
 * -------------------------------------------------------
 ***********************************************************************/
//升级包包头
typedef struct _UPGRADE_FILE_HEADER_S_
{
     unsigned int       magic;      //UPGRADE_MAGIC
     unsigned int       machine;    //UPGRADE_MACHINE
     unsigned int       version;    //
     unsigned int       file_num;   // 这个升级包里面的文件个数
     unsigned int       total_len;  //升级包的数据长度，不包括这个包头
     unsigned int       crc32;      //32位crc 校验
     unsigned char      res[40];    //保留
}upgrade_file_header_s;

#define UPGRADE_FILE_HEADER_LEN (sizeof(upgrade_file_header_s))

typedef struct _NET_UPGRADE_MAN_S_
{
     unsigned char      status;     //UPGRADE_STATUS_E
     unsigned char      percent;    //0:未升级，1-99:升级中，100升级完成，>100 || < 0升级失败
     unsigned char      res[2];
     unsigned int       total;      //升级包总长度，包括升级包包头
     unsigned int       recv_size;  //接收到的数据长度
     unsigned int       write_size; //写到nand flash 的长度
     unsigned char      file_header[UPGRADE_FILE_HEADER_LEN];   //升级包头
     FILE               *src_file;  //升级包暂存到/tmp目录下
     FILE               *upgrade_file;  //将要写到nand flash的包，其实和src_file 指向同一个
     int                timer_id;   //定时器id
     pthread_t          tid;        //线程id， 该线程用于把升级包写入nand flash
     void               *usr_arg;
     net_upgrade_cb     upgrade_cb;
}upgrade_man_s;

//升级包中的文件信息
typedef struct _UPGRADE_FILE_INFO_S_
{
     char           file_name[UPGRADE_FILE_NAME_LEN];
     unsigned int   part_no;        //uboot: oxf0f0f0f0...
     unsigned int   file_len;       //
     unsigned int   file_attr;      //文件属性
     unsigned char  res[116];       // ???
}upgrade_file_info_s;


/*=========================================================================*\
 * #function#                                                              *
\*=========================================================================*/


/*=====================================================================\
* Function   (名称): net_upgrade_open()
* Description(功能): 打开升级
* Called By  (被调): 1.
* Call_B file(被文): 1.
* Calls list (调用): 1.
* Calls file (调文): 1.
* Input      (输入): 1.
* Output     (输出): 1.
* Return     (返回):
*         success  :
*         failure  :
* Change log (修改): 1.
* Others     (其他): 1.
\*=====================================================================*/
int net_upgrade_open(net_upgrade_cb func, void *usrArg);



/*=====================================================================\
* Function   (名称): net_upgrade_write()
* Description(功能): 接收完数据，写数据到nandflase
* Called By  (被调): 1.
* Call_B file(被文): 1.
* Calls list (调用): 1.
* Calls file (调文): 1.
* Input      (输入): 1.
* Output     (输出): 1.
* Return     (返回):
*         success  :
*         failure  :
* Change log (修改): 1.
* Others     (其他): 1.
\*=====================================================================*/
int net_upgrade_write(char *data, int size);


/*=====================================================================\
* Function   (名称): upgrade_do()
* Description(功能): 写nand flash 执行函数
* Called By  (被调): 1.
* Call_B file(被文): 1.
* Calls list (调用): 1.
* Calls file (调文): 1.
* Input      (输入): 1.
* Output     (输出): 1.
* Return     (返回):
*         success  :
*         failure  :
* Change log (修改): 1.
* Others     (其他): 1.
\*=====================================================================*/
int upgrade_do(void);


/*=====================================================================\
* Function   (名称): net_upgrade_check()
* Description(功能): 进行一些检查，例如文件类型等
* Called By  (被调): 1.
* Call_B file(被文): 1.
* Calls list (调用): 1.
* Calls file (调文): 1.
* Input      (输入): 1.
* Output     (输出): 1.
* Return     (返回):
*         success  :
*         failure  :
* Change log (修改): 1.
* Others     (其他): 1.
\*=====================================================================*/
int net_upgrade_check(void);


/*=====================================================================\
* Function   (名称): net_upgrade_close()
* Description(功能): 关闭升级
* Called By  (被调): 1.
* Call_B file(被文): 1.
* Calls list (调用): 1.
* Calls file (调文): 1.
* Input      (输入): 1.
* Output     (输出): 1.
* Return     (返回):
*         success  :
*         failure  :
* Change log (修改): 1.
* Others     (其他): 1.
\*=====================================================================*/
int net_upgrade_close(void);


/*=====================================================================\
* Function   (名称): upgrade_get_file_size()
* Description(功能): 获取升级文件的大小
* Called By  (被调): 1.
* Call_B file(被文): 1.
* Calls list (调用): 1.
* Calls file (调文): 1.
* Input      (输入): 1.
* Output     (输出): 1.
* Return     (返回):
*         success  :
*         failure  :
* Change log (修改): 1.
* Others     (其他): 1.
\*=====================================================================*/
int upgrade_get_file_size(char *data, int recv_size);



/*=====================================================================\
* Function   (名称): upgrade_schedule()
* Description(功能): 更新升级进度
* Called By  (被调): 1.
* Call_B file(被文): 1.
* Calls list (调用): 1.
* Calls file (调文): 1.
* Input      (输入): 1.
* Output     (输出): 1.
* Return     (返回):
*         success  :
*         failure  :
* Change log (修改): 1.
* Others     (其他): 1.
\*=====================================================================*/
int upgrade_schedule(int write_size, int recv_size);


/*=====================================================================\
* Function   (名称): upgrade_task()
* Description(功能): 执行升级任务，主要是写入nand flash
* Called By  (被调): 1.
* Call_B file(被文): 1.
* Calls list (调用): 1.
* Calls file (调文): 1.
* Input      (输入): 1.
* Output     (输出): 1.
* Return     (返回):
*         success  :
*         failure  :
* Change log (修改): 1.
* Others     (其他): 1.
\*=====================================================================*/
void *upgrade_task(void *arg);



/*=====================================================================\
* Function   (名称): upgrade_file_open()
* Description(功能): 打开升级文件
* Called By  (被调): 1.
* Call_B file(被文): 1.
* Calls list (调用): 1.
* Calls file (调文): 1.
* Input      (输入): 1.
* Output     (输出): 1.
* Return     (返回):
*         success  :
*         failure  :
* Change log (修改): 1.
* Others     (其他): 1.
\*=====================================================================*/
int upgrade_file_open(char *file_name);


/*=====================================================================\
* Function   (名称): upgrade_file_close()
* Description(功能): 关闭升级文件
* Called By  (被调): 1.
* Call_B file(被文): 1.
* Calls list (调用): 1.
* Calls file (调文): 1.
* Input      (输入): 1.
* Output     (输出): 1.
* Return     (返回):
*         success  :
*         failure  :
* Change log (修改): 1.
* Others     (其他): 1.
\*=====================================================================*/
int upgrade_file_close(void);


/*=====================================================================\
* Function   (名称): upgrade_proc()
* Description(功能): 升级处理
* Called By  (被调): 1.
* Call_B file(被文): 1.
* Calls list (调用): 1.
* Calls file (调文): 1.
* Input      (输入): 1.
* Output     (输出): 1.
* Return     (返回):
*         success  :
*         failure  :
* Change log (修改): 1.
* Others     (其他): 1.
\*=====================================================================*/
int upgrade_proc(unsigned int total_len, unsigned int file_num);


/*=====================================================================\
* Function   (名称): upgrade_file()
* Description(功能): 升级rootfs的某个文件
* Called By  (被调): 1.
* Call_B file(被文): 1.
* Calls list (调用): 1.
* Calls file (调文): 1.
* Input      (输入): 1.
* Output     (输出): 1.
* Return     (返回):
*         success  :
*         failure  :
* Change log (修改): 1.
* Others     (其他): 1.
\*=====================================================================*/
int upgrade_file(upgrade_file_info_s *file_info, int file_len);



/*=====================================================================\
* Function   (名称): file_delete()
* Description(功能): 删除文件
* Called By  (被调): 1.
* Call_B file(被文): 1.
* Calls list (调用): 1.
* Calls file (调文): 1.
* Input      (输入): 1.
* Output     (输出): 1.
* Return     (返回):
*         success  :
*         failure  :
* Change log (修改): 1.
* Others     (其他): 1.
\*=====================================================================*/
int file_delete(char *file);


/*=====================================================================\
* Function   (名称): create_dir_by_filename()
* Description(功能): 通过文件名新建目录
* Called By  (被调): 1.
* Call_B file(被文): 1.
* Calls list (调用): 1.
* Calls file (调文): 1.
* Input      (输入): 1.
* Output     (输出): 1.
* Return     (返回):
*         success  :
*         failure  :
* Change log (修改): 1.
* Others     (其他): 1.
\*=====================================================================*/
int create_dir_by_filename(char *file);



/*=====================================================================\
* Function   (名称): create_dir()
* Description(功能): 新建文件夹
* Called By  (被调): 1.
* Call_B file(被文): 1.
* Calls list (调用): 1.
* Calls file (调文): 1.
* Input      (输入): 1.
* Output     (输出): 1.
* Return     (返回):
*         success  :
*         failure  :
* Change log (修改): 1.
* Others     (其他): 1.
\*=====================================================================*/
int create_dir(char *dir, mode_t mode);

int upgrade_part(upgrade_file_info_s *file_info, int file_len);
int upgrade_part_proc(char *part_name, int data_len);
int upgrade_erase_part(int fd, int data_len);
int upgrade_write_part(int fd, int data_len);

#ifdef __cplusplus
}
#endif

#endif

