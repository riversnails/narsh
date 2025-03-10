/**
  ******************************************************************************
  * @file    RTC/Calendar/main.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main program body
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
#include "stm32_eval.h"
#include <stdio.h>

/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup RTC_Calendar
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define RTCClockOutput_Enable  /* RTC Clock/64 is output on tamper pin(PC.13) */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t TimeDisplay = 0;
USART_InitTypeDef USART_InitStructure;

/* Private function prototypes -----------------------------------------------*/
void RTC_Configuration(void);
void NVIC_Configuration(void);
uint32_t Time_Regulate(void);
void Time_Adjust(void);
void Time_Show(void);
void Time_Display(uint32_t TimeVar);
uint8_t USART_Scanf(uint32_t value);

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/*
//Seven Segment_ONE
unsigned int count = 0;
char update = 0;

void TIM1_UP_IRQHandler (void) 
{
  if ((TIM1->SR & 0x0001) != 0) {                 // check interrupt source

	count ++;
	update = 1;
	printf("second : %4d\r", count);

    TIM1->SR &= ~(1<<0);                          // clear UIF flag
 }
}
*/

/*
//Seven Segment_TWO

unsigned int count = 0;

void TIM1_UP_IRQHandler (void) 
{
  if ((TIM1->SR & 0x0001) != 0) {                 // check interrupt source

	count ++;

    TIM1->SR &= ~(1<<0);                          // clear UIF flag
 }
}

void SevenSegment_Count_1st (void)
{
	(*(volatile unsigned int *)0x40010C0C) |= (0x01<<0);	//PB0 ON
	(*(volatile unsigned int *)0x40010C0C) &= ~(0x01<<1);	//PB1 OFF
	(*(volatile unsigned int *)0x4001080C) &= ~ (0xFF);		//clear	
	switch( count % 10 )
			{
			case 0:
				(*(volatile unsigned int *)0x4001080C) |= 0x03;
				break;
	
			case 1 :
				(*(volatile unsigned int *)0x4001080C) |= 0x3F;
				break;

			case 2 :
				(*(volatile unsigned int *)0x4001080C) |= 0x49;
				break;
	
			case 3 :
				(*(volatile unsigned int *)0x4001080C) |= 0x29;
				break;
	
			case 4 :
				(*(volatile unsigned int *)0x4001080C) |= 0x35;
				break;
	
			case 5 :
				(*(volatile unsigned int *)0x4001080C) |= 0xA1;
				break;
	
			case 6 :
				(*(volatile unsigned int *)0x4001080C) |= 0x81;
				break;
	
			case 7 :
				(*(volatile unsigned int *)0x4001080C) |= 0x3B;
				break;
	
			case 8 :
				(*(volatile unsigned int *)0x4001080C) |= 0x01;
				break;
	
			case 9 :
				(*(volatile unsigned int *)0x4001080C) |= 0x21;
				break;
	
			default :
				(*(volatile unsigned int *)0x4001080C) |= 0x00;
				break; 
			}		
}

void SevenSegment_Count_2nd (void)
{
	(*(volatile unsigned int *)0x40010C0C) &= ~(0x01<<0);	//PB0 OFF
	(*(volatile unsigned int *)0x40010C0C) |= (0x01<<1);	//PB1 ON
	(*(volatile unsigned int *)0x4001080C) &= ~ (0xFF);		//clear	
	switch( count / 10 )
			{
			case 0:
				(*(volatile unsigned int *)0x4001080C) |= 0x03;
				break;
	
			case 1 :
				(*(volatile unsigned int *)0x4001080C) |= 0x3F;
				break;

			case 2 :
				(*(volatile unsigned int *)0x4001080C) |= 0x49;
				break;
	
			case 3 :
				(*(volatile unsigned int *)0x4001080C) |= 0x29;
				break;
	
			case 4 :
				(*(volatile unsigned int *)0x4001080C) |= 0x35;
				break;
	
			case 5 :
				(*(volatile unsigned int *)0x4001080C) |= 0xA1;
				break;
	
			case 6 :
				(*(volatile unsigned int *)0x4001080C) |= 0x81;
				break;
	
			case 7 :
				(*(volatile unsigned int *)0x4001080C) |= 0x3B;
				break;
	
			case 8 :
				(*(volatile unsigned int *)0x4001080C) |= 0x01;
				break;
	
			case 9 :
				(*(volatile unsigned int *)0x4001080C) |= 0x21;
				break;
	
			default :
				(*(volatile unsigned int *)0x4001080C) |= 0x00;
				break; 
			}
	
}

void Delay (void)
{
	unsigned int i, j;
	for( i = 0 ; i < 1 ; i++)
			for(j = 0 ; j < 60000 ; j++);
			
}
*/

