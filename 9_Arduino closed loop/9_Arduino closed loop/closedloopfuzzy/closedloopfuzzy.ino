#include <avr/pgmspace.h>
#include <Wire.h>
#include <digitalWriteFast.h>
#include <PWM.h>
#include "FLC1.h"
#include "FLC2.h"
#include "FLC3.h"

// IMU
#define MPU_ADDR 0x68
#define SMPRT_DIV 0x19
#define CONFIG_REG 0x20
#define PWR_MGMT_1 0x6B
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C

#define ACCEL_XOUT_H 0x3B
#define GYRO_XOUT_H 0x43

#define IMU_READ_SUCCESS   0
#define I2C_TIMEOUT_ERROR  5
#define I2C_TIMEOUT        1000
#define I2C_DATA_NO        14
uint8_t i2cData[I2C_DATA_NO];

#define CALIB_DATA_NO 50
float tempData = 0;

int16_t accelX, accelY, accelZ;
int16_t gyroX, gyroY, gyroZ;
float angle, rate;
float estAngle = 0;

float angle_offs = -0.388;
float rate_offs = -0.03;

#define ANGLE_ATAN

// H-BRIDGE, MOTOR+ENC
#define PWMFREQ 10000

#define MOT1_CHA 4
#define MOT1_CHB 2
#define IN1 11
#define IN2 12
#define ENA 10

#define MOT2_CHA 3
#define MOT2_CHB 5
#define IN3 8
#define IN4 7
#define ENB 9

int mot1Pos = 0;
int mot2Pos = 0;
int mot1Pwm;
int mot2Pwm;

// DUMMY
uint32_t timerXms;
uint32_t timerXs;
uint32_t timerXs2;

int vel1;
int vel2;
int vel;
int pos1 = 0;
int pos2 = 0;
int pos = 0;
int yaw = 0;
int desPos = 0;
int desYaw = 0;

int angleControlPWM = 0;
int yawControlPWM = 0;

//#define CALIB_GYRO
#define DEAD_BAND
#define DEAD_BAND_PWMVAL 110
//#define DESVALS
int desValStates = 0;

int conseqTimeOutCntr = 0;

#define SEND_TO_PC

void setup()
{
  InitTimersSafe();  
  SetPinFrequencySafe(ENA, PWMFREQ);
  SetPinFrequencySafe(ENB, PWMFREQ);

  Serial.begin(9600);

  // IMU
  Wire.begin();
  Wire.setClock(400000UL); 
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(PWR_MGMT_1);
  Wire.write(0b00000000); // deactivate sleep mode
  Wire.endTransmission();

  delay(100);

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(SMPRT_DIV);
  Wire.write(0b00000100); // sampling 200Hz
  Wire.endTransmission();

  delay(100);

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(CONFIG_REG);
  Wire.write(0x03);       // Accel 44 Hz cut, Gyro 42 Hz cut, 1000 Hz sampling
  Wire.endTransmission();

  delay(100);

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(GYRO_CONFIG);
  Wire.write(0x00000000); // 250 dps
  Wire.endTransmission();

  delay(100);

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(ACCEL_CONFIG);
  Wire.write(0b00000000); // 2 g
  Wire.endTransmission();

  // H-BRIDGE, MOTOR+ENC
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  //pinMode(ENA, OUTPUT);
  //digitalWriteFast(ENA, LOW);
  pwmWrite(ENA, 0);

  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  //pinMode(ENB, OUTPUT);
  //digitalWriteFast(ENA, LOW);
  pwmWrite(ENB, 0);

  pinMode(MOT1_CHA, INPUT);
  pinMode(MOT1_CHB, INPUT);
  pinMode(MOT2_CHA, INPUT);
  pinMode(MOT2_CHB, INPUT);

  attachInterrupt(digitalPinToInterrupt(MOT1_CHB), MOT1_CHB_FALL, FALLING);
  attachInterrupt(digitalPinToInterrupt(MOT2_CHA), MOT2_CHA_FALL, FALLING);

#ifdef CALIB_GYRO
  for (int i = 0; i < CALIB_DATA_NO; i++)
  {
    getImuData();
    tempData = tempData + rate;
  }

  rate_offs = tempData / CALIB_DATA_NO;
  Serial.println(rate_offs);
#endif

  pinMode(LED_BUILTIN, OUTPUT);

  delay(3000);  
}

