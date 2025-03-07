
#include <stm32f10x_lib.h>                        // STM32F10x Library Definitions
#include "STM32_Init.h"                           // STM32 Initialization

#include <stdio.h>
#include<stdarg.h>



//#ifdef __GNUC__
//  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
//     set to 'Yes') calls __io_putchar() */
//  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
//#else
//  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
//#endif /* __GNUC__ */

	
	
int fputc(int ch, FILE *f)
{
	while(!(USART1->SR & (0x01 << 7)));
  USART1->DR = (unsigned char)ch;
	
  return ch;
}
	
	
//		USART1->DR = 0x30 + i;
	
/*----------------------------------------------------------*\
 | HARDWARE DEFINE                                          |
\*----------------------------------------------------------*/
#define LED             ( 1 << 5 )                // PB5: LED D2
#define LED6            ( 1 << 6 )                // PB5: LED D2
#define LED7            ( 1 << 7 )                // PB5: LED D2

//void Delay(vu32 nCount) {
//  for(; nCount != 0; nCount--);
//  }


unsigned 	int test=5;
	
//===============================
int i,j;

volatile unsigned int TimingDelay;	
volatile unsigned long sys_count;

unsigned long c_mic;
unsigned long p_mic1;
unsigned long p_mic2;

int led_toggle = 0;

int count_pwm = 0;
int pwm_duty = 20;

int count_servo = 0;
int servo_duty = 700;

unsigned long rising_time = 0;
unsigned long falling_time = 0;
unsigned long diff_time = 0;

unsigned long remocon_time[40];
unsigned int remocon_diff_time[40];
unsigned int remocon_data_bit[40];

int rem_count = 0;

int remocon_hex_data;
int remocon_flag = 0;
char remocon_data=0;

char rem_data[10] = {0x68,0x30,0x18,0x7a,0x10,0x38,0x5a,0x42,0x4a,0x52};
int remocon_value = 0;
int remocon_pre_value = 0;

unsigned short adc_value_x = 0;


void exit_callback_func();


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


void static_led_pwm()
{
	if(count_pwm == 100)
	{
		count_pwm = 0;
		GPIOA->BSRR = 0x01 << 5;
	}
	else if(count_pwm == pwm_duty)
	{
		GPIOA->BRR = 0x01 << 5;
	}
	
	count_pwm++;
}

void static_servo()
{
	if(count_servo == 625)
	{
		count_servo = 0;
		GPIOA->BSRR = 0x01 << 8;
	}
	else if(count_servo == servo_duty)
	{
		GPIOA->BRR = 0x01 << 8;
	}
	
	count_servo++;
}

// ISR Routine
//------------------------------------
void SysTick_Handler(void)
{
	TimingDelay_Decrement();
	sys_count++;
}

void EXTI0_IRQHandler(void)
{
	if (EXTI->PR & (1<<0)) {                       // EXTI0 interrupt pending?

		remocon_time[rem_count] = micros_10us() * 10;
		
		if(rem_count > 0){
			remocon_diff_time[rem_count-1] = remocon_time[rem_count] - remocon_time[rem_count-1];
			
			if(remocon_diff_time[rem_count-1] > 13000 && remocon_diff_time[rem_count-1] < 14000)
			{
				rem_count = 1;
			}
			if(remocon_diff_time[rem_count-1] > 11000 && remocon_diff_time[rem_count-1] < 12000)
			{
				rem_count = -1;
				remocon_value = remocon_pre_value;
			}			
		}

		
		rem_count++;
		if(rem_count == 34)
		{
			rem_count = 0;
			remocon_flag = 1;
		}
		
		EXTI->PR = (1<<0);                          // clear pending interrupt
	}
}

int toggle1 = 0;
int toggle2 = 0;

void EXTI1_IRQHandler(void)
{
	if (EXTI->PR & (1<<1)) {                       // EXTI0 interrupt pending?
		if(toggle1)
			GPIOA->ODR |= 0x01 << 11;
		else 
			GPIOA->ODR &= ~(0x01 << 11);
		
		toggle1 = !toggle1;
		
		//EXTI->PR = (1<<1);                          // clear pending interrupt
	}
}


//void EXTI2_IRQHandler(void)
//{
//	if (EXTI->PR & (1<<2)) {                       // EXTI0 interrupt pending?
//		if(toggle2)
//			GPIOA->ODR |= 0x01 << 12;
//		else 
//			GPIOA->ODR &= ~(0x01 << 12);
//		
//		toggle2 = !toggle2;
//		
//		EXTI->PR = (1<<2);                          // clear pending interrupt
//	}
//}



void EXTI2_IRQHandler(void)
{
	if (EXTI->PR & (1<<2)) {                       // EXTI0 interrupt pending?
		
		//printf("haha\r\n");
		
		EXTI->PR = (1<<2);                          // clear pending interrupt
	}
}


//EXTI3---------------------------------------------------------------------------------------------------------------------------
int rem_count1 = 0;
unsigned long falling_edge_timing[40];
unsigned long diff_falling_edge_timing[33];
int falling_edge_index = 0;
int value_bit[32];
unsigned char value_hex = 0;
unsigned char value_num[10] = {0x16, 0x0C, 0x18, 0x5E, 0x08, 0x1C, 0x5A, 0x42, 0x52, 0x4A};
unsigned char remocon_num = 0;
char exti_callback_flag = 0;
int servo_duty_1 = 70;
int servo_millis_enable = 0;

