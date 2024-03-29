/*
                                           __                __        __ 
                                          /  |              /  |      /  |
  _______   ______   __    __   ______   _%% |_     ______  %%/   ____%% |
 /       | /      \ /  |  /  | /      \ / %%   |   /      \ /  | /    %% |
/%%%%%%%/ /%%%%%%  |%% |  %% |/%%%%%%  |%%%%%%/   /%%%%%%  |%% |/%%%%%%% |
%% |      %% |  %%/ %% |  %% |%% |  %% |  %% | __ %% |  %% |%% |%% |  %% |
%% \_____ %% |      %% \__%% |%% |__%% |  %% |/  |%% \__%% |%% |%% \__%% |
%%       |%% |      %%    %% |%%    %%/   %%  %%/ %%    %%/ %% |%%    %% |
 %%%%%%%/ %%/        %%%%%%% |%%%%%%%/     %%%%/   %%%%%%/  %%/  %%%%%%%/ 
                    /  \__%% |%% |                                        
                    %%    %%/ %% |                                        
                     %%%%%%/  %%/ 
                     
Main STM32 Firmware                                  
*/

#include <Arduino.h>
#include <Servo.h>
#include <Adafruit_NeoPixel.h>
#include <Version.h>

//**********************
//Motor Control      //*
#define M1_A A0      //*
#define M1_B A1      //*
#define M2_A A2      //*
#define M2_B A3      //*
#define M3_A A4      //*
#define M3_B A5      //*
#define M4_A 8       //*
#define M4_B 9       //*
#define D1_ENA A6    //*
#define D1_ENB A7    //*
#define D2_ENA A8    //*
#define D2_ENB A9    //*
                     //*
// External          //*
#define EXT_1 19     //*
#define EXT_2 20     //*
#define EXT_3 22     //*
#define EXT_4 23     //*
#define EXT_5 24     //*
#define EXT_6 25     //*
                     //*
// Other             //*
#define WS_DATA 29   //*
#define RX 27        //*
#define TX 26        //*
#define numPixels 40 //*
#define SPKR 28      //*
//**********************

String serialStr, serialCmd, iStr;
String args[4];
int outPins[8] = {M1_A, M1_B, M2_A, M2_B, M3_A, M3_B, M4_A, M4_B};
int extPins[7] = {EXT_1, EXT_2, EXT_3, EXT_4, EXT_5, EXT_6};
int mcSPins[4];
int rInt, gInt, bInt, ledID;
bool verboseOut = false;
Servo servo1, servo2;
Adafruit_NeoPixel pixels(numPixels, WS_DATA, NEO_GRB + NEO_KHZ800);
HardwareSerial Serial3(RX, TX);

void setup() {
  Serial3.begin(115200, SERIAL_8E1); // open Serial Line
  Serial3.print("\r\nCryptoid - Main Control Board - STM32 Operational\r\n"); // send welcome msg
  for(int i = 0; i < 8; i++) {
    pinMode(outPins[i], OUTPUT);
  }
  tone(SPKR, 1500, 750); // beep
  pixels.begin(); // init LEDs
  pixels.show(); // clear LEDs
  for(int i = 0; i < numPixels; i++) { // set all LEDs to Red
    pixels.setPixelColor(i, pixels.Color(255, 0, 0));
    pixels.show();
  }
}

String recv() {
  while(Serial3.available() <= 0) {} // wait until data is recieved over serial
  serialStr = Serial3.readStringUntil('\n'); // read until terminator
  return serialStr; // return data
}

void recvArgs(int argsRequired) {
  for(int i = 0; i < argsRequired; i++) { // loop for every argument required
    args[i] = recv(); // store in arg array
    Serial3.print("OK\r\n");
  }
}

