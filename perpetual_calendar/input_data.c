/*==============================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : input_data.c
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

//前三个都是值结果参数， 因为都要返回给调用函数！！！
int input_data(int *r_year, int *r_month, int *r_day, int select_fun)
{
     int year = 0;
     int month = 0;
     int day = 0;
     int bissextile = 0;
     int day_limit = 0;
     int scanf_ret = 0;
     
     while (1)
     {
          printf("please enter the year：");
          scanf_ret = scanf("%d", &year);
//          printf("scanf ret = %d\n", scanf_ret);
          while(getchar() != '\n');
//          rewind(stdin);          
          if (year < 0 || year > 30000)
          {
               printf("enter error!\n");
               continue;
          }
          if ((year%4==0 && year%100 != 0) || (year%400==0))
          {
               bissextile = 1;
          }

          printf("please enter the month: ");
          scanf_ret = scanf("%d" , &month);
//          printf("scanf ret = %d\n", scanf_ret);         
          while(getchar() != '\n'); 
//          rewind(stdin);
                   
          if (month <1 || month >12)
          {
               printf("enter error!\n");
               continue;
          }

          if (select_fun == '1') //without intput day. care '1'!!!
          {
              day = 1;
          }
          else                  //func = 2
          {
              printf("please enter the day: ");
              scanf_ret = scanf("%d", &day);
//              printf("scanf ret = %d\n", scanf_ret);
              while(getchar() != '\n');
          }
          
 /*             if (day < 1 || day > 31)
              {
                   printf("enter error!\n");
                   continue;
              }
*/
              if (bissextile == 1 && month == 2)
              {
                   day_limit = 29;
                   
                   if (day < 1 || day > 29)
                   {
                       printf("enter error!\n");
                       continue;
                   }
              }
              else if (bissextile == 0 && month == 2)
              {
                   day_limit = 28;
                   
                   if (day < 1 || day > 28)
                   {
                       printf("enter error!\n");
                       continue;
                   }
              }
              else if (month ==4 || month == 6
                        || month == 9 || month == 11)
              {
                   day_limit = 30;
                   if (day < 1 || day > 30)
                   {
                       printf("enter error!\n");
                       continue;
                   }
              }
              else              
              {
                    day_limit = 31; 
                    if (day < 1 || day > 31)
                    {
                         printf("enter error!\n");
                         continue;
                    }
              }
          

          break;
     }


     if (month <= 2)
     {
          year--;
          month += 12;
     }

    *r_year = year;
    *r_month = month;
    *r_day = day;
    
    return day_limit;
}
