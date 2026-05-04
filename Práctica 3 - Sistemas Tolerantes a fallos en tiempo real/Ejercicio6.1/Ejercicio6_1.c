#define TASK1_T 200
#define ITERATE_1 200000
#define ITERATE_ISR 200000

TaskHandle_t xGPIOint = NULL;

static void IRAM_ATTR ExtPin0_ISR_handler(void *args) {
  uint32_t gpio_num = (uint32_t) args;
  xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void gpio_task_ISR(void* arg) {
  uint32_t io_num;
  while(1) {
    if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
      for (long i=0; i < ITERATE_ISR; i++) {
        __asm__ __volatile__("NOP");
      }
      printf("***************\n");
    }
    vTaskDelay(1/portTICK_PERIOD_MS);
  }
}

void app_main(void) {
  gpio_config_t myGPIOconfig;
   
  myGPIOconfig.pin_bit_mask = 1ULL<< PULSADOR;
  myGPIOconfig.mode = GPIO_MODE_INPUT;
  myGPIOconfig.pull_up_en = true;
  myGPIOconfig.pull_down_en = false;
  myGPIOconfig.intr_type = GPIO_INTR_NEGEDGE;
   
  gpio_config(&myGPIOconfig);
  gpio_install_isr_service(0);
  gpio_isr_handler_add(PULSADOR, ExtPin0_ISR_handler, NULL);

  gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));

  xTaskCreate(gpio_task_ISR, "ISR", STACK_SIZE, NULL, 10, &xGPIOint);
}