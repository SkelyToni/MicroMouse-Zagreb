#include "sensors.h"

#ifdef SIMULATOR
#include "api.h"

void Sensors::init() {
    // Nothing to init in simulator
}

bool Sensors::wallFront() { return API::wallFront(); }
bool Sensors::wallLeft()  { return API::wallLeft(); }
bool Sensors::wallRight() { return API::wallRight(); }

uint16_t Sensors::readFront() { return wallFront() ? 500 : 100; }
uint16_t Sensors::readLeft()  { return wallLeft()  ? 500 : 100; }
uint16_t Sensors::readRight() { return wallRight() ? 500 : 100; }

#else
// ---- HARDWARE VERSION ----
// Fill these in when your ESP32 is wired up

#include <Arduino.h>

// Pin assignments (update to match your wiring)
static const int PIN_IR_FRONT_EMIT = 25;
static const int PIN_IR_FRONT_RECV = 34;
static const int PIN_IR_LEFT_EMIT  = 26;
static const int PIN_IR_LEFT_RECV  = 35;
static const int PIN_IR_RIGHT_EMIT = 27;
static const int PIN_IR_RIGHT_RECV = 32;

// Threshold: above this value = wall present
static const uint16_t WALL_THRESHOLD = 300;

void Sensors::init() {
    pinMode(PIN_IR_FRONT_EMIT, OUTPUT);
    pinMode(PIN_IR_LEFT_EMIT,  OUTPUT);
    pinMode(PIN_IR_RIGHT_EMIT, OUTPUT);
}

static uint16_t readIR(int emitPin, int recvPin) {
    digitalWrite(emitPin, HIGH);
    delayMicroseconds(100);
    uint16_t val = analogRead(recvPin);
    digitalWrite(emitPin, LOW);
    return val;
}

uint16_t Sensors::readFront() { return readIR(PIN_IR_FRONT_EMIT, PIN_IR_FRONT_RECV); }
uint16_t Sensors::readLeft()  { return readIR(PIN_IR_LEFT_EMIT,  PIN_IR_LEFT_RECV); }
uint16_t Sensors::readRight() { return readIR(PIN_IR_RIGHT_EMIT, PIN_IR_RIGHT_RECV); }

bool Sensors::wallFront() { return readFront() > WALL_THRESHOLD; }
bool Sensors::wallLeft()  { return readLeft()  > WALL_THRESHOLD; }
bool Sensors::wallRight() { return readRight() > WALL_THRESHOLD; }

#endif
