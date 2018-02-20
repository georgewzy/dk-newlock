/**
  ******************************************************************************
  * @file    stm32l1xx_clock_config.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    24-January-2012
  * @brief   This file provides firmware functions to configure the STM32L1xx
  *          system clock frequency to MSI, HSI, HSE, 4 MHz, 8 MHz, 16 MHz and
  *          32 MHz.
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

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx_clock_config.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ErrorStatus HSEStartUpStatus;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Selects MSI (Default Value, 2MHz) as System clock source and configure
  *         HCLK, PCLK2 and PCLK1 prescalers.
  * @param  None
  * @retval None
  */
void SetHCLKToMSI_2MHz(void)
{
  /* RCC system reset */
  RCC_DeInit();

  /* Flash 0 wait state */
  FLASH_SetLatency(FLASH_Latency_0);

  /* Disable Prefetch Buffer */
  FLASH_PrefetchBufferCmd(DISABLE);

  /* Disable 64-bit access */
  FLASH_ReadAccess64Cmd(DISABLE);

  /* Enable the PWR APB1 Clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Select the Voltage Range 3 (1.2V) */
  PWR_VoltageScalingConfig(PWR_VoltageScaling_Range3);

  /* Wait Until the Voltage Regulator is ready */
  while (PWR_GetFlagStatus(PWR_FLAG_VOS) != RESET)
  {}
}

/**
  * @brief  Selects MSI (64KHz) as System clock source and configure
  *         HCLK, PCLK2 and PCLK1 prescalers.
  * @param  None
  * @retval None
  */
void SetHCLKToMSI_64KHz(void)
{
  /* RCC system reset */
  RCC_DeInit();

  /* Flash 0 wait state */
  FLASH_SetLatency(FLASH_Latency_0);

  /* Disable Prefetch Buffer */
  FLASH_PrefetchBufferCmd(DISABLE);

  /* Disable 64-bit access */
  FLASH_ReadAccess64Cmd(DISABLE);

  /* Enable the PWR APB1 Clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Select the Voltage Range 3 (1.2V) */
  PWR_VoltageScalingConfig(PWR_VoltageScaling_Range3);

  /* Wait Until the Voltage Regulator is ready */
  while (PWR_GetFlagStatus(PWR_FLAG_VOS) != RESET)
  {}
  /* HCLK = SYSCLK */
  RCC_HCLKConfig(RCC_SYSCLK_Div1);

  /* PCLK2 = HCLK */
  RCC_PCLK2Config(RCC_HCLK_Div1);

  /* PCLK1 = HCLK */
  RCC_PCLK1Config(RCC_HCLK_Div1);

  /* Set MSI clock range to 64KHz */
  RCC_MSIRangeConfig(RCC_MSIRange_0);

  /* Select MSI as system clock source */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_MSI);

  /* Wait till PLL is used as system clock source */
  while (RCC_GetSYSCLKSource() != 0x00)
  {}
}

/**
  * @brief  Selects MSI (128KHz) as System clock source and configure
  *         HCLK, PCLK2 and PCLK1 prescalers.
  * @param  None
  * @retval None
  */
void SetHCLKToMSI_128KHz(void)
{
  /* RCC system reset */
  RCC_DeInit();

  /* Flash 0 wait state */
  FLASH_SetLatency(FLASH_Latency_0);

  /* Disable Prefetch Buffer */
  FLASH_PrefetchBufferCmd(DISABLE);

  /* Disable 64-bit access */
  FLASH_ReadAccess64Cmd(DISABLE);

  /* Enable the PWR APB1 Clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Select the Voltage Range 3 (1.2V) */
  PWR_VoltageScalingConfig(PWR_VoltageScaling_Range3);

  /* Wait Until the Voltage Regulator is ready */
  while (PWR_GetFlagStatus(PWR_FLAG_VOS) != RESET)
  {}
  /* HCLK = SYSCLK */
  RCC_HCLKConfig(RCC_SYSCLK_Div1);

  /* PCLK2 = HCLK */
  RCC_PCLK2Config(RCC_HCLK_Div1);

  /* PCLK1 = HCLK */
  RCC_PCLK1Config(RCC_HCLK_Div1);

  /* Set MSI clock range to 128KHz */
  RCC_MSIRangeConfig(RCC_MSIRange_1);

  /* Select MSI as system clock source */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_MSI);

  /* Wait till PLL is used as system clock source */
  while (RCC_GetSYSCLKSource() != 0x00)
  {}
}

