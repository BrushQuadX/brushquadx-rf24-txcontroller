#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 10); // CE, CSN

static const uint64_t pipe = 0xE8E8F0F0E1LL;

void setup() {
  radio.begin();
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(0, pipe);
  radio.startListening();
  Serial.begin(9600);  // starting the Serial Monitor
  Serial.println("Receiver started");
}

void loop() {
  char text[8] = {};

  if (radio.available()) {
      radio.read(&text, sizeof(text));

      Serial.print("transData: ");
      Serial.println(text);
  }
  delay(200); // Small delay for readability
}
