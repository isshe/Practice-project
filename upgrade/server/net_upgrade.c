/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : net_upgrade.c
* Version      : v1.0.0
* Author       : 初代
* Date         : 2016/10/23
* Description  :
* Function list: 1.
*                2.
*                3.
* History      :
\*=========================================================================*/

/*-----------------------------------------------------------*
 * 头文件                                                    *
 *-----------------------------------------------------------*/
#include "net_upgrade.h"


static upgrade_man_s    g_upgrade_man = {0};

//这里有一个比较特别的define
//#define UPGRADE_FILE_HEADER_LEN     (sizeof(upgrade_file_header_s))


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/23
* Others     (其他): 定时器处理函数
\*=======================================================================*/
static void upgrade_timer_proc(int timer_id)
{
    if (g_upgrade_man.upgrade_cb != NULL)
    {
         if (g_upgrade_man.percent > 100)   //upgrade fail
         {
             //在这个程序中，upgrade_cb就是一个显示进度的函数(net_upgrade_report)
             g_upgrade_man.upgrade_cb((-1), g_upgrade_man.usr_arg);
         }
         else
         {
             g_upgrade_man.upgrade_cb(g_upgrade_man.percent,
                     g_upgrade_man.usr_arg);
         }
    }
}


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/23
* Others     (其他): 打开升级
\*=======================================================================*/
int net_upgrade_open(net_upgrade_cb func, void *usrArg)
{
     printf("upgrade begin\n");

     if ((g_upgrade_man.status != UPGRADE_IDLE)
             && (g_upgrade_man.status != UPGRADE_FINISH))
     {
         printf("upgrade is busy\n");
         return -1;
     }

     g_upgrade_man.status = UPGRADE_BUSY;

     //打开文件, wb和w+的区别??????
     if ((g_upgrade_man.upgrade_file = fopen(UPGRADE_FILE_NAME, "w+")) == NULL)
     {
          printf("open %s file error\n", UPGRADE_FILE_NAME);
          g_upgrade_man.status = UPGRADE_IDLE;
          return -1;
     }
     else
     {
         printf("open %s file success\n", UPGRADE_FILE_NAME);
     }

     g_upgrade_man.upgrade_cb   = func;
     g_upgrade_man.usr_arg      = usrArg;
     g_upgrade_man.total        = 0;
     g_upgrade_man.recv_size    = 0;
     g_upgrade_man.write_size   = 0;
     g_upgrade_man.timer_id     = timer_set(1000, upgrade_timer_proc);

     memset(g_upgrade_man.file_header, 0, sizeof(g_upgrade_man.file_header));

     printf("upgrade timer id %d\n", g_upgrade_man.timer_id);

     return 0;
}


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/23
* Others     (其他): 写接收到的数据
\*=======================================================================*/
int net_upgrade_write(char *data, int size)
{
     int        ret = 0;

     debug("upgrade write!!!\n");
     if (g_upgrade_man.upgrade_file == NULL)
     {
         printf("upgrade file is null\n");
         return -1;
     }

     //接收完升级文件，开始写nand flash
     if (size <= 0)
     {
          if (g_upgrade_man.upgrade_file != NULL)
          {
              fclose(g_upgrade_man.upgrade_file);
              g_upgrade_man.upgrade_file = NULL;
          }

          g_upgrade_man.recv_size = g_upgrade_man.total;
          if ((ret = upgrade_do()) != 0)
          {
               net_upgrade_close();
          }
     }
     else   //recv upgrade file to /tmp
     {
          if (g_upgrade_man.total == 0)
          {
               if (upgrade_get_file_size(data, size) < 0)
               {
                   printf("upgrade file get size fail\n");
                   return -1;
               }
          }

          if (size != (int)fwrite(data, 1, size, g_upgrade_man.upgrade_file))
          {
               printf("upgrade fwrite data error\n");
               return -1;
          }

          //进度
          upgrade_schedule(0, size);
          ret = 0;
     }

     return 0;
}



/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/23
* Others     (其他): 接收完升级文件，写nand flash
\*=======================================================================*/
int upgrade_do(void)
{
    debug("upgrade_do\n");
     if (net_upgrade_check() != 0)
     {
         printf("upgrade file check error\n");
         g_upgrade_man.percent = 101;   // 错误
         upgrade_timer_proc(0);         //这里timer id为什么是0
         return -1;
     }

     debug("upgrade check finish\n");

/*
     if (pthread_create(&g_upgrade_man.tid,
                 NULL, (void *)upgrade_task, (void *)NULL) != 0)
     {
         debug("pthread create error\n");
         return -1;
     }
*/
     return 0;
}


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/23
* Others     (其他): crc校验
\*=======================================================================*/
static int upgrade_check_crc(unsigned int crc)
{
     return 0;
}