/**
  * @brief  Selects MSI (256KHz) as System clock source and configure
  *         HCLK, PCLK2 and PCLK1 prescalers.
  * @param  None
  * @retval None
  */
void SetHCLKToMSI_256KHz(void)
{
  /* RCC system reset */
  RCC_DeInit();

  /* Flash 0 wait state */
  FLASH_SetLatency(FLASH_Latency_0);

  /* Disable Prefetch Buffer */
  FLASH_PrefetchBufferCmd(DISABLE);

  /* Disable 64-bit access */
  FLASH_ReadAccess64Cmd(DISABLE);

  /* Enable the PWR APB1 Clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Select the Voltage Range 3 (1.2V) */
  PWR_VoltageScalingConfig(PWR_VoltageScaling_Range3);

  /* Wait Until the Voltage Regulator is ready */
  while (PWR_GetFlagStatus(PWR_FLAG_VOS) != RESET)
  {}
  /* HCLK = SYSCLK */
  RCC_HCLKConfig(RCC_SYSCLK_Div1);

  /* PCLK2 = HCLK */
  RCC_PCLK2Config(RCC_HCLK_Div1);

  /* PCLK1 = HCLK */
  RCC_PCLK1Config(RCC_HCLK_Div1);

  /* Set MSI clock range to 256KHz */
  RCC_MSIRangeConfig(RCC_MSIRange_2);

  /* Select MSI as system clock source */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_MSI);

  /* Wait till PLL is used as system clock source */
  while (RCC_GetSYSCLKSource() != 0x00)
  {}
}

/**
  * @brief  Selects MSI (512KHz) as System clock source and configure
  *         HCLK, PCLK2 and PCLK1 prescalers.
  * @param  None
  * @retval None
  */
void SetHCLKToMSI_512KHz(void)
{
  /* RCC system reset */
  RCC_DeInit();

  /* Flash 0 wait state */
  FLASH_SetLatency(FLASH_Latency_0);

  /* Disable Prefetch Buffer */
  FLASH_PrefetchBufferCmd(DISABLE);

  /* Disable 64-bit access */
  FLASH_ReadAccess64Cmd(DISABLE);

  /* Enable the PWR APB1 Clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Select the Voltage Range 3 (1.2V) */
  PWR_VoltageScalingConfig(PWR_VoltageScaling_Range3);

  /* Wait Until the Voltage Regulator is ready */
  while (PWR_GetFlagStatus(PWR_FLAG_VOS) != RESET)
  {}
  /* HCLK = SYSCLK */
  RCC_HCLKConfig(RCC_SYSCLK_Div1);

  /* PCLK2 = HCLK */
  RCC_PCLK2Config(RCC_HCLK_Div1);

  /* PCLK1 = HCLK */
  RCC_PCLK1Config(RCC_HCLK_Div1);

  /* Set MSI clock range to 512KHz */
  RCC_MSIRangeConfig(RCC_MSIRange_3);

  /* Select MSI as system clock source */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_MSI);

  /* Wait till PLL is used as system clock source */
  while (RCC_GetSYSCLKSource() != 0x00)
  {}
}

/**
  * @brief  Selects MSI (1MHz) as System clock source and configure
  *         HCLK, PCLK2 and PCLK1 prescalers.
  * @param  None
  * @retval None
  */
