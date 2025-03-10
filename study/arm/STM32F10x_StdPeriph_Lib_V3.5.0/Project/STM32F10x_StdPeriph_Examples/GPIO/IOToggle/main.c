/**
  ******************************************************************************
  * @file    GPIO/IOToggle/main.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main program body.
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
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
//#include "stm32_eval.h"
#include <stdio.h>

int fputc(int ch, FILE *f)
{
	while(!(USART1->SR & (0x01 << 7)));
  USART1->DR = (unsigned char)ch;
	
  return ch;
}

/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup GPIO_IOToggle
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
GPIO_InitTypeDef GPIO_InitStructure;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

volatile unsigned int TimingDelay;	
volatile unsigned long sys_count;

unsigned long micros_10us()
{
	return sys_count;
}


void Delay(unsigned int nTime)
{ 
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

void Delay_ms(unsigned int nTime)
{ 
  Delay(nTime * 100);
}

void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}


// ISR Routine
//------------------------------------

void SysTick_Handler(void)
{
	TimingDelay_Decrement();
	sys_count++;
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f10x_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f10x.c file
     */     

//if (SysTick_Config(720))
//{ 
//	/* Capture error */ 
//	while (1);
//}


	//USART 1
	RCC->APB2ENR |= (0x01 << 2) | (0x01 << 14);
	GPIOA->CRH &= ~(0x0F << 2 * 4); // GPIO:9
	GPIOA->CRH &= ~(0x0F << 3 * 4); // GPIO:10
	GPIOA->CRH |= (0x0B << 1 * 4); // GPIO:9
	GPIOA->CRH |= (0x04 << 2 * 4); // GPIO:10
	USART1->CR1 = (0x01 << 2) | (0x01 << 3 )| (0x01 << 13); // RE, TE, UE
	USART1->CR2 = 0x00;
	USART1->CR3 = 0x00;
	USART1->BRR = 0x271; // BRR:115200
	//NVIC->ISER[1] |= (0x01 << 5); // USART ON

	
	printf("hello");
while(1);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

while(1)
{
 GPIO_SetBits(GPIOE, GPIO_Pin_2); 
 Delay(100000);
 GPIO_ResetBits(GPIOE, GPIO_Pin_2);
 Delay(100000);
}
while(1);



#ifdef IOTOGGLE_BOARD       
  /* GPIOD Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
#else
  /* GPIOD Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

  /* Configure PD0 and PD2 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
#endif
  /* To achieve GPIO toggling maximum frequency, the following  sequence is mandatory. 
     You can monitor PD0 or PD2 on the scope to measure the output signal. 
     If you need to fine tune this frequency, you can add more GPIO set/reset 
     cycles to minimize more the infinite loop timing.
     This code needs to be compiled with high speed optimization option.  */
  while (1)
  {
  	unsigned int i,j;

#ifdef IOTOGGLE_BOARD       
    /* Set PD0 and PD2 */
    GPIOA->BSRR = 0x00000100;
    GPIOB->BSRR = 0x00008000;
#else
    /* Set PD0 and PD2 */
    GPIOD->BSRR = 0x00000005;
#endif

	//delay(100);
		// 0.5 sec delay
	  for(i=0;i<100;i++)
	  	for(j=0;j<60000;j++);

#ifdef IOTOGGLE_BOARD       
    /* Reset PD0 and PD2 */
    GPIOA->BRR  = 0x00000100;
    GPIOB->BRR  = 0x00008000;
#else
    /* Reset PD0 and PD2 */
    GPIOD->BRR  = 0x00000005;
#endif

	//delay(100);
		// 0.5 sec delay
	  for(i=0;i<100;i++)
	  	for(j=0;j<60000;j++);


 #if 0
    /* Set PD0 and PD2 */
    GPIOD->BSRR = 0x00000005;
    /* Reset PD0 and PD2 */
    GPIOD->BRR  = 0x00000005;

    /* Set PD0 and PD2 */
    GPIOD->BSRR = 0x00000005;
    /* Reset PD0 and PD2 */
    GPIOD->BRR  = 0x00000005;

    /* Set PD0 and PD2 */
    GPIOD->BSRR = 0x00000005;
    /* Reset PD0 and PD2 */
    GPIOD->BRR  = 0x00000005;

    /* Set PD0 and PD2 */
    GPIOD->BSRR = 0x00000005;
    /* Reset PD0 and PD2 */
    GPIOD->BRR  = 0x00000005;

    /* Set PD0 and PD2 */
    GPIOD->BSRR = 0x00000005;
    /* Reset PD0 and PD2 */
    GPIOD->BRR  = 0x00000005;

    /* Set PD0 and PD2 */
    GPIOD->BSRR = 0x00000005;
    /* Reset PD0 and PD2 */
    GPIOD->BRR  = 0x00000005;

    /* Set PD0 and PD2 */
    GPIOD->BSRR = 0x00000005;
    /* Reset PD0 and PD2 */
    GPIOD->BRR  = 0x00000005;

    /* Set PD0 and PD2 */
    GPIOD->BSRR = 0x00000005;
    /* Reset PD0 and PD2 */
    GPIOD->BRR  = 0x00000005;

    /* Set PD0 and PD2 */
    GPIOD->BSRR = 0x00000005;
    /* Reset PD0 and PD2 */
    GPIOD->BRR  = 0x00000005;
 #endif
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}

#endif

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
