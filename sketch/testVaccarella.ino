#include <Grove_LED_Bar.h>
#include <AMS.h> //include the AMS library
#include "RunningAverage.h"
#include "Keyboard.h"


RunningAverage myRA(250);

AMS AMSa(5915, 0x28,0,100);

Grove_LED_Bar bar(8, 9, 1, LED_BAR_10);

uint8_t j = 0;

float readP(uint8_t sec);
void debug(long a, long b=0, long c=0, long d=0);

float ZERO = 0;
float zOut = 0;
float zIn = 0;

bool zeros = false;
long tS=0;
bool x=false;

int N = 0;

bool zO = false;
bool zI = false;

  
long iT = 0;

bool prima = true;
bool e = false;
float ppp;
float media;

float readUntilStable(bool z = false);
void reise();
uint8_t led(float p);
void blink(uint8_t n);

float p;

void setup() {
  myRA.clear();
  bar.begin();
  Keyboard.begin();

  pinMode(13,INPUT_PULLUP);
  
  delay(1000);
  bar.setLevel(10);
  
  ZERO = readP(200); //2 seconds

  bar.setLevel(0);
  delay(1000);

  

  while (!zeros) {
    p = readP(1);
    if (p > 20 && !zO) {
      zOut = readUntilStable(true);
      if (zOut > 0 && !digitalRead(13)) {
        zO = true;
      }
      
    }
    else if (p < -20 && !zI){
      zIn = readUntilStable(true);
      if (zIn < 0 && !digitalRead(13)) {
        zI = true;
      }
    }
    else {
      reise();
    }

    if ((zO) && (zI)) {
      zeros = true;
      while (!digitalRead(13));
    }

  }
  
}

float pO;
uint8_t sp = 0;
long t1 = 0;
long t2 = 0;

float zz = 0;

void loop() {
  
  p = readP(1);

  if (p > zOut) {
    pO = readUntilStable() - zOut;
    if (pO > 0 && !digitalRead(13)) {
      sp = 1;
      zz = zOut;
    }
  }
  else if (p < zIn) {
    pO = readUntilStable() - zIn;
    if (pO < 0 && !digitalRead(13)) {
      sp = 1;
      zz = zIn;
    }
  }
  else {
    walk();
  }

  while (sp > 0) {
    if (sp == 1) {
      leggiTestVaccarella(pO);
      t1 = millis();
      sp = 2;
    }
    else if (sp == 2) {
      if (millis() - t1 > 1000) {
        leggiTestVaccarella(readP(10)-zz);
        t2 = millis();
        sp = 3;
      }
    }
    else {
      if (millis() - t2 > 2500) {
        leggiTestVaccarella(readP(10)-zz);
        sp = 0;
        Keyboard.write(KEY_LEFT_ARROW);
        Keyboard.write(KEY_LEFT_ARROW);
        Keyboard.write(KEY_LEFT_ARROW);
        Keyboard.write(KEY_DOWN_ARROW);
      }
    }
  }
  

}

void leggi(float plpl) {
  if (plpl > 0) {
    Keyboard.print((int)plpl);
    Keyboard.write(KEY_DOWN_ARROW);
    Keyboard.write(KEY_LEFT_ARROW);
  }
  else {
    Keyboard.print(abs((int)plpl));
    Keyboard.write(KEY_RIGHT_ARROW);
  }
  while(!digitalRead(13));
}

void leggiTestVaccarella(float plpl) {

  if (plpl > 0) {
    Keyboard.print((int)plpl);
    Keyboard.write(KEY_RIGHT_ARROW);
  }
  else {
    Keyboard.print(abs((int)plpl));
    Keyboard.write(KEY_RIGHT_ARROW);
  }
  while(!digitalRead(13));
}

float readP(int cS) {
  long Pressure=0;
   for (int i=0;i<cS;i++) {
    Pressure += AMSa.readPressure()*100;
    if (cS > 1) {
      delay(10);
    }
   }
   return Pressure/cS - ZERO;
}

float readUntilStable(bool z = false) {

  ppp = readP(1);
  myRA.addValue(ppp);
  
  if (myRA.getStandardError() < 2) {
    media = myRA.getAverage();
    if (z) {
      bar.setLevel(led(media*7));
    }
    else {
      bar.setLevel(led(media));
    }
    return media;
  }
  else {
    if (z) {
      blink(led(ppp*7));
    }
    else {
      blink(led(ppp));
    }
  }
  return 0;
  
}

void reise() {
  if (N > 10) {
    N = 0;
  }
  bar.setBits(N);
  N++;
  delay(100);
}


void blink(uint8_t n) {
  if ((millis() - tS) > 50) {
    tS = millis();
    if (x) {
      bar.setLevel(0);
      x = 0;
    }
    else {
      bar.setLevel(n);
      x = 1;
    }
  }
}

uint8_t led(float pl) {
  if (abs(pl)<700) {
    return map(abs(pl),0,700,0,8);
  }
  else if (abs(pl) < 1000) {
    return 9;
  }
  else {
    return 10;
  }
}

long iJ = 0;
bool up = true;

void walk() {
  
  if (millis() - iJ > 1000) {
    iJ = millis();

    if (j>1) {
      bar.setBits(pow(2,j)+1);
    }
    else {
      bar.setBits(pow(2,j));
    }
    
    
    if (up) {
      j++;
      if (j > 7) {
        up = false;
      }
    }
    else {
      j--;
      if (j < 1) {
        up = true;
      }
    }
    
  }

 
}

