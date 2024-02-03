#include "tm1620b.h"
#include "stdint.h"
#include "string.h"

#define USE_SPI	//switch between driving the displays through spi and gpio

					/* 0 	 1	  2	  3    4    5    6    7    8    9*/
uint8_t digits[10] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};
						/* привет */
uint8_t on_str[6] = {0x37,0x73,0x3E,0x7F,0x79,0x78};
						/* пока */
uint8_t off_str[4] = {0x37,0x3F,0x76,0x77};

extern SPI_HandleTypeDef hspi1;

static void TM1620B_config(void);
static void TM1620B_STB_toggle(e_toggle_state state);
static void TM1620B_STB_trobeGen(void);
static void TM1620B_8b_data_send(uint8_t data);
static void TM1620B_16b_data_send(uint8_t firstB,uint8_t secondB);
static void TM1620B_delay(e_TM1620B_delay_us time_us);
static uint8_t u32_to_displayCh(uint8_t digit);

#ifndef USE_SPI
static void data_bit_is(uint8_t state);
static void TM1620B_clk_1_pulseGen(void);
static void TM1620B_clk_toggle(e_toggle_state state);
#endif

static void TM1620B_delay(e_TM1620B_delay_us time_us){
	__HAL_TIM_SET_COUNTER(&htim1,0);
	while(__HAL_TIM_GET_COUNTER(&htim1) < time_us);
}

 void TM1620B_init(void){
	 TM1620B_config();
	 print_array(on_str);
	 HAL_Delay(1000);
 }

 static void TM1620B_config(void){
	TM1620B_8b_data_send((uint8_t)DISPLAY_MODE);
	TM1620B_delay(T_CLKSTB);
	TM1620B_STB_trobeGen();
	TM1620B_8b_data_send((uint8_t)ADDR_TYPE);
	TM1620B_delay(T_CLKSTB);
	TM1620B_STB_trobeGen();
 }

 void TM1620B_clear_all(void){
	 for(uint8_t num = 1;num < 7;num++)
		 TM1620B_writeToDisplay(num,0x00);
 }


void TM1620B_STB_toggle(e_toggle_state state){
	switch(state){
		case TO_LOW: HAL_GPIO_WritePin(STB_GPIO_Port, STB_Pin, GPIO_PIN_RESET);
			break;
		case TO_HIGH: HAL_GPIO_WritePin(STB_GPIO_Port, STB_Pin, GPIO_PIN_SET);
			break;
		default:
			break;
	}
}

#ifndef USE_SPI
void TM1620B_clk_toggle(e_toggle_state state){
	switch(state){
		case TO_LOW: HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin, GPIO_PIN_RESET);
			break;
		case TO_HIGH: HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin, GPIO_PIN_SET);
			break;
		default:
			break;
	}
}

static void TM1620B_clk_1_pulseGen(void){
	TM1620B_clk_toggle(TO_LOW);
	TM1620B_delay(PW_CLK);
	TM1620B_clk_toggle(TO_HIGH);
	TM1620B_delay(PW_CLK);
}

static void data_bit_is(uint8_t state){
	switch(state){
		case 0: HAL_GPIO_WritePin(Data_GPIO_Port, Data_Pin, GPIO_PIN_RESET);
			break;
		case 1: HAL_GPIO_WritePin(Data_GPIO_Port, Data_Pin, GPIO_PIN_SET);
			break;
		default:
			break;
	}
}
#endif

static void TM1620B_STB_trobeGen(void){
	TM1620B_STB_toggle(TO_HIGH);
	TM1620B_delay(PW_STB);
	TM1620B_STB_toggle(TO_LOW);
}

static void TM1620B_8b_data_send(uint8_t data){
	TM1620B_STB_toggle(TO_LOW);
#ifdef USE_SPI
	HAL_SPI_Transmit_DMA(&hspi1, &data, 1);
#else
	uint8_t pos;
	for(pos=0;pos<8;pos++){
		TM1620B_clk_toggle(TO_LOW);
		if((data>>pos) & 0x01)
			data_bit_is(1);
		else
			data_bit_is(0);
		TM1620B_delay(PW_CLK);
		TM1620B_clk_toggle(TO_HIGH);
	}
#endif
}

