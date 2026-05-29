#include "API.h"
#include <Arduino.h>

// =============================================================
// PIN DEFINICIJE
// =============================================================

// IR senzori — emiteri
#define IR_EMIT_L   25
#define IR_EMIT_F   26
#define IR_EMIT_R   27

// IR senzori — primači (ADC)
#define IR_RECV_L   4
#define IR_RECV_F   36
#define IR_RECV_R   39

// Motor A (lijevi)
#define MOTOR_A_IN1 5
#define MOTOR_A_IN2 22

// Motor B (desni)
#define MOTOR_B_IN1 19
#define MOTOR_B_IN2 21

// Enkoderi
#define ENC_A1 34
#define ENC_A2 35
#define ENC_B1 32
#define ENC_B2 33

// =============================================================
// KONSTANTE — izmjeri i podesi!
// =============================================================

// Teorijske vrijednosti za N20 300RPM, gear 1:30, kotač 34mm
// OBAVEZNO kalibriraj enkoder testom prije koristenja!
#define TICKS_PER_CELL   354   // tickova za 180mm ravno
#define TICKS_PER_90DEG  185   // tickova za 90° okret (izmjeri wheelbase!)

// Brzina motora (0-255 PWM)
#define MOTOR_SPEED      180   // osnovna brzina kretanja
#define TURN_SPEED       150   // brzina okretanja

// IR threshold — ADC vrijednost ispod koje smatramo da postoji zid
// Kalibrirati: emiter ON, primač čita, izmjeri vrijednosti sa zidom i bez
#define WALL_THRESHOLD   2000  // podesi nakon kalibracije!

// =============================================================
// ENKODERI — interrupt varijable
// =============================================================

volatile long enc_a = 0;
volatile long enc_b = 0;

void IRAM_ATTR isr_enc_a() {
    if (digitalRead(ENC_A2) == HIGH) enc_a++;
    else enc_a--;
}

void IRAM_ATTR isr_enc_b() {
    if (digitalRead(ENC_B2) == HIGH) enc_b++;
    else enc_b--;
}

// =============================================================
// INTERNI HELPERS
// =============================================================

// Postavi brzinu i smjer motora
// speed > 0 = naprijed, speed < 0 = nazad, speed = 0 = stop
static void motor_a_set(int speed) {
    if (speed > 0) {
        analogWrite(MOTOR_A_IN1, speed);
        digitalWrite(MOTOR_A_IN2, LOW);
    } else if (speed < 0) {
        digitalWrite(MOTOR_A_IN1, LOW);
        analogWrite(MOTOR_A_IN2, -speed);
    } else {
        // Brake: oba HIGH za DRV8833
        digitalWrite(MOTOR_A_IN1, HIGH);
        digitalWrite(MOTOR_A_IN2, HIGH);
    }
}

static void motor_b_set(int speed) {
    if (speed > 0) {
        analogWrite(MOTOR_B_IN1, speed);
        digitalWrite(MOTOR_B_IN2, LOW);
    } else if (speed < 0) {
        digitalWrite(MOTOR_B_IN1, LOW);
        analogWrite(MOTOR_B_IN2, -speed);
    } else {
        digitalWrite(MOTOR_B_IN1, HIGH);
        digitalWrite(MOTOR_B_IN2, HIGH);
    }
}

static void motors_stop() {
    motor_a_set(0);
    motor_b_set(0);
}

// Čitanje jednog IR senzora: emiter ON → ADC → emiter OFF
// Vraća true ako je zid detektiran
static bool read_ir(int emit_pin, int recv_pin) {
    digitalWrite(emit_pin, HIGH);
    delayMicroseconds(100);             // kratka pauza da se emiter stabilizira
    int val = analogRead(recv_pin);
    digitalWrite(emit_pin, LOW);
    return (val > WALL_THRESHOLD);      // veća vrijednost = više reflektiranog svjetla = zid
}

// =============================================================
// INICIJALIZACIJA — pozovi u setup() prije korištenja API-ja
// =============================================================