void EXTI3_IRQHandler(void)
{
	int i, j, rem_count = 0;
	
	
	if (EXTI->PR & (1<<3)) {                       // EXTI0 interrupt pending?
		//printf("%d \r\n", rem_count1++);
		
		falling_edge_timing[falling_edge_index] = micros_10us() * 10;
		if(falling_edge_index > 0)
		{
			int diff_time = 0;
			diff_falling_edge_timing[falling_edge_index - 1] = 
			falling_edge_timing[falling_edge_index] - falling_edge_timing[falling_edge_index - 1];
			
			diff_time = diff_falling_edge_timing[falling_edge_index - 1];
			
			if(diff_time > 13000 && diff_time < 14000)
			{
				//printf("Lead Code\r\n");
				falling_edge_index = 1;
			}
			else if(diff_time > 11000 && diff_time < 12000)
			{
				printf("%d\r\n", remocon_num);
				//printf("R\r\n");
				falling_edge_index = -1;
			}
		}
		
		
		falling_edge_index++;
		if(falling_edge_index == 34)
		{
			falling_edge_index = 0;
			exti_callback_flag = 1;
		}
		
		
		EXTI->PR = (1<<3);                          // clear pending interrupt
	}
}


void exit_callback_func()
{
	//printf("data_print\r\n");
			
			for(i = 0; i < 32; i++)
			{
				if(diff_falling_edge_timing[i + 1] > 1000 && diff_falling_edge_timing[i + 1] < 1500) value_bit[i] = 0;
				else if(diff_falling_edge_timing[i + 1] > 2000 && diff_falling_edge_timing[i + 1] < 2500) value_bit[i] = 1;
			}
			
//			for(i = 0; i <32; i++)
//			{
//				//printf("%d \r\n", (int)falling_edge_timing[i]);
//				printf("%d \r\n", (int)value_bit[i]);
//			}
			
			for(i = 0; i < 8; i++)
			{
				value_hex >>= 1;
				
				if(value_bit[i+16] == 1) 
				{
					value_hex |= 0x80;
				}
			}
			
			//printf("%02x\r\n", value_hex);
			
			for(i = 0; i< 10; i++)
			{
				if(value_num[i] == value_hex) 
				{
					remocon_num = i;
				}
			}
			
			printf("%d\r\n", remocon_num);
			
			if(remocon_num == 1) 
		{
			servo_millis_enable = 0;
			servo_duty_1 = 70;
		}
		else if(remocon_num == 2)
		{
			servo_millis_enable = 0;
			servo_duty_1 = 230;
		}
		else if(remocon_num == 3)
		{
			servo_millis_enable = 0;
			servo_duty_1 = 160;
		}
		else if(remocon_num == 4)
		{
			servo_millis_enable = 1;
		}
}

		
//void EXTI15_10_IRQHandler(void)
//{
//	if (EXTI->PR & (1<<13)) {                       // EXTI0 interrupt pending?

//		if( GPIOC->IDR & (0x01 << 13) )
//		{
//			rising_time = micros_10us();
//		}
//		else
//		{
//			falling_time = micros_10us();
//			diff_time = (falling_time - rising_time) * 10;
//		}
//		
//		
//		EXTI->PR = (1<<13);                          // clear pending interrupt
//	}
//}


void EXTI9_5_IRQHandler(void)
{
	if (EXTI->PR & (1<<7)) {                       // EXTI0 interrupt pending?

		printf("haha\r\n");
		
		
		EXTI->PR = (1<<7);                          // clear pending interrupt
	}
}


void EXTI15_10_IRQHandler(void)
{
	if (EXTI->PR & (1<<13)) {                       // EXTI0 interrupt pending?

		if( GPIOC->IDR & (0x01 << 13) )
		{
			printf("Button 6 : up\r\n");
		}
		else
		{
			printf("Button 6 : down\r\n");
		}
		
		
		EXTI->PR = (1<<13);                          // clear pending interrupt
	}
}


//void TIM2_IRQHandler (void) {

//	 if ((TIM2->SR & 0x0001) != 0) { // check update interrupt source
//			
//			//static_led_pwm();
//			GPIOA->BSRR = 0x01 << 5;
//	    
//			TIM2->SR &= ~(1<<0); // clear UIF flag
//	 }
//	 else if ((TIM2->SR & 0x0002) != 0) { // check capture compare interrupt source
//			
//			//static_led_pwm();
//			GPIOA->BRR = 0x01 << 5;
//	    
//			TIM2->SR &= ~(1<<1); // clear UIF flag
//	 }		
//}


//TIMER2--------------------------------------------------------------------------------------------------


//void TIM2_IRQHandler (void) {

//	 if ((TIM2->SR & 0x0001) != 0) { // check update interrupt source
//		 
//			GPIOA->ODR |= (0x01 << 4);
//			TIM2->CCR1 = servo_duty_1;
//		 
//			TIM2->SR &= ~(1<<0); // clear UIF flag
//	 }
//	 else if ((TIM2->SR & 0x0002) != 0) { // check capture compare interrupt source
//			
//			GPIOA->ODR &= ~(0x01 << 4);
//	    
//			TIM2->SR &= ~(1<<1); // clear CC1E flag
//	 }		
//}

