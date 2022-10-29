//Included libraries
#include <LiquidCrystal.h>

//Defined pins
#define lcd_rs 2
#define lcd_en 3
#define lcd_d4 4
#define lcd_d5 5
#define lcd_d6 6
#define lcd_d7 7
#define button_pin 8
#define water_sensor1_pin 9
#define water_sensor2_pin 10
#define heater_re1ay1_pin 11
#define blender_relay2_pin 12
#define heater_re1ay3_pin 13
#define temperature_sensor_pin A0
#define dc_motor1_pin A1
#define dc_motor2_pin A2
#define dc_motor3_pin A3

//declaration of constants
const int DC_MOTOR1_DELAY = 5000;
const int DC_MOTOR2_DELAY = 5000;
const int DC_MOTOR3_DELAY = 5000;
const int SPIRAL1_DELAY = 5000;
const int SPIRAL2_DELAY = 5000;
const int BLENDER_DELAY = 5000;
const uint8_t BOIL_TEMPERATURE = 100;

//constants for temperature
const long RT0 = 80000;      //Ω
const long B = 3977;       // K
const uint8_t VCC = 5;
const long R = RT0;
const float T0 = 25 + 273.15;

//Declaration of variables
bool pool_mode; //time implementation is true and water level implementation is false

//Create constructor of objects
LiquidCrystal Lcd(lcd_rs, lcd_en, lcd_d4, lcd_d5, lcd_d6, lcd_d7);

//Prototype of functions
void initPins();
bool isButtonPressed();
uint8_t checkWaterLevel();
void turnOnMotor1();
void turnOnHeater1();
float checkTemperature();
void turnOnBlender();
void turnOnMotor2();
void turnOnMotor3();
void turnOnHeater2();

void setup()
{
  Serial.begin(9600);

  initPins();
  pool_mode = false;
  Lcd.begin(16, 2);
}

void loop()
{
  Lcd.clear();
  Lcd.setCursor(0, 0);
  Lcd.print("System is OFF");
  Lcd.setCursor(0, 1);
  Lcd.print("Press but. to on");

  while (!isButtonPressed())
  {
    /* nothing happens */
  }

  turnOnMotor1();
  turnOnHeater1();
  turnOnBlender();
  turnOnMotor2();
  turnOnMotor3();
  turnOnHeater2();
}



/* Private functions implementation*/

//Function initializes mode of all pins
void initPins()
{
  pinMode(button_pin, INPUT);
  pinMode(dc_motor1_pin, OUTPUT);
  pinMode(water_sensor1_pin, INPUT);
  pinMode(water_sensor2_pin, INPUT);
  pinMode(heater_re1ay1_pin, OUTPUT);
  pinMode(blender_relay2_pin, OUTPUT);
  pinMode(temperature_sensor_pin, INPUT);
  pinMode(dc_motor2_pin, OUTPUT);
  pinMode(dc_motor3_pin, OUTPUT);
  pinMode(heater_re1ay3_pin, OUTPUT);
}

/*
   Function checks if buttons is pressed and
   returns true an if not returns false
*/
bool isButtonPressed()
{
  if (digitalRead(button_pin) == HIGH)
  {
    return true;
  }
  else
  {
    return false;
  }
}

/*
   Function returns 2 if water level is 2, returns 1 if
   water level is 1 and return 0 if water isn't in pool
*/
uint8_t checkWaterLevel()
{
  delay(1);
  if (digitalRead(water_sensor2_pin) == LOW) return 2;
  if (digitalRead(water_sensor1_pin) == LOW) return 1;

  return 0;
}

/*
   Function turns on motor1 in whater lavel mode or time mode and
   when motor1 isn't needed then turns off
*/
void turnOnMotor1()
{
  int startWaterLevel = checkWaterLevel(); //check water level
  digitalWrite(dc_motor1_pin, HIGH); //turn on motor1
  Lcd.clear();
  Lcd.setCursor(0, 0);
  Lcd.print("Motor1's working");

  if (pool_mode)
  {
    //for time implementation
    long saveTime = millis();
    long saveTimeLeft = saveTime;
    int timeLeft = DC_MOTOR1_DELAY / 1000;
    Lcd.setCursor(0, 1);
    Lcd.print("Time left: ");
    Lcd.print(timeLeft);
    Lcd.print(" sec");

    while (millis() - saveTime <= DC_MOTOR1_DELAY)
    {
      if (millis() - saveTimeLeft >= 1000)
      {
        saveTimeLeft = millis();
        timeLeft--;
        Lcd.setCursor(11, 1);
        Lcd.print(timeLeft);
      }
    }
  }
  else
  {
    //for whater level implementation
    if (startWaterLevel == 2)
    {
      Lcd.setCursor(0, 1);
      Lcd.print("Water level: ");
      Lcd.print(startWaterLevel);

      while (checkWaterLevel() == 2)
      {
        //nothing happens
      }
    }
    else
    {
      Lcd.setCursor(0, 1);
      Lcd.print("Water level: ");
      Lcd.print(startWaterLevel);
      
      while (checkWaterLevel() == 1)
      {
        //nothing happens
      }
    }
  }

  digitalWrite(dc_motor1_pin, LOW); //turn off motor1
  Lcd.clear();
  Lcd.setCursor(0, 0);
  Lcd.print("Motor1's OFF");
}

//Function checks temperature and returns it
float checkTemperature()
{
  float vrt = analogRead(A0);     //analog value of vrt
  vrt = (5.00 / 1023.00) * vrt;     //voltage
  float vr = VCC - vrt;
  float rt = vrt / (vr / R);      //resistance of rt
  float ln = log(rt / RT0);
  float tx = (1 / ((ln / B) + (1 / T0)));
  tx = tx - 273.15;

  return tx;
}