void loop()
{
  //10ms
  if ((micros() - timerXms) >= 10000)
  {
    timerXms = micros();

    vel1 = mot1Pos;         // inc/10ms
    vel2 = mot2Pos;         // inc/10ms
    mot1Pos = 0;
    mot2Pos = 0;

    if(getImuData() == IMU_READ_SUCCESS)
    {
      estAngle = 0.99f * (estAngle + rate * 0.01f) + 0.01f * angle;
      conseqTimeOutCntr = 0;
    }
    else
    {
      conseqTimeOutCntr++;
      if (conseqTimeOutCntr > 50)
      {
        errorHandling();     
      }
    }

    pos1 = pos1 + vel1;
    pos2 = pos2 + vel2;
    pos = (pos1+pos2)>>1;
    vel = (vel1+vel2)>>1;
    yaw = pos2-pos1;
    
    #if 1
    if ((estAngle > -45) && (estAngle < 45))
    {
      angleControlPWM = FLC((0-estAngle), (0-rate), in1min_FLC2, in2min_FLC2, res1_FLC2, res2_FLC2, *FLC2, NROW_FLC2, NCOL_FLC2) - FLC(((float)(desPos-pos)), ((float)(0-vel)), in1min_FLC1, in2min_FLC1, res1_FLC1, res2_FLC1, *FLC1, NROW_FLC1, NCOL_FLC1);
      yawControlPWM = -FLC(((float)(desYaw-yaw)), ((float)(0-(vel2-vel1))), in1min_FLC3, in2min_FLC3, res1_FLC3, res2_FLC3, *FLC3, NROW_FLC3, NCOL_FLC3);
      
      #ifdef DEAD_BAND
        if (angleControlPWM > 0)
        {
          setMot1Pwm(angleControlPWM+yawControlPWM + DEAD_BAND_PWMVAL);
          setMot2Pwm(angleControlPWM-yawControlPWM + DEAD_BAND_PWMVAL);
        }
        else
        {
          setMot1Pwm(angleControlPWM+yawControlPWM - DEAD_BAND_PWMVAL);
          setMot2Pwm(angleControlPWM-yawControlPWM - DEAD_BAND_PWMVAL);
        }
      #else
        setMot1Pwm(angleControlPWM);
        setMot2Pwm(angleControlPWM);
      #endif
    }
    else
    {
      pos1 = 0;
      pos2 = 0;
      vel1 = 0;
      vel2 = 0;
      mot1Pos = 0;
      mot2Pos = 0;
      pos = 0;
      vel = 0;
      stopMotors();
    }
    #endif
  }

  #ifdef DESVALS // 4sec
  if(((millis() - timerXs) >= 4000))
  {
    timerXs = millis();

    if(desValStates == 0)
    {
      desPos = 592;
      desValStates = 1;
    }
    else if(desValStates == 1)
    {
      //desPos = 0;
      desYaw = 748;
      desValStates = 2;
    }
    else if(desValStates == 2)
    {
      desPos = 100;
      desValStates = 3;
    }
    else if(desValStates == 3)
    {
      desYaw = -200;
      desValStates = 4;
    }
    else if(desValStates == 4)
    {
      desPos = -400;
      desValStates = 5;
    }
    else if(desValStates == 5)
    {
      desPos = 0;
      desYaw = 0;
      desValStates = 0;
    }
  }  
  #endif

  #ifdef SEND_TO_PC // 40ms
  if(((millis() - timerXs2) >= 50))
  {
    timerXs2 = millis();
  
    Serial.print(desPos);
    Serial.print(',');
    Serial.print(pos);
    Serial.print(',');
    Serial.print(desYaw);
    Serial.print(',');
    Serial.print(yaw);
    Serial.print(',');
    Serial.print(angle);
    Serial.print(',');
    Serial.print(estAngle);
    Serial.print(',');
    Serial.print(mot1Pwm);
    Serial.print(',');
    Serial.println(mot2Pwm);
  }
  #endif
}

void errorHandling()
{
  while (1)      
  {
    stopMotors();
    stopMotors();
        
    digitalWriteFast(LED_BUILTIN, HIGH);   
    delay(500);                            
    digitalWriteFast(LED_BUILTIN, LOW);    
    delay(500);
   }
}

