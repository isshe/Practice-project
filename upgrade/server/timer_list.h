/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : timer_list.h
* Version      : v1.0.0
* Author       : i.sshe
* Github       : github.com/isshe
* Date         : 2016/10/21
* Description  :
* Function list: 1.
*                2.
*                3.
* History      :
\*=========================================================================*/

#ifndef _TIMER_LIST_H_
#define _TIMER_LIST_H_

#ifdef __cplusplus
extern "C"{
#endif

/*=========================================================================*\
 * #include#                                                               *
\*=========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdint.h>
#include <signal.h>
#include <sys/time.h>
#include <pthread.h>

#include "debug.h"

/*=========================================================================*\
 * #define#                                                                *
\*=========================================================================*/
#define TIMER_LIST_MAX      64
typedef void (*sig_func)(int32_t signal);
typedef void (*timer_proc)(int32_t event_id);

/*=========================================================================*\
 * #enum#                                                                  *
\*=========================================================================*/

/*=========================================================================*\
 * #struct#                                                                *
\*=========================================================================*/

typedef struct _timer_s_
{
    uint8_t         valid;          //标志定时器是否有效
    uint8_t         res[3];         //保留???
    uint32_t        interval;
    uint32_t        elaspe;
    timer_proc      proc;
}timer_s;

typedef struct _timer_list_t_
{
     uint8_t        init;
     uint8_t        max_timer_no;
     uint8_t        run_out_time_cnt;       //计算定时器连续被阻塞的次数
     uint8_t        run_out_err_flag;
     uint32_t       grain;
     uint32_t       run_index;
     pthread_rwlock_t   rw_lock;
     struct itimerval   old_itv;
     sig_func       old_sig_fun;
     timer_proc     default_timer_proc;
     timer_s        timer_arr[TIMER_LIST_MAX];
}timer_list_t;


/*=========================================================================*\
 * #function#                                                              *
\*=========================================================================*/


/*=====================================================================\
* Function   (名称): timer_list_init()
* Description(功能): 初始化定时器列表
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
int32_t timer_list_init(void);



/*=====================================================================\
* Function   (名称): timer_list_start()
* Description(功能):
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
int32_t timer_list_start(uint32_t grain, timer_proc default_timer_proc);


/*=====================================================================\
* Function   (名称): timer_set()
* Description(功能): 设置一个timer,并返回timer id
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
int32_t timer_set(uint32_t elaspe, timer_proc proc);


/*=====================================================================\
* Function   (名称): timer_get_id()
* Description(功能): 获取一个没有在使用的timer id, 接下来使用
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
int32_t timer_get_id(void);



/*=====================================================================\
* Function   (名称): timer_kill()
* Description(功能):
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
int32_t timer_kill(int32_t event_id);

#ifdef __cplusplus
}
#endif

#endif

