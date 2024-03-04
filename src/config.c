#include <delay.h>
#include <gpio.h>
#include <stm32.h>
#include <string.h>
#include <stdbool.h>
#include "config.h"



void config() {
    //Włączamy taktowanie
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    
    //Włączamy taktowanie do obsługi DMA
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

    //Włączenie taktowania (z wykładu do obsługi przerwań)
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    __NOP();
    RedLEDoff();
    GreenLEDoff();
    BlueLEDoff();
    Green2LEDoff();

    GPIOoutConfigure(RED_LED_GPIO,
    RED_LED_PIN,
    GPIO_OType_PP,
    GPIO_Low_Speed,
    GPIO_PuPd_NOPULL);

    GPIOoutConfigure(BLUE_LED_GPIO,
    BLUE_LED_PIN,
    GPIO_OType_PP,
    GPIO_Low_Speed,
    GPIO_PuPd_NOPULL);

    GPIOoutConfigure(GREEN_LED_GPIO,
    GREEN_LED_PIN,
    GPIO_OType_PP,
    GPIO_Low_Speed,
    GPIO_PuPd_NOPULL);

    GPIOoutConfigure(GREEN2_LED_GPIO,
    GREEN2_LED_PIN,
    GPIO_OType_PP,
    GPIO_Low_Speed,
    GPIO_PuPd_NOPULL);

    //Konfiguracja przerwań dla wszystkich przycisków
    GPIOinConfigure(UP_BTN_GPIO, UP_BTN_PIN, GPIO_PuPd_UP,
                    EXTI_Mode_Interrupt,
                    EXTI_Trigger_Rising_Falling);

    GPIOinConfigure(DOWN_BTN_GPIO, DOWN_BTN_PIN, GPIO_PuPd_UP,
                    EXTI_Mode_Interrupt,
                    EXTI_Trigger_Rising_Falling);

    GPIOinConfigure(LEFT_BTN_GPIO, LEFT_BTN_PIN, GPIO_PuPd_UP,
                    EXTI_Mode_Interrupt,
                    EXTI_Trigger_Rising_Falling);

    GPIOinConfigure(RIGHT_BTN_GPIO, RIGHT_BTN_PIN, GPIO_PuPd_UP,
                    EXTI_Mode_Interrupt,
                    EXTI_Trigger_Rising_Falling);

    GPIOinConfigure(FIRE_BTN_GPIO, FIRE_BTN_PIN, GPIO_PuPd_UP,
                    EXTI_Mode_Interrupt,
                    EXTI_Trigger_Rising_Falling);

    GPIOinConfigure(MODE_BTN_GPIO, MODE_BTN_PIN, GPIO_PuPd_UP,
                    EXTI_Mode_Interrupt,
                    EXTI_Trigger_Rising_Falling);

    GPIOinConfigure(USER_BTN_GPIO, USER_BTN_PIN, GPIO_PuPd_UP,
                    EXTI_Mode_Interrupt,
                    EXTI_Trigger_Rising_Falling);

    //TODO wyzerować exti->pr

    //Konfiguracja UARTU (wyklad o DMA)
    GPIOafConfigure(GPIOA,
    2,
    GPIO_OType_PP,
    GPIO_Fast_Speed,
    GPIO_PuPd_NOPULL,
    GPIO_AF_USART2);

    GPIOafConfigure(GPIOA,
    3,
    GPIO_OType_PP,
    GPIO_Fast_Speed,
    GPIO_PuPd_UP,
    GPIO_AF_USART2);

    //Konfigurowanie UARTU cd (wyklad o DMA)
    //Parametry transmisji: 8 bitów, bez kontroli parzystości, 1 bit stopu
    USART2->CR1 = USART_CR1_TE;
    USART2->CR2 = 0;
    USART2->BRR = (PCLK1_HZ + (BAUD / 2U)) / BAUD;
    
    //Wysyłanie i odbieranie za pomocą DMA
    USART2->CR3 = USART_CR3_DMAT;
    
    //USART2 TX korzysta ze strumienia 6 i kanału 4, tryb
    //bezpośredni, transfery 8-bitowe, wysoki priorytet, zwiększanie
    //adresu pamięci po każdym przesłaniu, przerwanie po
    //zakończeniu transmisji
    DMA1_Stream6->CR = 4U << 25 |
        DMA_SxCR_PL_1 |
        DMA_SxCR_MINC |
        DMA_SxCR_DIR_0 |
        DMA_SxCR_TCIE;

    //Adres układu peryferyjnego
    DMA1_Stream6->PAR = (uint32_t)&USART2->DR;

    //To samo co w handlerze do zakończenia przesyłu
    DMA1->HIFCR = DMA_HIFCR_CTCIF6;

    //Włączenie przerwań od DMA
    NVIC_EnableIRQ(DMA1_Stream6_IRQn);

    //Włączenie przerwań zewnętrznych
    NVIC_EnableIRQ(EXTI15_10_IRQn);
    NVIC_EnableIRQ(EXTI3_IRQn);
    NVIC_EnableIRQ(EXTI4_IRQn);
    NVIC_EnableIRQ(EXTI9_5_IRQn);
    NVIC_EnableIRQ(EXTI0_IRQn);

    //Uaktywnij układ peryferyjny
    USART2->CR1 |= USART_CR1_UE;
}