#include <Blynk.h>
#include <ESP8266WiFi.h>  
#include <WiFiClient.h>  
#include <ThingSpeak.h>  
#include <Servo.h>
#include <SimpleTimer.h>
#include <BlynkSimpleEsp8266.h>
#define PIN V1
SimpleTimer timer;
char auth[] = "21989e649cc24caba3e33f009c6f70f8";
Servo servo_motor; //initialize servo motor
const char* ssid = "PCK";  //Hotspot name
const char* password = "KHEDEKAR63";  //Hotspot password
WiFiClient client;  
unsigned long myChannelNumber = 592837;  
const char * myWriteAPIKey = "VF908KP48YDYWITK";  


// defines pins numbers
const int trigPin = 14;  //D5
const int echoPin = 12;  //D6
const int buzzPin = 5;   //D1
const int motorPin = 4;  //D2
int angle = 0;
// defines variables
long duration;
int distance;

int percent;

void setup() {
Blynk.begin(auth, ssid, password);//Blynk is ON

pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
pinMode(echoPin, INPUT); // Sets the echoPin as an Input
pinMode(buzzPin, OUTPUT); // buzz pin is output to control buzzering
pinMode(BUILTIN_LED, OUTPUT); //initializing LED
servo_motor.attach(motorPin); //D2 signal pin
Serial.begin(115200); // Starts the serial communication
// Connect to WiFi network  
  Serial.println();  
  Serial.println();  
  Serial.print("Connecting to ");  
  Serial.println(ssid);  
  WiFi.begin(ssid, password);  
  while (WiFi.status() != WL_CONNECTED)
   {  
   delay(500);  
   Serial.print(".");  
  }  
  Serial.println("");  
  Serial.println("WiFi connected");  
  // Print the IP address  
  Serial.println(WiFi.localIP());  
  ThingSpeak.begin(client);
  timer.setInterval(1000, sendPercent);
}
void sendPercent(){
  Blynk.virtualWrite(PIN, percent);
  }


void loop() {
// Clears the trigPin
digitalWrite(trigPin, LOW);
delayMicroseconds(2);

// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);

// Reads the echoPin, returns the 0sound wave travel time in microseconds
duration = pulseIn(echoPin, HIGH);

// Calculating the distance
distance= duration*0.034/2;

percent = 100-(distance*100)/25;//Convert into percentage

  Blynk.virtualWrite(V1, percent);//Sent data to blynk server
// Prints the distance on the Serial Monitor
Serial.print("Distance: ");
Serial.println(distance);//divide for output in 100digit
//delay(1000);
if (distance <= 5) {
      // Buzz
      Serial.print("Water tank is full");
      digitalWrite(buzzPin, HIGH);
      digitalWrite(BUILTIN_LED, HIGH);// LED OFF
      Blynk.email("MailID", "Water level", "Tank is Full !!!");//your mail id
     servo_motor.detach(); //deatach the servo motor
    } 
    else if(distance >= 20) {
       Serial.print("Sensor is distracted.....");
       digitalWrite(BUILTIN_LED, LOW);// LED on
       digitalWrite(buzzPin, LOW);
       
      }
    else if(distance<=17 && distance>=13) {
      digitalWrite(BUILTIN_LED, LOW);            // LED on
      digitalWrite(buzzPin, LOW);                // Buzzer OFF
      servo_motor.attach(motorPin);              //Attach motor
      for(angle = 0; angle < 180; angle += 1)    // command to move from 0 degrees to 180 degrees 
        {                                  
          servo_motor.write(angle);             //command to rotate the servo to the specified angle
          delay(15);                       
        } 
        delay(100);
       for(angle = 180; angle>=1; angle-=5)    // command to move from 180 degrees to 0 degrees 
        {                                
          servo_motor.write(angle);             //command to rotate the servo to the specified angle
          delay(15);                       
        } 
      }
    else {
      // Don't buzz
      servo_motor.attach(motorPin);              //Attach motor
      for(angle = 0; angle < 180; angle += 1)    // command to move from 0 degrees to 180 degrees 
        {                                  
          servo_motor.write(angle);             //command to rotate the servo to the specified angle
          delay(15);                       
        } 
        delay(100);
       for(angle = 180; angle>=1; angle-=5)    // command to move from 180 degrees to 0 degrees 
        {                                
          servo_motor.write(angle);             //command to rotate the servo to the specified angle
          delay(15);                       
        }
      digitalWrite(BUILTIN_LED, LOW);
      digitalWrite(buzzPin, LOW);
    }
    
    Blynk.run();
    timer.run(); // Initiates SimpleTimer
  ThingSpeak.writeField(myChannelNumber, 1, distance, myWriteAPIKey);
   delay(100); // ThingSpeak will only accept updates every 15 seconds.
}
