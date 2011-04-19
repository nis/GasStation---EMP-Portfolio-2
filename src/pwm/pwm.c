/*****************************************************************************
* Odense University College of Enginerring
* Embedded C Programming (EMP)
*
* MODULENAME.: fan.c
*
* PROJECT....: EMP
*
* DESCRIPTION:
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 03192011  NS    Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/

#include "../inc/lm3s6965.h"
#include "../inc/binary.h"
#include "../inc/emp_type.h"

/*****************************    Defines    *******************************/

#define PWM_PERIOD 		12000
#define PWM_LOAD  		(PWM_PERIOD-1)

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

INT16U pwm_duty_cycle = 0;

/*****************************   Functions   *******************************/

void init_pwm(void)
/*****************************************************************************
*   Function : See h-file for specification.
*****************************************************************************/
{
	INT8U dummy;
	
	SYSCTL_RCGC0_R |= SYSCTL_RCC_USEPWMDIV; // Enable clock
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB;
	
	dummy = SYSCTL_RCGC2_R; // Dummy! Yay!
	
	
	GPIO_PORTB_AFSEL_R |= 0x03;	// Alt conf.
	GPIO_PORTB_DEN_R |= 0x03;
	
	SYSCTL_RCC_R |= SYSCTL_RCC_USEPWMDIV; // RCC uses PWM divide and divide by two
	SYSCTL_RCC_R &= ~SYSCTL_RCC_PWMDIV_M;
	SYSCTL_RCC_R |= SYSCTL_RCC_PWMDIV_2;
	
	PWM_1_CTL_R = 0;	// PWM countdown
	PWM_1_GENA_R |= 0x8C;
	PWM_1_GENB_R |= 0x80C;
	
	PWM_1_LOAD_R |= PWM_LOAD; // The period
	
	PWM_1_CMPA_R = 0; // Pulse width, PWM2 pin
	
	PWM_INVERT_R |= PWM_INVERT_PWM2INV;
	
	PWM_1_CTL_R |= 0x1; // Start PWM generator 1 timers
	
	PWM_ENABLE_R |= 0xC; // Enable outputs
}

void pwm_set_duty_cycle(INT16U dc)
/*****************************************************************************
*   Function : See h-file for specification.
*****************************************************************************/
{
	pwm_duty_cycle = (12000/100)*dc;
	
	if(pwm_duty_cycle > PWM_LOAD)
	{
		pwm_duty_cycle = PWM_LOAD - 1;
	}
}

void pwm_task(void)
/*****************************************************************************
*   Function : See h-file for specification.
*****************************************************************************/
{
	PWM_1_CMPA_R = pwm_duty_cycle;
}

/****************************** End Of Module *******************************/












