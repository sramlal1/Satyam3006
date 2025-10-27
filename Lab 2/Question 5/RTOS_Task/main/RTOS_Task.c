//ID: 816031707
//Question 5

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "esp_heap_caps.h"

#define LED_PIN        2
#define PRIO_LED_ON    3
#define PRIO_LED_OFF   2
#define PRIO_STATUS    1
#define configUSE_IDLE_HOOK 1

static SemaphoreHandle_t g_ledMutex;
static volatile uint32_t idleCounter = 0;   // counts idle task executions

// Idle Hook Function //
void vApplicationIdleHook(void)
{
    // This function runs whenever all other tasks are blocked/delayed
    idleCounter++;
}

//Idle Hook is used. Its impact on task operation is minimal as it runs at the lowest priority and will only execute when other tasks are delayed.

// Initialize LED GPIO //
static void led_gpio_init(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LED_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
    gpio_set_level(LED_PIN, 0);
}

static inline void led_write(int level) {
    gpio_set_level(LED_PIN, level);
}

// Task A - LED ON for 0.5s (busy-wait) //
static void taskLedOn(void *arg) {
    (void)arg;
    for (;;) {
        xSemaphoreTake(g_ledMutex, portMAX_DELAY);
        led_write(1);
        int64_t start = esp_timer_get_time();
        while ((esp_timer_get_time() - start) < 500000) {
            // busy-wait for 0.5 seconds
        }
        xSemaphoreGive(g_ledMutex);
        taskYIELD();
    }
}

// Task B - LED OFF for 1s (delay) //
static void taskLedOff(void *arg) {
    (void)arg;
    for (;;) {
        xSemaphoreTake(g_ledMutex, portMAX_DELAY);
        led_write(0);
        xSemaphoreGive(g_ledMutex);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// Task C - Status print via UART every 1s //
static void taskStatus(void *arg) {
    (void)arg;
    uint32_t tick = 0;
    uint32_t prevIdle = 0;

    for (;;) {
        uint32_t heap = (uint32_t)esp_get_free_heap_size();
        uint32_t idleNow = idleCounter;
        uint32_t idleDelta = idleNow - prevIdle;
        prevIdle = idleNow;

        printf("[Status] t=%lu s | Heap=%u bytes | IdleCount Δ=%lu\n",
               (unsigned long)tick++,
               heap,
               (unsigned long)idleDelta);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// Main application entry //
void app_main(void) {
    led_gpio_init();
    led_write(0);

    // Create mutex for shared LED pin
    g_ledMutex = xSemaphoreCreateMutex();
    configASSERT(g_ledMutex != NULL);

    // Create three tasks with unique priorities
    BaseType_t ok;
    ok = xTaskCreate(taskLedOn,  "LED_ON",   2048, NULL, PRIO_LED_ON,  NULL);
    configASSERT(ok == pdPASS);
    ok = xTaskCreate(taskLedOff, "LED_OFF",  2048, NULL, PRIO_LED_OFF, NULL);
    configASSERT(ok == pdPASS);
    ok = xTaskCreate(taskStatus, "STATUS",   4096, NULL, PRIO_STATUS,  NULL);
    configASSERT(ok == pdPASS);

}

