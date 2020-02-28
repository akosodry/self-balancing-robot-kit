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

#define DECODE1X

int mot1Pos = 0;
int mot2Pos = 0;
int mot1Pwm;
int mot2Pwm;

uint32_t timerXms;
uint32_t timerXs;
uint32_t timerXs2;

int tmpVal1;
int tmpVal2;

int flag = 0;
int flag2 = 1;

uint8_t increment = 0;

void setup() 
{
  Serial.begin(115200);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  digitalWrite(ENA, LOW);
  
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);
  digitalWrite(ENB, LOW);

  pinMode(MOT1_CHA, INPUT);
  pinMode(MOT1_CHB, INPUT);
  pinMode(MOT2_CHA, INPUT);
  pinMode(MOT2_CHB, INPUT);

  #ifdef DECODE1X
    attachInterrupt(digitalPinToInterrupt(MOT1_CHB), MOT1_CHB_FALL, FALLING);
    attachInterrupt(digitalPinToInterrupt(MOT2_CHA), MOT2_CHA_FALL, FALLING);
  #else
    attachInterrupt(digitalPinToInterrupt(MOT1_CHB), MOT1_CHB_CHANGE, CHANGE);
    attachInterrupt(digitalPinToInterrupt(MOT2_CHA), MOT2_CHA_CHANGE, CHANGE);
  #endif
}

void loop() 
{
  //20ms
  if((micros() - timerXms) >= 20000) 
  {
    tmpVal1 = mot1Pos;
	tmpVal2 = mot2Pos;
    mot1Pos = 0;
	mot2Pos = 0;
    
    timerXms = micros();
    
    Serial.print(mot1Pwm);
    Serial.print(',');
    Serial.print(tmpVal1);
    Serial.print(',');
    Serial.print(mot2Pwm);
    Serial.print(',');
    Serial.println(tmpVal2);
  }

  //2sec
  if(((millis() - timerXs) >= 2000))
  {
    timerXs = millis();

    setMot1Pwm(150*flag);
	setMot2Pwm(150*flag);

    if(flag == 1)
      flag = -1;
    else if(flag == -1)
      flag = 0;
    else if(flag == 0)
      flag = 1;
  }

}

void setMot1Pwm(int pwmVal)
{
	if(pwmVal>255)
		pwmVal = 255;
	else if(pwmVal<-255)
		pwmVal = -255;
	
	if(pwmVal > 0)						//forward
	{
		digitalWrite(ENA, LOW);
		digitalWrite(IN1, LOW);
		digitalWrite(IN2, HIGH);
		analogWrite(ENA, pwmVal);
	}
	else								//backward
	{
		digitalWrite(ENA, LOW);
		digitalWrite(IN1, HIGH);
		digitalWrite(IN2, LOW);
		analogWrite(ENA, -pwmVal);	
	}
	
	mot1Pwm = pwmVal;
}

void setMot2Pwm(int pwmVal)
{
	if(pwmVal>255)
		pwmVal = 255;
	else if(pwmVal<-255)
		pwmVal = -255;
	
	if(pwmVal > 0)						//forward
	{
		digitalWrite(ENB, LOW);
		digitalWrite(IN3, HIGH);
		digitalWrite(IN4, LOW);
		analogWrite(ENB, pwmVal);
	}
	else								//backward
	{
		digitalWrite(ENB, LOW);
		digitalWrite(IN3, LOW);
		digitalWrite(IN4, HIGH);
		analogWrite(ENB, -pwmVal);	
	}
	
	mot2Pwm = pwmVal;
}

#ifdef DECODE1X
  void MOT1_CHB_FALL()
  {
    if(digitalRead(MOT1_CHA) == HIGH)
      mot1Pos++;
    else
      mot1Pos--;
  }

  void MOT2_CHA_FALL()
  {
    if(digitalRead(MOT2_CHB) == HIGH)
      mot2Pos++;
    else
      mot2Pos--;
  }
#else
  void MOT1_CHB_CHANGE()
  {
    if(digitalRead(MOT1_CHA) == HIGH)
    {
      if(digitalRead(MOT1_CHB) == LOW)
        mot1Pos++;
      else
        mot1Pos--;
    }
    else
    {
      if(digitalRead(MOT1_CHB) == LOW)
        mot1Pos--;
      else
        mot1Pos++;   
    } 
  }
#endif

