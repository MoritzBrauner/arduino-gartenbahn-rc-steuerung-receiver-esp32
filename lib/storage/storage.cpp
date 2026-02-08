#include "storage.h"
#include <Arduino.h>

#define RW_MODE false
#define RO_MODE true

#define PREFERENCES_NAME_SPACE "light_statusses"
#define ALREADY_INITIALIZED_KEY "nvsInit"

#define INTERIOR_LIGHTS_ACTIVE "intLightsOn"
#define EXTERIOR_LIGHTS_ACTIVE "extLightsOn"
#define REAR_LIGHTS_ACTIVE "rearLightsOn"
#define LZ1_ACTIVE "lz1Active"

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
        preferences.putBool(EXTERIOR_LIGHTS_ACTIVE, false);
        preferences.putBool(REAR_LIGHTS_ACTIVE, false);
        preferences.putBool(LZ1_ACTIVE, false);

        preferences.putBool(ALREADY_INITIALIZED_KEY, true);

        preferences.end();
        preferences.begin(PREFERENCES_NAME_SPACE, RO_MODE);
    }

    //puzzle together the result 
    Preferences_Data_Struct result;
    result.interiorLightsActive = preferences.getBool(INTERIOR_LIGHTS_ACTIVE, false);
    result.exteriorLightsActive = preferences.getBool(EXTERIOR_LIGHTS_ACTIVE, false);
    result.rearLightsActive     = preferences.getBool(REAR_LIGHTS_ACTIVE, false);
    result.lz1Active            = preferences.getBool(LZ1_ACTIVE, false);
    preferences.end();
    return result;
}

void storeLightStates(Preferences_Data_Struct states) {
    preferences.begin(PREFERENCES_NAME_SPACE, RW_MODE); 
    preferences.putBool(INTERIOR_LIGHTS_ACTIVE, states.interiorLightsActive);
    preferences.putBool(EXTERIOR_LIGHTS_ACTIVE, states.exteriorLightsActive);
    preferences.putBool(REAR_LIGHTS_ACTIVE, states.rearLightsActive);
    preferences.putBool(LZ1_ACTIVE, states.lz1Active);
    preferences.end(); 
}
