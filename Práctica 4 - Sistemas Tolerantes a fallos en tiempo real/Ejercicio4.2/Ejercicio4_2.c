#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "freertos/semphr.h"
#include "esp_timer.h"

#define ITER_L   1000000
#define ITER_M   20000000
#define ITER_H   100000

#define STACK_SIZE 2*1024

static SemaphoreHandle_t lock;

void TaskLowPrio(void * pvParameters)
{
    const TickType_t xDelayTicks = 500 / portTICK_PERIOD_MS;

    while(1)
    {
       printf("\tS\t\t\t\t\t%llu ms\n", esp_timer_get_time()/1000);

       xSemaphoreTake(lock, portMAX_DELAY);

       printf("\tI\t\t\t\t\t%llu ms\n", esp_timer_get_time()/1000);

       for (long i = 0; i < ITER_L; i++) {
           __asm__ __volatile__("NOP");
       }

       printf("\tO\t\t\t\t\t%llu ms\n", esp_timer_get_time()/1000);

       xSemaphoreGive(lock);

       printf("\tX\t\t\t\t\t%llu ms\n", esp_timer_get_time()/1000);

       vTaskDelay(xDelayTicks);
    }
}

void TaskMedPrio(void * pvParameters)
{
    const TickType_t xDelayTicks = 10000 / portTICK_PERIOD_MS;

    while(1)
    {
       printf("\t\t\tI\t\t\t%lu ms\n", xTaskGetTickCount());

       for (long i = 0; i < ITER_M; i++) {
           __asm__ __volatile__("NOP");
       }

       printf("\t\t\tO\t\t\t%lu ms\n", xTaskGetTickCount());

       vTaskDelay(xDelayTicks);
    }
}

void TaskHighPrio(void * pvParameters)
{
    const TickType_t xDelayTicks = 500 / portTICK_PERIOD_MS;

    while(1)
    {
       printf("\t\t\t\t\tS\t%llu ms\n", esp_timer_get_time()/1000);

       xSemaphoreTake(lock, portMAX_DELAY);

       printf("\t\t\t\t\tI\t%lu ms\n", xTaskGetTickCount());

       for (long i = 0; i < ITER_H; i++) {
           __asm__ __volatile__("NOP");
       }

       printf("\t\t\t\t\tO\t%lu ms\n", xTaskGetTickCount());

       xSemaphoreGive(lock);

       printf("\t\t\t\t\tX\t%llu ms\n", esp_timer_get_time()/1000);

       vTaskDelay(xDelayTicks);
    }
}

void app_main(void)
{
    printf("\tL\t\tM\t\tH\t\n");
    printf("--------------------------------------------------\n");

    lock = xSemaphoreCreateMutex();

    xTaskCreate(TaskLowPrio,  "PRIO_BAJA",  STACK_SIZE, NULL, 1, NULL);
    vTaskDelay(1 / portTICK_PERIOD_MS);
    xTaskCreate(TaskHighPrio, "PRIO_ALTA", STACK_SIZE, NULL, 3, NULL);
    xTaskCreate(TaskMedPrio,  "PRIO_MEDIA",STACK_SIZE, NULL, 2, NULL);

    while (1)
    {
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
}