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
#include "../pwm/pwm.h"

/*****************************    Defines    *******************************/

#define MAX_SPEED  100
#define MIN_SPEED  0
#define RPM_CALCULATION_INTERVAL  50 // How many ticks should there be between calcualtion of the RPM?

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

INT8U ref_speed = MIN_SPEED; 	// Speed of the fan. From 0 to 100
INT16U fan_current = 0;			// Current in the fan in mA
INT16U fan_rpm = 0;				// Rounds Per Minute for the fan
INT16U fan_encoder_counter = 0;	// Counter for the interruptroutine

/*****************************   Functions   *******************************/

INT16U fan_get_rpm()
/*****************************************************************************
*   Function : See h-file for specification.
*****************************************************************************/
{
	return fan_rpm;
}

INT16U fan_get_current()
/*****************************************************************************
*   Function : See h-file for specification.
*****************************************************************************/
{
	return fan_current;
}

INT8U fan_get_ref_speed()
/*****************************************************************************
*   Function : See h-file for specification.
*****************************************************************************/
{
	return ref_speed;
}

void fan_task(void)
/*****************************************************************************
*   Function : See h-file for specification.
*****************************************************************************/
{
	// Set speed if changed
	static old_speed;
	if(old_speed != ref_speed)
	{
		pwm_set_duty_cycle(ref_speed);
	}
	old_speed = ref_speed;
	
	// Read the current
	if (ADC_RIS_R && ADC_RIS_INR2) {
		INT16U data = (0x3FF & ADC_SSFIFO2_R);
		
		ADC_ISC_R |= ADC_ISC_IN2;
		
		fan_current = data/4; // Ballpark calculation
		
		fan_current = (12*ref_speed/100)*fan_current;
		
		ADC_PSSI_R |= ADC_PSSI_SS2; // Enable the ADC for next time
	}
	
	// Calculate RPM
	static INT8U rpm_calculation = RPM_CALCULATION_INTERVAL;
	if(rpm_calculation == 0)
	{
		fan_rpm = (fan_encoder_counter/2)*(1000/(RPM_CALCULATION_INTERVAL*10))*60; // RPM Calculation
		fan_encoder_counter = 0;
		rpm_calculation = RPM_CALCULATION_INTERVAL;
	} else {
		rpm_calculation--;
	}
}

void portd_isr(void) 
/*****************************************************************************
*   Function : 	PORTD interupt routine
******************************************************************************/
{
	fan_encoder_counter++;
	GPIO_PORTD_ICR_R |= 0b10000000; // Clear interrupts on PORTD
}

void init_fan(void)
/*****************************************************************************
*   Function : See h-file for specification.
*****************************************************************************/
{
	// Init the ADC to check the current through the fan.

	SYSCTL_RCGC0_R |= SYSCTL_RCGC0_ADC; // ADC enabled.

	INT8U dummy = SYSCTL_RCGC0_R; // Dummy! Yay!

	ADC_ACTSS_R &= ~ADC_ACTSS_ASEN2; // Disable sequencer two.

	ADC_SSMUX2_R |= 4;

	ADC_SSCTL2_R |=  ADC_SSCTL2_IE0 | ADC_SSCTL2_END0;
	
	ADC_ACTSS_R |= ADC_ACTSS_ASEN2;

	ADC_PSSI_R |= ADC_PSSI_SS2|ADC_PSSI_SS3;
	
	// Setup PORTD
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOD; // Enable PORTD

	dummy = SYSCTL_RCGC2_R; // Dummy! Yay!
 
	GPIO_PORTD_DIR_R&= ~(0b10000000); // Direction

	GPIO_PORTD_DEN_R |= 0b10000000; // Digital enable

	GPIO_PORTD_ODR_R &= ~(0b10000000); // No open drain

	GPIO_PORTD_AFSEL_R &= ~(0b10000000); // No alternative function
	
	// Init PORTD interrupt for the encoder
	GPIO_PORTD_IS_R &= ~(0b10000000);
	
	GPIO_PORTD_IBE_R &= ~(0b10000000);
	
	GPIO_PORTD_IEV_R |= 0b10000000;
	
	GPIO_PORTD_ICR_R |= 0b10000000; // Clear interrupts on PORTD
	
	// NVIC setup
	// program NVIC, vector number 19, Interrupt Number = 3
	// Clear pending interrupt
	NVIC_UNPEND0_R |= NVIC_UNPEND0_INT3;
	// Set Priority to 0x10, first clear then set. 
	NVIC_PRI0_R &= ~(NVIC_PRI0_INT3_M);
	NVIC_PRI0_R |= (NVIC_PRI0_INT3_M & (0x10<<NVIC_PRI0_INT3_S));
	NVIC_EN0_R |= NVIC_EN0_INT3;	// enable NVIC interrupt
	
	GPIO_PORTD_IM_R |= 0b10000000; // Enable PORTD interrupts
}

void fan_set_speed( INT8U ds )
/*****************************************************************************
*   Function : See h-file for specification.
*****************************************************************************/
{
	if(ds <= MAX_SPEED && ds >= MIN_SPEED)
	{
		ref_speed = ds;
	}
}

void fan_speed_up( INT8U ds )
/*****************************************************************************
*   Function : See h-file for specification.
*****************************************************************************/
{
	if( ref_speed > (MAX_SPEED - ds))
	{
		ref_speed = MAX_SPEED;
	} else {
		ref_speed = ref_speed + ds;
	}
}

void fan_speed_down( INT8U ds )
/*****************************************************************************
*   Function : See h-file for specification.
*****************************************************************************/
{
	if( ref_speed < ds)
	{
		ref_speed = MIN_SPEED;
	} else {
		ref_speed = ref_speed - ds;
	}
}
/****************************** End Of Module *******************************/