void API_hw_init() {
    // Motori
    pinMode(MOTOR_A_IN1, OUTPUT);
    pinMode(MOTOR_A_IN2, OUTPUT);
    pinMode(MOTOR_B_IN1, OUTPUT);
    pinMode(MOTOR_B_IN2, OUTPUT);
    motors_stop();

    // Enkoderi
    pinMode(ENC_A1, INPUT);
    pinMode(ENC_A2, INPUT);
    pinMode(ENC_B1, INPUT);
    pinMode(ENC_B2, INPUT);
    attachInterrupt(digitalPinToInterrupt(ENC_A1), isr_enc_a, RISING);
    attachInterrupt(digitalPinToInterrupt(ENC_B1), isr_enc_b, RISING);

    // IR emiteri
    pinMode(IR_EMIT_L, OUTPUT);
    pinMode(IR_EMIT_F, OUTPUT);
    pinMode(IR_EMIT_R, OUTPUT);
    digitalWrite(IR_EMIT_L, LOW);
    digitalWrite(IR_EMIT_F, LOW);
    digitalWrite(IR_EMIT_R, LOW);

    // IR primači — input only, nema pinMode potreban za ADC
    // GPIO4 može trebati pinMode(IR_RECV_L, INPUT)
    pinMode(IR_RECV_L, INPUT);
    // GPIO36 i GPIO39 su strogo input-only, ne trebaju pinMode
}

// =============================================================
// API IMPLEMENTACIJA
// =============================================================

int API_mazeWidth() {
    return 16;
}

int API_mazeHeight() {
    return 16;
}

int API_wallFront() {
    return read_ir(IR_EMIT_F, IR_RECV_F) ? 1 : 0;
}

int API_wallRight() {
    return read_ir(IR_EMIT_R, IR_RECV_R) ? 1 : 0;
}

int API_wallLeft() {
    return read_ir(IR_EMIT_L, IR_RECV_L) ? 1 : 0;
}

int API_moveForward() {
    enc_a = 0;
    enc_b = 0;

    motor_a_set(MOTOR_SPEED);
    motor_b_set(MOTOR_SPEED);

    // Čekaj dok oba enkodera ne dostignu TICKS_PER_CELL
    while (enc_a < TICKS_PER_CELL && enc_b < TICKS_PER_CELL) {
        // TODO: ovdje dodati PID korekciju kad kalibriramo
        delayMicroseconds(100);
    }

    motors_stop();
    delay(50); // kratka pauza da se robot smiri
    return 1;
}

void API_turnRight() {
    enc_a = 0;
    enc_b = 0;

    // Lijevi motor naprijed, desni nazad
    motor_a_set(TURN_SPEED);
    motor_b_set(-TURN_SPEED);

    while (enc_a < TICKS_PER_90DEG && enc_b < TICKS_PER_90DEG) {
        delayMicroseconds(100);
    }

    motors_stop();
    delay(50);
}

void API_turnLeft() {
    enc_a = 0;
    enc_b = 0;

    // Desni motor naprijed, lijevi nazad
    motor_a_set(-TURN_SPEED);
    motor_b_set(TURN_SPEED);

    while (enc_a < TICKS_PER_90DEG && enc_b < TICKS_PER_90DEG) {
        delayMicroseconds(100);
    }

    motors_stop();
    delay(50);
}

// Vizualne/debug funkcije — ispisuju na Serial umjesto simulatora
void API_setWall(int x, int y, char direction) {
    // no-op na hardware, mapa je interno u maze.c
}

void API_clearWall(int x, int y, char direction) {
    // no-op
}

void API_setColor(int x, int y, char color) {
    // no-op
}

void API_clearColor(int x, int y) {
    // no-op
}

void API_clearAllColor() {
    // no-op
}

void API_setText(int x, int y, char* text) {
    // Opcionalno: ispis na Serial za debug
    // Serial.printf("[%d,%d] %s\n", x, y, text);
}

void API_clearText(int x, int y) {
    // no-op
}

void API_clearAllText() {
    // no-op
}

// Reset — na hardware provjeravamo fizički gumb ili pin
// Za sada uvijek vraća false (nema reseta)
// TODO: spoji gumb na neki GPIO i provjeri ga ovdje
int API_wasReset() {
    return 0;
}

void API_ackReset() {
    // no-op
}