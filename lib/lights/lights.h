#ifndef LIGHTS_H
#define LIGHTS_H

#include <Arduino.h>

enum class LightMode: uint8_t {
    Off = 0,
    FrontOnly,
    FrontAndRear,
    Lz1
  };

class Lights {
    public:
        Lights(bool direction, LightMode mode, bool cabLightActive);
        void setMode(LightMode mode);
        void setDirection(bool direction);
        void setCabLightStatus(bool status);
        void write();
        void writeExteriorLights(bool fla, bool flb, bool fra, bool frb, bool ft, bool rla, bool rlb, bool rra, bool rrb, bool rt);
        LightMode getMode();
    private:
        bool direction;
        LightMode lightMode;
        bool cabLightActive;
        void writeLightsForward();
        void writeLightsBackward();
        void writeFrontLightsWhite();
        void writeFrontLightsRed();
        void writeFrontLightsOff();
        void writeRearLightsWhite();
        void writeRearLightsRed();
        void writeRearLightsOff();
        void writeLz1();
        void writeFrontLights(bool fla, bool flb, bool fra, bool frb, bool ft);
        void writeRearLights(bool rla, bool rlb, bool rra, bool rrb, bool rt);
};


#endif