//Seven Segment Function
void SevenSegment_Count_1st (unsigned int num)
{
	(*(volatile unsigned int *)0x40010C0C) |= (0x01<<0);	//PB0 ON
	(*(volatile unsigned int *)0x40010C0C) &= ~(0x01<<1);	//PB1 OFF
	(*(volatile unsigned int *)0x4001080C) &= ~ (0xFF);		//clear	
	switch( num % 10 )
			{
			case 0:
				(*(volatile unsigned int *)0x4001080C) |= 0x03;
				break;
	
			case 1 :
				(*(volatile unsigned int *)0x4001080C) |= 0x3F;
				break;

			case 2 :
				(*(volatile unsigned int *)0x4001080C) |= 0x49;
				break;
	
			case 3 :
				(*(volatile unsigned int *)0x4001080C) |= 0x29;
				break;
	
			case 4 :
				(*(volatile unsigned int *)0x4001080C) |= 0x35;
				break;
	
			case 5 :
				(*(volatile unsigned int *)0x4001080C) |= 0xA1;
				break;
	
			case 6 :
				(*(volatile unsigned int *)0x4001080C) |= 0x81;
				break;
	
			case 7 :
				(*(volatile unsigned int *)0x4001080C) |= 0x3B;
				break;
	
			case 8 :
				(*(volatile unsigned int *)0x4001080C) |= 0x01;
				break;
	
			case 9 :
				(*(volatile unsigned int *)0x4001080C) |= 0x21;
				break;
	
			default :
				(*(volatile unsigned int *)0x4001080C) |= 0x00;
				break; 
			}		
}

void SevenSegment_Count_2nd (unsigned int num)
{
	(*(volatile unsigned int *)0x40010C0C) &= ~(0x01<<0);	//PB0 OFF
	(*(volatile unsigned int *)0x40010C0C) |= (0x01<<1);	//PB1 ON
	(*(volatile unsigned int *)0x4001080C) &= ~ (0xFF);		//clear	
	switch( num / 10 )
			{
			case 0:
				(*(volatile unsigned int *)0x4001080C) |= 0x03;
				break;
	
			case 1 :
				(*(volatile unsigned int *)0x4001080C) |= 0x3F;
				break;

			case 2 :
				(*(volatile unsigned int *)0x4001080C) |= 0x49;
				break;
	
			case 3 :
				(*(volatile unsigned int *)0x4001080C) |= 0x29;
				break;
	
			case 4 :
				(*(volatile unsigned int *)0x4001080C) |= 0x35;
				break;
	
			case 5 :
				(*(volatile unsigned int *)0x4001080C) |= 0xA1;
				break;
	
			case 6 :
				(*(volatile unsigned int *)0x4001080C) |= 0x81;
				break;
	
			case 7 :
				(*(volatile unsigned int *)0x4001080C) |= 0x3B;
				break;
	
			case 8 :
				(*(volatile unsigned int *)0x4001080C) |= 0x01;
				break;
	
			case 9 :
				(*(volatile unsigned int *)0x4001080C) |= 0x21;
				break;
	
			default :
				(*(volatile unsigned int *)0x4001080C) |= 0x00;
				break; 
			}
	
}

void Delay (void)
{
	unsigned int i, j;
	for( i = 0 ; i < 1 ; i++)
			for(j = 0 ; j < 60000 ; j++);
			
}


void Display_SevenSegment (unsigned int num)
{
	SevenSegment_Count_1st(num);
	Delay();
	SevenSegment_Count_2nd(num);
	Delay();	

}

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
/*	
	//Seven Segment_ONE
  unsigned int num;
*/

  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f10x_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f10x.c file
     */     

  /* Initialize LED1 mounted on STM3210X-EVAL board */       
  //STM_EVAL_LEDInit(LED1);

  /* USARTx configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  STM_EVAL_COMInit(COM1, &USART_InitStructure);


  /* NVIC configuration */
  NVIC_Configuration();