void SetHCLKToMSI_1MHz(void)
{
  /* RCC system reset */
  RCC_DeInit();

  /* Flash 0 wait state */
  FLASH_SetLatency(FLASH_Latency_0);

  /* Disable Prefetch Buffer */
  FLASH_PrefetchBufferCmd(DISABLE);

  /* Disable 64-bit access */
  FLASH_ReadAccess64Cmd(DISABLE);

  /* Enable the PWR APB1 Clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Select the Voltage Range 3 (1.2V) */
  PWR_VoltageScalingConfig(PWR_VoltageScaling_Range3);

  /* Wait Until the Voltage Regulator is ready */
  while (PWR_GetFlagStatus(PWR_FLAG_VOS) != RESET)
  {}
  /* HCLK = SYSCLK */
  RCC_HCLKConfig(RCC_SYSCLK_Div1);

  /* PCLK2 = HCLK */
  RCC_PCLK2Config(RCC_HCLK_Div1);

  /* PCLK1 = HCLK */
  RCC_PCLK1Config(RCC_HCLK_Div1);

  /* Set MSI clock range to 1MHz */
  RCC_MSIRangeConfig(RCC_MSIRange_4);

  /* Select MSI as system clock source */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_MSI);

  /* Wait till PLL is used as system clock source */
  while (RCC_GetSYSCLKSource() != 0x00)
  {}
}

/**
  * @brief  Selects MSI (4MHz) as System clock source and configure
  *         HCLK, PCLK2 and PCLK1 prescalers.
  * @param  None
  * @retval None
  */
void SetHCLKToMSI_4MHz(void)
{
  /* RCC system reset */
  RCC_DeInit();

  /* Enable 64-bit access */
  FLASH_ReadAccess64Cmd(ENABLE);

  /* Enable Prefetch Buffer */
  FLASH_PrefetchBufferCmd(ENABLE);

  /* Flash 1 wait state */
  FLASH_SetLatency(FLASH_Latency_1);

  /* Enable the PWR APB1 Clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Select the Voltage Range 3 (1.2V) */
  PWR_VoltageScalingConfig(PWR_VoltageScaling_Range3);

  /* Wait Until the Voltage Regulator is ready */
  while (PWR_GetFlagStatus(PWR_FLAG_VOS) != RESET)
  {}
  /* HCLK = SYSCLK */
  RCC_HCLKConfig(RCC_SYSCLK_Div1);

  /* PCLK2 = HCLK */
  RCC_PCLK2Config(RCC_HCLK_Div1);

  /* PCLK1 = HCLK */
  RCC_PCLK1Config(RCC_HCLK_Div1);

  /* Set MSI clock range to 4MHz */
  RCC_MSIRangeConfig(RCC_MSIRange_6);

  /* Select MSI as system clock source */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_MSI);

  /* Wait till PLL is used as system clock source */
  while (RCC_GetSYSCLKSource() != 0x00)
  {}
}

/**
  * @brief  Selects HSI as System clock source and configure HCLK, PCLK2 and PCLK1 prescalers.
  * @param  None
  * @retval None
  */
void SetHCLKToHSI(void)
{
  __IO uint32_t StartUpCounter = 0, HSIStatus = 0;

  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSI */
  RCC_HSICmd(ENABLE);

  /* Wait till HSI is ready and if Time out is reached exit */
  do
  {
    HSIStatus = RCC_GetFlagStatus(RCC_FLAG_HSIRDY);
    StartUpCounter++;
  }
  while ((HSIStatus == 0) && (StartUpCounter != HSI_STARTUP_TIMEOUT));


  if (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) != RESET)
  {
    HSIStatus = (uint32_t)0x01;
  }
  else
  {
    HSIStatus = (uint32_t)0x00;
  }

  if (HSIStatus == 0x01)
  {
    /* Flash 0 wait state */
    FLASH_SetLatency(FLASH_Latency_0);

    /* Disable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(DISABLE);

    /* Disable 64-bit access */
    FLASH_ReadAccess64Cmd(DISABLE);

    /* Enable the PWR APB1 Clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    /* Select the Voltage Range 1 (1.8V) */
    PWR_VoltageScalingConfig(PWR_VoltageScaling_Range1);

    /* Wait Until the Voltage Regulator is ready */
    while (PWR_GetFlagStatus(PWR_FLAG_VOS) != RESET)
    {}

    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);

    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1);

    /* PCLK1 = HCLK */
    RCC_PCLK1Config(RCC_HCLK_Div1);

    /* Select HSI as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);

    /* Wait till HSI is used as system clock source */
    while (RCC_GetSYSCLKSource() != 0x04)
    {}
  }
  else
  { /* If HSI fails to start-up, the application will have wrong clock configuration.
                           User can add here some code to deal with this error */

    /* Go to infinite loop */
    while (1)
    {}
  }
}

