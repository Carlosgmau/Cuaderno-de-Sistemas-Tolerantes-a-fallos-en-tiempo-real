#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h" // Necesario para el Mutex
#include "esp_adc/adc_oneshot.h"

#define CANAL_ADC ADC_CHANNEL_6 // GPIO 6
#define MUESTRAS_POR_RAFAGA 1000

// Handles globales
SemaphoreHandle_t xMutex_ADC = NULL;
adc_oneshot_unit_handle_t adc_handle;

// Variables para almacenar registros históricos
int valor_minimo_historico = 4095;
int valor_maximo_historico = 0;

// Configuración del ADC
void configurar_adc() {
    adc_oneshot_unit_init_cfg_t init_config = { .unit_id = ADC_UNIT_1 };
    adc_oneshot_new_unit(&init_config, &adc_handle);
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,
    };
    adc_oneshot_config_channel(adc_handle, CANAL_ADC, &config);
}

// Tarea 1: Busca el valor Mínimo
void vTarea_Minimo(void *pvParameters) {
    int lectura, min_actual;
    for (;;) {
        if (xSemaphoreTake(xMutex_ADC, portMAX_DELAY) == pdTRUE) {
            printf("MIN - Mutex tomado en: %lu ms\n", (unsigned long)xTaskGetTickCount() * portTICK_PERIOD_MS);
            
            min_actual = 4095;
            for (int i = 0; i < MUESTRAS_POR_RAFAGA; i++) {
                adc_oneshot_read(adc_handle, CANAL_ADC, &lectura);
                if (lectura < min_actual) min_actual = lectura;
            }
            if (min_actual < valor_minimo_historico) valor_minimo_historico = min_actual;
            
            printf("MIN - Valor Mínimo Histórico: %d\n", valor_minimo_historico);
            printf("MIN - Mutex liberado en: %lu ms\n", (unsigned long)xTaskGetTickCount() * portTICK_PERIOD_MS);
            xSemaphoreGive(xMutex_ADC);
        }
        vTaskDelay(200 / portTICK_PERIOD_MS); // Periodicidad de 200ms
    }
}

// Tarea 2: Busca el valor Máximo
void vTarea_Maximo(void *pvParameters) {
    int lectura, max_actual;
    for (;;) {
        if (xSemaphoreTake(xMutex_ADC, portMAX_DELAY) == pdTRUE) {
            printf("MAX - Mutex tomado en: %lu ms\n", (unsigned long)xTaskGetTickCount() * portTICK_PERIOD_MS);
            
            max_actual = 0;
            for (int i = 0; i < MUESTRAS_POR_RAFAGA; i++) {
                adc_oneshot_read(adc_handle, CANAL_ADC, &lectura);
                if (lectura > max_actual) max_actual = lectura;
            }
            if (max_actual > valor_maximo_historico) valor_maximo_historico = max_actual;
            
            printf("MAX - Valor Máximo Histórico: %d\n", valor_maximo_historico);
            printf("MAX - Mutex liberado en: %lu ms\n", (unsigned long)xTaskGetTickCount() * portTICK_PERIOD_MS);
            xSemaphoreGive(xMutex_ADC);
        }
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}

// Tarea 3: Calcula el Promedio
void vTarea_Promedio(void *pvParameters) {
    int lectura;
    long suma;
    for (;;) {
        if (xSemaphoreTake(xMutex_ADC, portMAX_DELAY) == pdTRUE) {
            printf("PROM - Mutex tomado en: %lu ms\n", (unsigned long)xTaskGetTickCount() * portTICK_PERIOD_MS);
            
            suma = 0;
            for (int i = 0; i < MUESTRAS_POR_RAFAGA; i++) {
                adc_oneshot_read(adc_handle, CANAL_ADC, &lectura);
                suma += lectura;
            }
            
            printf("PROM - Valor Promedio Ráfaga: %ld\n", suma / MUESTRAS_POR_RAFAGA);
            printf("PROM - Mutex liberado en: %lu ms\n", (unsigned long)xTaskGetTickCount() * portTICK_PERIOD_MS);
            xSemaphoreGive(xMutex_ADC);
        }
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}

void app_main(void) {
    configurar_adc();

    // 1. Crear el Mutex (Punto 4.1.1)
    xMutex_ADC = xSemaphoreCreateMutex();

    if (xMutex_ADC != NULL) {
        // 2. Crear las tareas en un solo núcleo (Punto 4.1.1)
        xTaskCreatePinnedToCore(vTarea_Minimo, "Tarea_Min", 3072, NULL, 1, NULL, 0);
        xTaskCreatePinnedToCore(vTarea_Maximo, "Tarea_Max", 3072, NULL, 1, NULL, 0);
        xTaskCreatePinnedToCore(vTarea_Promedio, "Tarea_Prom", 3072, NULL, 1, NULL, 0);
    }
}