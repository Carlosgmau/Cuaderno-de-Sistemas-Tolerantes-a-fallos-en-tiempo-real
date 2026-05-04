/*
Creado por Javier Burgoa el 21/10/24
Problema de Inversión de Prioridad



*/

/********************************************************/
// INTERESA VER LA PRIMERA PARTE, JUSTO DESPUES DEL RESET
/********************************************************/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "freertos/semphr.h"

#define ITER_L   1000000      
#define ITER_M   20000000    
#define ITER_H   100000      

 
#define STACK_SIZE	2*1024	  //n x 1kByte es el tamaño de la pila  

static SemaphoreHandle_t lock;

//----------------------Tarea de prioridad baja -----------------------------------------------------------
void TaskLowPrio( void * pvParameters )
{
    const TickType_t 	xDelayTicks = 500/portTICK_PERIOD_MS;

    while(1)
    {
       printf("\tS\t\t\t\t\t%llu ms\n", esp_timer_get_time()/1000 );                //start task

       xSemaphoreTake(lock, portMAX_DELAY);                                      //take semaphore. I need the resource for myself only           
       
       printf("\tI\t\t\t\t\t%llu ms\n", esp_timer_get_time()/1000 );                   //notify task is doing something     
     
       for (long i = 0; i < ITER_L; i++) {   __asm__ __volatile__("NOP");    }   //Consume CPU cycles
       
       printf("\tO\t\t\t\t\t%llu ms\n", esp_timer_get_time()/1000 );             //notify task has ended doing usefull NOP's

       xSemaphoreGive(lock);                                                     //release resource

       printf("\tX\t\t\t\t\t%llu ms\n", esp_timer_get_time()/1000 );             // task -> bloqueo

       vTaskDelay(xDelayTicks);
    }	
}

//----------------------Tarea de prioridad media -----------------------------------------------------------
void TaskMedPrio( void * pvParameters )
{
    const TickType_t 	xDelayTicks = 10000/portTICK_PERIOD_MS;
    
    while(1)
    {
       printf("\t\t\tI\t\t\t%lu ms\n",  xTaskGetTickCount() );                 //start task
       
       for (long i = 0; i < ITER_M; i++) {  __asm__ __volatile__("NOP");   }   //Consume CPU cycles

       printf("\t\t\tO\t\t\t%lu ms\n",  xTaskGetTickCount() );                 //notify task has ended doing usefull NOP's
 
       vTaskDelay(xDelayTicks);
    }	
}

//----------------------Tarea de prioridad alta -----------------------------------------------------------
void TaskHighPrio( void * pvParameters )
{
    
    const TickType_t 	xDelayTicks = 500/portTICK_PERIOD_MS;
    
    while(1)
    {
       printf("\t\t\t\t\tS\t%llu ms\n", esp_timer_get_time()/1000 );           //start task

       xSemaphoreTake(lock, portMAX_DELAY);                                    //take semaphore. I need the resource for myself only 

       printf("\t\t\t\t\tI\t%lu ms\n", xTaskGetTickCount() );           //notify task is doing something 

       for (long i = 0; i < ITER_H; i++) {    __asm__ __volatile__("NOP");    } //Consume CPU cycles

       printf("\t\t\t\t\tO\t%lu ms\n", xTaskGetTickCount() );            //notify task has ended doing usefull NOP's

       xSemaphoreGive(lock);                                                    //release resource 

       printf("\t\t\t\t\tX\t%llu ms\n", esp_timer_get_time()/1000 );             // task -> bloqueo

       vTaskDelay(xDelayTicks);
    }	
}
//-----------------------------------------------------------------------------------------
void app_main(void) 
{   
    printf("\tL\t\tM\t\tH\t\n");  
    printf("--------------------------------------------------\n"); 

    lock = xSemaphoreCreateBinary();     //semáforo
    xSemaphoreGive(lock);
 
    xTaskCreate( TaskLowPrio,  "PRIO_BAJA ", STACK_SIZE, NULL, 1, NULL );   //Crea la tarea de prioridad baja0
    vTaskDelay(1/portTICK_PERIOD_MS);                                                     //On purpose 
    xTaskCreate( TaskHighPrio, "PRIO_ALTA",  STACK_SIZE, NULL, 3, NULL );   //Crea la tarea de prioridad alta
    xTaskCreate( TaskMedPrio,  "PRIO_MEDIA", STACK_SIZE, NULL, 2, NULL );   //Crea la tarea de prioridad media
    
    
    
    while (1) 
    {     
       vTaskDelay(5/portTICK_PERIOD_MS); 
    }
}