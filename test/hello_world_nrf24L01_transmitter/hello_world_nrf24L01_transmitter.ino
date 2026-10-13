#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(10, 9); // CE, CSN
static const uint64_t pipe = 0xE8E8F0F0E1LL;

void setup() {
  radio.begin();
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(pipe);
  Serial.begin(9600);  // starting the Serial Monitor

}

void loop() {
  const char text[] = "nrftest";  // Proper null-terminated C-string
  radio.write(&text, sizeof(text));  // Send only the used bytes (7 here)
  delay(2000);
}