/*
   Function turns on heater spiral and
   when temperature of water is 100 celcius
   functions turns off heater spiral
*/
void turnOnHeater1()
{
  digitalWrite(heater_re1ay1_pin, HIGH); //turn on heater spiral
  Lcd.clear();
  Lcd.setCursor(0, 0);
  Lcd.print("Water is heating");
  Lcd.setCursor(0, 1);
  Lcd.print("Temp: ");
  Lcd.setCursor(12, 1);
  Lcd.print(" C");

  while (true)
  {
    if (checkTemperature() >= BOIL_TEMPERATURE) break;
    Lcd.setCursor(7, 1);
    Lcd.print(checkTemperature());
  }

  int timeLeft = SPIRAL1_DELAY / 1000;
  Lcd.clear();
  Lcd.setCursor(0, 0);
  Lcd.print("Water is heating");
  Lcd.setCursor(0, 1);
  Lcd.print("Time left: ");
  Lcd.print(timeLeft);
  Lcd.print(" sec");
  long saveTime = millis();     //for turning off time
  long saveTimeLeft = millis();     //for lefted time

  while (true)
  {
    if ((millis() - saveTimeLeft) >= 1000)
    {
      saveTimeLeft = millis();
      timeLeft--;
      Lcd.setCursor(11, 1);
      Lcd.print(timeLeft);
    }

    if (millis() - saveTime >= SPIRAL1_DELAY) break;
  }

  digitalWrite(heater_re1ay1_pin, LOW); //tunr off heater spiral
  Lcd.clear();
  Lcd.setCursor(0, 0);
  Lcd.print("Heating is over");
}

/*
   Function turns on blender and motor1 and
   when motor1 isn't need and it is turned off then
   function also turns off blender
*/
void turnOnBlender()
{
  digitalWrite(blender_relay2_pin, HIGH); //turn on blender
  Lcd.clear();
  Lcd.setCursor(0, 0);
  Lcd.print("Blunding...");
  int leftTime = BLENDER_DELAY / 1000;
  Lcd.setCursor(0, 1);
  Lcd.print("Time left: ");
  Lcd.print(leftTime);
  Lcd.print(" sec");
  long saveTime = millis();     //for turning off time
  long saveLeftTime = millis();     //for lefted time

  while (true)
  {
    if ((millis() - saveLeftTime) >= 1000)
    {
      saveLeftTime = millis();
      leftTime--;
      Lcd.setCursor(11, 1);
      Lcd.print(leftTime);
    }

    if (millis() - saveTime >= BLENDER_DELAY)
    {
      turnOnMotor1();
      digitalWrite(blender_relay2_pin, LOW); //turn off blender
      break;
    }
  }

  Lcd.clear();
  Lcd.setCursor(0, 0);
  Lcd.print("End blunding");
}

/*
   Function turns on motor2 and when motor2
   isn't needed function turns off motor2
*/
void turnOnMotor2()
{
  digitalWrite(dc_motor2_pin, HIGH);
  long saveTime = millis();
  long saveTimeLeft = saveTime;
  int leftTime = DC_MOTOR2_DELAY / 1000;
  Lcd.clear();
  Lcd.setCursor(0, 0);
  Lcd.print("Motor2's working");
  Lcd.setCursor(0, 1);
  Lcd.print("Time left: ");
  Lcd.print(leftTime);
  Lcd.print(" sec");

  while (millis() - saveTime <= DC_MOTOR2_DELAY)
  {
    if (millis() - saveTimeLeft >= 1000)
    {
      saveTimeLeft = millis();
      leftTime--;
      Lcd.setCursor(11, 1);
      Lcd.print(leftTime);
    }
  }

  digitalWrite(dc_motor2_pin, LOW);   //turn off motor2
}

/*
   Function turns on motor3 and when constant time is left
   function turns off motor3
*/
void turnOnMotor3()
{
  digitalWrite(dc_motor3_pin, HIGH);    //turn on motor3
  long saveTime = millis();
  long saveTimeLeft = saveTime;
  int leftTime = DC_MOTOR2_DELAY / 1000;
  Lcd.clear();
  Lcd.setCursor(0, 0);
  Lcd.print("Pump3's working");
  Lcd.setCursor(0, 1);
  Lcd.print("Time left: ");
  Lcd.print(leftTime);
  Lcd.print(" sec");

  while (millis() - saveTime <= DC_MOTOR3_DELAY)
  {
    if (millis() - saveTimeLeft >= 1000)
    {
      Lcd.setCursor(11, 1);
      saveTimeLeft = millis();
      leftTime--;
      Lcd.print(leftTime);
    }
  }

  digitalWrite(dc_motor3_pin, LOW);   //turn off motor3
}

/*
   Function turns on heater spiral2 and when constant time is
   left function turns off heater spiral2
*/
void turnOnHeater2()
{
  digitalWrite(heater_re1ay3_pin, HIGH); //turn on heater spiral
  Lcd.clear();
  Lcd.setCursor(0, 0);
  Lcd.print("Water is heating");
  int leftTime = SPIRAL2_DELAY / 1000;
  Lcd.setCursor(0, 1);
  Lcd.print("Time left: ");
  Lcd.print(leftTime);
  Lcd.print(" sec");
  long saveTime = millis();
  long saveLeftTime = millis();

  while (true)
  {
    if ((millis() - saveLeftTime) >= 1000)
    {
      saveLeftTime = millis();
      leftTime--;
      Lcd.setCursor(11, 1);
      Lcd.print(leftTime);
    }

    if (millis() - saveTime >= BLENDER_DELAY) break;
  }

  digitalWrite(heater_re1ay3_pin, LOW);   //turn off heater spiral
}
