#include <stdio.h>
#include <string.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_timer.h"

#define TASK_A_T 200
#define TASK_B_T 200

#define ITER_A 400000
#define ITER_B 400000

#define STACK_SIZE 3*1024

#define VERBOSE 1

SemaphoreHandle_t xMutex1;
SemaphoreHandle_t xMutex2;

void TareaA( void * pvParameters )
{
    while(1)
    {
        if (xSemaphoreTake(xMutex1, 1000 / portTICK_PERIOD_MS) == pdTRUE)
        {
            printf("A: cogio mutex 1 # %llu ms\n", esp_timer_get_time()/1000);

            vTaskDelay(1/portTICK_PERIOD_MS);

            if (xSemaphoreTake(xMutex2, 1000 / portTICK_PERIOD_MS) == pdTRUE)
            {
                printf("A: cogio mutex 2 # %llu ms\n", esp_timer_get_time()/1000);

                for (long i = 0; i < ITER_A; i++) {
                    __asm__ __volatile__("NOP");
                }

                printf("A: libera mutex 2 y 1 # %llu ms\n", esp_timer_get_time()/1000);

                xSemaphoreGive(xMutex2);
            }
            else
            {
                printf("A: NO pudo coger mutex 2\n");
            }

            xSemaphoreGive(xMutex1);
        }
        else
        {
            printf("A: NO pudo coger mutex 1\n");
        }

        vTaskDelay(TASK_A_T / portTICK_PERIOD_MS);
    }
}

void TareaB( void * pvParameters )
{
    while(1)
    {
        if (xSemaphoreTake(xMutex2, 1000 / portTICK_PERIOD_MS) == pdTRUE)
        {
            printf("B: cogio mutex 2 # %llu ms\n", esp_timer_get_time()/1000);

            vTaskDelay(1/portTICK_PERIOD_MS);

            if (xSemaphoreTake(xMutex1, 1000 / portTICK_PERIOD_MS) == pdTRUE)
            {
                printf("B: cogio mutex 1 # %llu ms\n", esp_timer_get_time()/1000);

                for (long i = 0; i < ITER_B; i++) {
                    __asm__ __volatile__("NOP");
                }

                printf("B: libera mutex 1 y 2 # %llu ms\n", esp_timer_get_time()/1000);

                xSemaphoreGive(xMutex1);
            }
            else
            {
                printf("B: NO pudo coger mutex 1\n");
            }

            xSemaphoreGive(xMutex2);
        }
        else
        {
            printf("B: NO pudo coger mutex 2\n");
        }

        vTaskDelay(TASK_B_T / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    xMutex1 = xSemaphoreCreateMutex();
    xMutex2 = xSemaphoreCreateMutex();

    TaskHandle_t xHandle1 = NULL;
    TaskHandle_t xHandle2 = NULL;

    printf("\n\n\n");

    xTaskCreate( TareaA, "TAREA_A", STACK_SIZE, NULL, 1, &xHandle1 );
    xTaskCreate( TareaB, "TAREA_B", STACK_SIZE, NULL, 2, &xHandle2 );

    while (1)
    {
    }
}