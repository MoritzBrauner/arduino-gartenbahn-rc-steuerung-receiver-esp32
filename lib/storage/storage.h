#ifndef STORAGE_H
#define STORAGE_H

#include <Preferences.h>

struct Preferences_Data_Struct {
    bool interiorLightsActive; 
    bool exteriorLightsActive;  
    bool rearLightsActive; 
    bool lz1Active; 
};

Preferences_Data_Struct initPreferences();

void storeLightStates(Preferences_Data_Struct states); 

#endif
