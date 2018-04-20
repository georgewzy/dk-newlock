/**
  ******************************************************************************
  * @file    Project/STM32L1xx_StdPeriph_Templates/main.h 
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    16-May-2014
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H_
#define __MAIN_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */






#define MQTT_KEEP_ALIVE_INTERVAL		1000*60*2
#define HEARTBEAT						1000*60*3
#define BATT_VOLTAGE					1000*60*60


#define OEM_CODE				000001
#define SOFTWARE_VERSION		0100
#define HAREWARE_VERSION		0130



typedef struct
{
	uint8_t OEM_code[5];
	uint8_t software_version[4];
	uint8_t hareware_version[4];
	
} DEV_BASIC_INFO;













#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
