#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_adc/adc_oneshot.h"

// Definiciones de diseño
#define CANAL_POTENCIOMETRO ADC_CHANNEL_6 // GPIO 6 según enunciado
#define TAM_COLA 5                       // Tamaño de la cola de transferencia
#define VENTANA_PROMEDIO 10              // Tamaño del filtro media móvil

// Handles
QueueHandle_t xCola_ADC = NULL;
adc_oneshot_unit_handle_t adc_handle;

// Rutina de configuración del ADC (Sugerencia de la nota)
void configurar_adc() {
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
    };
    adc_oneshot_new_unit(&init_config, &adc_handle);

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12, // Equivalente a 11dB para rango 0-3.3V
    };
    adc_oneshot_config_channel(adc_handle, CANAL_POTENCIOMETRO, &config);
}

// Tarea 1: Lectura de sensor y envío a cola
void vTarea_Productora_ADC(void *pvParameters) {
    int valor_raw;
    for (;;) {
        // Leer valor analógico
        adc_oneshot_read(adc_handle, CANAL_POTENCIOMETRO, &valor_raw);
        
        // Enviar valor a la cola (Punto 3.1.2)
        // Se intenta enviar; si la cola está llena, espera 10ms y continúa
        xQueueSend(xCola_ADC, &valor_raw, 10 / portTICK_PERIOD_MS);

        // Periodicidad de 0.2 segundos (Punto 3.1.3)
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}

// Tarea 2: Procesamiento de filtro media móvil
void vTarea_Consumidora_Filtro(void *pvParameters) {
    int valor_recibido;
    int buffer[VENTANA_PROMEDIO] = {0};
    int indice = 0;
    int suma = 0;

    for (;;) {
        // Periodicidad de Tarea 2: Se activa cada vez que llega un dato (event-driven)
        // o espera máximo 500ms para no quedar bloqueada por siempre
        if (xQueueReceive(xCola_ADC, &valor_recibido, 500 / portTICK_PERIOD_MS)) {
            
            // Algoritmo de Filtro de Ventana Móvil
            suma -= buffer[indice];          // Restamos el valor más viejo
            buffer[indice] = valor_recibido; // Insertamos el nuevo
            suma += buffer[indice];          // Sumamos el nuevo
            indice = (indice + 1) % VENTANA_PROMEDIO; 

            int promediado = suma / VENTANA_PROMEDIO;

            printf("----------------------------------------\n");
            printf("Miembros del grupo: [TU NOMBRE / GRUPO]\n");
            printf("Dato recibido: %d | Valor Promediado (10): %d\n", valor_recibido, promediado);
        }
    }
}

void app_main(void) {
    configurar_adc();

    xCola_ADC = xQueueCreate(TAM_COLA, sizeof(int));

    if (xCola_ADC != NULL) {
        
        xTaskCreatePinnedToCore(vTarea_Productora_ADC, "Tarea_Lectura", 3072, NULL, 1, NULL, 0);
        xTaskCreatePinnedToCore(vTarea_Consumidora_Filtro, "Tarea_Filtro", 3072, NULL, 1, NULL, 0);
    }
}