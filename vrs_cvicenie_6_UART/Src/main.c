/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"


void SystemClock_Config(void);

void process_serial_data(uint8_t ch);

int main(void)
{

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  SystemClock_Config();

  MX_GPIO_Init();
  MX_USART2_UART_Init();

  USART2_RegisterCallback(process_serial_data);

  char tx_data = 'a';

  while (1)
  {
	  //zakomentujem odosielanie abecedy aby som videl co som stlacil v callback funkcii
	  //LL_USART_TransmitData8(USART2, tx_data++);
	  //tx_data == ('z' + 1) ? tx_data = 'a' : tx_data;

	  int i;

	  //ak LED je zapnuta
	  if((LL_GPIO_ReadInputPort(GPIOB) & (1 << 3)) >> 3){
		  uint8_t spravaON[7] = "ledON  "; //ano dve medzeri aby bolo prehladnejsie :D
		  for(i=0;i<7;i++){
			  LL_USART_TransmitData8(USART2, spravaON[i]);  //Poslem cely array charov na vypis
			  LL_mDelay(200);
		  }
	  //ak LED je vypnuta
	  }else{
		  uint8_t spravaOFF[8] = "ledOFF  ";
		  for(i=0;i<8;i++){
			  LL_USART_TransmitData8(USART2, spravaOFF[i]);  //Poslem cely array charov na vypis
			  LL_mDelay(200);
		  }
	  }

	  LL_mDelay(1000);
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);

  if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_0)
  {
  Error_Handler();  
  }
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {
    
  }
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
  {
  
  }
  LL_Init1msTick(8000000);
  LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
  LL_SetSystemCoreClock(8000000);
}


void process_serial_data(uint8_t ch)
{
	static uint8_t count = 0;
	static uint8_t ledON[5];
	static uint8_t ledOFF[6];
	int i;
	//LL_USART_TransmitData8(USART2, ch);

	//Ak led svieti
	if((LL_GPIO_ReadInputPort(GPIOB) & (1 << 3)) >> 3){
		ledOFF[count] = ch;  //postupne pridavam do arrayu chary
		count++;
		if(strcmp(ledOFF, "ledOFF") == 0){
			LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_3);  //ak sa array charov zhoduje s ledOFF tak zhasne
		}
		//resetovacia podmienka pre index pridavania charrov a tak isto aj premazanie arrayu
		//ak prve pismeno bude 'l' tak vtedy zacne ratat 6 charov a potom porovna, 'l' funguje ako spustac
		if(count == 6 || ledOFF[0]!='l'){
			count = 0;
			for(i=0;i<7;i++){
				ledOFF[i] = 0;
			}
		}
	//Ak je ledka zhasnuta
	}else{
		ledON[count] = ch;  //postupne pridava do arrayu charov
		count++;
		if(strcmp(ledON, "ledON") == 0){
			LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_3);   //ak sa array charov zhoduje s ledON tak sa zapne
		}
		//resetovacia podmienka pre index pridavania charrov a tak isto aj premazanie arrayu
		//ak prve pismeno bude 'l' tak vtedy zacne ratat 5 charov a potom porovna, 'l' funguje ako spustac
		if(count == 5 || ledON[0]!='l'){
			count = 0;
			for(i=0;i<6;i++){
				ledON[i] = 0;
			}
		}
	}

	/* povodne zadanie, s f a n charmi
	if(ch == 'f')
	{
		//ak teda pride 'f' tak resetnem LEDku na pine 3 a vzaroven ju aj odoslem aby som v putty videl co som stlacil
		LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_3);
		LL_USART_TransmitData8(USART2, ch);
	}

	if(ch == 'n'){
		//tak isto ak pride 'n' tak LEDku setnem na pine 3 a poslem cez usart do PC ten charakter aby sa vypisal v putty
		LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_3);
		LL_USART_TransmitData8(USART2, ch);
	}
	*/

}


/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
