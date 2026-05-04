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

#define TASK1_T 75
#define TASK2_T 200
#define TASK3_T 400
#define TASK4_T 400
#define TASK5_T 400

#define ITERATE_1 550000
#define ITERATE_2 400000
#define ITERATE_3 300000
#define ITERATE_4 300000
#define ITERATE_5 300000

#define PULSADOR 0

#define STACK_SIZE 4*1024

bool flag_stats = false;
void imprime_estadisticas(void);

void vTaskCode1( void * pvParameters )
{
    TickType_t xLastWakeTime;
    const TickType_t xDelayTicks = TASK1_T/portTICK_PERIOD_MS;

    xLastWakeTime = xTaskGetTickCount();
    while(1)
    {
       if (!flag_stats)
            printf("\tI\t\t\t\t\t%lu\n", xTaskGetTickCount() );

       for (long i = 0; i < ITERATE_1; i++) {
           __asm__ __volatile__("NOP");
       }

       if (!flag_stats)
            printf("\tO\t\t\t\t\t%lu\n", xTaskGetTickCount() );

       xTaskDelayUntil( &xLastWakeTime, xDelayTicks );
    }
}

void vTaskCode2( void * pvParameters )
{
    TickType_t xLastWakeTime;
    const TickType_t xDelayTicks = TASK2_T/portTICK_PERIOD_MS;

    xLastWakeTime = xTaskGetTickCount();
    while(1)
    {
       if (!flag_stats)
            printf("\t\tI\t\t\t\t%lu\n", xTaskGetTickCount() );

       for (long i = 0; i < ITERATE_2; i++) {
           __asm__ __volatile__("NOP");
       }
       if (!flag_stats)
            printf("\t\tO\t\t\t\t%lu\n", xTaskGetTickCount() );

       xTaskDelayUntil( &xLastWakeTime, xDelayTicks );
    }
}

void vTaskCode3( void * pvParameters )
{
    TickType_t xLastWakeTime;
    const TickType_t xDelayTicks = TASK3_T/portTICK_PERIOD_MS;

    xLastWakeTime = xTaskGetTickCount();

    while(1)
    {
       if (!flag_stats)
            printf("\t\t\tI\t\t\t%lu\n", xTaskGetTickCount() );

       for (long i = 0; i < ITERATE_3; i++) {
           __asm__ __volatile__("NOP");
       }
       if (!flag_stats)
            printf("\t\t\tO\t\t\t%lu\n", xTaskGetTickCount() );

       xTaskDelayUntil( &xLastWakeTime, xDelayTicks );
    }
}

void vTaskCode4( void * pvParameters )
{
    TickType_t xLastWakeTime;
    const TickType_t xDelayTicks = TASK4_T/portTICK_PERIOD_MS;

    xLastWakeTime = xTaskGetTickCount();
    while(1)
    {
       if (!flag_stats)
            printf("\t\t\t\tI\t\t%lu\n", xTaskGetTickCount() );

       for (long i = 0; i < ITERATE_4; i++) {
           __asm__ __volatile__("NOP");
       }
       if (!flag_stats)
            printf("\t\t\t\tO\t\t%lu\n", xTaskGetTickCount() );

       xTaskDelayUntil( &xLastWakeTime, xDelayTicks );
    }
}

void vTaskCode5( void * pvParameters )
{
    TickType_t xLastWakeTime;
    const TickType_t xDelayTicks = TASK4_T/portTICK_PERIOD_MS;
    xLastWakeTime = xTaskGetTickCount();
    while(1)
    {
       if (!flag_stats)
            printf("\t\t\t\t\tI\t%lu\n", xTaskGetTickCount() );

       for (long i = 0; i < ITERATE_4; i++) {
           __asm__ __volatile__("NOP");
       }

       if (!flag_stats)
            printf("\t\t\t\t\tO\t%lu\n", xTaskGetTickCount() );

       xTaskDelayUntil( &xLastWakeTime, xDelayTicks );
    }
}

#define GPIO_OUTPUT_IO_0    4
#define GPIO_OUTPUT_IO_1    5
#define PULSADOR            0

void app_main(void)
{
    gpio_set_direction(GPIO_OUTPUT_IO_0,GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_OUTPUT_IO_1,GPIO_MODE_OUTPUT);

    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << PULSADOR,
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = true,
        .pull_down_en = false,
    };
    gpio_config(&io_conf);

    TaskHandle_t xHandle1 = NULL;
    TaskHandle_t xHandle2 = NULL;
    TaskHandle_t xHandle3 = NULL;
    TaskHandle_t xHandle4 = NULL;
    TaskHandle_t xHandle5 = NULL;

    xTaskCreate( vTaskCode1, "TASK1", STACK_SIZE, NULL, 2, &xHandle1 );
    xTaskCreate( vTaskCode2, "TASK2", STACK_SIZE, NULL, 1, &xHandle2 );
    xTaskCreate( vTaskCode3, "TASK3", STACK_SIZE, NULL, 1, &xHandle3 );
    xTaskCreate( vTaskCode4, "TASK4", STACK_SIZE, NULL, 1, &xHandle4 );
    xTaskCreate( vTaskCode5, "TASK5", STACK_SIZE, NULL, 1, &xHandle5 );

    int entrada_digital_t0 = 0;
    int entrada_digital_t1 = 0;

    while (1)
    {
        entrada_digital_t0 = gpio_get_level(PULSADOR);

        if ((entrada_digital_t0==0)&&(entrada_digital_t1==1))
        {
           imprime_estadisticas();
        }
        entrada_digital_t1 = entrada_digital_t0;

        vTaskDelay(100/portTICK_PERIOD_MS);
    }
}

void imprime_estadisticas(void)
{
    char Buff[512]      = {0};
    char task_list[512] = {0};
    uint32_t cpu_freq_hz = 0;

    vTaskList(task_list);
    vTaskGetRunTimeStats(Buff);
    esp_clk_tree_src_get_freq_hz(SOC_CPU_CLK_SRC_PLL, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &cpu_freq_hz);

    printf("**************************************\n");
    printf("Estado de las tareas:\n%s\n", task_list);
    printf("%s ", Buff);
    printf("La CPU se ha configurado a : %lu MHz\n", cpu_freq_hz / 1000000);
    printf("TIME SLICE: %d Hz, %.1f ms\n", configTICK_RATE_HZ, (float)(1000.0/configTICK_RATE_HZ));
    printf("**************************************\n");
}