/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/23
* Others     (其他): 检查文件头
\*=======================================================================*/
static int upgrade_check_header(char *data, int file_size)
{
    upgrade_file_header_s   *upgrade_header = (upgrade_file_header_s *)data;

    //检查机器类型
#ifdef ARCH_AT91
    if (upgrade_header->machine != UPGRADE_MACHINE_AT91)
#else
    if (upgrade_header->machine != UPGRADE_MACHINE_2440)
#endif
    {
         printf("upgrade machine type check fail 0x%x\n", upgrade_header->machine);
         return -1;
    }

    if (upgrade_header->magic != UPGRADE_MAGIC)
    {
        printf("upgrade magic error\n");
        return -1;
    }

    if (upgrade_header->total_len != file_size - UPGRADE_FILE_HEADER_LEN)
    {
         printf("upgrade file size error(%d <==> %ld)\n",
                 upgrade_header->total_len,
                 file_size - UPGRADE_FILE_HEADER_LEN);
         return -1;
    }

    return 0;
}


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/23
* Others     (其他): 升级检查
\*=======================================================================*/
int net_upgrade_check(void)
{
     int    file_size = 0;
     upgrade_file_header_s  upgrade_header;

     //这里的UPGRADE_FILE_NAME 就是/tmp/
     if ((g_upgrade_man.src_file = fopen(UPGRADE_FILE_NAME, "rb")) == NULL)
     {
         printf("upgrade file %s open error\n", UPGRADE_FILE_NAME);
         return -1;
     }

     //获取文件大小
     fseek(g_upgrade_man.src_file, 0L, SEEK_END);
     file_size = ftell(g_upgrade_man.src_file);
     printf("upgrade tmp file size: %d\n", file_size);

     fseek(g_upgrade_man.src_file, 0, SEEK_SET);

     //把头部读出来
     if (fread(&upgrade_header, 1, UPGRADE_FILE_HEADER_LEN,
                 g_upgrade_man.src_file) != UPGRADE_FILE_HEADER_LEN)
     {
          upgrade_file_close();
          return -1;
     }

     //检查头部
     if (upgrade_check_header((char *)&upgrade_header, file_size) != 0)
     {
          upgrade_file_close();
          return -1;
     }

     //crc 校验
     if (upgrade_check_crc(upgrade_header.crc32) != 0)
     {
          upgrade_file_close();
          return -1;
     }

     upgrade_file_close();
     return 0;
}



