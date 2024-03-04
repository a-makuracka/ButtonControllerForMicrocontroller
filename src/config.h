#ifndef CONFIG_H
#define CONFIG_H

#define RED_LED_GPIO GPIOA
#define GREEN_LED_GPIO GPIOA
#define BLUE_LED_GPIO GPIOB
#define GREEN2_LED_GPIO GPIOA
#define RED_LED_PIN 6
#define GREEN_LED_PIN 7
#define BLUE_LED_PIN 0
#define GREEN2_LED_PIN 5

#define UP_BTN_GPIO GPIOB
#define UP_BTN_PIN 5
#define DOWN_BTN_GPIO GPIOB
#define DOWN_BTN_PIN 6
#define LEFT_BTN_GPIO GPIOB
#define LEFT_BTN_PIN 3
#define RIGHT_BTN_GPIO GPIOB
#define RIGHT_BTN_PIN 4
#define USER_BTN_GPIO GPIOC
#define USER_BTN_PIN 13
#define FIRE_BTN_GPIO GPIOB
#define FIRE_BTN_PIN 10
#define MODE_BTN_GPIO GPIOA
#define MODE_BTN_PIN 0

#define SCL_GPIO GPIOB
#define SDA_GPIO GPIOB
#define SCL_PIN 8
#define SDA_PIN 9

#define I2C_SPEED_HZ 100000
#define PCLK1_MHZ 16

#define OUT_X 0x29
#define OUT_Y 0x2B
#define OUT_Z 0x2D

#define RedLEDon() \
RED_LED_GPIO->BSRR = 1 << (RED_LED_PIN + 16)
#define RedLEDoff() \
RED_LED_GPIO->BSRR = 1 << RED_LED_PIN

#define BlueLEDon() \
BLUE_LED_GPIO->BSRR = 1 << (BLUE_LED_PIN + 16)
#define BlueLEDoff() \
BLUE_LED_GPIO->BSRR = 1 << BLUE_LED_PIN

#define GreenLEDon() \
GREEN_LED_GPIO->BSRR = 1 << (GREEN_LED_PIN + 16)
#define GreenLEDoff() \
GREEN_LED_GPIO->BSRR = 1 << GREEN_LED_PIN


#define Green2LEDon() \
GREEN2_LED_GPIO->BSRR = 1 << GREEN2_LED_PIN
#define Green2LEDoff() \
GREEN2_LED_GPIO->BSRR = 1 << (GREEN2_LED_PIN + 16)

#define USART_Mode_Rx_Tx (USART_CR1_RE | USART_CR1_TE)
#define USART_Enable USART_CR1_UE

#define USART_WordLength_8b 0x0000
#define USART_WordLength_9b USART_CR1_M

#define USART_Parity_No 0x0000
#define USART_Parity_Even USART_CR1_PCE
#define USART_Parity_Odd (USART_CR1_PCE | USART_CR1_PS)

#define USART_StopBits_1 0x0000
#define USART_StopBits_0_5 0x1000
#define USART_StopBits_2 0x2000
#define USART_StopBits_1_5 0x3000

#define USART_FlowControl_None 0x0000
#define USART_FlowControl_RTS USART_CR3_RTSE
#define USART_FlowControl_CTS USART_CR3_CTSE

#define HSI_HZ 16000000U
#define PCLK1_HZ HSI_HZ
#define BAUD 9600U

void config();

#endif