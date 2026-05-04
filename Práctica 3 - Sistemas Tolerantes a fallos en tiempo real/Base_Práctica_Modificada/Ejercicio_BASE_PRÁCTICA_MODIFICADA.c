/*
Practica 3,
Creado por Javier Burgoa el 24/11/2024
Código base para la práctica 3.
5 tareas se ejecutan de forma concurrente.
5 tareas de igual prioridad
5 tareas con la misma carga computacional

Al pulsar BOOT se muestran las estadístcias por pantalla

*/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "driver/ledc.h"
#include "esp_timer.h"
#include "driver/uart.h"
#include "esp_clk_tree.h"
#include "esp_log.h"

//PERIODOS TAREAS en ms

#define TASK1_T 400
#define TASK2_T 400
#define TASK3_T 400
#define TASK4_T 400
#define TASK5_T 400

//CONTADORES
#define ITERATE_1 15000
#define ITERATE_2 15000
#define ITERATE_3 15000
#define ITERATE_4 15000
#define ITERATE_5 15000

//GPIO SALIDAS Y ENTRADAS DIGITALES
#define PULSADOR 0

#define STACK_SIZE	4*1024	     //N x 1kByte es el tamaño de la piLa  

bool flag_stats = false;
void imprime_estadisticas(void);

//----------------------------------------------------------------
//--------------Task1--------------------------------------------- 
//----------------------------------------------------------------
void vTaskCode1( void * pvParameters )           
{
    TickType_t 		    xLastWakeTime; 
    const TickType_t 	xDelayTicks = TASK1_T/portTICK_PERIOD_MS;
      
    xLastWakeTime = xTaskGetTickCount ();         // Initialise the xLastWakeTime variable with the current time. 
    while(1)
    {
       if (!flag_stats) 
            printf("\tI\t\t\t\t\t%lu\n", xTaskGetTickCount() );            

       //Consume CPU cycles
       for (long i = 0; i < ITERATE_1; i++) {
           __asm__ __volatile__("NOP");
       }

       if (!flag_stats) 
            printf("\tO\t\t\t\t\t%lu\n", xTaskGetTickCount() );            

       //bloquea y espera hasta TASK1_T ms        
       xTaskDelayUntil( &xLastWakeTime, xDelayTicks );        
    }	
}
//----------------------------------------------------------------
//-----------------Task2 -----------------------------------------
//----------------------------------------------------------------
void vTaskCode2( void * pvParameters )           //
{
    TickType_t 		    xLastWakeTime; 
    const TickType_t 	xDelayTicks = TASK2_T/portTICK_PERIOD_MS;

    xLastWakeTime = xTaskGetTickCount ();         // Initialise the xLastWakeTime variable with the current time. 
    while(1)
    {
       if (!flag_stats)       
            printf("\t\tI\t\t\t\t%lu\n", xTaskGetTickCount() );  
 
       //Consume CPU cycles
       for (long i = 0; i < ITERATE_2; i++) {
           __asm__ __volatile__("NOP");
       }
       if (!flag_stats) 
            printf("\t\tO\t\t\t\t%lu\n", xTaskGetTickCount() );
 
       //bloquea y espera hasta TASK2_T ms               
       xTaskDelayUntil( &xLastWakeTime, xDelayTicks );         
    }	      
}
//----------------------------------------------------------------
//-----------------Task3: ----------------------------------------
//----------------------------------------------------------------
void vTaskCode3( void * pvParameters )           
{
    TickType_t 		    xLastWakeTime; 
    const TickType_t 	xDelayTicks = TASK3_T/portTICK_PERIOD_MS;
      
    xLastWakeTime = xTaskGetTickCount ();         // Initialise the xLastWakeTime variable with the current time. 

    while(1)
    {
       if (!flag_stats) 
            printf("\t\t\tI\t\t\t%lu\n", xTaskGetTickCount() );       

       //Consume CPU cycles
       for (long i = 0; i < ITERATE_3; i++) {
           __asm__ __volatile__("NOP");
       }
       if (!flag_stats) 
            printf("\t\t\tO\t\t\t%lu\n", xTaskGetTickCount() ); 

       //bloquea y espera hasta TASK3_T ms        
       xTaskDelayUntil( &xLastWakeTime, xDelayTicks );             
    }	
}

