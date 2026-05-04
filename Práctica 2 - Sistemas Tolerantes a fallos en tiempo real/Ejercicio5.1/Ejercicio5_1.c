#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"

#define LED_T1 4        // GPIO 4 (Blink 10Hz)
#define LED_T2 5        // GPIO 5 (Estado Botón)
#define BTN_BOOT 0      // GPIO 0 (Botón BOOT)
#define CANAL_ADC ADC_CHANNEL_6 // GPIO 6 (Potenciómetro)

SemaphoreHandle_t xSemaforo_Tarea3 = NULL;
adc_oneshot_unit_handle_t adc_handle;

void configurar_todo() {
    // Configurar LEDs
    gpio_reset_pin(LED_T1);
    gpio_set_direction(LED_T1, GPIO_MODE_OUTPUT);
    gpio_reset_pin(LED_T2);
    gpio_set_direction(LED_T2, GPIO_MODE_OUTPUT);

    // Configurar Botón
    gpio_reset_pin(BTN_BOOT);
    gpio_set_direction(BTN_BOOT, GPIO_MODE_INPUT);
    gpio_pullup_en(BTN_BOOT);

    // Configurar ADC (API Oneshot para v5.5.2)
    adc_oneshot_unit_init_cfg_t init_config = { .unit_id = ADC_UNIT_1 };
    adc_oneshot_new_unit(&init_config, &adc_handle);
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,
    };
    adc_oneshot_config_channel(adc_handle, CANAL_ADC, &config);
}

void vTarea1_Blink(void *pvParameters) {
    for (;;) {
        gpio_set_level(LED_T1, 1);
        vTaskDelay(50 / portTICK_PERIOD_MS); // 10Hz = 100ms periodo (50 on / 50 off)
        gpio_set_level(LED_T1, 0);
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

void vTarea2_Control(void *pvParameters) {
    int last_state = 1;
    int led_state = 0;
    for (;;) {
        int current_state = gpio_get_level(BTN_BOOT);
        if (last_state == 1 && current_state == 0) { // Flanco de bajada
            printf("¡Botón pulsado!\n");
            led_state = !led_state;
            gpio_set_level(LED_T2, led_state);

            xSemaphoreGive(xSemaforo_Tarea3); // Activa la Tarea 3
            vTaskDelay(200 / portTICK_PERIOD_MS); // Debounce
        }
        last_state = current_state;
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}

void vTarea3_Medicion(void *pvParameters) {
    int lectura;
    for (;;) {
        if (xSemaphoreTake(xSemaforo_Tarea3, portMAX_DELAY) == pdTRUE) {
            int suma = 0;
            for (int i = 0; i < 10; i++) {
                adc_oneshot_read(adc_handle, CANAL_ADC, &lectura);
                suma += lectura;
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }
            printf("Promedio ADC (10 muestras): %d\n", suma / 10);
        }
    }
}

void app_main(void) {
    printf("\n--- INICIANDO EJERCICIO 5.1 ---\n");
    configurar_todo();

    xSemaforo_Tarea3 = xSemaphoreCreateBinary();

    if (xSemaforo_Tarea3 != NULL) {
        xTaskCreate(vTarea1_Blink, "Tarea1", 2048, NULL, 1, NULL);
        xTaskCreate(vTarea2_Control, "Tarea2", 3072, NULL, 1, NULL);
        xTaskCreate(vTarea3_Medicion, "Tarea3", 3072, NULL, 1, NULL);
    }

    // Bucle infinito para que app_main no retorne
    while(1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}