#define rs_high() GPIOD->ODR |= (0x01 << 11);
#define rs_low() GPIOD->ODR &= ~(0x01 << 11);
#define rw_high() GPIOD->ODR |= (0x01 << 5);
#define rw_low() GPIOD->ODR &= ~(0x01 << 5);
#define backlight_on() GPIOE->ODR |= (0x01 << 5);
#define backlight_off() GPIOE->ODR &= ~(0x01 << 5);
#define bl_touch_high() GPIOC->ODR |= (0x01 << 6);
#define bl_touch_low() GPIOC->ODR &= ~(0x01 << 6);


int backlight_pwm_duty = 20;
int led_pwm_duty = 20;
int bl_touch_duty = 65;

void led_on();
void led_off();

char led_on_toggle = 0;

void TIM2_IRQHandler (void) {

	 if ((TIM2->SR & 0x0001) != 0) { // check update interrupt source
		 backlight_on();
		 TIM2->CCR1 = backlight_pwm_duty;
		 
		 if(led_on_toggle)
		 {
			 led_on();
			 TIM2->CCR2 = led_pwm_duty;
		 }
		 
		 TIM2->SR &= ~(1<<0); // clear UIF flag
	 }
	 else if ((TIM2->SR & 0x0002) != 0) { // check capture compare interrupt source
			backlight_off();
	    
			TIM2->SR &= ~(1<<1); // clear CC1E flag
	 }		
	 else if ((TIM2->SR & 0x0004) != 0) { // check capture compare interrupt source
		 if(led_on_toggle)
		 {
				led_off();
		 }
		 
			TIM2->SR &= ~(1<<2); // clear CC1E flag
	 }		
}



void TIM3_IRQHandler (void) {

	 if ((TIM3->SR & 0x0001) != 0) { // check update interrupt source
		 bl_touch_high();
		 TIM3->CCR1 = bl_touch_duty;
		 
		 TIM3->SR &= ~(1<<0); // clear UIF flag
	 }
	 else if ((TIM3->SR & 0x0002) != 0) { // check capture compare interrupt source
			bl_touch_low();
	    
			TIM3->SR &= ~(1<<1); // clear CC1E flag
	 }		
}


//void TIM3_IRQHandler (void) {

//	 if ((TIM3->SR & 0x0001) != 0) { // check update interrupt source
//			
//			//static_servo();
//			GPIOA->BSRR = 0x01 << 8;
//	    
//			TIM3->SR &= ~(1<<0); // clear UIF flag
//	 }	
//	 else if ((TIM3->SR & 0x0002) != 0) { // check capture compare interrupt source
//			
//			GPIOA->BRR = 0x01 << 8;
//	    
//			TIM3->SR &= ~(1<<1); // clear UIF flag
//	 }		 
//} 
//--------------------------------------------------------------------------------------------------------




char buff[20];
int count = 0;
char key_flag[5];
char *up_string[5] = {"Left UP", "Right UP", "Up UP", "Down UP", "Enter UP"};
char *down_string[5] = {"Left DOWN", "Right DOWN", "Up DOWN", "Down DOWN", "Enter DOWN"};
char *lcd_menu[6] = {"  1. LED On     ", 
															"  2. LED Off    ", 
															"  3. LED Pwm    ",
															"  4. SERVO Left ",
															"  5. SERVO Right", 
															"  6. SERVO Var  ", };

enum
{
	LEFT_BUTTON = 0,
	RIGHT_BUTTON,
	UP_BUTTON,
	DOWN_BUTTON,
	ENTER_BUTTON,
};

char enter_position = 0;

enum
{
	ENTER_POSITION_UPPER = 0,
	ENTER_POSITION_LOWER,
};

char menu_index = 0;
char enter_index = 0;


void enable_high_low()
{
	GPIOD->ODR |= (0x01 << 4);
	GPIOD->ODR &= ~(0x01 << 4);
}

void set_value_8bit(unsigned char value)
{
	GPIOE->ODR &= ~(0xFF << 8);
	GPIOE->ODR |= (value << 8);
}


void set_instruction_8bit(unsigned char inst)
{
	rs_low();
	set_value_8bit(inst);
	enable_high_low();
	rs_high();
	Delay(5); // 50us
}

void set_data_8bit(unsigned char data)
{
	set_value_8bit(data);
	enable_high_low();
	Delay(5); // 50us
}

void charLCD_init()
{
	backlight_on();
	rs_high();
	rw_low();
	
	set_instruction_8bit(0x30);
	set_instruction_8bit(0x30);
	set_instruction_8bit(0x30);
	set_instruction_8bit(0x38);
	set_instruction_8bit(0x06);
	set_instruction_8bit(0x0C);
	set_instruction_8bit(0x80);
	set_instruction_8bit(0x01); // Clear_all
	Delay(200);
}

void charLCD_string(char *string)
{
	int i;
	for(i = 0; i < strlen(string); i++)
	{
		set_data_8bit(string[i]);
	}
}

void set_cursor(char row, char col)
{
	if(row == 1)
	{
		set_instruction_8bit(0x80 + col);
	}
	else if(row == 2)
	{
		set_instruction_8bit(0xC0 + col);
	}
}

void printf_lcd(char *format, ...)
{
	char buf[512];
	va_list arglist;

	va_start(arglist, format);
	vsprintf(buf, format, arglist);
	va_end(arglist);
	charLCD_string(buf);
}

