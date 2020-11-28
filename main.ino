#include <LCD5110_Basic.h>
#include <HX711.h>

HX711 scale(5, 6);
HX711 scale1(3, 4);
float calibration_factor = 1930; // this calibration factor is adjusted according to my load cell
float calibration1_factor = 1100; // for second axis calibration. hasn't been adjusted yet
float units;
float units1;


LCD5110 myGLCD(8, 9, 10, 11, 12);
extern uint8_t SmallFont[];


volatile byte REV;
unsigned long int rpm, maxRPM;
unsigned long time;
unsigned int xrpm;
int lastrpm;
int ledPin = 13;
int led = 0, RPMlen , prevRPM;
long prevtime = 0;
int ks = 3;


float v1, v11;
float a1, a11;
#define LOOP 10


void setup()
{

  myGLCD.InitLCD();  //LCD Basladi
  myGLCD.setFont(SmallFont);

  Serial.begin(9600);
  attachInterrupt(0, RPMCount, RISING);
  REV = 0;
  rpm = 0;
  time = 0;
  pinMode(ledPin, OUTPUT);

  scale.set_scale();     // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();


  scale1.set_scale();
  scale1.tare();


  long zero_factor = scale.read_average();
  long first_factor = scale1.read_average();



  myGLCD.clrScr();
}

void loop()
{


  v11 = 0; a11 = 0;

  long currtime = micros();
  long idletime = currtime - prevtime;

  if (REV >= 8)
  {
    rpm = 60 * 1000000 / (micros() - time) * REV;
    
    Serial.print((micros() - time));
     time = micros();
    Serial.print("   ");
    Serial.print(REV);
    Serial.print("   ");
   
    REV = 0;

    prevtime = currtime;

    xrpm = (rpm / ks);
  }



  myGLCD.printNumI(xrpm, LEFT, 0, 5, '_');                     // rpm yazılıyo
  myGLCD.print("RPM", 32, 0);

  scale.set_scale(calibration_factor); //birinci calibre
  units = scale.get_units(), 10;
  float xunits = (units * 2.5);

  myGLCD.printNumF(xunits, 2, LEFT, 8);
  myGLCD.print("gr", 50, 8);

  scale1.set_scale(calibration1_factor); //diger calibre
  units1 = scale1.get_units(), 10;
  float xunits1 = (units1 * 2.5);

  myGLCD.printNumF(xunits1, 2, LEFT, 16);
  myGLCD.print("gr", 50, 16);

  for (int a = 0 ; a < LOOP ; a++)
  {
    v1 = analogRead(5);
    v1 = (float)v1 / 1007 * 25 ;
    v11 = v11 + v1 / LOOP;


    a1 = analogRead(0);
    a1 = ((float)a1 - 506.6) / 1023 * 60;
    a11 = a11 + a1 / LOOP;
  }

  float watt = (a11 * v11);
  float xwatt = watt;
  float xa11 = a11;
  float xv11 = v11;
  float xxunits = xunits;
  float xxunits1 = xunits1;
  int xxrpm = xrpm;



  myGLCD.print("A", LEFT, 24);
  myGLCD.printNumF(a11, 2, CENTER, 24);

  myGLCD.print("V", LEFT, 32);
  myGLCD.printNumF(v11, 2, CENTER, 32);

  myGLCD.print("W", LEFT, 40);
  myGLCD.printNumF(watt, 2, CENTER, 40);
  myGLCD.print("KS: ", 54, 40);
  myGLCD.printNumI(ks, 78, 40);
  /*

    String SerialString = String(xxrpm);

    Serial.println("{\"xrpm\":" + SerialString + ",\"xunits\":" + xxunits + ",\"xunits1\":" + xxunits1 + ",\"a11\":" + xa11 + ",\"v11\":" + xv11 + ",\"watt\":" + xwatt +"}");
  */
  Serial.print(xxrpm);
  Serial.print("  ");

  Serial.print(idletime);
  Serial.print("  ");

  Serial.print(xa11);
  Serial.print("  ");

  Serial.print(xv11);
  Serial.print("  ");

  Serial.print(xwatt);
  Serial.print("  ");

  Serial.print(xxunits);
  Serial.print("  ");

  Serial.print(xxunits1);
  Serial.print("  ");
  Serial.println("");

  //delay(250);


  if (idletime > 500) {
    xrpm = 0 ;

  }


}

void RPMCount()
{
  REV++;

 
}

