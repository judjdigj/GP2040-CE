#ifndef ADC_FILTER_H
#define ADC_FILTER_H

#include <stdint.h>
#include <stdbool.h>

// 配置参数
#define ADC_CH_COUNT    4       // 使用 4 个通道 (GP26, GP27, GP28, GP29)
#define SAMPLE_RATE_US  50     // 采样间隔 100us
#define WINDOW_2MS      40      // 2ms 窗口 (20 * 100us = 2ms)

// 函数声明
void adc_filter_init(void);
uint16_t adc_get_raw(uint8_t ch);
int16_t  adc_get_trend_raw_2ms(uint8_t ch);

#endif