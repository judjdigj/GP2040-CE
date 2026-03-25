#ifndef STAIKO_H
#define STAIKO_H

#include "gpaddon.h"
#include "GamepadEnums.h"
#include "BoardConfig.h"
#include "enums.pb.h"
#include "lib/NeoPico/src/NeoPico.h"

#ifndef STAIKO_ENABLED
#define STAIKO_ENABLED 0
#endif

#define STAIKO_HOLD_TIME_MS 1000
#define STAIKO_RELEASE_TIME_MS 1000

// Add-on Name
#define STaikoName "STaiko"

class STaikoAddon : public GPAddon
{
public:
    virtual void bootProcess() {}
    virtual bool available();
    virtual void setup();
    virtual void preprocess();
    virtual void process();
    virtual void postprocess(bool) {}
    virtual void reinit() {}
    virtual std::string name() { return STaikoName; }

private:
    const uint16_t PRESS_THRESHOLD = 500;
    const uint16_t PRESS_THRESHOLD_TREND = 300;
    const uint64_t HIT_DETECTION_TIME = 2000;
    const uint64_t DEBOUNCE_TIME = 1000;
    const uint64_t PRESS_HOLD_TIME_SIM = 8000;
    const uint64_t PRESS_HOLD_TIME = 25000;
    const uint16_t THRESHOLD_BIAS_VALUE = 300;
    const uint64_t BUTTON_PRESS_TIME = 2500000;

    bool triggeredDetection = false;
    bool hitDetected = false;
    bool debounceActive = false;
    bool runAnalysis = false;
    bool dynamicThresholdActive[4] = {false, false, false, false};
    bool ButtonStatus[4] = {false, false, false, false};
    bool ButtonPressed[4] = {false, false, false, false};
    bool hitDetectedStatus[4] = {false, false, false, false};
    uint64_t pressHoldTime = PRESS_HOLD_TIME;

    uint64_t hitDetectedTimer = 0;
    uint64_t releaseTimer[4] = {0, 0 ,0, 0};
    uint64_t dynamicThresholdTimer[4] = {0, 0, 0, 0};
    uint64_t pressHoldTimer[4] = {0, 0, 0, 0};
    uint64_t debounce_timer[4] = {0, 0, 0, 0};
    int16_t dynamicThreshold[4] = {PRESS_THRESHOLD, PRESS_THRESHOLD, PRESS_THRESHOLD, PRESS_THRESHOLD};
    uint64_t buttonPressTimer[4] = {0, 0, 0, 0};

    int16_t trend_val[4] = {0, 0, 0, 0};
    int16_t maxValue = 0;
    int16_t maxKey = -1;
    int16_t maxTrendValue = 0;
    int16_t maxTrendKey = -1;
    uint64_t timer = 0;
    uint64_t speedTimer = 0;

    NeoPico onboardLED;
    uint32_t colors[1];
    bool ledState = false;
    uint64_t ledDelayTimer = 0;

};

#endif