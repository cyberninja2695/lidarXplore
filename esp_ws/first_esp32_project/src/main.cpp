#include <Arduino.h>

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
  delay(500);
  Serial.begin(115200);
  delay(500);
  
  Serial.println("\n\n================================");
  Serial.println("Holaaaaa Baaaaabeeeeeeeee");
  Serial.print("Result: "); Serial.println(result);
  Serial.print("Chip Model: ");
  Serial.println(ESP.getChipModel());
  Serial.print("Chip version: ");
  Serial.println(ESP.getChipRevision());
  Serial.print("Numer of cores: ");
  Serial.println(ESP.getChipCores());
  Serial.print("Flash Chip Size: ");
  Serial.println(ESP.getFlashChipSize());
  Serial.print("Flash Chip Speed: ");
  Serial.println(ESP.getFlashChipSpeed());

  Serial.println("================================");
}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}