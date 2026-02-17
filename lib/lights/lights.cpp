#include "lights.h"
#include "pins.h"

Lights::Lights(bool direction, LightMode mode, bool cabLightActive) {
    this->direction = direction;
    this->lightMode = mode;
    this->cabLightActive = cabLightActive;
}

void Lights::setMode(LightMode mode) {
    this->lightMode = mode;
}

void Lights::setDirection(bool direction) {
    this->direction = direction;
}

void Lights::setCabLightStatus(bool status) {
    this->cabLightActive = status;
}

void Lights::write() {
    writePin(PIN_LIGHT_INTERIOR, this->cabLightActive);

    if (lightMode == LightMode::Lz1) {
        writeLz1();
        return;
    }

    if (lightMode == LightMode::Off) {
        writeFrontLightsOff();
        writeRearLightsOff();
        return;
    }

    if (direction) {
        writeLightsForward();
    } else {
        writeLightsBackward();
    }
}

void Lights::writeLightsForward() {
    switch (lightMode) {
        case LightMode::FrontOnly:
            writeFrontLightsWhite();
            writeRearLightsOff();
            break;
        case LightMode::FrontAndRear:
            writeFrontLightsWhite();
            writeRearLightsRed();
            break;
        default: ;
            break;
    }
}

void Lights::writeLightsBackward() {
    switch (lightMode) {
        case LightMode::FrontOnly:
            writeRearLightsWhite();
            writeFrontLightsOff();
            break;
        case LightMode::FrontAndRear:
            writeRearLightsWhite();
            writeFrontLightsRed();
            break;
        default: ;
            break;
    }
}

void Lights::writeFrontLightsWhite() {
    writeFrontLights(true, false, true, false, true);
}

void Lights::writeFrontLightsRed() {
    writeFrontLights(false, true, false, true, false);
}

void Lights::writeFrontLightsOff() {
    writeFrontLights(false, false, false, false, false);
}

void Lights::writeRearLightsWhite() {
    writeRearLights(true, false, true, false, true);
}
void Lights::writeRearLightsRed() {
    writeRearLights(false, true, false, true, false);
}

void Lights::writeRearLightsOff() {
    writeRearLights(false, false, false, false, false);
}

void Lights::writeLz1() {
    writeFrontLights(false, false, true, false, false);
    writeRearLights(true, false, false, false, false);
}

void Lights::writeFrontLights(bool fla, bool flb, bool fra, bool frb, bool ft) {
    writePin(PIN_LIGHT_FL_A, fla);
    writePin(PIN_LIGHT_FL_B, flb);
    writePin(PIN_LIGHT_FR_A, fra);
    writePin(PIN_LIGHT_FR_B, frb);
    writePin(PIN_LIGHT_FT,   ft);
}

void Lights::writeRearLights(bool rla, bool rlb, bool rra, bool rrb, bool rt) {
    writePin(PIN_LIGHT_RL_A, rla);
    writePin(PIN_LIGHT_RL_B, rlb);
    writePin(PIN_LIGHT_RR_A, rra);
    writePin(PIN_LIGHT_RR_B, rrb);
    writePin(PIN_LIGHT_RT,   rt);
}

void Lights::writeExteriorLights(bool fla, bool flb, bool fra, bool frb, bool ft, bool rla, bool rlb, bool rra, bool rrb, bool rt, bool interior) {
    writeFrontLights(fla, flb, fra, frb, ft);
    writeRearLights(rla, rlb, rra, rrb, rt);
    writeInteriorLights(interior);
}

LightMode Lights::getMode() {
    return this->lightMode;
}

bool Lights::getInteriorLightsStatus() {
    return cabLightActive;
}

bool Lights::getDirection() {
    return direction;
}
