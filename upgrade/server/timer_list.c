/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : timer_list.c
* Version      : v1.0.0
* Author       : 初代
* Date         : 2016/10/21
* Description  :
* Function list: 1.
*                2.
*                3.
* History      :
\*=========================================================================*/

/*-----------------------------------------------------------*
 * 头文件                                                    *
 *-----------------------------------------------------------*/

#include "timer_list.h"

static timer_list_t     timer_list;



/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/21
* Others     (其他): 默认的定时器处理函数
\*=======================================================================*/
static void default_timer_proc(int32_t event_id)
{
    //
    return ;
}

/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/21
* Others     (其他): 初始化定时器列表
\*=======================================================================*/
int32_t timer_list_init(void)
{
     memset(&timer_list, 0, sizeof(timer_list_t));

     if (timer_list_start(100, default_timer_proc) < 0)
     {
          debug("init timer list error\n");
          return -1;
     }

     timer_list.init = 1;       //已经初始化
     debug("timer list init ok\n");

     return 1;
}



/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/21
* Others     (其他):
\*=======================================================================*/
static void timer_signal_proc(int32_t signal)
{
     int i = 0;

     //定时器终端函数连续被阻塞5次，则认为系统被阻塞
     if (timer_list.run_out_time_cnt++ > 5)
     {
         if (timer_list.run_out_err_flag == 0)
         {
             debug("System Function run timeout with %d times\n",
                     timer_list.run_out_time_cnt - 1);
         }
         timer_list.run_out_err_flag = 1;

         //reboot system
         // return
     }

     for(; i < TIMER_LIST_MAX; i++)
     {
          //如果定时器无效则继续下一次测试
          if (timer_list.timer_arr[i].valid != 1)
          {
              continue;
          }

          //来到这里代表定时器有效
          //疑问：elaspe是什么
          timer_list.run_index = i;         //注意是i
          if ((++timer_list.timer_arr[i].elaspe) >= timer_list.timer_arr[i].interval)
          {
               //注意看这些在哪里设置
               if (timer_list.timer_arr[i].proc != NULL)
               {
                   timer_list.timer_arr[i].proc(i);
               }
               else if (timer_list.default_timer_proc != NULL)
               {
                    timer_list.default_timer_proc(i);
               }


               timer_list.timer_arr[i].elaspe = 0;
          }
     }

     timer_list.run_index = ~0;         //!!!
     timer_list.run_out_time_cnt--;     //这些地方不用锁吗?

}

/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/21
* Others     (其他): 开始定时器列表
\*=======================================================================*/
int32_t timer_list_start(uint32_t grain, timer_proc default_timer_proc)
{
     struct itimerval   itv;

     memset(&timer_list, 0, sizeof(timer_list_t));

     pthread_rwlock_init(&timer_list.rw_lock, NULL);
     pthread_rwlock_wrlock(&timer_list.rw_lock);

     timer_list.grain = grain;      //
     timer_list.default_timer_proc = default_timer_proc;

     pthread_rwlock_unlock(&timer_list.rw_lock);

     //设置signal
     if ((timer_list.old_sig_fun = signal(SIGALRM, timer_signal_proc)) == SIG_ERR)
     {
          pthread_rwlock_destroy(&timer_list.rw_lock);
          debug("timer signal error\n");
          return -1;
     }

     //设置itimerval结构
     memset(&itv, 0, sizeof(struct itimerval));

     itv.it_interval.tv_sec     = grain/1000;        //
     itv.it_interval.tv_usec    = (grain % 1000) /1000;
     itv.it_value.tv_sec        = 0;
     itv.it_value.tv_usec       = 1;

     if (0 != setitimer(ITIMER_REAL, &itv, &timer_list.old_itv))
     {
          pthread_rwlock_destroy(&timer_list.rw_lock);
          debug("timer setitimer error\n");
          return -1;
     }

     pthread_rwlock_wrlock(&timer_list.rw_lock);
     timer_list.init = 1;
     pthread_rwlock_unlock(&timer_list.rw_lock);

     return 0;
}


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/23
* Others     (其他): 设置定时器，返回一个timer id
\*=======================================================================*/
int32_t timer_set(uint32_t elaspe, timer_proc proc)
{
     int32_t        event_id = 0;

     if ((timer_list.init != 1)
             || (elaspe < timer_list.grain)
             || (event_id = timer_get_id()) < 0)
     {
         debug("timer set error\n");
         return -1;
     }

     pthread_rwlock_wrlock(&timer_list.rw_lock);

     //这块不懂!!!!
     timer_list.timer_arr[event_id].elaspe      = 0;
     timer_list.timer_arr[event_id].proc        = proc;
     timer_list.timer_arr[event_id].interval    =
         (elaspe + (timer_list.grain >> 1)) / timer_list.grain;
     timer_list.timer_arr[event_id].valid = 1;

     pthread_rwlock_unlock(&timer_list.rw_lock);

     return event_id;
}


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/23
* Others     (其他): 获取定时器id
\*=======================================================================*/
int32_t timer_get_id(void)
{
     int32_t    i = 0;

     if (timer_list.init != 1)
     {
         return -1;
     }

     pthread_rwlock_wrlock(&timer_list.rw_lock);

     //获取一个没有在用的index
     for (i = 0; i < TIMER_LIST_MAX; i++)
     {
          if (timer_list.timer_arr[i].valid != 1)
          {
              pthread_rwlock_unlock(&timer_list.rw_lock);
              return i;
          }
     }

     pthread_rwlock_unlock(&timer_list.rw_lock);

     return -1;
}


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/24
* Others     (其他):
\*=======================================================================*/
int32_t timer_kill(int32_t event_id)
{
     if ((timer_list.init == 1)
             && (event_id >= 0)
             && (event_id < TIMER_LIST_MAX))
     {
         pthread_rwlock_wrlock(&timer_list.rw_lock);
         timer_list.timer_arr[event_id].elaspe = 0;
         timer_list.timer_arr[event_id].interval = 0;
         timer_list.timer_arr[event_id].proc = NULL;
         timer_list.timer_arr[event_id].valid = 0;
         pthread_rwlock_unlock(&timer_list.rw_lock);
     }

     return 0;
}
