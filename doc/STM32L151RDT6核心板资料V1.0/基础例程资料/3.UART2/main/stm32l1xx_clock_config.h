/**
  ******************************************************************************
  * @file    stm32l1xx_clock_config.h 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    24-January-2012
  * @brief   Header for stm32l1xx_clock_config.c file
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * FOR MORE INFORMATION PLEASE READ CAREFULLY THE LICENSE AGREEMENT FILE
  * LOCATED IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32L1xx_CLOCK_CONFIG_H
#define __STM32L1xx_CLOCK_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void SetHCLKToMSI_2MHz(void);
void SetHCLKToMSI_64KHz(void);
void SetHCLKToMSI_128KHz(void);
void SetHCLKToMSI_256KHz(void);
void SetHCLKToMSI_512KHz(void);
void SetHCLKToMSI_1MHz(void);
void SetHCLKToMSI_4MHz(void);
void SetHCLKToHSI(void);
void SetHCLKToHSE(void);
void SetHCLKTo4(void);
void SetHCLKTo8(void);
void SetHCLKTo16(void);
void SetHCLKTo24(void);
void SetHCLKTo32(void);

#endif /* __STM32L1xx_CLOCK_CONFIG_H */

/******************* (C) COPYRIGHT 2012 STMicroelectronics *****END OF FILE****/
