#include "servo.h"

int servo_count = 0;
int servo_duty = 1;

void servo_init()
{
  DDRH |= SERVO_PIN;
  TCCR1A = 0x00;
  TCCR1B = 0x0b;
  OCR1A = 25;
  TIMSK1 = 0x00;
}

void servo_start()
{
  TIMSK1 = 0x02;
}

void servo_stop()
{
  servo_count = 0;
  servo_duty = 1;
  TIMSK1 = 0x00;
}

void servo_change(char get_duty)
{
  servo_duty = get_duty;
}
unsigned long p_mi = 0;
SIGNAL(TIMER1_COMPA_vect)
{
//  Serial.print(micros());
//  Serial.print(" ");
//  Serial.println(p_mi);
//  p_mi = micros();
  servo_count++;
  if(servo_count == 100)
  {
    servo_count = 0;
    PORTH |= SERVO_PIN;
  }
  else if(servo_count == servo_duty)
  {
    PORTH &= ~SERVO_PIN;
  }
}