/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/24
* Others     (其他):
\*=======================================================================*/
int net_upgrade_close(void)
{
     if (g_upgrade_man.status == UPGRADE_WRITE)
     {
         printf("upgrade is writing nand flash\n");
         return 0;
     }

     if (g_upgrade_man.status == UPGRADE_IDLE)
     {
          printf("upgrade is no using\n");
          return 0;
     }

     if (g_upgrade_man.upgrade_file != NULL)
     {
         fclose(g_upgrade_man.upgrade_file);
         g_upgrade_man.upgrade_file = NULL;
     }

     timer_kill(g_upgrade_man.timer_id);
     g_upgrade_man.timer_id = -1;

     //没有烧写nand flash的操作，状态设置为IDLE
     if (g_upgrade_man.status != UPGRADE_FINISH)
     {
          g_upgrade_man.status = UPGRADE_IDLE;
     }

     printf("upgrade close finish\n");
     return 0;
}


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/24
* Others     (其他): 获取升级文件大小
\*=======================================================================*/
int upgrade_get_file_size(char *data, int recv_size)
{
     int    max_copy_size = 0;  //这次调用中最多需要复制的字节数
     int    need_copy_size = 0; //这次调用中需要复制的字节数

     //先接收一个包头大小的的数据，从中获取实际数据的大小。
     if ((recv_size <= 0) || (g_upgrade_man.recv_size >= UPGRADE_FILE_HEADER_LEN))
     {
         return -1;
     }

     //注释看原文件
     // 源代码中考虑的是可能多次才能接收完一个包头UPGRADE_FILE_HEADER_LEN,
     // 但是为什么不一次接收呢？
     max_copy_size = UPGRADE_FILE_HEADER_LEN - g_upgrade_man.recv_size;
     need_copy_size = max_copy_size > recv_size ? recv_size : max_copy_size;

     memcpy(&g_upgrade_man.file_header[g_upgrade_man.recv_size],
             data, need_copy_size);

     //包头接收完成
     if (need_copy_size >= max_copy_size)
     {
          upgrade_file_header_s *upgrade_header =
              (upgrade_file_header_s *)&g_upgrade_man.file_header[0];

          if (upgrade_header->magic != UPGRADE_MAGIC)
          {
              //升级包的幻数不一致， 不是正确的或合法的升级包
              return -1;
          }
          else
          {
               g_upgrade_man.total = upgrade_header->total_len + UPGRADE_FILE_HEADER_LEN;
               return g_upgrade_man.total > 0 ? 0 : (-1);
          }
     }

     return -1;
}


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/24
* Others     (其他): 更新升级进度
\*=======================================================================*/
int upgrade_schedule(int write_size, int recv_size)
{
     if (write_size < 0 || recv_size < 0)
     {
         return -1;
     }

     if (g_upgrade_man.total <= 0)
     {
          g_upgrade_man.recv_size += recv_size;
          g_upgrade_man.percent = 0;
     }
     else
     {
         g_upgrade_man.recv_size += recv_size;
         if (g_upgrade_man.recv_size > g_upgrade_man.total)
         {
              g_upgrade_man.recv_size = g_upgrade_man.total;
         }

         g_upgrade_man.write_size += write_size;
         if (g_upgrade_man.write_size > g_upgrade_man.total)
         {
             g_upgrade_man.write_size = g_upgrade_man.total;
         }

         //升级进度分配：20%接收升级包，80%写到nand flash
         g_upgrade_man.percent = g_upgrade_man.recv_size * 20 / g_upgrade_man.total
             + g_upgrade_man.write_size * 80 / g_upgrade_man.total;
     }

     printf("percent:%d\n", g_upgrade_man.percent);
     return 0;
}



/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/25
* Others     (其他): 执行升级任务
\*=======================================================================*/
void *upgrade_task(void *arg)
{
     upgrade_file_header_s  upgrade_header = {0};
     int                    ret = 0;

     //把/tmp 下的升级包写到nand flash中
     g_upgrade_man.status = UPGRADE_WRITE;

     if (upgrade_file_open(UPGRADE_FILE_NAME) != 0)
     {
          ret = -1;
     }
     else
     {
         fseek(g_upgrade_man.src_file, 0, SEEK_SET);    //文件首部
         if (fread(&upgrade_header, 1, UPGRADE_FILE_HEADER_LEN, g_upgrade_man.src_file)
                 != UPGRADE_FILE_HEADER_LEN)
         {
              printf("read file header error\n");
              ret = -1;
//              upgrade_file_close();
         }
         else
         {
              upgrade_schedule(UPGRADE_FILE_HEADER_LEN, 0);

              //烧写升级包到nand flash中
              ret = upgrade_proc(upgrade_header.total_len, upgrade_header.file_num);

        }
    }

     g_upgrade_man.percent = ret < 0 ? 101 : 100;

     if (g_upgrade_man.percent == 100)
     {
          printf("upgrade success\n");
          unlink(UPGRADE_STATUS_FILE);
     }
     else
     {
         printf("upgrade fail\n");
     }

     //升级结束
     g_upgrade_man.status = UPGRADE_FINISH;

     upgrade_file_close();
     net_upgrade_close();

     //reboot

     return NULL;
}


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/25
* Others     (其他): 打开升级文件
\*=======================================================================*/
int upgrade_file_open(char *file_name)
{
     if ((g_upgrade_man.src_file = fopen(file_name, "r")) == NULL)
     {
         printf("upgrade_file %s open error\n", file_name);
         return -1;
     }

     return 0;
}


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/25
* Others     (其他): 关闭升级文件
\*=======================================================================*/
int upgrade_file_close(void)
{
     if (g_upgrade_man.src_file != NULL)
     {
         fclose(g_upgrade_man.src_file);
         g_upgrade_man.src_file = NULL;
         return 0;
     }

     return -1;
}