//----------------------------------------------------------------
//-----------------Task4:   --------------------------------------
//----------------------------------------------------------------
void vTaskCode4( void * pvParameters )             
{
     TickType_t 		xLastWakeTime; 
    const TickType_t 	xDelayTicks = TASK4_T/portTICK_PERIOD_MS;

    xLastWakeTime = xTaskGetTickCount ();         // Initialise the xLastWakeTime variable with the current time. 
    while(1)
    {
       if (!flag_stats)       
            printf("\t\t\t\tI\t\t%lu\n", xTaskGetTickCount() );

       //Consume CPU cycles
       for (long i = 0; i < ITERATE_4; i++) {
           __asm__ __volatile__("NOP");
       }
       if (!flag_stats) 
            printf("\t\t\t\tO\t\t%lu\n", xTaskGetTickCount() );

       //bloquea y espera hasta TASK4_T ms               
       xTaskDelayUntil( &xLastWakeTime, xDelayTicks );             
    }
}
//----------------------------------------------------------------
//-----------------Task5:   --- ----------------------------------
//----------------------------------------------------------------
void vTaskCode5( void * pvParameters )             
{
     TickType_t 		xLastWakeTime; 
    const TickType_t 	xDelayTicks = TASK4_T/portTICK_PERIOD_MS;
    xLastWakeTime = xTaskGetTickCount ();         // Initialise the xLastWakeTime variable with the current time. 
    while(1)
    {
       if (!flag_stats) 
            printf("\t\t\t\t\tI\t%lu\n", xTaskGetTickCount() );                         

       //Consume CPU cycles
       for (long i = 0; i < ITERATE_4; i++) {
           __asm__ __volatile__("NOP");
       }

       if (!flag_stats) 
            printf("\t\t\t\t\tO\t%lu\n", xTaskGetTickCount() );  
 
       //bloquea y espera hasta TASK5_T ms
       xTaskDelayUntil( &xLastWakeTime, xDelayTicks );             
    }  
}


//----------------------------------------------------------------
//-----------------Main loop    ----------------------------------
//----------------------------------------------------------------

#define GPIO_OUTPUT_IO_0    4     //GPIO 4  - salida 1 - 3Hz
#define GPIO_OUTPUT_IO_1    5     //GPIO 5  - salida 2 - 6Hz
#define PULSADOR            0     //GPIO 0  - entrada digital

void app_main(void) 
{   
    /************* Config GPIO ***************/    
    gpio_set_direction(GPIO_OUTPUT_IO_0,GPIO_MODE_OUTPUT)  ;  //GPIO_OUTPUT_IO_0 
    gpio_set_direction(GPIO_OUTPUT_IO_1,GPIO_MODE_OUTPUT)  ;  //GPIO_OUTPUT_IO_1

    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << PULSADOR,
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = true,             //pull-up habilitada
        .pull_down_en = false,            //pull-down deshabilitada
    };
    gpio_config(&io_conf);

    //Crea Handlers a las taraeas
    TaskHandle_t xHandle1 = NULL;  //Handler a la tarea 1
    TaskHandle_t xHandle2 = NULL;  //Handler a la tarea 2
    TaskHandle_t xHandle3 = NULL;  //Handler a la tarea 3 
    TaskHandle_t xHandle4 = NULL;  //Handler a la tarea 4  
    TaskHandle_t xHandle5 = NULL;  //Handler a la tarea 5

    //Crea tareas
    xTaskCreate( vTaskCode1, "TASK1", STACK_SIZE, NULL, 1, &xHandle1 );  //Prioridad 1
    xTaskCreate( vTaskCode2, "TASK2", STACK_SIZE, NULL, 1, &xHandle2 );  //Prioridad 1 
    xTaskCreate( vTaskCode3, "TASK3", STACK_SIZE, NULL, 1, &xHandle3 );  //Prioridad 1   
    xTaskCreate( vTaskCode4, "TASK4", STACK_SIZE, NULL, 1, &xHandle4 );  //Prioridad 1   
    xTaskCreate( vTaskCode5, "TASK5", STACK_SIZE, NULL, 1, &xHandle5 );  //Prioridad 1   

    int entrada_digital_t0 = 0;
    int entrada_digital_t1 = 0;
 
    while (1) 
    {
        //main loop  
        
        entrada_digital_t0 = gpio_get_level(PULSADOR);

        //en flanco de bajada    
        if ((entrada_digital_t0==0)&&(entrada_digital_t1==1))
        {
           imprime_estadisticas();   
        }
        entrada_digital_t1 = entrada_digital_t0;    

        vTaskDelay(100/portTICK_PERIOD_MS);   //Wait 1 sec
    }
}



void imprime_estadisticas(void)
{
    char Buff[512]      = {0};
    char task_list[512] = {0};
    uint32_t cpu_freq_hz = 0;
    
    //------------- Statistics ----------------------

    //Consulta el stado de todas las tareas
    vTaskList(task_list);
    vTaskGetRunTimeStats(Buff);    
    esp_clk_tree_src_get_freq_hz(SOC_CPU_CLK_SRC_PLL, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &cpu_freq_hz);

    //imprime por el terminal.
    printf("**************************************\n");
    printf("Estado de las tareas:\n%s\n", task_list);    

    //Imprime la carga de la CPU.
    printf("%s ", Buff);   

    //CPU Freq                
    printf("La CPU se ha configurado a : %lu MHz\n", cpu_freq_hz / 1000000);

    //ticks rate
    printf("TIME SLICE: %d Hz, %.1f ms\n", configTICK_RATE_HZ, (float)(1000.0/configTICK_RATE_HZ));    

    printf("**************************************\n"); 
}
