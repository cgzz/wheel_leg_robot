#include "my_bat.h"
#include <Arduino.h>
#include "esp_adc_cal.h"
#include "my_io.h"

// 电压检测
static esp_adc_cal_characteristics_t adc_chars;
static const adc1_channel_t channel = ADC1_CHANNEL_7;
static const adc_bits_width_t width = ADC_WIDTH_BIT_12;
// 修改自 static const adc_atten_t atten = ADC_ATTEN_DB_11;
static const adc_atten_t atten = ADC_ATTEN_DB_12;
static const adc_unit_t unit = ADC_UNIT_1;
float battery = 0.0;
/// 电压检测初始化
void bat_init()
{
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK)
        Serial.println("eFuse Two Point: Supported\n");
    else
        Serial.println("eFuse Two Point: NOT supported\n");
    // Check Vref is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK)
        Serial.println("eFuse Vref: Supported\n");
    else
        Serial.println("eFuse Vref: NOT supported\n");
    adc1_config_width(width);
    adc1_config_channel_atten(channel, atten);
    esp_adc_cal_characterize(unit, atten, width, 0, &adc_chars);
}
/// 电压检测更新
void bat_update()
{
    uint32_t sum = 0;
    sum = analogRead(BAT_PIN);
    uint32_t voltage = esp_adc_cal_raw_to_voltage(sum, &adc_chars);
    battery = voltage * 3.97 / 1000.0;
}
