#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include <string.h>
#include <unistd.h>
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "sdkconfig.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_intr_alloc.h"
portMUX_TYPE ISR_MUX = portMUX_INITIALIZER_UNLOCKED;
#define ESP_INTR_FLAG_DEFAULT 0

#define GPIO_INPUT_0_PIN    GPIO_NUM_4
#define GPIO_INPUT_1_PIN    GPIO_NUM_5
#define LED_BLUE            GPIO_NUM_21
#define LED_ORANGE          GPIO_NUM_33

#define DEBOUNCE_TARGET     50
#define BLINK_PERIOD        1000
#define BLINK_FAST_PERIOD   250
#define BUTTON_0            0
#define BUTTON_1            1
#define PRESS_TIME			3000
static const char* TAG = "timer";

typedef struct {
    gpio_num_t pin;
    volatile long count;
    volatile bool last_state;
    volatile bool current_state;
    volatile uint64_t press_start_time;
} Button;

Button buttons[2] = {
    {GPIO_INPUT_0_PIN, 0, true, true,0},
    {GPIO_INPUT_1_PIN, 0, true, true,0}
};

volatile bool led_blinking_enabled = false;
volatile bool led_blink_fast = false;

int sizeArray = sizeof(buttons)/sizeof(buttons[0]);

void IRAM_ATTR ISR_Read_Inputs(void* arg)
{
    Button* button = (Button*)arg;

    portENTER_CRITICAL(&ISR_MUX);
    button->last_state = false;
    gpio_isr_handler_remove(button->pin);
    portEXIT_CRITICAL(&ISR_MUX);
}

static void IRAM_ATTR ISR_HW_Timer(void* arg) {
    portENTER_CRITICAL(&ISR_MUX);

    for (int i = 0; i < sizeArray; i++) {
        Button* button = &buttons[i];
        button->current_state = gpio_get_level(button->pin);

        if (!button->last_state && button->current_state) {
            button->count++;
            gpio_isr_handler_add(button->pin, ISR_Read_Inputs, button);
        }

        button->last_state = button->current_state;
    }

    portEXIT_CRITICAL(&ISR_MUX);
}

void InitButtons()
{
    for (int i = 0; i < sizeArray; i++)
    {
        Button* button = &buttons[i];
        gpio_config_t io_config = {
            .intr_type = GPIO_INTR_NEGEDGE,
            .mode = GPIO_MODE_INPUT,
            .pin_bit_mask = (1ULL << button->pin),
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .pull_up_en = GPIO_PULLUP_ENABLE
        };
        gpio_config(&io_config);
        gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
        gpio_isr_handler_add(button->pin, ISR_Read_Inputs, button);
    }
}

static void LEDTimerCallback(void* arg) {
    static bool led_state = false;
    led_state = !led_state;
    gpio_set_level(LED_ORANGE, led_state);
}
void Init_ISR_HW_Timer_0() {
  

  const esp_timer_create_args_t timer1_args = {
    .callback = &ISR_HW_Timer,
    .name = "Timer 1"
  };
  esp_timer_handle_t timer1_handler;
  ESP_ERROR_CHECK(esp_timer_create(&timer1_args, &timer1_handler));
  ESP_ERROR_CHECK(esp_timer_start_periodic(timer1_handler, 80));
}
void InitLEDTimer() {
    esp_timer_create_args_t timer_args = {
        .callback = LEDTimerCallback,
        .name = "LED Timer"
    };
    esp_timer_handle_t timer;
    esp_timer_create(&timer_args, &timer);
    esp_timer_start_periodic(timer, BLINK_PERIOD * 1000);
}

void app_main() {
    gpio_set_direction(LED_BLUE, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_ORANGE, GPIO_MODE_OUTPUT);

    InitButtons();
    InitLEDTimer();
	Init_ISR_HW_Timer_0();
    while (true)
    {
        for (int i = 0; i < sizeArray; i++) {
            Button* button = &buttons[i];

            if (button->count > 0){
                button->count = 0;
                ESP_LOGI(TAG, "--> BUTTON [%d] PRESSED\r\n", button->pin);

                if (button->pin == GPIO_INPUT_0_PIN){
                	 uint64_t press_duration = esp_timer_get_time() - button->press_start_time;
                	 if (press_duration >= 3000000){


						led_blinking_enabled = !led_blinking_enabled;

						if (led_blinking_enabled){
							gpio_set_level(LED_ORANGE, 1);
						} else {
							gpio_set_level(LED_ORANGE, 0);
						}
                	 }
                } else if (button->pin == GPIO_INPUT_1_PIN) {
                    led_blink_fast = !led_blink_fast;
                }
            }
        }

        if (led_blinking_enabled)
		{
            int blink_period = led_blink_fast ? BLINK_FAST_PERIOD : BLINK_PERIOD;
            gpio_set_level(LED_BLUE, 1);
            vTaskDelay(blink_period / portTICK_PERIOD_MS);
            gpio_set_level(LED_BLUE, 0);
            vTaskDelay(blink_period / portTICK_PERIOD_MS);
        } else {
            vTaskDelay(30 / portTICK_PERIOD_MS);
        }
    }
}
