//#include <RH_ASK.h>
//#include <SPI.h>
//
//RH_ASK rf(2000, 11, 12);   // defaults: 2000 bps, TX=D12, RX=D11
//
//const int JOY_X  = A0;
//const int JOY_Y  = A1;
//const int JOY_SW = 2;
//
//// Packet format:
//// [0] 0xAA (start)
//// [1] X (0–255)
//// [2] Y (0–255)
//// [3] flags (bit0 = STOP)
//// [4] checksum = XOR(0..3)
//uint8_t packet[5];
//
//void setup() {
//  pinMode(JOY_SW, INPUT_PULLUP);
//  rf.init();
//}
//
//void loop() {
//  uint8_t x = analogRead(JOY_X) >> 2;  // 0–1023 → 0–255
//  uint8_t y = analogRead(JOY_Y) >> 2;
//
//  packet[0] = 0xAA;
//  packet[1] = x;
//  packet[2] = y;
//  packet[3] = (digitalRead(JOY_SW) == LOW) ? 0x01 : 0x00;
//  packet[4] = packet[0] ^ packet[1] ^ packet[2] ^ packet[3];
//
//  rf.send(packet, sizeof(packet));
//  rf.waitPacketSent();
//
//  delay(40);   // ~25 Hz update rate
//}

// ask_receiver.pde
// -*- mode: C++ -*-
// Simple example of how to use RadioHead to receive messages
// with a simple ASK transmitter in a very simple way.
// Implements a simplex (one-way) receiver with an Rx-B1 module
// Tested on Arduino Mega, Duemilanova, Uno, Due, Teensy, ESP-12

#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile

RH_ASK driver;

void setup()
{
    Serial.begin(9600);    // Debugging only
    if (!driver.init())
         Serial.println("init failed");
}

void loop()
{
    const char *msg = "Hello World!";
    driver.send((uint8_t *)msg, strlen(msg));
    driver.waitPacketSent();
    delay(1000);
}