void loop() {
  serialCmd = recv();

  if (serialCmd == "INIT\r") { // INIT - Initialize board
    Serial3.print("OK\r\n"); // send response
    tone(SPKR, 1500, 750);
    digitalWrite(PC13, HIGH);
    pixels.clear(); // clear LEDs
    for(int i = 0; i < numPixels; i++) { // set all LEDs to Green
      pixels.setPixelColor(i, pixels.Color(0, 255, 0));
      pixels.show();
    }
  } 
  else if (serialCmd == "SETM\r") { // SETM - Set motor
    Serial3.print("OK\r\n");
    recvArgs(3);
    if(verboseOut) {
      Serial3.print("Motor ID: " + args[0] + "\r\nDirection: " + args[1] + "\r\nSpeed: " + args[2] + "\r\n"); 
    }
    if(args[0].toInt() == 1) { // If we're targeting motor X
      mcSPins[1] = M1_A; // Set pins for that motor
      mcSPins[2] = M1_B;
      mcSPins[3] = D1_ENA;
    } else if (args[0].toInt() == 2) {
      mcSPins[1] = M2_A;
      mcSPins[2] = M2_B;
      mcSPins[3] = D1_ENB;
    } else if (args[0].toInt() == 3) {
      mcSPins[1] = M3_A;
      mcSPins[2] = M3_B;
      mcSPins[3] = D2_ENA;
    } else if (args[0].toInt() == 4) {
      mcSPins[1] = M4_A;
      mcSPins[2] = M4_B;
      mcSPins[3] = D2_ENB;
    } 
    if (args[1].toInt() == 1) { // Set MC pins for the direction
      digitalWrite(mcSPins[1], HIGH);
      digitalWrite(mcSPins[2], LOW);
    } else if (args[1].toInt() == 2) {
      digitalWrite(mcSPins[1], LOW);
      digitalWrite(mcSPins[2], HIGH);
    }
    analogWrite(mcSPins[3], args[2].toInt()); // Set speed through PWM
  } 
  else if (serialCmd == "STPM\r") { // Stop Motor
    Serial3.print("OK\r\n");
    recvArgs(1);
    if(verboseOut) {
      Serial3.print("Motor ID: " + args[0] + "\r\n");
    }
    if(args[0].toInt() == 1) {
      analogWrite(D1_ENA, 0);
    } else if (args[0].toInt() == 2) {
      analogWrite(D1_ENB, 0);
    } else if (args[0].toInt() == 3) {
      analogWrite(D2_ENA, 0);
    } else if (args[0].toInt() == 4) {
      analogWrite(D2_ENB, 0);
    } else {
      Serial3.print("ERROR\r\n");
    }
  } 
  else if (serialCmd == "LEDS\r") { // LEDS - LED Set
    Serial3.print("OK\r\n");
    recvArgs(4);
    if(verboseOut) {
      Serial3.print("LED ID: " + args[0] + "\r\nR: " + args[1] + "\r\nG: " + args[2] + "\r\nB: " + args[3] + "\r\n"); 
    }
    ledID = args[0].toInt();
    rInt = args[1].toInt(); // convert strings to integers
    gInt = args[2].toInt();
    bInt = args[3].toInt();
    pixels.setPixelColor(ledID, pixels.Color(rInt, gInt, bInt)); // set color
    pixels.show();
  } 
  else if (serialCmd == "LEDA\r") { // LEDA - Set all LEDs
    Serial3.print("OK\r\n");
    recvArgs(3);
    if(verboseOut) {
      Serial3.print("R: " + args[0] + "\r\nG: " + args[1] + "\r\nB: " + args[2] + "\r\n"); 
    }
    rInt = args[0].toInt(); // convert strings to integers
    gInt = args[1].toInt(); // more efficent than recalculating every iteration
    bInt = args[2].toInt();
    pixels.clear();
    for(int i = 0; i < numPixels; i++) { // set every LED to the specified color
      pixels.setPixelColor(i, pixels.Color(rInt, gInt, bInt));
      pixels.show();
    }
  } 
  else if (serialCmd == "EXTS\r") { // Set External
    Serial3.print("OK\r\n");
    for(int i = 0; i < 6; i++) {
      recvArgs(1); // Recieve data
      iStr = String(i+1);
      if(verboseOut) {
        Serial3.print("Pin: " + iStr + "\r\nFunction: " + args[0] + "\r\n"); 
      }
      if (args[0] == "SRVO\r" && (i == 0 || i == 1)) { // If the pin is connected to a servo + that configuration is supported
        if (servo1.attached() == false) { // If servo1 is unset
          servo1.attach(extPins[i]); // Set servo1 to the pin we're currently on
        } else if (servo1.attached() == true) { // If servo1 is set
          servo2.attach(extPins[i]); // Set servo2 to the pin we're currently on
        }
      } else if (args[0] == "OUTP\r") {  // If the pin is an output
        pinMode(extPins[i], OUTPUT); // Set pinmode to output
      } else if (args[0] == "INPT\r") {
        pinMode(extPins[i], INPUT);
      } else {
        Serial3.print("ERROR\r\n"); // Error if type not recognised or an attempt to make a pin above EXT_2 a servo is made
        break; // Crash loop
      }
    }
  } 
  else if (serialCmd == "SRVO\r") { // Set Servo
    Serial3.print("OK\r\n");
    recvArgs(2); // Recieve data
    if(verboseOut) {
      Serial3.print("Servo ID: " + args[0] + "\r\nAngle: " + args[1] + "\r\n");
    }
    if (args[0].toInt() == 1 && servo1.attached() == false) { // If EXT pins haven't been set
      Serial3.print("ERROR\r\n");
    } else if (args[0].toInt() == 2 && servo2.attached() == false) {
      Serial3.print("ERROR\r\n");
    } else {
      if(args[0].toInt() == 1) {
        servo1.write(args[1].toInt());
      } else if (args[0].toInt() == 2) {
        servo2.write(args[1].toInt());
      }
    }
  } 
  else if (serialCmd == "RSTS\r") { // Software Reset
    Serial3.print("OK\r\n");
    NVIC_SystemReset();
  } 
  else if (serialCmd == "VERS\r") { // Version
    Serial3.print("OK\r\n");
    Serial3.print("Version ID: " + String(VERSION) + "\r\n");
    Serial3.print("Built at: " + String(BUILD_TIMESTAMP) + "\r\n");
  }
  else if (serialCmd == "SPKR\r") { // Set Speaker
    Serial3.print("OK\r\n");
    recvArgs(2);
    if(verboseOut) {
      Serial3.print("Frequency: " + args[0] + "\r\nDuration: " + args[1] + "\r\n");
    }
    tone(SPKR, args[0].toInt(), args[1].toInt());
  }
  else if (serialCmd == "VRBS\r") { // Toggle Verbose Serial Output
    Serial3.print("OK\r\n");
    Serial3.print("Toggling verbose output...\r\n");
    verboseOut = !verboseOut;
  }
  else if (serialCmd == "PINH\r") { // Set pin HIGH
    Serial3.print("OK\r\n");
    recvArgs(1);
    if(verboseOut) {
      Serial3.print("Setting pin: " + args[0] + " HIGH\r\n");
    }
    digitalWrite(extPins[args[0].toInt()-1], HIGH);
  }
  else if (serialCmd == "PINL\r") { // Set pin LOW
    Serial3.print("OK\r\n");
    recvArgs(1);
    if(verboseOut) {
      Serial3.print("Setting pin: " + args[0] + " LOW\r\n");
    }
    digitalWrite(extPins[args[0].toInt()-1], LOW);
  }
  else if (serialCmd == "GETP\r") { // Read pin
    Serial3.print("OK\r\n");
    recvArgs(1);
    String readResult = String(digitalRead(extPins[args[0].toInt()-1]));
    Serial3.print(readResult + "\r\n");
    if(verboseOut) {
      Serial3.print("Pin " + args[0] + "'s state is " + readResult + "\r\n");
    }
  }
  else { // ERROR - Error if command not recognized
    Serial3.print("ERROR\r\n");
  }
}