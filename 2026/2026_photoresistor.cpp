int photoPin = A0;
int toggle_motor = A1;

void setup(){
  Serial.begin(9600);
  pinMode(photoPin, INPUT);
  pinMode(toggle_motor, OUTPUT);
}

void exit(){
  while(1);
}

void loop() {
  delay(1000); //Ensure a readable interval for monitoring
  int light_level = analogRead(photoPin); //This value represents what the photoresistor is measuring
  Serial.println(light_level);

  if(light_level > 200){ //As long as the light_level is above 200, motor is running
    digitalWrite(toggle_motor, LOW);   // Motor ON
  }

  else{ //When light_level drops below 200, motor shuts off
    digitalWrite(toggle_motor, HIGH);    // Motor OFF
    exit();
  }
}