#if 0

  if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
  {
    /* Backup data register value is not correct or not yet programmed (when
       the first time the program is executed) */

    printf("\r\n\n RTC not yet configured....");

    /* RTC Configuration */
    RTC_Configuration();

    printf("\r\n RTC configured....");

    /* Adjust time by values entered by the user on the hyperterminal */
    Time_Adjust();

    BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
  }
  else
  {
    /* Check if the Power On Reset flag is set */
    if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
    {
      printf("\r\n\n Power On Reset occurred....");
    }
    /* Check if the Pin Reset flag is set */
    else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
    {
      printf("\r\n\n External Reset occurred....");
    }

    printf("\r\n No need to configure RTC....");
    /* Wait for RTC registers synchronization */
    RTC_WaitForSynchro();

    /* Enable the RTC Second */
    RTC_ITConfig(RTC_IT_SEC, ENABLE);
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
  }

#ifdef RTCClockOutput_Enable
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Disable the Tamper Pin */
  BKP_TamperPinCmd(DISABLE); /* To output RTCCLK/64 on Tamper pin, the tamper
                                 functionality must be disabled */

  /* Enable RTC Clock Output on Tamper Pin */
  BKP_RTCOutputConfig(BKP_RTCOutputSource_CalibClock);
#endif

  /* Clear reset flags */
  RCC_ClearFlag();

  /* Display time in infinite loop */
  Time_Show();
#endif
/*
//Seven Segment_ONE
	
	//GPIOA : 4001 0800, GPIOB : 4001 0c00, TIM1 : 4001 2C00, RCC : 4002 1000
	
	//RCC_APB2ENR
	(*(volatile unsigned int *)0x40021018) |= (0x1<<2);	//RCC_APB2ENR : Port A clock enable
	(*(volatile unsigned int *)0x40021018) |= (0x1<<3);	//RCC_APB2ENR : Port B clock enable
	(*(volatile unsigned int *)0x40021018) |= (0x1<<11); //RCC_APB2ENR : TIM1 clock enable

	//GPIOA_CRL
	(*(volatile unsigned int *)0x40010800) &= ~(0xFFFFFFFF);	//GPIOA_CRL clear
	(*(volatile unsigned int *)0x40010800) |= 0x33333333;	//GPIOA_CRL : PA0 ~ PA7, MODE : 11, CNF : 00

	//GPIOB_CRL
	(*(volatile unsigned int *)0x40010C00) &= ~(0xF);	//GPIOB_CRL clear
	(*(volatile unsigned int *)0x40010C00) |= (0x3<<0);	//GPIOB_CRL :  PB0, MODE : 11, CNF : 00

	//TIM1_CH3
	(*(volatile unsigned int *)0x40012C00) |= (0x01<<7);	//TIM1_CR1_ARPE enable
	(*(volatile unsigned int *)0x40012C0C) |= (0x1<<0);		//TIM1_DIER UIE enable

	(*(volatile unsigned int *)0x40012C28) = (0x2710);		//TIM1_PSC 10000으로 분주
	(*(volatile unsigned int *)0x40012C2C) = (0x1C20);		//TIM1_ARR 7200으로
	(*(volatile unsigned int *)0x40012C1C) |= (0x68<<0);	//TIM1_CCMR2_CC3S 0 : output , OC3PE 1, OC3M 6
	(*(volatile unsigned int *)0x40012C20) |= (0x01<<8);	//TIM1_CCER_CC3E enable
	(*(volatile unsigned int *)0x40012C3C) = 0xE10;			//TIM1_CCR3 3600으로(duty)
	(*(volatile unsigned int *)0x40012C00) |= (0x01<<0);	//TIM1_CR1 CEN enable

	//GPIOB_PB0 ON(voltage on)
	(*(volatile unsigned int *)0x40010C0C) |= 0x01;	   //PB0

	for( ; ; )
	{	
		if( update == 1 )
		{
			update = 0 ;
			num = count;
			(*(volatile unsigned int *)0x4001080C) &= ~ (0xFF);		

			switch( num % 10 )
			{
			case 0:
				(*(volatile unsigned int *)0x4001080C) |= 0x03;
				break;
	
			case 1 :
				(*(volatile unsigned int *)0x4001080C) |= 0x3F;
				break;

			case 2 :
				(*(volatile unsigned int *)0x4001080C) |= 0x49;
				break;
	
			case 3 :
				(*(volatile unsigned int *)0x4001080C) |= 0x29;
				break;
	
			case 4 :
				(*(volatile unsigned int *)0x4001080C) |= 0x35;
				break;
	
			case 5 :
				(*(volatile unsigned int *)0x4001080C) |= 0xA1;
				break;
	
			case 6 :
				(*(volatile unsigned int *)0x4001080C) |= 0x81;
				break;
	
			case 7 :
				(*(volatile unsigned int *)0x4001080C) |= 0x3B;
				break;
	
			case 8 :
				(*(volatile unsigned int *)0x4001080C) |= 0x01;
				break;
	
			case 9 :
				(*(volatile unsigned int *)0x4001080C) |= 0x21;
				break;
	
			default :
				(*(volatile unsigned int *)0x4001080C) |= 0x00;
				break; 
			}
		}
	}
*/

