/*****************************************************************************
* Odense University College of Enginerring
* Embedded C Programming (ECP)
*
* MODULENAME.: led.h
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
* 050128  NS    Module created.
*
*****************************************************************************/

#ifndef _KEYBOARD_H
  #define _KEYBOARD_H

/***************************** Include files *******************************/
/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/

void init_keyboard(void);
/*****************************************************************************
*   Function : Initiates and sets up the ports and pins for the keyboard.
*****************************************************************************/

void keyboard_task( void );
/*****************************************************************************
*   Function : The task for the keyboard.
*****************************************************************************/

/****************************** End Of Module *******************************/
#endif
