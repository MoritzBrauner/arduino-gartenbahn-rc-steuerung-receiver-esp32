#ifndef STORAGE_H
#define STORAGE_H
#include "../lights/lights.h"

struct Preferences_Data_Struct {
    LightMode lightMode;
    bool interiorLightsActive; 
    bool direction;
};

Preferences_Data_Struct initPreferences();

void storeLightStates(Preferences_Data_Struct states); 

#endif