/*
//Seven Segment TWO
	//GPIOA : 4001 0800, GPIOB : 4001 0c00, TIM1 : 4001 2C00, RCC : 4002 1000
	
	//RCC_APB2ENR
	(*(volatile unsigned int *)0x40021018) |= (0x1<<2);	//RCC_APB2ENR : Port A clock enable
	(*(volatile unsigned int *)0x40021018) |= (0x1<<3);	//RCC_APB2ENR : Port B clock enable
	(*(volatile unsigned int *)0x40021018) |= (0x1<<11); //RCC_APB2ENR : TIM1 clock enable

	//GPIOA_CRL
	(*(volatile unsigned int *)0x40010800) &= ~(0xFFFFFFFF);	//GPIOA_CRL clear
	(*(volatile unsigned int *)0x40010800) |= 0x33333333;	//GPIOA_CRL : PA0 ~ PA7, MODE : 11, CNF : 00

	//GPIOB_CRL
	(*(volatile unsigned int *)0x40010C00) &= ~(0xFF);	//GPIOB_CRL clear
	(*(volatile unsigned int *)0x40010C00) |= (0x3<<0);	//GPIOB_CRL :  PB0, MODE : 11, CNF : 00
	(*(volatile unsigned int *)0x40010C00) |= (0x3<<4);	//GPIOB_CRL : PB1, MODE : 11, CNF : 00

	//TIM1_CH3
	(*(volatile unsigned int *)0x40012C00) |= (0x01<<7);	//TIM1_CR1_ARPE enable
	(*(volatile unsigned int *)0x40012C0C) |= (0x1<<0);		//TIM1_DIER UIE enable

	(*(volatile unsigned int *)0x40012C28) = (0x2710);		//TIM1_PSC 10000으로 분주
	(*(volatile unsigned int *)0x40012C2C) = (0x1C20);		//TIM1_ARR 7200으로
	(*(volatile unsigned int *)0x40012C1C) |= (0x68<<0);	//TIM1_CCMR2_CC3S 0 : output , OC3PE 1, OC3M 6
	(*(volatile unsigned int *)0x40012C20) |= (0x01<<8);	//TIM1_CCER_CC3E enable
	(*(volatile unsigned int *)0x40012C3C) = 0xE10;			//TIM1_CCR3 3600으로(duty)
	(*(volatile unsigned int *)0x40012C00) |= (0x01<<0);	//TIM1_CR1 CEN enable



	for( ; ; )
	{

		SevenSegment_Count_1st();
		Delay();
		SevenSegment_Count_2nd();
		Delay();
		
	}
*/

//Seven Segment Function
	
	//RCC_APB2ENR
	(*(volatile unsigned int *)0x40021018) |= (0x1<<2);	//RCC_APB2ENR : Port A clock enable
	(*(volatile unsigned int *)0x40021018) |= (0x1<<3);	//RCC_APB2ENR : Port B clock enable
	(*(volatile unsigned int *)0x40021018) |= (0x1<<11); //RCC_APB2ENR : TIM1 clock enable

	//GPIOA_CRL
	(*(volatile unsigned int *)0x40010800) &= ~(0xFFFFFFFF);	//GPIOA_CRL clear
	(*(volatile unsigned int *)0x40010800) |= 0x33333333;	//GPIOA_CRL : PA0 ~ PA7, MODE : 11, CNF : 00

	//GPIOB_CRL
	(*(volatile unsigned int *)0x40010C00) &= ~(0xFF);	//GPIOB_CRL clear
	(*(volatile unsigned int *)0x40010C00) |= (0x3<<0);	//GPIOB_CRL :  PB0, MODE : 11, CNF : 00
	(*(volatile unsigned int *)0x40010C00) |= (0x3<<4);	//GPIOB_CRL : PB1, MODE : 11, CNF : 00



	for( ; ; )
	{
		Display_SevenSegment(31);
	}

}