static void TM1620B_16b_data_send(uint8_t firstB,uint8_t secondB){
	TM1620B_STB_toggle(TO_LOW);
#ifdef USE_SPI
	HAL_SPI_Transmit_DMA(&hspi1, &firstB, 1);
	HAL_SPI_Transmit_DMA(&hspi1, &secondB, 1);
#else
	uint8_t pos;
	for(pos=0;pos<8;pos++){
		if((firstB>>pos) & 1)
			data_bit_is(1);
		else
			data_bit_is(0);
		TM1620B_clk_1_pulseGen();
	}
	for(pos=0;pos<8;pos++){
			if((secondB>>pos) & 1)
				data_bit_is(1);
			else
				data_bit_is(0);
			TM1620B_clk_1_pulseGen();
	}
#endif
}


void TM1620B_writeToDisplay(e_TM1620B_displayNum disp_num,uint8_t data){
	switch(disp_num){
		case DISPLAY_1:
			TM1620B_16b_data_send(0xC0,data);
			TM1620B_delay(T_CLKSTB);
			TM1620B_STB_trobeGen();
			break;
		case DISPLAY_2:
			TM1620B_16b_data_send(0xC2,data);
			TM1620B_delay(T_CLKSTB);
			TM1620B_STB_trobeGen();
			break;
		case DISPLAY_3:
			TM1620B_16b_data_send(0xC4,data);
			TM1620B_delay(T_CLKSTB);
			TM1620B_STB_trobeGen();
			break;
		case DISPLAY_4:
			TM1620B_16b_data_send(0xC6,data);
			TM1620B_delay(T_CLKSTB);
			TM1620B_STB_trobeGen();
			break;
		case DISPLAY_5:
			TM1620B_16b_data_send(0xC8,data);
			TM1620B_delay(T_CLKSTB);
			TM1620B_STB_trobeGen();
			break;
		case DISPLAY_6:
			TM1620B_16b_data_send(0xCA,data);
			TM1620B_delay(T_CLKSTB);
			TM1620B_STB_trobeGen();
			break;
	}

	TM1620B_8b_data_send(BRIGHTNESS);

	TM1620B_delay(T_CLKSTB);

	TM1620B_STB_toggle(TO_HIGH);
}

void Num_PrintOut(uint32_t num){
	TM1620B_writeToDisplay(DISPLAY_1,u32_to_displayCh(num%10));
	TM1620B_writeToDisplay(DISPLAY_2,u32_to_displayCh((num/10)%10));
	TM1620B_writeToDisplay(DISPLAY_3,u32_to_displayCh((num/100)%10));
	TM1620B_writeToDisplay(DISPLAY_4,u32_to_displayCh((num/1000)%10));
	TM1620B_writeToDisplay(DISPLAY_5,u32_to_displayCh((num/10000)%10));
	TM1620B_writeToDisplay(DISPLAY_6,u32_to_displayCh((num/100000)%10));
}

static uint8_t u32_to_displayCh(uint8_t digit){
	switch(digit){
		case 0:
			return digits[0];
		break;
		case 1:
			return digits[1];
		break;
		case 2:
			return digits[2];
		break;
		case 3:
			return digits[3];
		break;
		case 4:
			return digits[4];
		break;
		case 5:
			return digits[5];
		break;
		case 6:
			return digits[6];
		break;
		case 7:
			return digits[7];
		break;
		case 8:
			return digits[8];
		break;
		case 9:
			return digits[9];
		break;
		default: break;
	}
}

void standby_effect(void){
	TM1620B_writeToDisplay(DISPLAY_1,0x0F);
	TM1620B_writeToDisplay(DISPLAY_2,0x09);
	TM1620B_writeToDisplay(DISPLAY_3,0x09);
	TM1620B_writeToDisplay(DISPLAY_4,0x09);
	TM1620B_writeToDisplay(DISPLAY_5,0x09);
	TM1620B_writeToDisplay(DISPLAY_6,0x39);
	HAL_Delay(1000);
	TM1620B_clear_all();
	HAL_Delay(500);
}

void print_array(uint8_t chArr[]){
	for(uint8_t cnt = 0,num=6;cnt<strlen((char)chArr),num>0;cnt++,num--)
		TM1620B_writeToDisplay((e_TM1620B_displayNum)num,chArr[cnt]);
}
