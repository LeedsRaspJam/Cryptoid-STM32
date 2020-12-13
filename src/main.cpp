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
#define WS_DATA 10   //*
#define RX 27        //*
#define TX 26        //*
#define numPixels 2  //*
//**********************

String serialStr, serialCmd;
String args[3];
int mcBPins[8] = {M1_A, M1_B, M2_A, M2_B, M3_A, M3_B, M4_A, M4_B};
int extPins[7] = {EXT_1, EXT_2, EXT_3, EXT_4, EXT_5, EXT_6};
int mcSPins[4];
Servo servo1, servo2;
Adafruit_NeoPixel pixels(numPixels, WS_DATA, NEO_GRB + NEO_KHZ800);
HardwareSerial Serial3(RX, TX);

void setup() {
  Serial3.begin(115200, SERIAL_8E1); // open Serial Line
  Serial3.print("\r\nCryptoid - Main Control Board - STM32 Operational\r\n"); // send welcome msg
  for(int i = 0; i < 8; i++) {
    pinMode(mcBPins[i], OUTPUT);
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
  if(serialCmd == "INIT\r") { // INIT - Initialize board
    Serial3.print("OK\r\n"); // send response
    digitalWrite(PC13, HIGH);
  } else if (serialCmd == "SETM\r") { // SETM - Set motor
    Serial3.print("OK\r\n");
    recvArgs(3);
    Serial3.print("Motor ID: " + args[0] + "\r\n");
    Serial3.print("Direction is: " + args[1] + "\r\n");
    Serial3.print("Speed is: " + args[2] + "\r\n");
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

  } else if (serialCmd == "LEDS\r") { // LEDS - LED Set
    Serial3.print("OK\r\n");
    recvArgs(4);
    Serial3.print("Number is: " + args[0] + "\r\n");
    Serial3.print("R: " + args[1] + "\r\n");
    Serial3.print("G: " + args[2] + "\r\n");
    Serial3.print("B: " + args[3] + "\r\n");
    pixels.setPixelColor(args[0].toInt(), pixels.Color(args[1].toInt(), args[2].toInt(), args[3].toInt()));
    pixels.show();
  } else if (serialCmd == "LEDA\r") { // LEDA - Set all LEDs
    Serial3.print("OK\r\n");
    recvArgs(3);
    Serial3.print("R: " + args[0] + "\r\n");
    Serial3.print("G: " + args[1] + "\r\n");
    Serial3.print("B: " + args[2] + "\r\n");
    for(int i = 0; i < numPixels; i++) { // loop for all pixels
      pixels.setPixelColor(i, pixels.Color(args[0].toInt(), args[1].toInt(), args[2].toInt())); // set pixel
      pixels.show();
    }
  } else if (serialCmd == "SEXT\r") { // Set External
    for(int i = 0; i < 6; i++) {
      recvArgs(1); // Recieve data
      if (args[0] == "SRVO\r" && i == (EXT_1 || EXT_2)) { // If the pin is connected to a servo + that configuration is supported
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
  } else if (serialCmd == "SRVO\r") {
    recvArgs(2); // Recieve data
    if (args[0].toInt() == 1 && servo1.attached() == false) {
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
  
  } else if (serialCmd == "RSTS\r") { // Software Reset
    Serial3.print("OK\r\n");
    NVIC_SystemReset();
  } else if (serialCmd == "TEST\r") {
    digitalWrite(M1_A, HIGH);
    digitalWrite(M1_B, LOW);
    digitalWrite(M2_A, HIGH);
    digitalWrite(M2_B, LOW);
    digitalWrite(M3_A, HIGH);
    digitalWrite(M3_B, LOW);
    digitalWrite(M4_A, HIGH);
    digitalWrite(M4_B, LOW);

    analogWrite(D1_ENA, 255);
    analogWrite(D1_ENB, 255);
    analogWrite(D2_ENA, 255);
    analogWrite(D2_ENB, 255);
  } else {
    Serial3.print("ERROR\r\n"); // ERROR - Error if command not recognized
  }
}