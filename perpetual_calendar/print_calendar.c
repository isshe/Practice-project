/*==============================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : print_calendar.c
* Version      : v1.0.0
* Author       : 初代
* Date         : 2016/01/02
* Description  :
* Function list: 1.
*                2.
*                3.
* History      :
\*==============================================================================*/

#include <stdio.h>

//print_calendar
void print_calendar(int week, int day_limit)
{
    int i = 0;
    
//    printf("day_limit = %d\n", day_limit);
    printf("\n=================================================\n\n");
    printf("  Mon. Tues. wed.  Thur.  Fri.  Sat.  Sun.\n");
    for (i = 0; i < week; i++)
    {
        printf("      ");
    }
    
    for (i = 1; i < day_limit + 1; i++)
    {
        printf("  %2d  ", i);
        week++;
        if (week == 7)      //if week behind if , set 6;
        {
            week = 0;
            printf("\n");
        }
    }
    printf("\n==================================================\n");
}

//print_week
void print_week(int week)
{
    char *arr_week[] = {"Monday", "Tuesday", "Wednesday", "Thurday", 
                         "Friday", "Saturday","Sunday", };
                        
    printf("The day is %s!\n", arr_week[week]);
}
