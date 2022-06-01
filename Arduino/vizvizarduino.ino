//Musa Akyüz
//12:34 19.05.2022
//Step motor test program

/*
-----MOTOR INFORMATION-----
Rated voltage ： 5VDC 
Number of Phase 4 
Speed Variation Ratio 1/64 
Stride Angle 5.625° /64 
Frequency 100Hz 
DC resistance 50Ω±7%(25℃) 
Idle In-traction Frequency > 600Hz 
Idle Out-traction Frequency > 1000Hz 
In-traction Torque >34.3mN.m(120Hz) 
Self-positioning Torque >34.3mN.m 
Friction torque 600-1200 gf.cm 
Pull in torque 300 gf.cm 
Insulated resistance >10MΩ(500V) 
Insulated electricity power 600VAC/1mA/1s 
Insulation grade A 
Rise in Temperature <40K(120Hz) 
Noise <35dB(120Hz,No load,10cm) 
Model 28BYJ-48 – 5V
*/

#include <Servo.h>
#include <SPI.h>
#include <SD.h>
#include <nRF24L01.h>
#include <RF24.h>

//NRF24L01
RF24 radio(9, 10);
const byte address[6] = "00001";
int sendinfo;
//SD CARD
File dataFile;
Servo myservo;

/*
  Left Step motor pins
*/
#define INL1 5 
#define INL2 6
#define INL3 7 
#define INL4 8

/*
  Right Step motor pins
*/
#define INR1 A5
#define INR2 A4
#define INR3 A3
#define INR4 A2

/*
  Global Variables
*/
//Step Motors
int sure = 3; //delay time per step for step motor 3 milisecond
int MOTOR_STEPS = 512; //one tour steps count equals 360 degree

//Ultrasonic Sensor
long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement
int trigPin = A1;
int echoPin = A0;
int d0, d90, d180;

void setup() {

  /*
    All motor pins output
  */
  pinMode(INL1, OUTPUT); 
  pinMode(INL2, OUTPUT); 
  pinMode(INL3, OUTPUT); 
  pinMode(INL4, OUTPUT); 
  pinMode(INR1, OUTPUT);
  pinMode(INR2, OUTPUT);
  pinMode(INR3, OUTPUT);
  pinMode(INR4, OUTPUT);

  /*
    Ultrasonıc sensors pins
  */
  pinMode(trigPin, OUTPUT); //trig
  pinMode(echoPin, INPUT); //echo
  Serial.begin(9600);

  SD.begin(4);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);       
  radio.stopListening();

  /*
    Servo Motor pins and initialization
  */
  myservo.attach(3);
  myservo.write(0);
  delay(4000);
  myservo.write(180);
  delay(4000);
  myservo.write(0);
  delay(4000);
}

void loop() {
  
    //Checking around 0 to 180 front of car
    for(int i = 0; i < 180; i++)
    {
      myservo.write(i);
      delay(10);
      sendinfo = MeasureDistance();
      if(radio.begin()) {
        radio.write(&sendinfo, sizeof(int)); 
      }
      dataFile = SD.open("data.txt", FILE_WRITE);
      dataFile.println(sendinfo);
      dataFile.close();
    }

    //Looks specially 0, 90, 180
    //assign in order of d0, d90, d180
    CheckForward();

    //If front is aprroximately 10 cm free
    if(d90 < 20)
    {
      TurnAroundClockwise(90); //Right 90 degree
    }
    else
    {
      Forward(10); //Go 5cm forward
      // TurnAroundClockwise(90); //Right 90 degree
      // CheckForward(); //up to date new status
      // if(d0 < 10)
      // {
      //   Forward(5); //Go 5cm forward
      // }
      // else
      // {
      //   Forward(5);
      //   TurnAroundCounterClockwise(90);
      // }
    }
}

void Forward(float centimeter) {
  /*
    512 motor steps equals to 360 degree
    512 motor steps equals to 1 wheel tour
    one wheelRound equals to 18.5 cm
  */
  float wheelTour = centimeter / 18.5;
  for (int cnt = 0; cnt < wheelTour * MOTOR_STEPS; cnt++) {
    digitalWrite(INL4, HIGH);
    digitalWrite(INR1, HIGH);
    delay(sure);
    digitalWrite(INL4, LOW);
    digitalWrite(INL3, HIGH);
    digitalWrite(INR1, LOW);
    digitalWrite(INR2, HIGH);
    delay(sure);
    digitalWrite(INL3, LOW);
    digitalWrite(INL2, HIGH);
    digitalWrite(INR2, LOW);
    digitalWrite(INR3, HIGH);
    delay(sure);
    digitalWrite(INL2, LOW);
    digitalWrite(INL1, HIGH);
    digitalWrite(INR3, LOW);
    digitalWrite(INR4, HIGH);
    delay(sure);
    digitalWrite(INR4, LOW);
    digitalWrite(INL1, LOW);
  }
  /*
    To communication protocol:
    -3 prints.
    -3 means Forward 5cm.
  */
  sendinfo = -3;
  if(radio.begin()) {
    radio.write(&sendinfo, sizeof(int)); 
  }
  dataFile = SD.open("data.txt", FILE_WRITE);
  dataFile.println("-3");
  dataFile.close();
}

