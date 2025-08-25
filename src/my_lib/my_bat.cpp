//
// Created by 22480 on 24-12-11.
//

#include "my_bat.h"
#include <Arduino.h>
#include "esp_adc_cal.h"

// 电压检测
uint16_t bat_check_num = 0;
int BAT_PIN = 35; // select the input pin for the ADC
static esp_adc_cal_characteristics_t adc_chars;
static const adc1_channel_t channel = ADC1_CHANNEL_7;
static const adc_bits_width_t width = ADC_WIDTH_BIT_12;
// 修改自 static const adc_atten_t atten = ADC_ATTEN_DB_11;
static const adc_atten_t atten = ADC_ATTEN_DB_12;
static const adc_unit_t unit = ADC_UNIT_1;
// #define LED_BAT 13
/// 电压检测更新
void bat_update()
{
    if (bat_check_num > 1000)
    {
        // 电压读取
        uint32_t sum = 0;
        sum = analogRead(BAT_PIN);
        uint32_t voltage = esp_adc_cal_raw_to_voltage(sum, &adc_chars);
        double battery = voltage * 3.97 / 1000.0;
        Serial.println(battery);
        // // 电量显示
        // if (battery > 7.8)
        //     digitalWrite(LED_BAT, HIGH);
        // else
        //     digitalWrite(LED_BAT, LOW);
        bat_check_num = 0;
    }
    else
        bat_check_num++;
}

/// 电压检测初始化
void bat_setup()
{
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK)
        printf("eFuse Two Point: Supported\n");
    else
        printf("eFuse Two Point: NOT supported\n");
    // Check Vref is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK)
        printf("eFuse Vref: Supported\n");
    else
        printf("eFuse Vref: NOT supported\n");
    adc1_config_width(width);
    adc1_config_channel_atten(channel, atten);
    esp_adc_cal_characterize(unit, atten, width, 0, &adc_chars);
    // 电量显示LED
    // pinMode(LED_BAT, OUTPUT);
}
