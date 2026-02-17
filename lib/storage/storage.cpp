#include "storage.h"
#include <Preferences.h>
#include <Arduino.h>

#define RW_MODE false
#define RO_MODE true

#define PREFERENCES_NAME_SPACE "light_statusses"
#define ALREADY_INITIALIZED_KEY "nvsInit"

#define INTERIOR_LIGHTS_ACTIVE "intLightsOn"
#define EXTERIOR_LIGHT_DIRECTION "direction"
#define LIGHT_MODE_KEY "lightMode"

Preferences preferences;

Preferences_Data_Struct initPreferences() {
    Serial.println("Setting up preferences, starting in RO mode...");
    preferences.begin(PREFERENCES_NAME_SPACE, RO_MODE);

    Serial.print("Checking for existance of namespace: "); 
    bool tpInit = preferences.isKey(ALREADY_INITIALIZED_KEY);

    Serial.print("namespace "); 
    Serial.println(tpInit ? "does already exist. Continuing in RO mode." : "does not exist. Now initializing namespace..."); 
    //init namespace if doesn't exist already
    if (!tpInit) {
        preferences.end();
        preferences.begin(PREFERENCES_NAME_SPACE, RW_MODE);

        preferences.putBool(INTERIOR_LIGHTS_ACTIVE, false);
        preferences.putBool(EXTERIOR_LIGHT_DIRECTION, true);
        preferences.putUInt(LIGHT_MODE_KEY, static_cast<uint8_t>(LightMode::Off));
        preferences.putBool(ALREADY_INITIALIZED_KEY, true);
        preferences.end();

        preferences.begin(PREFERENCES_NAME_SPACE, RO_MODE);
    }
    //puzzle together the result 
    Preferences_Data_Struct result;
    result.interiorLightsActive = preferences.getBool(INTERIOR_LIGHTS_ACTIVE, false);
    result.direction = preferences.getBool(EXTERIOR_LIGHT_DIRECTION, false);
    uint8_t storedMode = preferences.getUInt(LIGHT_MODE_KEY, static_cast<uint8_t>(LightMode::Off));
    result.lightMode = static_cast<LightMode>(storedMode);
    preferences.end();
    return result;
}

void storeLightStates(Preferences_Data_Struct states) {
    preferences.begin(PREFERENCES_NAME_SPACE, RW_MODE); 
    preferences.putBool(INTERIOR_LIGHTS_ACTIVE, states.interiorLightsActive);
    preferences.putBool(EXTERIOR_LIGHT_DIRECTION, states.direction);
    preferences.putUInt(LIGHT_MODE_KEY, static_cast<uint8_t>(states.lightMode));
    preferences.end();
}
