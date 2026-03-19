#include <string.h>
#include "adc_filter.h"
#include "hardware/adc.h"
#include "pico/time.h"   // 关键：解决“不完整类型”错误

typedef struct {
    uint16_t raw_history[WINDOW_2MS]; 
    volatile uint16_t last_raw;
    volatile int16_t last_trend_raw;
} ADC_Slot_t;

static ADC_Slot_t slots[ADC_CH_COUNT];
static uint16_t ptr = 0;

// 将 timer 移出函数作用域，或者保持静态，确保其生命周期贯穿程序始终
static struct repeating_timer adc_timer;

static bool __not_in_flash_func(adc_timer_callback)(struct repeating_timer *t) {
    for (uint8_t i = 0; i < ADC_CH_COUNT; i++) {
        adc_select_input(i);
        uint16_t x = adc_read();
        
        uint16_t old_x = slots[i].raw_history[ptr];
        slots[i].last_raw = x;
        slots[i].last_trend_raw = (int16_t)x - (int16_t)old_x;
        
        slots[i].raw_history[ptr] = x;
    }

    if (++ptr >= WINDOW_2MS) {
        ptr = 0;
    }
    return true;
}

void adc_filter_init(void) {
    adc_init();
    memset(slots, 0, sizeof(slots));
    
    // 初始化 GP26, 27, 28, 29
    for (uint8_t i = 0; i < ADC_CH_COUNT; i++) {
        adc_gpio_init(26 + i);
    }

    // 负值间隔表示从“回调开始时间”计算下一次触发，提供更稳定的采样频率
    add_repeating_timer_us(-SAMPLE_RATE_US, adc_timer_callback, NULL, &adc_timer);
}

uint16_t adc_get_raw(uint8_t ch) {
    return (ch < ADC_CH_COUNT) ? slots[ch].last_raw : 0;
}

int16_t adc_get_trend_raw_2ms(uint8_t ch) {
    return (ch < ADC_CH_COUNT) ? slots[ch].last_trend_raw : 0;
}