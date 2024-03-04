#include <delay.h>
#include <gpio.h>
#include <stm32.h>
#include <string.h>
#include <stdbool.h>
#include "irq.h"
#include "config.h"


#define UP_BTN      0
#define LEFT_BTN    1
#define RIGHT_BTN   2
#define DOWN_BTN    3
#define FIRE_BTN    4
#define USER_BTN    5
#define MODE_BTN    6

#define UP_MSG_LEN_PRESSED     12
#define RIGHT_MSG_LEN_PRESSED  15
#define USL_MSG_LEN_PRESSED    14
#define UP_MSG_LEN_RELEASED     13
#define RIGHT_MSG_LEN_RELEASED  16
#define USL_MSG_LEN_RELEASED    15

#define TX_BUFFER_SIZE 64
#define BUTTONS_COUNT 7


struct msgAndLen {
    int len;
    char* msg;
};

struct msgAndLen txBuffer[TX_BUFFER_SIZE];

int txBufferHead = 0;     
int txBufferTail = 0;     


const char* buttonPressedMessages[BUTTONS_COUNT] = {
    "UP PRESSED\n\r",
    "LEFT PRESSED\n\r",
    "RIGHT PRESSED\n\r",
    "DOWN PRESSED\n\r",
    "FIRE PRESSED\n\r",
    "USER PRESSED\n\r",
    "MODE PRESSED\n\r"
};

const char* buttonReleasedMessages[BUTTONS_COUNT] = {
    "UP RELEASED\n\r",
    "LEFT RELEASED\n\r",
    "RIGHT RELEASED\n\r",
    "DOWN RELEASED\n\r",
    "FIRE RELEASED\n\r",
    "USER RELEASED\n\r",
    "MODE RELEASED\n\r"
};


void handle_end_of_transfer() {
    DMA1->HIFCR = DMA_HIFCR_CTCIF6;
}


bool corect_type_of_interrupt(){
    // Odczytaj zgłoszone przerwania DMA1. 
    uint32_t isr = DMA1->HISR;
    return (isr & DMA_HISR_TCIF6);
}


bool canTransferBeInitiated() {
    return (DMA1_Stream6->CR & DMA_SxCR_EN) == 0 && (DMA1->HISR & DMA_HISR_TCIF6) == 0;
}


int readButtonState(GPIO_TypeDef* gpio, uint32_t pin) {
    if(pin == MODE_BTN_PIN) {
        return ((gpio->IDR >> pin) & 1);
    }
    return !((gpio->IDR >> pin) & 1);
}


void addToQueue(const char* message, const int len) {
    struct msgAndLen currectMsg;
    currectMsg.len = len;
    currectMsg.msg = message;
    txBuffer[txBufferHead] = currectMsg;
    txBufferHead = (txBufferHead + 1) % TX_BUFFER_SIZE; 
}


void sendUsingDMA(const char* message, const int len) {
    DMA1_Stream6->M0AR = (uint32_t)message;
    DMA1_Stream6->NDTR = len;
    DMA1_Stream6->CR |= DMA_SxCR_EN;
}


int lenOfMsgPressed(int btnId) {
    // Pasuje do wszystkich poza UP i RIGHT.
    int len = USL_MSG_LEN_PRESSED; 
    
    switch(btnId) {
        case UP_BTN:
            len = UP_MSG_LEN_PRESSED;
        case RIGHT_BTN:
            len = RIGHT_MSG_LEN_PRESSED;
    } 
    
    return len;
}


int lenOfMsgReleased(int btnId) {
    // Pasuje do wszystkich poza UP i RIGHT.
    int len = USL_MSG_LEN_RELEASED; 
    
    switch(btnId) {
        case UP_BTN:
            len = UP_MSG_LEN_RELEASED;
        case RIGHT_BTN:
            len = RIGHT_MSG_LEN_RELEASED;
    } 
    
    return len;
}


void handle_btn_interruption(GPIO_TypeDef* gpio, uint32_t pin, int btnId) {
    if(readButtonState(gpio, pin)) {
        if(canTransferBeInitiated()) {
            sendUsingDMA(buttonPressedMessages[btnId], lenOfMsgPressed(btnId));
        }
        else {
            addToQueue(buttonPressedMessages[btnId], lenOfMsgPressed(btnId));
        }
    }
    else {
        if(canTransferBeInitiated()) {
            sendUsingDMA(buttonReleasedMessages[btnId], lenOfMsgReleased(btnId));
        }
        else {
            addToQueue(buttonReleasedMessages[btnId], lenOfMsgReleased(btnId));
        }
    }
}


void EXTI15_10_IRQHandler(void) {
    //fire 10
    //user 13
    int pr = EXTI->PR;
    if (pr & EXTI_PR_PR13) {
        EXTI->PR = EXTI_PR_PR13;
        handle_btn_interruption(USER_BTN_GPIO, USER_BTN_PIN, USER_BTN);
    }
    if (pr & EXTI_PR_PR10) {
        EXTI->PR = EXTI_PR_PR10;
        handle_btn_interruption(FIRE_BTN_GPIO, FIRE_BTN_PIN, FIRE_BTN);
    }
}


void EXTI9_5_IRQHandler(void) {
    //up 5
    //down 6
    int pr = EXTI->PR;
    if (pr & EXTI_PR_PR5) {
        EXTI->PR = EXTI_PR_PR5;
        handle_btn_interruption(UP_BTN_GPIO, UP_BTN_PIN, UP_BTN);
    }
    if (pr & EXTI_PR_PR6) {
        EXTI->PR = EXTI_PR_PR6;
        handle_btn_interruption(DOWN_BTN_GPIO, DOWN_BTN_PIN, DOWN_BTN);
    }
}


void EXTI3_IRQHandler(void) {
    //left
    EXTI->PR = EXTI_PR_PR3;
    handle_btn_interruption(LEFT_BTN_GPIO, LEFT_BTN_PIN, LEFT_BTN);
}


void EXTI4_IRQHandler(void) {
    //right
    EXTI->PR = EXTI_PR_PR4;
    handle_btn_interruption(RIGHT_BTN_GPIO, RIGHT_BTN_PIN, RIGHT_BTN);
}


void EXTI0_IRQHandler(void) {
    //mode
    EXTI->PR = EXTI_PR_PR0;
    handle_btn_interruption(MODE_BTN_GPIO, MODE_BTN_PIN, MODE_BTN);
}


//Rozwiązanie z buforem cyklicznym
void DMA1_Stream6_IRQHandler(void) {
    // Odczytaj zgłoszone przerwania DMA1. 
    uint32_t isr = DMA1->HISR;

    if (isr & DMA_HISR_TCIF6) {
        // Obsłuż zakończenie transferu
        // w strumieniu 6. 
        DMA1->HIFCR = DMA_HIFCR_CTCIF6;
        
        // Jeśli jest coś do wysłania,
        // wystartuj kolejną transmisję. 
        if(txBufferHead != txBufferTail) {
            sendUsingDMA(txBuffer[txBufferTail].msg, txBuffer[txBufferTail].len);
            txBufferTail = (txBufferTail + 1) % TX_BUFFER_SIZE;
        }
    }
}


int main(void) {

    config();

    for (;;) {}

}