/**
  * @brief  Selects HSE as System clock source and configure HCLK, PCLK2 and PCLK1 prescalers.
  * @param  None
  * @retval None
  */
void SetHCLKToHSE(void)
{
  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if (HSEStartUpStatus == SUCCESS)
  {
    /* Flash 0 wait state */
    FLASH_SetLatency(FLASH_Latency_0);

    /* Disable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(DISABLE);

    /* Disable 64-bit access */
    FLASH_ReadAccess64Cmd(DISABLE);

    /* Enable the PWR APB1 Clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    /* Select the Voltage Range 2 (1.5V) */
    PWR_VoltageScalingConfig(PWR_VoltageScaling_Range2);

    /* Wait Until the Voltage Regulator is ready */
    while (PWR_GetFlagStatus(PWR_FLAG_VOS) != RESET)
    {}

    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);

    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1);

    /* PCLK1 = HCLK */
    RCC_PCLK1Config(RCC_HCLK_Div1);

    /* Select HSE as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_HSE);

    /* Wait till HSE is used as system clock source */
    while (RCC_GetSYSCLKSource() != 0x08)
    {}
  }
  else
  { /* If HSE fails to start-up, the application will have wrong clock configuration.
                           User can add here some code to deal with this error */

    /* Go to infinite loop */
    while (1)
    {}
  }
}

/**
  * @brief  Selects HSE as System clock source and configure HCLK, PCLK2 and PCLK1 prescalers.
  * @param  None
  * @retval None
  */
void SetHCLKTo4(void)
{
  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if (HSEStartUpStatus == SUCCESS)
  {
    /* Flash 0 wait state */
    FLASH_SetLatency(FLASH_Latency_0);

    /* Disable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(DISABLE);

    /* Disable 64-bit access */
    FLASH_ReadAccess64Cmd(DISABLE);

    /* Enable the PWR APB1 Clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    /* Select the Voltage Range 2 (1.5V) */
    PWR_VoltageScalingConfig(PWR_VoltageScaling_Range2);

    /* Wait Until the Voltage Regulator is ready */
    while (PWR_GetFlagStatus(PWR_FLAG_VOS) != RESET)
    {}

    /* HCLK = SYSCLK/2 */
    RCC_HCLKConfig(RCC_SYSCLK_Div2);

    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1);

    /* PCLK1 = HCLK */
    RCC_PCLK1Config(RCC_HCLK_Div1);

    /* Select HSE as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_HSE);

    /* Wait till HSE is used as system clock source */
    while (RCC_GetSYSCLKSource() != 0x08)
    {}
  }
  else
  { /* If HSE fails to start-up, the application will have wrong clock configuration.
                           User can add here some code to deal with this error */

    /* Go to infinite loop */
    while (1)
    {}
  }
}

/**
  * @brief  Selects HSE as System clock source and configure HCLK, PCLK2 and PCLK1 prescalers.
  * @param  None
  * @retval None
  */
