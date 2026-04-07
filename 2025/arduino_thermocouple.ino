#include <SPI.h>
#include "Adafruit_MAX31855.h"

#define SAMPLE_SIZE 7
#define MAXDO   8          // MISO (Data Out)
#define MAXCS   10          // Chip Select (CS)
#define MAXCLK  13          // Serial Clock (SCK)

Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO); //initialize an arduino object

const int    power          = A1; //port for power output
const double null_temp      = 0.0;
const double max_delta_temp = 10.0; //threshold for stop
const double initial_temp   = 20.0;
const long   interval       = 300; //Time between temp measurements

void setup() {
  Serial.begin(9600);
  Serial.println("MAX31855 Thermocouple Test");

  if (!thermocouple.begin()) {
    Serial.println("Could not find a valid MAX31855 sensor!"); //Error initializing arduino
    while (1);
  }

  pinMode(power, OUTPUT);
}

void off() { //Shut off the engine
  digitalWrite(power, HIGH);
  Serial.println("off");
  while (1);
}

double temp_one;
double temp_two;
double temp_three;

void loop() {
  digitalWrite(power, LOW);

  delay(1000);
  double curr_temp = initial_temp;
  double delta_temp = 0.0;

  Serial.print("initial_temp: "); //Print initial temperature and begin loop
  Serial.println(initial_temp);
  Serial.println("Starting loop!");

  unsigned long previousMillis = millis(); //update previous and current time counters
  unsigned long currentMillis = millis();

  temp_one = thermocouple.readCelsius(); //measure current temp

  while (isnan(temp_one)){ //attempt read continuously until a real value is found
    temp_one = thermocouple.readCelsius();
  }

  temp_two = temp_one; //Update 2nd and third temperatures
  temp_three = temp_one;

  int count = 0; //decide which temp to measure
  while (delta_temp < max_delta_temp) { //Repeat this loop until temperature difference exceeds threshold

    if (currentMillis - previousMillis >= interval) { //After one interval of time passes
      previousMillis = currentMillis; //set the last time a measurement was taken

      curr_temp = thermocouple.readCelsius(); //measure temp

      if (!isnan(curr_temp)) { //if the measurement is a real number
        if (count == 0) { //Update a certain variable based on the value of 'count'
          temp_one = curr_temp;
        }
	else if (count == 1) {
          temp_two = curr_temp;
        }
	else {
          temp_three = curr_temp;
        }

	bool 1over2 = (temp_one >= temp_two); //Compare each temp
	bool 1over3 = (temp_one >= temp_three);
	bool 2over3 = (temp_two >= temp_three);

	//determine median temp for comparison
        if ((1over2 && !1over3) || (!1over2 && 1over3)){ //If temp_one is the median
          curr_temp = temp_one;
        }

	else if ((!1over2 && !2over3) || (1over2 && 2over3)) { //If temp_two is the median
          curr_temp = temp_two;
        }

	else { //Otherwise 3 must be the median
          curr_temp = temp_three;
        }

        delta_temp = curr_temp - initial_temp; //calculate overall change in temp

        Serial.print("curr_temp: ");
        Serial.print(curr_temp);
        Serial.print(", delta_temp: ");
        Serial.println(delta_temp);

        count++; //increment count
        if (count > 2) { //reset count when it exceeds 2
          count = 0;
        }
      }

      else { //Print 0 when temp value is read incorrectly
        Serial.println("null temp");
      }
    }

    currentMillis = millis(); //Update current time
  }

  off(); //shut off after loop concludes
}
