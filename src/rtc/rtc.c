/*****************************************************************************
* Odense University College of Enginerring
* Embedded C Programming (ECP)
*
* MODULENAME.: cpu.c
*
* PROJECT....: ECP
*
* DESCRIPTION:
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 050128  KA    Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include "inc/lm3s6965.h"
#include "../inc/emp_type.h"
#include "../inc/binary.h"
#include "rtc.h"

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

INT32U seconds = 0;

/*****************************   Functions   *******************************/

INT32U rtc_get_time(void)
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	return seconds;
}

void rtc_task(void)
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	if(seconds >= 86400)
	{
		seconds = 0;
	} else {
		seconds++;
	}
}

/****************************** End Of Module *******************************/
