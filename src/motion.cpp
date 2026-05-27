#include "motion.h"

#ifdef SIMULATOR
#include "api.h"

void Motion::init() {}

void Motion::moveForward() { API::moveForward(); }
void Motion::turnLeft()    { API::turnLeft(); }
void Motion::turnRight()   { API::turnRight(); }
void Motion::turnAround()  { API::turnAround(); }

void Motion::setPID(float kp, float ki, float kd) {
    // No-op in simulator
    (void)kp; (void)ki; (void)kd;
}

#else
// ---- HARDWARE VERSION ----
#include <Arduino.h>

// Motor pins (update to match your TB6612FNG wiring)
static const int PIN_AIN1 = 16;
static const int PIN_AIN2 = 17;
static const int PIN_PWMA = 18;
static const int PIN_BIN1 = 19;
static const int PIN_BIN2 = 21;
static const int PIN_PWMB = 22;
static const int PIN_STBY = 23;

static float pidKp = 1.0, pidKi = 0.0, pidKd = 0.0;

void Motion::init() {
    pinMode(PIN_AIN1, OUTPUT);
    pinMode(PIN_AIN2, OUTPUT);
    pinMode(PIN_BIN1, OUTPUT);
    pinMode(PIN_BIN2, OUTPUT);
    pinMode(PIN_STBY, OUTPUT);
    digitalWrite(PIN_STBY, HIGH);

    // Set up PWM channels
    ledcSetup(0, 5000, 8);
    ledcSetup(1, 5000, 8);
    ledcAttachPin(PIN_PWMA, 0);
    ledcAttachPin(PIN_PWMB, 1);
}

void Motion::moveForward() {
    // TODO: use encoders + PID to drive exactly one cell
    // Placeholder: drive forward for a fixed time
    digitalWrite(PIN_AIN1, HIGH);
    digitalWrite(PIN_AIN2, LOW);
    digitalWrite(PIN_BIN1, HIGH);
    digitalWrite(PIN_BIN2, LOW);
    ledcWrite(0, 150);
    ledcWrite(1, 150);
    delay(500);  // Replace with encoder-based distance
    ledcWrite(0, 0);
    ledcWrite(1, 0);
}

void Motion::turnLeft() {
    // TODO: encoder-based 90-degree turn
    digitalWrite(PIN_AIN1, LOW);
    digitalWrite(PIN_AIN2, HIGH);
    digitalWrite(PIN_BIN1, HIGH);
    digitalWrite(PIN_BIN2, LOW);
    ledcWrite(0, 120);
    ledcWrite(1, 120);
    delay(300);
    ledcWrite(0, 0);
    ledcWrite(1, 0);
}

void Motion::turnRight() {
    digitalWrite(PIN_AIN1, HIGH);
    digitalWrite(PIN_AIN2, LOW);
    digitalWrite(PIN_BIN1, LOW);
    digitalWrite(PIN_BIN2, HIGH);
    ledcWrite(0, 120);
    ledcWrite(1, 120);
    delay(300);
    ledcWrite(0, 0);
    ledcWrite(1, 0);
}

void Motion::turnAround() {
    // Single continuous 180° spin — same direction as turnRight but double duration
    digitalWrite(PIN_AIN1, HIGH);
    digitalWrite(PIN_AIN2, LOW);
    digitalWrite(PIN_BIN1, LOW);
    digitalWrite(PIN_BIN2, HIGH);
    ledcWrite(0, 120);
    ledcWrite(1, 120);
    delay(600);  // ~2x turnRight duration, tune with encoders
    ledcWrite(0, 0);
    ledcWrite(1, 0);
}

void Motion::setPID(float kp, float ki, float kd) {
    pidKp = kp;
    pidKi = ki;
    pidKd = kd;
}

#endif

// Shared logic
void Motion::face(Direction target, Direction& current) {
    int diff = ((int)target - (int)current + 4) % 4;
    if (diff == 1) {
        turnRight();
    } else if (diff == 3) {
        turnLeft();
    } else if (diff == 2) {
        turnAround();
    }
    current = target;
}