void SetHCLKTo8(void)
{
  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if (HSEStartUpStatus == SUCCESS)
  {
    /* Flash 0 wait state */
    FLASH_SetLatency(FLASH_Latency_0);

    /* Disable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(DISABLE);

    /* Disable 64-bit access */
    FLASH_ReadAccess64Cmd(DISABLE);

    /* Enable the PWR APB1 Clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    /* Select the Voltage Range 2 (1.5V) */
    PWR_VoltageScalingConfig(PWR_VoltageScaling_Range2);

    /* Wait Until the Voltage Regulator is ready */
    while (PWR_GetFlagStatus(PWR_FLAG_VOS) != RESET)
    {}

    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);

    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1);

    /* PCLK1 = HCLK */
    RCC_PCLK1Config(RCC_HCLK_Div1);

    /* Select HSE as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_HSE);

    /* Wait till HSE is used as system clock source */
    while (RCC_GetSYSCLKSource() != 0x08)
    {}
  }
  else
  { /* If HSE fails to start-up, the application will have wrong clock configuration.
                           User can add here some code to deal with this error */

    /* Go to infinite loop */
    while (1)
    {}
  }
}

/**
  * @brief  Selects PLL as System clock source and configure HCLK, PCLK2 and PCLK1 prescalers.
  * @param  None
  * @retval None
  */
void SetHCLKTo16(void)
{
  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if (HSEStartUpStatus == SUCCESS)
  {
    /* Enable 64-bit access */
    FLASH_ReadAccess64Cmd(ENABLE);

    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(ENABLE);

    /* Flash 1 wait state */
    FLASH_SetLatency(FLASH_Latency_1);

    /* Enable the PWR APB1 Clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    /* Select the Voltage Range 2 (1.5V) */
    PWR_VoltageScalingConfig(PWR_VoltageScaling_Range2);

    /* Wait Until the Voltage Regulator is ready */
    while (PWR_GetFlagStatus(PWR_FLAG_VOS) != RESET)
    {}

    /* HCLK = SYSCLK/2 */
    RCC_HCLKConfig(RCC_SYSCLK_Div2);

    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1);

    /* PCLK1 = HCLK */
    RCC_PCLK1Config(RCC_HCLK_Div1);

    /*  PLL configuration: PLLCLK = (HSE * 12) / 3 = 32MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE, RCC_PLLMul_12, RCC_PLLDiv_3);

    /* Enable PLL */
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {}

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while (RCC_GetSYSCLKSource() != 0x0C)
    {}
  }
  else
  { /* If HSE fails to start-up, the application will have wrong clock configuration.
                           User can add here some code to deal with this error */

    /* Go to infinite loop */
    while (1)
    {}
  }
}

/**
  * @brief  Selects PLL as System clock source and configure HCLK, PCLK2 and PCLK1 prescalers.
  * @param  None
  * @retval None
  */
void SetHCLKTo32(void)
{
  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if (HSEStartUpStatus == SUCCESS)
  {
    /* Enable 64-bit access */
    FLASH_ReadAccess64Cmd(ENABLE);

    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(ENABLE);

    /* Flash 1 wait state */
    FLASH_SetLatency(FLASH_Latency_1);

    /* Enable the PWR APB1 Clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    /* Select the Voltage Range 1 (1.8V) */
    PWR_VoltageScalingConfig(PWR_VoltageScaling_Range1);

    /* Wait Until the Voltage Regulator is ready */
    while (PWR_GetFlagStatus(PWR_FLAG_VOS) != RESET)
    {}

    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);

    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1);

    /* PCLK1 = HCLK */
    RCC_PCLK1Config(RCC_HCLK_Div1);

    /*  PLL configuration: PLLCLK = (HSE * 12) / 3 = 32MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE, RCC_PLLMul_12, RCC_PLLDiv_3);

    /* Enable PLL */
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {}

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while (RCC_GetSYSCLKSource() != 0x0C)
    {}
  }
  else
  { /* If HSE fails to start-up, the application will have wrong clock configuration.
                           User can add here some code to deal with this error */

    /* Go to infinite loop */
    while (1)
    {}
  }
}

/******************* (C) COPYRIGHT 2012 STMicroelectronics *****END OF FILE****/
