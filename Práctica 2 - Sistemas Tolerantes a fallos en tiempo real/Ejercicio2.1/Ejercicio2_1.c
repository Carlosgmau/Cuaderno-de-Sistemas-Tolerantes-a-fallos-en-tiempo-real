#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

// En el ESP32-S3, asegúrate de que estos pines estén libres
#define LED_1 4
#define LED_2 5
#define PUSH_BUTTON 0 

TaskHandle_t xHandle2 = NULL;

void Tarea1(void *pvParameters) {
    gpio_reset_pin(LED_1);
    gpio_set_direction(LED_1, GPIO_MODE_OUTPUT);
    for(;;) {
        gpio_set_level(LED_1, 1);
        vTaskDelay(166 / portTICK_PERIOD_MS); 
        gpio_set_level(LED_1, 0);
        vTaskDelay(166 / portTICK_PERIOD_MS);
    }
}

void Tarea2(void *pvParameters) {
    gpio_reset_pin(LED_2);
    gpio_set_direction(LED_2, GPIO_MODE_OUTPUT);
    for(;;) {
        gpio_set_level(LED_2, 1);
        vTaskDelay(83 / portTICK_PERIOD_MS);
        gpio_set_level(LED_2, 0);
        vTaskDelay(83 / portTICK_PERIOD_MS);
    }
}

void app_main(void) {
    // Declaración de variables al principio
    int last_state = 1; 
    int current_state;

    printf("Iniciando Aplicacion...\n");

    // Configuración del botón
    gpio_reset_pin(PUSH_BUTTON);
    gpio_set_direction(PUSH_BUTTON, GPIO_MODE_INPUT);
    gpio_pullup_en(PUSH_BUTTON); 

    // Crear tareas
    xTaskCreate(Tarea1, "Tarea1", 3072, NULL, 1, NULL);
    xTaskCreate(Tarea2, "Tarea2", 3072, NULL, 1, &xHandle2);

    while(1) {
        current_state = gpio_get_level(PUSH_BUTTON);

        if (last_state == 1 && current_state == 0) {
            printf("Boton presionado detectado\n");
            eTaskState state = eTaskGetState(xHandle2);

            if (state == eSuspended) {
                printf("Esperando 3s para reanudar...\n");
                vTaskDelay(3000 / portTICK_PERIOD_MS);
                vTaskResume(xHandle2);
                printf("Tarea 2 activa.\n");
            } else {
                vTaskSuspend(xHandle2);
                printf("Tarea 2 suspendida.\n");
            }
            // Pequeño debounce para evitar rebotes del botón
            vTaskDelay(200 / portTICK_PERIOD_MS);
        }
        
        last_state = current_state;
        
        // ESTE DELAY ES VITAL: permite que el IDLE task se ejecute y alimente al Watchdog
        vTaskDelay(20 / portTICK_PERIOD_MS); 
    }
}