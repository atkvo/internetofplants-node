#include <SPI.h>
#include "RF24.h"
#include "SparkFun_Si7021_Breakout_Library.h"

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 9 & 10 */
RF24 radio(9,10);

Weather sensor; /* I2C based Si7021 */

byte hubID[6] = "HUB";    /* ID of the central hub. Do not edit */
byte nodeID[6] = "Node0"; /* ID of the plant node. EDIT. */

struct payload_send_t {                
  uint16_t light; 
  uint16_t moisture; 
  uint16_t humidity;
  uint16_t temp; 
};

void packageData(payload_send_t *payload);

void setup() {
    Serial.begin(115200);
    sensor.begin();
    radio.begin();

    // radio.setPALevel(RF24_PA_MIN);
    radio.setPALevel(RF24_PA_MAX);
  
    radio.openReadingPipe(1, nodeID);
    radio.openWritingPipe(hubID);
    Serial.println("begin listening");
    // Start the radio listening for data
    radio.startListening();  
}

void loop() {
    unsigned long got_time;
    if( radio.available() ) {
                                                                    // Variable for the received timestamp
      while (radio.available()) {                                   // While there is data ready
        radio.read( &got_time, sizeof(unsigned long) );             // Get the payload
      }

      radio.stopListening();                                        // First, stop listening so we can talk 
      payload_send_t response;
      packageData(&response);
      
      radio.write( &response, sizeof(payload_send_t) );
      radio.startListening();                                       // Now, resume listening so we catch the next packets.     
      Serial.print(F("Sent response "));
      Serial.println(got_time);  
   }
}

void packageData(payload_send_t *payload) { 
    payload->light = analogRead(A0);
    payload->moisture = analogRead(A2);
    payload->humidity = sensor.getRH();
    payload->temp = sensor.getTempF();
    Serial.println(payload->light);
    Serial.println(payload->moisture);
    Serial.println(payload->humidity);
    Serial.println(payload->temp);
}

