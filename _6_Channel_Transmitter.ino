// 6 Channel Transmitter
// Matches the 6-channel receiver structure

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Data packet structure (must match receiver)
struct Signal {
  byte throttle;
  byte yaw;
  byte pitch;
  byte roll;
  byte aux1;
  byte aux2;
};

Signal data;

// Radio: CE pin 7, CSN pin 8
RF24 radio(7, 8);
const uint64_t pipeOut = 0xE9E8F0F0E1LL;

// Joystick / potentiometer pins
#define THROTTLE_PIN A0
#define YAW_PIN      A1
#define PITCH_PIN    A2
#define ROLL_PIN     A3
#define AUX1_PIN     4   // Button / switch
#define AUX2_PIN     A4

// Set default control positions
void ResetData() {
  data.roll     = 127;
  data.pitch    = 127;
  data.throttle = 0;
  data.yaw      = 127;
  data.aux1     = 0;
  data.aux2     = 127;
}

void setup() {
  radio.begin();
  radio.openWritingPipe(pipeOut);
  radio.setAutoAck(false);
  radio.setCRCLength(RF24_CRC_8);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(108);
  ResetData();
}

void loop() {
  // Read and map joystick/potentiometer values
  data.throttle = map(analogRead(THROTTLE_PIN), 0, 1023, 0, 255);
  data.yaw      = map(analogRead(YAW_PIN),      0, 1023, 0, 255);
  data.pitch    = map(analogRead(PITCH_PIN),    0, 1023, 0, 255);
  data.roll     = map(analogRead(ROLL_PIN),     0, 1023, 0, 255);

  // Read auxiliary inputs
  data.aux1     = digitalRead(AUX1_PIN);
  data.aux2     = map(analogRead(AUX2_PIN),     0, 1023, 0, 255);

  // Send packet
  radio.write(&data, sizeof(Signal));

  delay(20); // 50 Hz update rate
}
