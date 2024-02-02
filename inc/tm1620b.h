#ifndef INC_TM1620B_H_
#define INC_TM1620B_H_
#include "tim.h"

typedef enum{
	TO_LOW,
	TO_HIGH
}e_toggle_state;

typedef enum{
	PW_CLK=1,
	PW_STB=1,
	T_CLKSTB=1,
	T_SETUP=1,
	T_HOLD=1
}e_TM1620B_delay_us;

typedef enum{ //counting from right  to the left
	DISPLAY_1 = 1,
	DISPLAY_2,
	DISPLAY_3,
	DISPLAY_4,
	DISPLAY_5,
	DISPLAY_6
}e_TM1620B_displayNum;

typedef enum{
	DISPLAY_MODE = 0x02, // Set display mode to 6 digits 7 segments
	ADDR_TYPE 	 = 0x44, // use a fixed address
	BRIGHTNESS 	 = 0x8F  // display control max brightness
}e_command;

void TM1620B_writeToDisplay(e_TM1620B_displayNum disp_num,uint8_t data);
void TM1620B_clear_all(void);
void TM1620B_init(void);
void Num_PrintOut(uint32_t num);
void standby_effect(void);
void print_array(uint8_t chArr[]);
#endif /* INC_TM1620B_H_ */
