#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN  30
#define CSN_PIN 31

RF24 radio(CE_PIN, CSN_PIN); 

const byte rxAddr[6] = "00001";

unsigned long timer;
long distance = 10;

int leftDistance = 0;
int rightDistance = 0;

#define MOVE 1000

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);

  radio.begin();
  radio.setPALevel(RF24_PA_MIN);   // RF24_PA_MIN ,RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate( RF24_250KBPS ); // RF24_250KBPS, RF24_1MBPS, RF24_2MBPS
  
  // Set the reading pipe and start listening
  radio.openReadingPipe(0, rxAddr);
  radio.startListening();
}

void stopMove() {
  Serial2.print("0 0 0 0");
  Serial2.print('\n');

  Serial2.print("1 0 0 0");
  Serial2.print('\n');
}

int maxVal(int a, int b) {return a > b ? a : b;}
int minVal(int a, int b) {return a > b ? b : a;}

void moveTrack(int curDisL, int curDisR, int padX, int padY) {
  int way = padX >= 0 ? 1 : -1;
  int distanceL = curDisL + (MOVE * way);
  int distanceR = curDisR + (MOVE * way);
  int speedR, speedL;

  if(padX > 0) {
    if(padY > 0) {
      speedR = abs(padX - padY);
      speedL = padX;
    } else if (padY == 0) {
      speedR = padX;
      speedL = padX;
    } else {
      speedL = abs(padX - abs(padY));
      speedR = padX;
    }
  } else if(padX == 0) {
    if(padY > 0) {
      speedL = padY;
      speedR = 0;
      way = -way;
    } else if (padY == 0) {
      speedR = 0;
      speedL = 0;
    } else {
      speedR = abs(padY);
      speedL = 0;
      way = -way;
    }
  } else {
    if(padY > 0) {
      speedR = abs(-padX - padY);
      speedL = -padX;
    } else if (padY == 0) {
      speedR = -padX;
      speedL = -padX;
    } else {
      speedL = abs(-padX - abs(padY));
      speedR = -padX;
    }
  }
  
  Serial2.print("0 " + String(distanceL) + " " + String(speedL) + " " + String(speedL));
  Serial2.print('\n');
  Serial2.print("1 " + String(distanceR) + " " + String(speedR) + " " + String(speedR));
  Serial2.print('\n');
}

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void loop() {

  if (Serial2.available()) {
    leftDistance = Serial2.parseInt();
    rightDistance = Serial2.parseInt();
    char eol = Serial2.read();
  }

  if (Serial.available()) {
    int inByte = Serial.read();
    Serial2.write(inByte);
  }

  if (radio.available())
  {
    char text[100] = {0};
    radio.read(&text, sizeof(text));
    int x = getValue(text, ' ', 0).toInt();
    int y = getValue(text, ' ', 1).toInt();
    Serial.print(String(x) + " " + String(y));
    // moveTrack(leftDistance, rightDistance, x, y);
  }
}