uint8_t getImuData()
{
  uint32_t i2cTimeOutTimer;

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(ACCEL_XOUT_H);

  uint8_t i2cReturnCode = Wire.endTransmission(false);
  if (i2cReturnCode)
  {
    return i2cReturnCode;
  }

  Wire.requestFrom(MPU_ADDR, I2C_DATA_NO, (uint8_t)true);

  for (uint8_t i = 0; i < I2C_DATA_NO; i++)
  {
    if (Wire.available())
    {
      i2cData[i] = Wire.read();
    }
    else
    {
      i2cTimeOutTimer = micros();
      while (((micros() - i2cTimeOutTimer) < I2C_TIMEOUT) && !Wire.available());
      if (Wire.available())
      {
        i2cData[i] = Wire.read();
      }
      else
      {
        return I2C_TIMEOUT_ERROR;
      }
    }
  }

  accelX = ((i2cData[0] << 8) | i2cData[1]);
  //accelY = ((i2cData[2] << 8) | i2cData[3]);
  accelZ = ((i2cData[4] << 8) | i2cData[5]);
  //int16_t tempOut = ((i2cData[6] << 8) | i2cData[7]);
  //gyroX = ((i2cData[8] << 8) | i2cData[9]);
  gyroY = ((i2cData[10] << 8) | i2cData[11]);
  //gyroZ = ((i2cData[12] << 8) | i2cData[13]);

#ifdef ANGLE_ATAN
  angle = atan2((float)accelX, (float)accelZ) * RAD_TO_DEG;
#else
  angle = ((float)accelX) / 16384.0f;
#endif
  angle = angle - angle_offs;

  rate = -((float)gyroY) / 131.0f;
  rate = rate - rate_offs;

  return IMU_READ_SUCCESS;
}

void stopMotors()
{
  digitalWriteFast(IN1, LOW);
  digitalWriteFast(IN2, LOW);
  pwmWrite(ENA, 0);

  digitalWriteFast(IN3, LOW);
  digitalWriteFast(IN4, LOW);
  pwmWrite(ENB, 0);
}

void setMot1Pwm(int pwmVal)
{
  if (pwmVal > 255)
    pwmVal = 255;
  else if (pwmVal < -255)
    pwmVal = -255;

  if (pwmVal > 0)						//forward
  {
    //digitalWriteFast(ENA, LOW);
    digitalWriteFast(IN1, LOW);
    digitalWriteFast(IN2, HIGH);
    //analogWrite(ENA, pwmVal);
    pwmWrite(ENA, pwmVal);
  }
  else								//backward
  {
    //digitalWriteFast(ENA, LOW);
    digitalWriteFast(IN1, HIGH);
    digitalWriteFast(IN2, LOW);
    //analogWrite(ENA, -pwmVal);
    pwmWrite(ENA, -pwmVal);
  }

  mot1Pwm = pwmVal;
}

void setMot2Pwm(int pwmVal)
{
  if (pwmVal > 255)
    pwmVal = 255;
  else if (pwmVal < -255)
    pwmVal = -255;

  if (pwmVal > 0)						//forward
  {
    //digitalWriteFast(ENB, LOW);
    digitalWriteFast(IN3, HIGH);
    digitalWriteFast(IN4, LOW);
    //analogWrite(ENB, pwmVal);
    pwmWrite(ENB, pwmVal);
  }
  else								      //backward
  {
    //digitalWriteFast(ENB, LOW);
    digitalWriteFast(IN3, LOW);
    digitalWriteFast(IN4, HIGH);
    //analogWrite(ENB, -pwmVal);
    pwmWrite(ENB, -pwmVal);
  }

  mot2Pwm = pwmVal;
}


void MOT1_CHB_FALL()
{
  if (digitalRead(MOT1_CHA) == HIGH)
    mot1Pos++;
  else
    mot1Pos--;
}

void MOT2_CHA_FALL()
{
  if (digitalRead(MOT2_CHB) == HIGH)
    mot2Pos++;
  else
    mot2Pos--;
}


int FLC(float e, float de, float in1min, float in2min, float res1, float res2, const int16_t* LUT, int rows, int cols)
{
  // saturation
  if(e<in1min)  e = in1min;
  if(de<in2min) de = in2min;
  
  // row and column indexes
  int ind1 = round((e-in1min)/res1); 
  int ind2 = round((de-in2min)/res2);
  
  // saturation
  if(ind1>rows-1) ind1 = rows-1;
  if(ind2>cols-1) ind2 = cols-1;
  
  return pgm_read_word_near(&LUT[ind1*cols+ind2]);
}
