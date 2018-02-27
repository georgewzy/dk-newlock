

#ifndef __BUTTON_H_
#define __BUTTON_H_
#include <stm32l1xx.h>


typedef enum
{
	BUTTON_NULL,
	BUTTON_UP,
	BUTTON_CLICK,
	BUTTON_DOWN,
	BUTTON_PRESS
	
} button_state_e;


typedef struct
{
	button_state_e  	butt_state;
	uint8_t 			butt_en;
	uint8_t				butt_cnt;
	uint32_t			butt_long_press_timer;

} button_info_s;






#define BUTTON_VALID_CNT			20
#define BUTTON_SUM					1
#define BUTTON_UP_CNT				0
#define BUTTON_DOWN_CNT				5





void button_gpio_init(void);
void button_timer_ms(void);
button_state_e button_get_state(uint8_t button_name, uint32_t long_time);


#endif