/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  /* Enable the RTC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Configures the RTC.
  * @param  None
  * @retval None
  */
void RTC_Configuration(void)
{
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Reset Backup Domain */
  BKP_DeInit();

  /* Enable LSE */
  RCC_LSEConfig(RCC_LSE_ON);
  /* Wait till LSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {}

  /* Select LSE as RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  /* Enable RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC registers synchronization */
  RTC_WaitForSynchro();

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Enable the RTC Second */
  RTC_ITConfig(RTC_IT_SEC, ENABLE);

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Set RTC prescaler: set RTC period to 1sec */
  RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
}

/**
  * @brief  Returns the time entered by user, using Hyperterminal.
  * @param  None
  * @retval Current time RTC counter value
  */
uint32_t Time_Regulate(void)
{
  uint32_t Tmp_HH = 0xFF, Tmp_MM = 0xFF, Tmp_SS = 0xFF;

  printf("\r\n==============Time Settings=====================================");
  printf("\r\n  Please Set Hours");

  while (Tmp_HH == 0xFF)
  {
    Tmp_HH = USART_Scanf(23);
  }
  printf(":  %d", Tmp_HH);
  printf("\r\n  Please Set Minutes");
  while (Tmp_MM == 0xFF)
  {
    Tmp_MM = USART_Scanf(59);
  }
  printf(":  %d", Tmp_MM);
  printf("\r\n  Please Set Seconds");
  while (Tmp_SS == 0xFF)
  {
    Tmp_SS = USART_Scanf(59);
  }
  printf(":  %d", Tmp_SS);

  /* Return the value to store in RTC counter register */
  return((Tmp_HH*3600 + Tmp_MM*60 + Tmp_SS));
}

/**
  * @brief  Adjusts time.
  * @param  None
  * @retval None
  */
void Time_Adjust(void)
{
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
  /* Change the current time */
  RTC_SetCounter(Time_Regulate());
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
}

/**
  * @brief  Displays the current time.
  * @param  TimeVar: RTC counter value.
  * @retval None
  */
void Time_Display(uint32_t TimeVar)
{
  uint32_t THH = 0, TMM = 0, TSS = 0;
  
  /* Reset RTC Counter when Time is 23:59:59 */
  if (RTC_GetCounter() == 0x0001517F)
  {
     RTC_SetCounter(0x0);
     /* Wait until last write operation on RTC registers has finished */
     RTC_WaitForLastTask();
  }
  
  /* Compute  hours */
  THH = TimeVar / 3600;
  /* Compute minutes */
  TMM = (TimeVar % 3600) / 60;
  /* Compute seconds */
  TSS = (TimeVar % 3600) % 60;

  printf("Time: %0.2d:%0.2d:%0.2d\r", THH, TMM, TSS);
}

/**
  * @brief  Shows the current time (HH:MM:SS) on the Hyperterminal.
  * @param  None
  * @retval None
  */   
void Time_Show(void)
{
  printf("\n\r");

  /* Infinite loop */
  while (1)
  {
    /* If 1s has been elapsed */
    if (TimeDisplay == 1)
    {
      /* Display current time */
      Time_Display(RTC_GetCounter());
      TimeDisplay = 0;
    }
  }
}


/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(EVAL_COM1, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_TC) == RESET)
  {}

  return ch;
}

/**
  * @brief  Gets numeric values from the hyperterminal.
  * @param  None
  * @retval None
  */
uint8_t USART_Scanf(uint32_t value)
{
  uint32_t index = 0;
  uint32_t tmp[2] = {0, 0};

  while (index < 2)
  {
    /* Loop until RXNE = 1 */
    while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_RXNE) == RESET)
    {}
    tmp[index++] = (USART_ReceiveData(EVAL_COM1));
    if ((tmp[index - 1] < 0x30) || (tmp[index - 1] > 0x39))
    {
      printf("\n\rPlease enter valid number between 0 and 9");
      index--;
    }
  }
  /* Calculate the Corresponding value */
  index = (tmp[1] - 0x30) + ((tmp[0] - 0x30) * 10);
  /* Checks */
  if (index > value)
  {
    printf("\n\rPlease enter valid number between 0 and %d", value);
    return 0xFF;
  }
  return index;
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
  {}
}

#endif

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