/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/25
* Others     (其他): 升级处理
*                       total_len: 该升级包的有效数据总长度
*                       file_num : 该升级包包含的有效文件包的个数
\*=======================================================================*/
int upgrade_proc(unsigned int total_len, unsigned int file_num)
{
     upgrade_file_info_s        file_info;
//     int                        index = 0;
     int                        ret = 0;
     int                        i = 0;

     memset(&file_info, 0, sizeof(file_info));

     for (i = 0; i < file_num; i++)
     {
          //读升级包的每个有效的文件包的数据头
          if (fread(&file_info, 1, sizeof(upgrade_file_info_s), g_upgrade_man.src_file)
                  != sizeof(upgrade_file_info_s))
          {
              printf("fread file info error\n");
              return -1;
          }

          upgrade_schedule(sizeof(upgrade_file_info_s), 0);

          if (file_info.part_no == UPGRADE_FILE)
          {
               ret = upgrade_file(&file_info, file_info.file_len);
          }
          else
          {
              ret = upgrade_part(&file_info, file_info.file_len);
          }

          if (ret != 0)
          {
               return -1;
          }
     }

     return 0;
}



/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/25
* Others     (其他): 升级rootfs中的某个文件
\*=======================================================================*/
int upgrade_file(upgrade_file_info_s *file_info, int file_len)
{
     FILE   *file = NULL;
     char   data[UPGRADE_WRITE_BLOCK_LEN] = {0};
     int    read_size = 0;

     //为什么有删除???
     file_delete(UPGRADE_FILE_NAME);

     if (create_dir_by_filename(file_info->file_name) != 0)
     {
          printf("create dir error\n");
          return -1;
     }

     if ((file = fopen(file_info->file_name, "w+")) == NULL)
     {
         printf("open file %s error: %s\n", file_info->file_name, strerror(errno));
         return -1;
     }

     while(file_len > 0)
     {
          read_size = file_len > UPGRADE_PER_WRITE_LEN ? UPGRADE_PER_WRITE_LEN : file_len;
          if (fread(data, 1, read_size, g_upgrade_man.src_file) != read_size)
          {
              printf("read file data error: %s\n", strerror(errno));
              break;
          }

          if (fwrite(data, 1, read_size, file) != read_size)
          {
               printf("write file data error: %s\n", strerror(errno));
               break;
          }

          upgrade_schedule(read_size, 0);
          file_len -= read_size;
          usleep(5);    //???
     }

     fclose(file);
     file = NULL;

     if (file_len > 0)
     {
          printf("upgrade write file %s error\n", file_info->file_name);
          return -1;
     }

     printf("upgrade write file %s ok\n", file_info->file_name);

     if (chmod(file_info->file_name, 0755) != 0)
     {
          printf("upgrade file chmod error\n");
     }

     return 0;
}



/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/25
* Others     (其他): 删除文件
\*=======================================================================*/
int file_delete(char *file)
{
     if (file == NULL)
     {
         return -1;
     }

     chmod(file, 0x777);

     if (unlink(file) < 0)
     {
          printf("unlink file %s error %s\n", file, strerror(errno));
     }
     else
     {
         printf("unlink file %s ok\n", file);
     }
     return 0;
}



/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/25
* Others     (其他): 通过文件名创建文件夹
\*=======================================================================*/
int create_dir_by_filename(char *file)
{
     char   dir[UPGRADE_FILE_NAME_LEN] = {0};
     char   *begin = dir;
     char   *dir_ptr = NULL;
     int    len = 0;

     if (file == NULL)
     {
          return -1;
     }

     len = strlen(file);
     len = len < UPGRADE_FILE_NAME_LEN ? len : UPGRADE_FILE_NAME_LEN;

     memcpy(dir, file, len);
     dir_ptr = strrchr(dir, '/');

     if ((dir_ptr != NULL) && (dir_ptr != begin))
     {
          *dir_ptr = '\0';

          if (create_dir(dir, 0777) != 0)
          {
              return -1;
          }
          else
          {
               return 0;
          }
     }

     return -1;
}



/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/25
* Others     (其他): 创建目录, 这个程序不理解
\*=======================================================================*/
int create_dir(char *dir, mode_t mode)
{
     char   sz_dir[UPGRADE_FILE_NAME_LEN] = {0};
     char   *begin = sz_dir;
     char   *end = NULL;

     strncpy(sz_dir, dir, UPGRADE_FILE_NAME_LEN -1);
     begin = strstr(sz_dir, "/");

     // ???
     if (begin != NULL && begin + 1 != NULL
             && (end = strstr(begin + 1, "/")) != NULL)
     {
          *end = '\0';
     }

     while(begin)
     {
         //检查文件权限以及创建目录
         if (access(begin, F_OK) != 0
                 && mkdir(begin, mode) != 0)
         {
              return -1;
         }

         if (end == NULL || end + 1 == NULL)
         {
             break;
         }

         *end = '/';

         if ((end = strstr(end + 1, "/")) != NULL)
         {
              *end = '\0';
         }
     }

     return 0;
}



