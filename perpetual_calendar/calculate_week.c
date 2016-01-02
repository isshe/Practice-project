/*==============================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : calculate_week.c
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

int 
calculate_week(int year, int month, int day)
{

//     int cent = 0;
     int c = 0;
     int y = 0;
     int m = 0;
     int d = 0;
     int week = 0;
/*     注释掉的是泰勒公式计算的，因为有错（例如9999年）， 所以换成基姆拉尔森公式了
     cent = year/100;
     year %= 100;

     y = year + year/4;
     m = 13 * (month + 1) /5 ;
     c = cent/4 - 2* cent;
     d = day - 1;
*/
     m = 2*month + 3*(month+1)/5; 
     y = year + year/4 - year/100 + year/400;
     d = day;
     week = (c + y + m + d) % 7;
     
//     w = year + (year/4) + (cent/4) - 2*cent + (26*(month+1)/10) + day -1;

//     printf("cent = %d, year = %d, month = %d, day = %d, week = %d\n",
//             cent, year, month, day, week);
 
     return week;            
}
