#include "addons/staiko.h"
#include "storagemanager.h"
#include "helper.h"
#include "config.pb.h"
#include "lib/ADCFilter/adc_filter.h"
#include <iostream>
#include "drivermanager.h"

bool STaikoAddon::available()
{
    //const STaikoOptions& options = Storage::getInstance().getAddonOptions().staikoOptions;
    //return options.enabled;
    return true; // For testing, always return true to enable the add-on
}

void STaikoAddon::setup()
{
    const STaikoOptions& options = Storage::getInstance().getAddonOptions().staikoOptions;
    adc_filter_init();
    onboardLED.Setup(16, 1, LED_FORMAT_GRB, pio0, 0);

    if (!gpio_get(7)) {
        colors[0] = 0x00333300;
        onboardLED.SetFrame(colors);
        onboardLED.Show();
        ledState = true;
    }

    ledDelayTimer = time_us_64();
}

void STaikoAddon::preprocess()
{
    if(!ledState && time_us_64() - ledDelayTimer > 500000) {
        if(DriverManager::getInstance().getInputMode() == INPUT_MODE_KEYBOARD) {
            pressHoldTime == PRESS_HOLD_TIME_SIM;
            colors[0] = 0x00003333; 
            onboardLED.SetFrame(colors);
            onboardLED.Show();
            ledState = true;
        }
        else if (DriverManager::getInstance().getInputMode() == INPUT_MODE_SWITCH_PRO) {
            colors[0] = 0x00003300; 
            onboardLED.SetFrame(colors);
            onboardLED.Show();
            ledState = true;
        }
        else if (DriverManager::getInstance().getInputMode() == INPUT_MODE_P5GENERAL) {
            colors[0] = 0x00000033;
            onboardLED.SetFrame(colors);
            onboardLED.Show();
            ledState = true;
        }
        else if (DriverManager::getInstance().getInputMode() == INPUT_MODE_XINPUT) {
            colors[0] = 0x00330000;
            onboardLED.SetFrame(colors);
            onboardLED.Show();
            ledState = true;
        }
        else { 
            ledState = false;
        }
    }
}

void STaikoAddon::process()
{

    for(int i = 0; i < 4; i++) {
        trend_val[i] = adc_get_trend_raw_2ms(i);
    }

    if(ButtonPressed[0] || time_us_64() - pressHoldTimer[0] < pressHoldTime){
        Gamepad *gamepad = Storage::getInstance().GetGamepad();
        gamepad->state.buttons |= GAMEPAD_MASK_L2;
    }
    if(ButtonPressed[1] || time_us_64() - pressHoldTimer[1] < pressHoldTime){
        Gamepad *gamepad = Storage::getInstance().GetGamepad();
        gamepad->state.buttons |= GAMEPAD_MASK_R3;
    }
    if(ButtonPressed[2] || time_us_64() - pressHoldTimer[2] < pressHoldTime){
        Gamepad *gamepad = Storage::getInstance().GetGamepad();
        gamepad->state.buttons |= GAMEPAD_MASK_R2;
    }
    if(ButtonPressed[3] || time_us_64() - pressHoldTimer[3] < pressHoldTime){
        Gamepad *gamepad = Storage::getInstance().GetGamepad();
        gamepad->state.buttons |= GAMEPAD_MASK_L3;
    }
    //printf("Raw Values: %d, %d, %d, %d\n", adc_get_raw(0), adc_get_raw(1), adc_get_raw(2), adc_get_raw(3));
    //printf("Trend Values: %d, %d, %d, %d\n", trend_val[0], trend_val[1], trend_val[2], trend_val[3]);

    //PressButton
    if(!ButtonStatus[0] && !ButtonStatus[1] && !ButtonStatus[2] && !ButtonStatus[3]) {
        if(!runAnalysis &&
            ((trend_val[0] > PRESS_THRESHOLD_TREND && !ButtonStatus[0]) ||
            (trend_val[1] > PRESS_THRESHOLD_TREND && !ButtonStatus[1]) ||
            (trend_val[2] > PRESS_THRESHOLD_TREND && !ButtonStatus[2]) ||
            (trend_val[3] > PRESS_THRESHOLD_TREND && !ButtonStatus[3]))) {

            maxValue = 0;
            maxKey = -1;
            
            hitDetectedTimer = time_us_64();
            runAnalysis = true;
        }
    }

    if(runAnalysis) {
        if(time_us_64() - hitDetectedTimer < HIT_DETECTION_TIME) {
            if(adc_get_raw(0) > maxValue && !ButtonPressed[0]) {
                maxValue = adc_get_raw(0);
                maxKey = 0;
            }
            if(adc_get_raw(1) > maxValue && !ButtonPressed[1]) {
                maxValue = adc_get_raw(1);
                maxKey = 1;
            }
            if(adc_get_raw(2) > maxValue && !ButtonPressed[2]) {
                maxValue = adc_get_raw(2);
                maxKey = 2;
            }
            if(adc_get_raw(3) > maxValue && !ButtonPressed[3]) {
                maxValue = adc_get_raw(3);
                maxKey = 3;
            }
        }
        else {
            debounce_timer[0] = time_us_64();
            debounce_timer[1] = time_us_64();
            debounce_timer[2] = time_us_64();
            debounce_timer[3] = time_us_64();

            ButtonStatus[0] = true;
            ButtonStatus[1] = true;
            ButtonStatus[2] = true;
            ButtonStatus[3] = true;

            ButtonPressed[maxKey] = true;
            pressHoldTimer[maxKey] = time_us_64();
            runAnalysis = false;
        }
    }

    //ReleaseMesure: Trend Based
    for(uint16_t i=0;i<4;i++){
        if(ButtonStatus[i]){
            if(trend_val[i] < 0){
                if(time_us_64() - debounce_timer[i] > DEBOUNCE_TIME){
                    if(ButtonPressed[i]){
                        ButtonPressed[i] = false;
                    }
                    ButtonStatus[i] = false;
                }
            }
            else{
                debounce_timer[i] = time_us_64();
            }
        }
    }
}