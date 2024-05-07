#include <stdint.h>
#include <FreeRTOS.h>
#include <task.h>
#include "tm4c123gh6pm.h"


/* led macros */
#define RED_LED 0x02
#define BLUE_LED 0x04
#define GREEN_LED 0x08

/* port macros */
#define PORT_A 0x01
#define PORT_B 0x02
#define PORT_C 0x04
#define PORT_D 0x08
#define PORT_E 0x10
#define PORT_F 0x20


void port_init(uint8_t _port)
{
	if( (_port == PORT_A) || (_port == PORT_B) || (_port == PORT_C) || (_port == PORT_D) || (_port == PORT_E) || (_port == PORT_F) )
	{
		/* enable clock for port */
		SYSCTL_RCGCGPIO_R |= _port;  

		/* wait until enable clock is done */
		// while( (SYSCTL_PRGPIO_R & _port) != _port);
	}
}


void led_init(uint8_t _led)
{
	if( (_led == RED_LED) || (_led == BLUE_LED) || (_led == GREEN_LED))
	{
		/* unlock port F */
		GPIO_PORTF_LOCK_R = 0x4C4F434B;
		

		GPIO_PORTF_CR_R |= _led;
		
		/* config PF1,PF2,PF3 as digitial */
		GPIO_PORTF_DIR_R |= _led;
		
		/* config PF1,PF2,PF3 as output */
		GPIO_PORTF_DEN_R |= _led;
		
		GPIO_PORTF_AMSEL_R &= ~_led;
		GPIO_PORTF_AFSEL_R &= ~_led;
		GPIO_PORTF_PCTL_R  &= ~0x0000FFF0;  
		
		/*write 0 in PF1,PF2,PF3 */
		GPIO_PORTF_DATA_R &= ~_led;
	}
}



void led_on(uint8_t _led) { GPIO_PORTF_DATA_R = (GPIO_PORTF_DATA_R & ~0x0E ) | _led ; }

void led_off(uint8_t _led) { GPIO_PORTF_DATA_R &= ~_led; }

void led_toggle(uint8_t _led) { GPIO_PORTF_DATA_R ^= _led; }



uint32_t SystemCoreClock = 16000000;

void vPeriodicTask(void *pvParameters)
{
	// Establish the task's period.
	const TickType_t xDelay = pdMS_TO_TICKS(1000);
	TickType_t xLastWakeTime = xTaskGetTickCount();
	for (;;) {

		led_toggle(BLUE_LED);
		// Block until the next release time.
		vTaskDelayUntil(&xLastWakeTime, xDelay);
		//vTaskDelay(1000); 
	}

}	



int main()
{
	port_init(PORT_F);
	led_init(RED_LED);
	led_init(BLUE_LED);
	
;
	
	xTaskCreate(vPeriodicTask, "My Task", 256, NULL, 1, NULL);

	// Startup of the FreeRTOS scheduler.  The program should block here.  
	vTaskStartScheduler();
	
	// The following line should never be reached.  Failure to allocate enough
	//	memory from the heap would be one reason.
	for (;;);
	
}

