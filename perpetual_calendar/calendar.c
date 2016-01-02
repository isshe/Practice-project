/*==============================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : calendar.c
* Version      : v1.0.0
* Author       : 初代
* Date         : 2016/01/02
* Description  : 实现两个功能： 查询星期几；查询特定月份。
* Function list: 1.
*                2.
*                3.
* History      :
\*==============================================================================*/

#include "input_data.h"
#include "calculate_week.h"
#include "print_calendar.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int
main(void)
{
     int year = 0;
     int month = 0;
     int day = 0;
     int week = 0;
     int select_fun;
     int day_limit = 0;
    
     while (1)
     {
         printf("1.查询月份\n");
         printf("2.查询星期几\n");
         printf("3.exit\n");
         printf("please enter: ");
         select_fun = getc(stdin);

         switch (select_fun)
         {
             case '1':
                 //input data
                 day_limit = input_data(&year, &month, &day, select_fun);
                 
                 //calculate week
                 week = calculate_week(year, month, day);
                 
                 //output data
                 print_calendar(week, day_limit);
                 fflush(stdin);
                 fflush(stdout);

                 break;

             case '2':
                 //input data
                 day_limit = input_data(&year, &month, &day, select_fun);
                 
                 //calculate week
                 week = calculate_week(year, month, day);
                 
                 //output data
                 print_week(week);
                 
                 break;
             
             case '3':
                 exit(0);

             default:
                 printf("select func error!\n");
                 continue;                  ////////
         }


    }

     exit(0);


}