void enter_character(char upper_lower)
{
	if(upper_lower == ENTER_POSITION_UPPER)
	{
		set_cursor(2,0);
		printf_lcd(" ");
		set_cursor(1,0);
		printf_lcd(">");
	}
	else if(upper_lower == ENTER_POSITION_LOWER)
	{
		set_cursor(1,0);
		printf_lcd(" ");
		set_cursor(2,0);
		printf_lcd(">");
	}
	
}

void led_on()
{
	GPIOE->ODR |= (0x01 << 4);
}

void led_off()
{
	GPIOE->ODR &= ~(0x01 << 4);
}


typedef struct
{
	int a;
	int b;
	int c;
} aa;

typedef struct
{
	volatile unsigned int CRL;
	volatile unsigned int CRH;
	volatile unsigned int IDR;
	volatile unsigned int ODR;
	volatile unsigned int BSRR;
	volatile unsigned int BRR;
	volatile unsigned int LCKR;
} GPIO_Type;


#define GPIOAA (*(volatile unsigned int *)0x40010800)
#define GPIOAAA ((GPIO_Type *)0x40010800)


/*--------------------------------------------------------------------------------------------------------------------*\
 | MIAN ENTRY                                               																																																													|
\*--------------------------------------------------------------------------------------------------------------------*/
int main (void) {
//  	int a =5;
	int i = 0;
	
	char now_cursor = 0;
	
	unsigned long c_micros = 0, p_micros = 0;
	unsigned long curr_millis = 0, prev_millis_button = 0, prev_millis_pwm = 0, prev_millis_pwm_led = 0, prev_millis_bl_touch = 0;
	unsigned long prev_millis_bl_touch_switch = 0;
	int test_toggle = 0;
	char bl_touch_toggle = 0;
	char servo_toggle = 0;
	unsigned short in_bltouch_switch = 0;
	
	
	
	aa a; // 12byte
	aa *b; // 4byte
	
	GPIO_Type GPIOa;
	
	stm32_Init ();                                  // STM32 setup
	
	
	
//	//GPIOB
//	GPIOB->CRL &= ~(0x0F<< 5 * 4); // GPIOB:5
//	GPIOB->CRL |= (0x03 << 5  * 4); //  GPIOB:5
//	GPIOB->BSRR = 0x01 << 5; // BUZZER OFF
//	
//	//USART 1
//	RCC->APB2ENR |= (0x01 << 2) | (0x01 << 14);
//	GPIOA->CRH &= ~(0x0F << 2 * 4); // GPIO:9
//	GPIOA->CRH &= ~(0x0F << 3 * 4); // GPIO:10
//	GPIOA->CRH |= (0x0B << 1 * 4); // GPIO:9
//	GPIOA->CRH |= (0x04 << 2 * 4); // GPIO:10
//	USART1->CR1 = (0x01 << 2) | (0x01 << 3 )| (0x01 << 13); // RE, TE, UE
//	USART1->CR2 = 0x00;
//	USART1->CR3 = 0x00;
//	USART1->BRR = 0x271; // BRR:115200
//	NVIC->ISER[1] |= (0x01 << 5); // USART ON
//	//-------------------------------------------------------------------------------------------------------------
//	//2021-4-8
//	
//	b = (aa *)0x20002000;
//	b->a = 3;
//	b->b = 4;
//	b->c = 5;
//	
//	printf("add of a:%p\r\n", &a);
//	printf("add of b:%p\r\n", b);
//	
//	
//	
//	
//	
//	while(1);
	
	
	//-------------------------------------------------------------------------------------------------------------
	//2021-4-1, 2020-04-07
	
	
	//SysTick
	SysTick->LOAD = 720;
	SysTick->CTRL = 0x07;
	
	//RCC
	RCC->APB2ENR |= (0x01 << 3) | (0x01 << 5) | (0x01 << 6); // GPIOB, GPIOD, GPIOE
	
	//GPIOB
	GPIOB->CRL &= ~(0x0F<< 5 * 4); // GPIOB:5
	GPIOB->CRL |= (0x03 << 5  * 4); //  GPIOB:5
	GPIOB->BSRR = 0x01 << 5; // BUZZER OFF
	
	//GPIOD
	GPIOD->CRL &= ~(0x0F << 4 * 4); // GPIO:4 , EN
	GPIOD->CRL &= ~(0x0F << 5 * 4); // GPIO:5 , RW
	GPIOD->CRH &= ~(0x0F << 3 * 4); // GPIO:11 , RS
	GPIOD->CRL |= (0x03 << 4 * 4); // GPIO:4
	GPIOD->CRL |= (0x03 << 5 * 4); // GPIO:5
	GPIOD->CRH |= (0x03 << 3 * 4); // GPIO:1

	//GPIOE
	GPIOE->CRL &= ~(0x0F << 5 * 4); // GPIO:5
	GPIOE->CRH &= ~(0xFFFFFFFF); // CLEAR_ALL
	GPIOE->CRL |= (0x03 << 5 * 4); // GPIO:5
	GPIOE->CRH |= (0x33333333); // 
	
	//Key
	RCC->APB2ENR |= (0x01 << 4); // GPIOC
	GPIOC->CRL &= ~(0x0FFFFF); // GPIO:0 , left, GPIO:1 , right, GPIO:2 , up, GPIO:3 , down, GPIO:4 , enter
	GPIOC->CRL |= (0x044444); // GPIO:0 GPIO:1 GPIO:2  GPIO:3 GPIO:4
	
	//BL Touch
	GPIOC->CRL &= ~(0x0F << 6*4);
	GPIOC->CRL |= (0x03 << 6*4);
	
	//TIMER2
	RCC->APB1ENR |= (0x01 << 0);
	TIM2->CR1 = 0x01;
	TIM2->DIER = 0x03 | 0x04;
	TIM2->CNT = 0;
	TIM2->PSC = 71;
	TIM2->ARR = 1999;
	TIM2->CCMR1 = 0x6868;
	TIM2->CCER = 0x11;
	TIM2->CCR1 = 200;
	TIM2->CCR2 = 200;
	NVIC->ISER[0] |= 0x01 << 28; // TIM2 En
	
	//TIMER3
	RCC->APB1ENR |= (0x01 << 1);
	TIM3->CR1 = 0x01;
	TIM3->DIER = 0x03;
	TIM3->CNT = 0;
	TIM3->PSC = 719;
	TIM3->ARR = 1999;
	TIM3->CCMR1 = 0x68;
	TIM3->CCER = 0x01;
	TIM3->CCR1 = 70;
	NVIC->ISER[0] |= 0x01 << 29; // TIM3 En
	
	//PE.4
	GPIOE->CRL &= ~(0x0F << 4 * 4);
	GPIOE->CRL |= (0x03 << 4 * 4);
	
	// Char LCD
//	GPIOD->BRR = (0x01 << 5); // RW_LOW
//	GPIOD->BRR = (0x01 << 11); // RS_LOW
//	GPIOE->BSRR = (0x30 << 8); // PUSH_DATA
//	GPIOD->BSRR = (0x01 << 4); // EN_HIGH
//	GPIOD->BRR = (0x01 << 4); // EN_LOW
//	// data 타이밍 빨라서 조심 적용되는데 시간필요
//	GPIOD->BSRR = (0x01 << 11); // RS_HIGH
	
	
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
	
	//BlTouch switch
//	GPIOA->CRL &= ~(0x0f << 2);
//	GPIOA->CRL |= (0x04 << 2);
	GPIOC->CRL &= ~(0x0F << 7 * 4);
	GPIOC->CRL |= (0x04 << 7 * 4);
	
	//EIXT
	RCC->APB2ENR |= (0x01 << 0);
	EXTI->IMR = (0x01 << 7);
	EXTI->FTSR |= (0x01 << 7);
	AFIO->EXTICR[1] |= (0x02 << 3 * 4);
	
	NVIC->ISER[0] = (0x01 << 23);
	
	
	
	charLCD_init();
	
	
	
	//charLCD_string("hello_world");
	set_cursor(1,0);
	printf_lcd(lcd_menu[0]);
	set_cursor(2,0);
	printf_lcd(lcd_menu[1]);
	set_cursor(1,0);
	printf_lcd(">");
	
	printf("power_on");
	
	while(1)
	{
		curr_millis = micros_10us();
		
		
		if(curr_millis - prev_millis_button > 1000) // 10ms
		{
			prev_millis_button = curr_millis;
			
			
			for(i = 0; i < 5; i++)
			{
				if(GPIOC->IDR & (0x01 << i)) // up
				{
					if(key_flag[i] == 0)
					{
						key_flag[i] = 1;
						set_cursor(1,0);
						//printf_lcd(up_string[i]);
					}
				}
				else
				{ 
					if(key_flag[i] == 1) // down
					{
						key_flag[i] = 0;
						set_cursor(1,0);
						//printf_lcd(down_string[i]);
						
						if(i == DOWN_BUTTON)
						{
							if(enter_position == ENTER_POSITION_UPPER) 
							{
								enter_position = ENTER_POSITION_LOWER;
								enter_character(enter_position);
							}
							else if(enter_position == ENTER_POSITION_LOWER)
							{
								
								menu_index++;
								if(menu_index > 4) menu_index = 4;
								
								set_cursor(1,0);
								printf_lcd(lcd_menu[menu_index]);
								set_cursor(2,0);
								printf_lcd(lcd_menu[menu_index + 1]);
								enter_character(enter_position);
							}
							
							enter_index = enter_position + menu_index;
						}
						else if(i == UP_BUTTON)
						{
							if(enter_position == ENTER_POSITION_LOWER)
							{
								enter_position = ENTER_POSITION_UPPER;
								enter_character(enter_position);
							}
							else if(enter_position == ENTER_POSITION_UPPER && menu_index != 0)
							{
								menu_index--;
								if(menu_index < 0) menu_index = 0;
								
								set_cursor(1,0);
								printf_lcd(lcd_menu[menu_index]);
								set_cursor(2,0);
								printf_lcd(lcd_menu[menu_index + 1]);
								enter_character(enter_position);
							}
							
							enter_index = enter_position + menu_index;
						}
						else if(i == ENTER_BUTTON)
						{
							if(enter_index == 0)
							{
								led_on_toggle = 0;
								led_on();
							}
							else if(enter_index == 1)
							{
								led_on_toggle = 0;
								led_off();
							}
							else if(enter_index == 2)
							{
								led_on_toggle = 1;
							}
							else if(enter_index == 3)
							{
									servo_toggle = 0;
								 bl_touch_duty = 65;
							}
							else if(enter_index == 4)
							{
									servo_toggle = 0;
								 bl_touch_duty = 147;
							}
							else if(enter_index == 5)
							{
								servo_toggle = 1;
							}
						}
						
					}
				}
			}
			
		}
		
		if(curr_millis - prev_millis_pwm > 1000) // 10ms
		{
			prev_millis_pwm = curr_millis;
			
			backlight_pwm_duty+= 20;
			if(backlight_pwm_duty > 1980) backlight_pwm_duty = 20;
		}
		
		if(curr_millis - prev_millis_pwm_led > 1000)
		{
			prev_millis_pwm_led = curr_millis;
			
			led_pwm_duty += 60;
			if(led_pwm_duty > 1980) led_pwm_duty = 20;
		}
		
		if(servo_toggle)
		{
			if(curr_millis - prev_millis_bl_touch > 100000)
			{
				prev_millis_bl_touch = curr_millis;
				
				if(bl_touch_toggle == 0)
				{
					bl_touch_toggle = 1;
					bl_touch_duty = 65;
				}
				else
				{
					bl_touch_toggle = 0;
					bl_touch_duty = 147;
				}
			}
		}
		
		if(curr_millis - prev_millis_bl_touch_switch > 10000)  // 1ms
			{
				prev_millis_bl_touch_switch = curr_millis;
				
//				in_bltouch_switch = GPIOC->IDR;
//				if(in_bltouch_switch & (0x01 << 7))
//				{
//					printf("bltouch up\r\n");
//				}
//				else
//				{
//					printf("bltouch down\r\n");
//				}
			}
			
			
	}
	
	
	
	while(1)
	{
		set_cursor(1,0);
		printf_lcd("count = %d", count++);
		//set_data_8bit(0x31);
		Delay_ms(1000);
	}
	
	
	while(1)
	{
		GPIOE->BSRR = (0x01 << 5);
		Delay(50000);
		GPIOE->BRR = (0x01 << 5);
		Delay(50000);
	}
	
	
	
	while(1);
	
	
	//-------------------------------------------------------------------------------------------------------------
	//2021-3-31
	
	
	//SysTick
	SysTick->LOAD = 720;
	SysTick->CTRL = 0x07;
	
	RCC->APB2ENR |= (0x01 << 2) | (0x01 << 3) | (0x01 << 6) | (0x01 << 14); // GPIOA, GPIOB, GPIOE, USART1
	
	//GPIOA
	GPIOA->CRH &= ~(0x0F << 2 * 4); // GPIO:9
	GPIOA->CRH &= ~(0x0F << 3 * 4); // GPIO:10
	GPIOA->CRH |= (0x0B << 1 * 4); // GPIO:9
	GPIOA->CRH |= (0x04 << 2 * 4); // GPIO:10
	
	//GPIOB
	GPIOB->CRL &= ~(0x0F<< 5 * 4); // GPIOB:5
	GPIOB->CRL |= (0x03 << 5  * 4); //  
	
	//GPIOE
	GPIOE->CRL &= ~(0x0F<< 2 * 4); // GPIOE:2
	GPIOE->CRL |= (0x03 << 2  * 4); //
	
	//USART 1
	USART1->CR1 = (0x01 << 2) | (0x01 << 3 )| (0x01 << 13); // RE, TE, UE
	USART1->CR2 = 0x00;
	USART1->CR3 = 0x00;
	USART1->BRR = 0x271; // BRR:115200
	
	
	GPIOB->BSRR = 0x01 << 5; // BSRR:5 ODR에서 HIGH LOW 분리한느낌
	
	
	while(1)
	{
		//printf("hehe \r\n");
		//Delay(100000);
	}
	
	while(1)
	{
		GPIOE->BSRR = (0x01 << 2);
		Delay(100000);
		GPIOE->BRR = (0x01 << 2);
		Delay(100000);
	}
	
	

	while(1);
	
	
	//-------------------------------------------------------------------------------------------------------------
	
	
	
	//SysTick
	SysTick->LOAD = 720;
	SysTick->CTRL = 0x07;
	
	
	//RCC clock
	RCC->APB2ENR |= (0x01 << 2) | (0x01 << 14); // GPIOA, USART1
	RCC->APB1ENR |= (0x01 << 0); // TIM2 clock
	
	
	//GPIOA
	GPIOA->CRH &= (~0x0F << 4); // CLEAR:9
	GPIOA->CRH &= (~0x0F << 8); // CLEAR:10
	GPIOA->CRH |= (0x0B<< 4) | (0x04 << 8); // Altemate:9, Floatin:10
	
	GPIOA->CRL &= ~(0x0F << 4* 4);
	GPIOA->CRL |= (0x03 << 4 * 4);
	
	
	//USART 1
	USART1->CR1 = (0x01 << 2) | (0x01 << 3 )| (0x01 << 13); // RE, TE, UE
	USART1->CR2 = 0x00;
	USART1->CR3 = 0x00;
	USART1->BRR = 0x271; // BRR:115200
	
	
	//EXTI 3
	EXTI->IMR |= (0x01 << 3);
	EXTI->FTSR |= (0x01 << 3);
	AFIO->EXTICR[0] |= (0x00 << 3);  // PA.0 interrupt
	
	
	
	//TIMER2
	TIM2->CR1 = (0x01 << 0); // CEN
	TIM2->CR2 = 0x00;
	TIM2->CNT = 0;
	TIM2->PSC = 720 - 1;
	TIM2->ARR = 2000 - 1;
	TIM2->DIER = (0x01 << 0 )| (0x01 << 1); // UIE, CC1IE
	TIM2->CCMR1 = 0x68; // CC1S, PWMmode
	TIM2->CCER = (0x01 << 0); // CC1E
	TIM2->CCR1 = 70;
	
	
	
	//NVIC
	NVIC->ISER[0] |= 0x01 << 9; //EXTI3 interrupt
	NVIC->ISER[0] |= 0x01 << 28; // TIM2 En
	
	
	
	
//USART1->DR = 0x33; // DATA:1
	
	
	
//	for(i = 0; i < 10; i++) {
//		while(!(USART1->SR & (0x01 << 7)));
//		USART1->DR = 0x30 + i;
//	}
	
	
//	for(i = 0; i < 10; i++) {
//		printf("%d\r\n", i);
//	}
	
	printf("\r\ninit_finish\r\n");
	
	
	while(1) 
	{
		c_micros = micros_10us();
		if(c_micros - p_micros > 100 * 20 && servo_millis_enable == 1) // 100000 == 1s
		{
			p_micros = c_micros;
			
			servo_duty_1++;
			if(servo_duty_1 > 230) servo_duty_1 = 70;
			
//			if(test_toggle == 0) 
//			{
//				//GPIOA->ODR |= (0x01 << 4);
//				servo_duty_1 = 70;
//				test_toggle = 1;
//			}
//			else 
//			{
//				//GPIOA->ODR &= ~(0x01 << 4);
//				servo_duty_1 = 230;
//				test_toggle = 0;
//			}
		}
		
		//----------------------------------
		//Remocon
		
		if(exti_callback_flag == 1)
		{
			exti_callback_flag = 0;
			exit_callback_func();
		}
	}
	
	
	while(1);
	
	
	//--------------------------------------------------------------------------------------
	
	
	
	
	// systick
	SysTick->LOAD = 720;
	SysTick->CTRL = 0x07;

	//clock
	RCC->APB2ENR |= 0x01 << 2;  // GPIOA clock enable
	RCC->APB2ENR |= 0x01 << 4;  // GPIOC clock enable
	RCC->APB2ENR |= 0x01 << 9;  // ADC1 clock enable	
	RCC->APB1ENR |= 0x01 << 0; // timer2 clock enable
	RCC->APB1ENR |= 0x01 << 1; // timer3 clock enable
	RCC->AHBENR |= 0x01 << 0; // DMA1 clock enable
	
	//GPIOA.5 : GPIO output push/pull 50MHz
	GPIOA->CRL &= ~(0x0f << 20);
	GPIOA->CRL |= 0x03 << 20;
	//GPIOA.8 : GPIO output push/pull 50MHz
	GPIOA->CRH &= ~(0x0f << 0);
	GPIOA->CRH |= 0x03 << 0;
	//GPIOC.0 : GPIO floating Input mode 
	GPIOC->CRL &= ~(0x0f << 0);
	GPIOC->CRL |= 0x04 << 0;	
	//GPIOC.13 : GPIO floating Input mode 
	GPIOC->CRH &= ~(0x0f << 24);
	GPIOC->CRH |= 0x04 << 24;		
	//GPIOA.0 : GPIO analog input
	GPIOA->CRL &= ~(0x0f << 0);
	GPIOA->CRL |= 0x00 << 0;	
	
	
	//EXTI 0
	EXTI->IMR |= 0x01;
	EXTI->FTSR |= 0x01;
	AFIO->EXTICR[0] |= 0x02;  // PC.0 interrupt

	// EXTI 13
	EXTI->IMR |= 0x01 << 13;
	EXTI->FTSR |= 0x01 << 13;
	EXTI->RTSR |= 0x01 << 13;
	AFIO->EXTICR[3] |= 0x20;  // PC.13 interrupt

	//timer2
	TIM2->CR1 |= 0x01;  // timer2 count enable
	TIM2->PSC = 72;  // 72MHz / 72 = 1MHz
	TIM2->ARR = 2000;
	TIM2->CNT = 0x00;
	TIM2->DIER |= 0x03; // update/capture compare interrupt enable
	TIM2->CCMR1 = 0x60;
	TIM2->CCR1 = 200;

	//timer3
	TIM3->CR1 |= 0x01;  // timer3 count enable
	TIM3->PSC = 72;  // 72MHz / 72 = 1MHz
	TIM3->ARR = 20000;
	TIM3->CNT = 0x00;
	TIM3->DIER |= 0x03; // update/capture compare interrupt enable
	TIM3->CCMR1 = 0x60;
	TIM3->CCR1 = 2000;
	
	
	//DMA
	DMA1_Channel1->CNDTR = 0x01;
	DMA1_Channel1->CPAR = 0x4001244c;  // adc data register
	DMA1_Channel1->CMAR = (unsigned int)&adc_value_x;
	DMA1_Channel1->CCR = 0x521;
	
	// ADC
	ADC1->CR1 = 0x100;
	ADC1->CR2 = 0x1E0103;
	ADC1->SMPR1 = 0x00;
	ADC1->SMPR2 = 0x05;
	
	
	//NVIC
	NVIC->ISER[0] |= 0x01 << 28; //timer2 interrupt enable
	NVIC->ISER[0] |= 0x01 << 29; //timer3 interrupt enable
	
	NVIC->ISER[0] |= 0x01 << 6; //EXTI0 interrupt enable
	NVIC->ISER[1] |= 0x01 << (40-32); //EXTI15_10 interrupt enable

//=================================
// Servo/LED pwm variable duty

	while(1)
	{
		c_mic = micros_10us();
		if(c_mic - p_mic1 > 1000)
		{
			p_mic1 = c_mic;
			
			pwm_duty+=20;
			if(pwm_duty > 1999) pwm_duty = 20;	

			TIM2->CCR1 = pwm_duty;			
		}	

		if(c_mic - p_mic2 > 2000)
		{
			p_mic2 = c_mic;
			
			servo_duty+=16;
			if(servo_duty > 2300) servo_duty = 700;	
			TIM3->CCR1 = servo_duty;
			
		}			
		
		
		//------------------------------------------
		// Remocon
			if(remocon_flag == 1)
			{
				remocon_flag = 0;
				
				for(i=0;i<32;i++){
					if(remocon_diff_time[i+1] > 1000 && remocon_diff_time[i+1] < 1500)
					{
						remocon_data_bit[i] = 0;
					}
					else if(remocon_diff_time[i+1] > 2000 && remocon_diff_time[i+1] < 2500)
					{
						remocon_data_bit[i] = 1;
					}						
					
				}
				
				remocon_hex_data = 0;
				for(i=0;i<8;i++)
				{
					remocon_hex_data = remocon_hex_data << 1;
					if(remocon_data_bit[16+i]  == 1){
						remocon_hex_data |= 0x01;
					}
				}
				
				remocon_data = (char)remocon_hex_data;
				
				remocon_value = 0;
				for(i=0;i<10;i++)
				{
					if(remocon_data == rem_data[i])
					{
						remocon_value = i;
						remocon_pre_value= i;
						break;
					}						
				}
			}
		
	}

//=================================
// LED pwm variable duty

//	while(1)
//	{
//		c_mic = micros_10us();
//		if(c_mic - p_mic1 > 1000)
//		{
//			p_mic1 = c_mic;
//			
//			pwm_duty+=20;
//			if(pwm_duty > 1999) pwm_duty = 20;	

//			TIM2->CCR1 = pwm_duty;			
//		}		
//	}

//=================================
// LED pwm variable duty

//	while(1)
//	{
//		c_mic = micros_10us();
//		if(c_mic - p_mic1 > 1000)
//		{
//			p_mic1 = c_mic;
//			
//			pwm_duty++;
//			if(pwm_duty == 99) pwm_duty = 1;				
//		}		
//	}
	
//=================================
// LED toggle

//	while(1)
//	{
//		c_mic = micros_10us();
//		if(c_mic - p_mic1 > 10000)
//		{
//			p_mic1 = c_mic;
//			
//			if(led_toggle == 0){
//				led_toggle = 1;
//				GPIOA->BSRR = 0x01 << 5;
//			}
//			else if(led_toggle == 1){
//				led_toggle = 0;
//				GPIOA->BRR = 0x01 << 5;
//			}
//				
//		}
//	}
	
//=====================================================	
	
//	while(1)
//	{
//		GPIOA->ODR |= 0x01 << 5;
//		//GPIOA->BSRR = 0x01 << 5;
//		
//		Delay(100);
//		
//		//GPIOA->ODR &= ~(0x01 << 5);
//		GPIOA->BRR = 0x01 << 5;
//		
//		Delay(100);		
//	}

	
	
	//====================================================
	
	


	
				*(volatile unsigned int *)0x40021018 |= 0x08;
	
	RCC->APB2ENR |= 0x04;
	
		GPIOB->CRL = 0x33333333;
		GPIOB->ODR = 0x20;
	
		GPIOB->ODR |= 0xC0;
	
	
//				*(volatile unsigned int *)0x40011004 &= ~(0x0f << 20);
//	 *(volatile unsigned int *)0x40011004 |= (0x04 << 20);
	
//	
//				*(volatile unsigned int *)0x40010800 = 0x03;
	
	//GPIOB->CRL &= ~(0x0f << 28);
	//GPIOB->CRL |= (0x04 << 28);
	
 for(;;) {


//			*(volatile unsigned int *)0x4001100c = 0x00;	 
//			Delay(20000);
//			*(volatile unsigned int *)0x4001100c = 0x2000;
			Delay(20000);		
		
	// test=GPIOB->IDR;
		
//    GPIOB->ODR &= ~LED;                           // switch on LED
//    GPIOB->ODR &= ~LED6;                           // switch on LED
//    GPIOB->ODR |= LED7;                           // switch on LED
//	Delay(2000000);
//    GPIOB->ODR |=  LED;                           // switch off LED
//    GPIOB->ODR |=  LED6;                           // switch on LED
//    GPIOB->ODR &=  ~LED7;                           // switch on LED
	//Delay(2000000);

    }
   
  }
/*----------------------------------------------------------*\
 | END OF FILE                                              |
\*----------------------------------------------------------*/
