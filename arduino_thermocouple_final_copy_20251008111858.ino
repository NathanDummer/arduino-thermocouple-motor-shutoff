#include <SPI.h>
#include "Adafruit_MAX31855.h"

#define SAMPLE_SIZE 7
#define MAXDO   12          // MISO (Data Out)
#define MAXCS   10          // Chip Select (CS)
#define MAXCLK  13          // Serial Clock (SCK)

Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);

const int power = A1;
const double null_temp = 0.0;
const double max_delta_temp = 10.0;
const double initial_temp = 20.0;
const long interval = 300;

void setup() {
  Serial.begin(9600);
  Serial.println("MAX31855 Thermocouple Test");

  if (!thermocouple.begin()) {
    Serial.println("Could not find a valid MAX31855 sensor!");
    while (1);
  }

  pinMode(power, OUTPUT);
}

void off() {
  digitalWrite(power, HIGH);
  Serial.println("off");
  while (1); 
}

double find_median(temp_one, temp_two, temp_three) {
  if ((temp_one >= temp_two && temp_one <= temp_three) || (temp_one >= temp_three && temp_one <= temp_two)) {
    return temp_one;
  } else if ((temp_two >= temp_one && temp_two <= temp_three) || (temp_two >= temp_three && temp_two <= temp_one)) {
    return temp_two;
  } else {
    return temp_three;
  }
}

void loop() {
  digitalWrite(power, LOW);
  delay(1000);

  double delta_temp = 0.0;
  double curr_temp, temp_one, temp_two, temp_three;

  Serial.print("initial_temp: ");
  Serial.println(initial_temp);
  Serial.println("Starting loop!");

  unsigned long previousMillis = millis(); 
  unsigned long currentMillis = millis();
  
  temp_one = thermocouple.readCelsius();
  while (isnan(temp_one)) {
    temp_one = thermocouple.readCelsius();
  }
  temp_two = temp_one;
  temp_three = temp_one;

  int count = 0;
  while (delta_temp < max_delta_temp) {

    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      curr_temp = thermocouple.readCelsius();

      if (!isnan(curr_temp)) {
        if (count == 0) {
          temp_one = curr_temp;
        } else if (count == 1) {
          temp_two = curr_temp;
        } else {
          temp_three = curr_temp;
        }

        // finding the median temp from the last three readings
        curr_temp = find_median(temp_one, temp_two, temp_three);
        delta_temp = curr_temp - initial_temp;
        
        Serial.print("curr_temp: ");
        Serial.print(curr_temp);
        Serial.print(", delta_temp: ");
        Serial.println(delta_temp);

        count++;
        if (count > 2) {
          count = 0;
        }
      } else {
        Serial.println("null temp");
      }
    }

    currentMillis = millis();
  }
  off();
}
