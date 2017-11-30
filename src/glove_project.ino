#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "I2Cdev.h"
#include "MPU6050.h"
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE//Arduino IDE version problem covarage, wire library bug
#include "Wire.h"
#endif

#define ACCELOREMETERFILTERSIZE 15
#define DISTANCEFILTERSIZE 5
#define TRIGGERPIN 8//HCSR-04 trigger pin
#define ECHOPIN 7//HCSR-04 echo pin
#define MOTORCONTROL 6//motor PWM output

#define FUNCTIONBASE 0.4

#define TIMEQUANTUM 10//ms 
#define GYROTASKPERIOD 100//ms
#define MOTORTASKPERIOD 100//ms
#define LCDTASKPERIOD 300//ms

int16_t ax, ay, az;
int16_t gx, gy, gz;

//Signal filtering scturcture
struct Signal {
  int16_t *signalSeq;
  uint16_t seqSize;
  uint16_t seqCursor;
  int16_t sum;
  int16_t avarage;
};
//Filters' pre defined signal arrays
int16_t acceloremeterSeqX[ACCELOREMETERFILTERSIZE];
int16_t acceloremeterSeqY[ACCELOREMETERFILTERSIZE];
int16_t acceloremeterSeqZ[ACCELOREMETERFILTERSIZE];
int16_t distanceSeq[DISTANCEFILTERSIZE];

int16_t  insertMeasurement(Signal signalA, int16_t value);
void initializeSignal(Signal *signalA);
int16_t getDistance(uint8_t trPin, uint8_t ecPin);

LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display
MPU6050 accelgyro;

Signal signalX;//three axis signals
Signal signalY;
Signal signalZ;

Signal distanceSignal;

void setup()
{
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE//wire bug fix, without fix Arduino can't find LCD
  Fastwire::setup(400, true);
#endif

  Serial.println("Initializing..");
  accelgyro.initialize();
  //signals initialization
  signalX.signalSeq = acceloremeterSeqX;
  signalY.signalSeq = acceloremeterSeqY;
  signalZ.signalSeq = acceloremeterSeqZ;
  distanceSignal.signalSeq = distanceSeq;
  signalX.seqSize = ACCELOREMETERFILTERSIZE;
  signalY.seqSize = ACCELOREMETERFILTERSIZE;
  signalZ.seqSize = ACCELOREMETERFILTERSIZE;
  distanceSignal.seqSize = DISTANCEFILTERSIZE;
  initializeSignal(&signalX);
  initializeSignal(&signalY);
  initializeSignal(&signalZ);
  initializeSignal(&distanceSignal);

  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
}

void loop()
{
  int16_t dispX;//filter avarages of the acceloremeter axis measurements
  int16_t dispY;
  int16_t dispZ;

  pinMode(TRIGGERPIN, OUTPUT);
  pinMode(ECHOPIN, INPUT);
  pinMode(MOTORCONTROL, OUTPUT);
  int16_t distance = 0;
  int16_t avrDistance = 0;
  double duty = 0;//PWM duty value
  unsigned long systemTime = 0;//common time
  unsigned long gyroTaskLastTime = 0;//basic scheduling values, last execution times
  unsigned long lcdTaskLastTime = 0;
  unsigned long motorTaskLastTime = 0;

  while (1)//endless loop, user can power off the system or reset with the button
  {
    systemTime =  millis();
    //GYRO TASK BLOCK
    if ( (systemTime - gyroTaskLastTime) > GYROTASKPERIOD )
    {
      gyroTaskLastTime =  millis();
      accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
      dispX = insertMeasurement(&signalX, ax);
      dispY = insertMeasurement(&signalY, ay);
      dispZ = insertMeasurement(&signalZ, az);
      //gyro measured and filtered
    }
    systemTime = millis();
    //MOTOR TASK BLOCK
    if ( (systemTime - motorTaskLastTime) > MOTORTASKPERIOD )
    {
      motorTaskLastTime = millis();
      if (-10000 > dispY)// palm looking forward
      {
        distance =  getDistance(TRIGGERPIN, ECHOPIN);
        if (distance > 400)
        {
          distance = 400;
        }
        avrDistance = insertMeasurement(&distanceSignal, distance);

        duty = pow(FUNCTIONBASE, float(float(avrDistance) / 100)); //duty calculation via selected base value
        uint8_t dDuty = uint8_t(duty * 255);//analog output value of vibration
        analogWrite(MOTORCONTROL, dDuty);
      }
      else
      {
        analogWrite(MOTORCONTROL, 0);// palm not looking forward
      }
    }
    systemTime = millis();
    //LCD TASK BLOCK
    if ( (systemTime - lcdTaskLastTime) > LCDTASKPERIOD )
    {
      lcdTaskLastTime = millis();
      if (-10000 > dispY)// palm looking forward
      {
        //print motor vibraiton and distance
        lcd.clear();
        lcd.print(avrDistance); lcd.print(" cm");
        lcd.setCursor(0, 1);
        lcd.print("%"); lcd.print(duty * 100); lcd.print(" ");
      }
      else
      {
        lcd.clear();
        lcd.print("NOT ACTIVE ");
      }
    }
    delay(TIMEQUANTUM);
  }
}

void initializeSignal(Signal *signalA)
{
  //all filter values are zero
  signalA->avarage = 0;
  signalA->seqCursor = 0;
  signalA->sum = 0;
  for (int i = 0; i < signalA->seqSize; i++)
  {
    signalA->signalSeq[i] = 0;
  }
}
int16_t  insertMeasurement(Signal *signalA, int16_t value)
{
  //calculate the avarage with only changing values, do not repeat the whole calculation
  int removeSlot = (signalA->seqCursor + 1) % signalA->seqSize;
  signalA->avarage -= signalA->signalSeq[removeSlot] / signalA->seqSize;
  //cyclic memory usage
  signalA->signalSeq[removeSlot] = value;
  signalA->avarage += signalA->signalSeq[removeSlot] / signalA->seqSize;
  signalA->seqCursor = removeSlot;
  return signalA->avarage;
}
int16_t getDistance(uint8_t trPin, uint8_t ecPin)
{
  unsigned long realDuration = 0;
  uint16_t duration;
  uint16_t distance;
  digitalWrite(trPin, LOW);
  delayMicroseconds(4);
  digitalWrite(trPin, HIGH);
  delayMicroseconds(10);//trigger the pin
  digitalWrite(trPin, LOW);
  realDuration = pulseIn(ecPin, HIGH);//wait for response
  duration = realDuration;
  distance = (duration / 2) / 29,41;//calculate the distance in cm form
  delay(50);//wait for reliable sensor read
  return distance;
}

