#pragma once

#ifndef STICK_H
#define STICK_H

#include <Arduino.h>

class RightStickPathDetector {
public:
    enum class Zone {
        Center,
        Up,
        Down,
        Left,
        Right
    };

    enum class Path {
        None,
        Up_Left,
        Up_Right,
        Right_Up,
        Right_Down,
        Down_Left,
        Down_Right,
        Left_Up,
        Left_Down
    };

    struct Result {
        bool pathDetected;
        Path path;
    };

    RightStickPathDetector(uint16_t threshold) : _threshold(threshold) {}

    Result update(uint16_t x, uint16_t y) {
        Zone currentZone = determineZone(x, y);
        Result result{false, Path::None};

        switch (_state) {
            case State::Idle:
                if (currentZone != Zone::Center) {
                    _firstZone = currentZone;
                    _state = State::FirstDetected;
                }
                break;
            case State::FirstDetected:
                /*if (currentZone == Zone::Center) {
                }else*/
                    if (currentZone != _firstZone) {
                    result.pathDetected = true;
                    result.path = buildPath(_firstZone, currentZone);
                    _state = State::Completed;
                }
                break;
            case State::Completed:
                if (currentZone == Zone::Center) {
                    reset();
                }
                break;
        }
        return result;
    }

private:
    enum class State {
        Idle,
        FirstDetected,
        Completed
    };

    State _state = State::Idle;
    Zone _firstZone = Zone::Center;
    uint16_t _threshold;

    void reset() {
        _state = State::Idle;
        _firstZone = Zone::Center;
    }

    Zone determineZone(uint16_t x, uint16_t y) {
        if (y > 1024 - _threshold && _firstZone != Zone::Up) {
            return Zone::Up;
        }
        if (y < _threshold && _firstZone != Zone::Down) {
            return Zone::Down;
        }
        if (x > 1024 - _threshold) {
            return Zone::Left;
        }
        if (x < _threshold) {
            return Zone::Right;
        }
        return Zone::Center;
    }

    Path buildPath(Zone from, Zone to) {
        if (from == Zone::Up && to == Zone::Left)   return Path::Up_Left;
        if (from == Zone::Up && to == Zone::Right)  return Path::Up_Right;

        if (from == Zone::Right && to == Zone::Up)    return Path::Right_Up;
        if (from == Zone::Right && to == Zone::Down)  return Path::Right_Down;

        if (from == Zone::Down && to == Zone::Left)  return Path::Down_Left;
        if (from == Zone::Down && to == Zone::Right) return Path::Down_Right;

        if (from == Zone::Left && to == Zone::Up)    return Path::Left_Up;
        if (from == Zone::Left && to == Zone::Down)  return Path::Left_Down;

        return Path::None;
    }

};
#endif