/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/25
* Others     (其他): 升级kernel uboot之类的
\*=======================================================================*/
int upgrade_part(upgrade_file_info_s *file_info, int file_len)
{
     char   dev[32] = {0};

     switch(file_info->part_no)     //升级类型 uboot，kernel等
     {
         case UPGRADE_UBOOT_PART:
             strcpy(dev, UPGRADE_UBOOT_MTD);
             break;
         case UPGRADE_KERNEL_PART:
             strcpy(dev, UPGRADE_KERNEL_MTD);
             break;
         case UPGRADE_ROOTFS_PART:
             strcpy(dev, UPGRADE_ROOTFS_MTD);
             break;
         case UPGRADE_USRFS_PART:
             strcpy(dev, UPGRADE_USRFS_MTD);
             break;
         default:
             printf("upgrade part no error");
             return -1;
     }

     return upgrade_part_proc(dev, file_len);
}



/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/25
* Others     (其他): 部分升级函数，可升级uboot，kernel等
\*=======================================================================*/
int upgrade_part_proc(char *part_name, int data_len)
{
     int fd = 0;

     printf("upgrade part open %s\n", part_name);
     if ((fd = flash_open(part_name)) < 0)
     {
         printf("open flash %s error %s\n", part_name, strerror(errno));
         return -1;
     }

     if ((upgrade_erase_part(fd, data_len) < 0)
             || (upgrade_write_part(fd, data_len) < 0))
     {
          flash_close(fd);
          return -1;
     }

     printf("upgrade part %s success\n", part_name);
     flash_close(fd);

     return 0;
}


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/25
* Others     (其他): 擦除相关部分
\*=======================================================================*/
int upgrade_erase_part(int fd, int data_len)
{
     int    part_len = flash_mtd_size(fd);
     int    i = 0;
     int    offset = 0;
     int    block_num = part_len / UPGRADE_WRITE_BLOCK_LEN;     // ???

     printf("upgrade part erase size %d\n", part_len);

     for (i = 0; i < block_num; i++)
     {
          if (flash_erase(fd, i * UPGRADE_WRITE_BLOCK_LEN + offset,
                      UPGRADE_WRITE_BLOCK_LEN) < 0)
          {
              flash_close(fd);
              return -1;
          }

          upgrade_schedule((data_len > 1) / block_num, 0);
          usleep(5);
     }

     return 0;
}



/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/25
* Others     (其他): 写刚刚擦除的部分
\*=======================================================================*/
int upgrade_write_part(int fd, int data_len)
{
     int    ret = 0;
     int    read_len = 0;
     int    write_len = 0;
     int    write_num = 0;
     int    i = 0;
     int    j = 0;
     char   data[UPGRADE_WRITE_BLOCK_LEN] = {0};

     for (i = 0; data_len > 0; i++)
     {
          read_len = data_len > UPGRADE_WRITE_BLOCK_LEN ? UPGRADE_WRITE_BLOCK_LEN : data_len;

          if (fread(data, 1, data_len, g_upgrade_man.src_file) != read_len)
          {
              printf("read_file data error\n");
              return -1;
          }

          printf("read file %d\n", read_len);

          //???
          write_num = (read_len + UPGRADE_PER_WRITE_LEN - 1) / UPGRADE_PER_WRITE_LEN;

          for (j = 0; j < write_num; j++)
          {
               write_len = (read_len - j * UPGRADE_PER_WRITE_LEN) >= UPGRADE_PER_WRITE_LEN ?
                   UPGRADE_PER_WRITE_LEN : (read_len - j * UPGRADE_PER_WRITE_LEN);

               printf("write flash len %d\n", write_len);

               if ((ret = flash_write(fd,
                               i * UPGRADE_WRITE_BLOCK_LEN + j * UPGRADE_PER_WRITE_LEN,
                               &data[j * UPGRADE_PER_WRITE_LEN],
                               UPGRADE_PER_WRITE_LEN)) < 0)
               {
                    printf("write part error(%d, %p) %d, %s\n",
                            ret, &data[j * UPGRADE_PER_WRITE_LEN],
                            errno, strerror(errno));
               }
          }

          data_len -= read_len;
          upgrade_schedule(read_len >> 1, 0);
     }

     return 0;
}