void Backward(float centimeter) {
  /*
    512 motor steps equals to 360 degree
    512 motor steps equals to 1 wheel tour
    one wheelRound equals to 18.5 cm
  */
  float wheelTour = centimeter / 18.5;
  for (int cnt = 0; cnt < wheelTour * MOTOR_STEPS; cnt++) {
    digitalWrite(INL1, HIGH);
    digitalWrite(INR4, HIGH);
    delay(sure);
    digitalWrite(INR4, LOW);
    digitalWrite(INR3, HIGH);
    digitalWrite(INL1, LOW);
    digitalWrite(INL2, HIGH);
    delay(sure);
    digitalWrite(INL2, LOW);
    digitalWrite(INL3, HIGH);
    digitalWrite(INR3, LOW);
    digitalWrite(INR2, HIGH);
    delay(sure);
    digitalWrite(INR2, LOW);
    digitalWrite(INR1, HIGH);
    digitalWrite(INL3, LOW);
    digitalWrite(INL4, HIGH);
    delay(sure);
    digitalWrite(INL4, LOW);
    digitalWrite(INR1, LOW);
  }
  /*
    To communication protocol:
    -4 prints.
    -4 means backward 5cm.
  */
  sendinfo = -4;
  if(radio.begin()) {
    radio.write(&sendinfo, sizeof(int)); 
  }
  dataFile = SD.open("data.txt", FILE_WRITE);
  dataFile.println("-4");
  dataFile.close();
}

/*
  Counter Clockwise TurnAround
  Distance between two wheel = 9.75 cm
  Round distance between two wheel = 4.875 cm
  2*pi*r = 2 * 3.14 * 4.875 = 30.6384 cm
  30.6384 / (360 / angle)
  if angle equals 90 degree
  30.6384 / 4 = 7.6596 cm
  Wheel Round = 18.5 cm
  7.6596 cm /  18.5 cm = 0.4140 tour 
*/
void TurnAroundCounterClockwise(float angle)
{
  /*
    512 motor steps equals to 360 degree
    512 motor steps equals to 1 wheel tour
    one wheelRound equals to 18.5 cm
  */
  float wheelTour = (30.6384 / (360 / angle)) / 18.5;
  for (int cnt = 0; cnt < wheelTour * MOTOR_STEPS; cnt++) {
    digitalWrite(INL1, HIGH);
    digitalWrite(INR1, HIGH);
    delay(sure);
    digitalWrite(INR1, LOW);
    digitalWrite(INR2, HIGH);
    digitalWrite(INL1, LOW);
    digitalWrite(INL2, HIGH);
    delay(sure);
    digitalWrite(INL2, LOW);
    digitalWrite(INL3, HIGH);
    digitalWrite(INR2, LOW);
    digitalWrite(INR3, HIGH);
    delay(sure);
    digitalWrite(INR3, LOW);
    digitalWrite(INR4, HIGH);
    digitalWrite(INL3, LOW);
    digitalWrite(INL4, HIGH);
    delay(sure);
    digitalWrite(INL4, LOW);
    digitalWrite(INR4, LOW);
  }
  /*
    To communication protocol:
    -2 prints.
    -2 means turning anti clockwise 90 degree.
  */
  sendinfo = -2;
  if(radio.begin()) {
    radio.write(&sendinfo, sizeof(int)); 
  }
  dataFile = SD.open("data.txt", FILE_WRITE);
  dataFile.println("-2");
  dataFile.close();
}

/*
  Clockwise TurnAround
  Distance between two wheel = 9.75 cm
  Round distance between two wheel = 4.875 cm
  2*pi*r = 2 * 3.14 * 4.875 = 30.6384 cm
  30.6384 / (360 / angle)
  if angle equals -90 degree
  30.6384 / -4 = -7.6596 cm
  Wheel Round = 18.5 cm
  -7.6596 cm /  18.5 cm = -0.4140 tour 
*/
void TurnAroundClockwise(float angle)
{
  /*
    512 motor steps equals to 360 degree
    512 motor steps equals to 1 wheel tour
    one wheelRound equals to 18.5 cm
  */
  float wheelTour = (30.6384 / (360 / angle)) / 18.5;
  for (int cnt = 0; cnt < wheelTour * MOTOR_STEPS; cnt++) {
    digitalWrite(INL4, HIGH);
    digitalWrite(INR4, HIGH);
    delay(sure);
    digitalWrite(INR4, LOW);
    digitalWrite(INR3, HIGH);
    digitalWrite(INL4, LOW);
    digitalWrite(INL3, HIGH);
    delay(sure);
    digitalWrite(INL3, LOW);
    digitalWrite(INL2, HIGH);
    digitalWrite(INR3, LOW);
    digitalWrite(INR2, HIGH);
    delay(sure);
    digitalWrite(INR2, LOW);
    digitalWrite(INR1, HIGH);
    digitalWrite(INL2, LOW);
    digitalWrite(INL1, HIGH);
    delay(sure);
    digitalWrite(INL1, LOW);
    digitalWrite(INR1, LOW);
  }
  /*
    To communication protocol:
    -1 prints.
    -1 means turning clocwise 90 degree.
  */
  sendinfo = -1;
  if(radio.begin()) {
    radio.write(&sendinfo, sizeof(int)); 
  }
  dataFile = SD.open("data.txt", FILE_WRITE);
  dataFile.println("-1");
  dataFile.close();
}

/*
  Ultrasonic Sensor Distance Calculation

  In order to generate the ultrasound we need to set the Trigger Pin
  on a High State for 10 µs. That will send out an 8 cycle sonic burst
  which will travel at the speed sound and it will be received in the
  Echo Pin. The Echo Pin will output the time in microseconds the sound 
  wave traveled.

  Sound_speed : 340 m/s
  Distance = Time * Sound_Speed / 2
*/
int MeasureDistance()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;  // Speed of sound wave divided by 2 (go and back)
  return distance;
}

void CheckForward()
{
  myservo.write(0);
  delay(700);
  d0 = MeasureDistance();
  delay(10);
  myservo.write(90);
  delay(700);
  d90 = MeasureDistance();
  delay(10);
  myservo.write(180);
  delay(700);
  d180 = MeasureDistance();
  delay(10);
}