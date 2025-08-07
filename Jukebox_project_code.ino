#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>


// Pins
const int analogInPin = A0;
const int segmentPins[7] = {D0, D3, D4, D5, D6, D7, D8};


// Variables
int sensorValue = 0;
int state = -1;         // Current button state: -1 = no button, 0 = UP, 1 = DOWN
int lastState = -1;     // Previous stable button state
int number = 0;         // Displayed number (0–9)
int lastPlayed = -1;    // Tracks last played number


// 7-segment digit patterns (common cathode)
const byte digits[10][7] = {
  {1, 1, 1, 1, 1, 1, 0}, // 0
  {0, 1, 1, 0, 0, 0, 0}, // 1
  {1, 1, 0, 1, 1, 0, 1}, // 2
  {1, 1, 1, 1, 0, 0, 1}, // 3
  {0, 1, 1, 0, 0, 1, 1}, // 4
  {1, 0, 1, 1, 0, 1, 1}, // 5
  {1, 0, 1, 1, 1, 1, 1}, // 6
  {1, 1, 1, 0, 0, 0, 0}, // 7
  {1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 1, 0, 1, 1}  // 9
};


SoftwareSerial mySerial(D1, D2);  // RX, TX
DFRobotDFPlayerMini player;


void setup() {
  Serial.begin(115200);
  mySerial.begin(9600);


  Serial.println("Initializing DFPlayer...");
  if (player.begin(mySerial)) {
    Serial.println("DFPlayer initialized.");
    player.volume(20); // Set volume (0–30)
  } else {
    Serial.println("DFPlayer initialization failed.");
  }


  for (int i = 0; i < 7; i++) {
    pinMode(segmentPins[i], OUTPUT);
  }


  showDigit(number);
  stopMusic(); // Ensure nothing plays on startup
}


void loop() {
  readSwitch();


  if (state != lastState) {
    Serial.print("Button state changed: ");
    Serial.print(lastState);
    Serial.print(" -> ");
    Serial.println(state);
    lastState = state;


    if (state == 0 && number < 9) {     // UP button pressed
      number++;
      updateDisplayAndPlay();
    } else if (state == 1 && number > 0) {  // DOWN button pressed
      number--;
      updateDisplayAndPlay();
    }
  }


  delay(50);  // Small delay to reduce bouncing and rapid cycling
}


void readSwitch() {
  sensorValue = analogRead(analogInPin);
  Serial.print("Analog read: ");
  Serial.println(sensorValue);


  if (sensorValue < 150 && sensorValue > 1) {
    state = 0;  // UP button
  } else if (sensorValue < 1024 && sensorValue > 650) {
    state = 1;  // DOWN button
  } else {
    state = -1; // No button
  }
}


void showDigit(int num) {
  for (int i = 0; i < 7; i++) {
    digitalWrite(segmentPins[i], digits[num][i]);
  }
}


void updateDisplayAndPlay() {
  Serial.print("Display Number: ");
  Serial.println(number);


  showDigit(number);


  Serial.println("Waiting 1 second before playing track...");
  delay(1000);  // 1 second delay to let the speaker rest


  handleMusic(number);
}


void handleMusic(int num) {
  if (num == 0) {
    if (lastPlayed != -1) {
      Serial.println("Display is 0 → stopping any playing track.");
      stopMusic();
      lastPlayed = -1;
    } else {
      Serial.println("Display is 0 → nothing to stop.");
    }
    return;
  }


  if (num != lastPlayed) {
    Serial.print("Playing track: 00");
    Serial.println(num);
    player.play(num);  // Plays 001 to 009
    lastPlayed = num;
  } else {
    Serial.println("Same track already playing → no action.");
  }
}


void stopMusic() {
  Serial.println("Sending stop command to DFPlayer...");
  player.stop();  // Stop any playing track
}




 
