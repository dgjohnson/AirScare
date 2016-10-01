// ---------------------------------------------------------------------------
// This file controls pneumatic valves based on proximity of an object.
// using an Arduino Pro Mini, an HC-SR04 ultrasonic sensor and the NewPing library
// this code will detect when an object breaks a configurable threshold to enable a pneumatic 
// valve(s) to release air and "scare" the unsuspecting guest.
// ---------------------------------------------------------------------------
#include <NewPing.h>

#define SENSITIVITY   50 // the change threshold that triggers a blast of air.
#define AIR_LENGTH    500// the blast of air length in ms.
#define AIR_DELAY    5000// wait between blasts of air length in ms.
#define AIR1_PIN   8 // Arduino pin tied to solenoid 1.
#define AIR2_PIN   9 // Arduino pin tied to solenoid 2.
#define TRIGGER_PIN   12 // Arduino pin tied to trigger pin on ping sensor.
#define ECHO_PIN      11 // Arduino pin tied to echo pin on ping sensor.
#define MAX_DISTANCE 100 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

unsigned int pingSpeed = 50; // How frequently are we going to send out a ping (in milliseconds). 50ms would be 20 times a second.
unsigned long pingTimer;     // Holds the next ping time.
unsigned long airTimer;     // Holds the next air off time.
int furthest = 0; // the furthest distance we have detected. 

void setup() {
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
  pinMode(AIR1_PIN, OUTPUT); //solenoid 1
  pinMode(AIR2_PIN, OUTPUT); //solenoid 2
  pingTimer = millis(); // Start now.
}

void loop() {
  // Notice how there's no delays in this sketch to allow you to do other processing in-line while doing distance pings.
  if (millis() >= pingTimer) {   // pingSpeed milliseconds since last ping, do another ping.
    pingTimer += pingSpeed;      // Set the next ping time.
    sonar.ping_timer(echoCheck); // Send out the ping, calls "echoCheck" function every 24uS where you can check the ping status.
  }
  // Do other stuff here, really. Think of it as multi-tasking.
}

void echoCheck() { // Timer2 interrupt calls this function every 24uS where you can check the ping status.
  // Don't do anything here!
  if (sonar.check_timer()) { // This is how you check to see if the ping was received.
    // Here's where you can add code.
    int currentDistance = sonar.ping_result / US_ROUNDTRIP_CM;
    furthest = furthest < currentDistance ? currentDistance:furthest; 
    if(furthest-currentDistance > SENSITIVITY && millis()>airTimer + AIR_DELAY)
    {
      furthest = currentDistance;
      airTimer = millis()+AIR_LENGTH;
      if(digitalRead(AIR1_PIN)==LOW)
      {
        Serial.print("Air ON!!! ");
        digitalWrite(AIR1_PIN, HIGH);
        digitalWrite(AIR2_PIN, HIGH);
      }
    }
    else
    {
      //turn off the air if it is past time
      if(millis() >= airTimer && digitalRead(AIR1_PIN)==HIGH)
      {
        Serial.print("Air OFF!!! ");
        digitalWrite(AIR1_PIN, LOW);
        digitalWrite(AIR2_PIN, LOW);
        
      }
    }
    Serial.print("Ping: ");
    Serial.print(currentDistance); // Ping returned, uS result in ping_result, convert to cm with US_ROUNDTRIP_CM.
    Serial.println("cm");
  }
  // Don't do anything here!
}
