

#ifndef __BUTTON_H_
#define __BUTTON_H_
#include <stm32l1xx.h>


typedef enum
{
	buuton_null,
	button_up,
	button_click,
	button_long_press
	
} button_state_e;


typedef struct
{
	button_state_e  	butt_state;
	uint8_t 			butt_en;
	uint8_t				butt_cnt;
	uint32_t			butt_long_press_timer;

} button_info_s;






#define BUTTON_VALID_CNT			10
#define BUTTON_SUM					1



uint8_t button_get_value(void);
void button_timer_ms(void);
button_state_e button_get_state(uint8_t button_name, uint32_t long_time);


#endif
