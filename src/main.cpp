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
#include <Adafruit_NeoPixel.h>

String serialStr, serialCmd;
String args[3];
#define numPixels 2
Adafruit_NeoPixel pixels(numPixels, 10, NEO_GRB + NEO_KHZ800);
HardwareSerial Serial3(PB11, PB10);

void setup() {
  Serial3.begin(115200, SERIAL_8E1); // open Serial Line
  Serial3.print("\nCryptoid - Main Control Board - STM32 Operational\n"); // send welcome msg
}

String recv() {
  while(Serial3.available() <= 0) {} // wait until data is recieved over serial
  serialStr = Serial3.readStringUntil('\n'); // read until terminator
  return serialStr; // return data
}

void recvArgs(int argsRequired) {
  for(int i = 0; i < argsRequired; i++) { // loop for every argument required
    args[i] = recv(); // store in arg array
    Serial3.print("OK\n");
  }
}

void loop() {
  serialCmd = recv();
  if(serialCmd == "INIT") { // INIT - Initialize board
    Serial3.print("OK\n"); // send response
  } else if (serialCmd == "SETM") { // SETM - Set motor
    Serial3.print("OK\n");
    recvArgs(3);
    Serial3.print("Motor ID: " + args[0] + "\n");
    Serial3.print("Direction is: " + args[1] + "\n");
    Serial3.print("Speed is: " + args[2] + "\n");
  } else if (serialCmd == "LEDS") { // LEDS - LED Set
    Serial3.print("OK\n");
    recvArgs(4);
    Serial3.print("Number is: " + args[0] + "\n");
    Serial3.print("R: " + args[1] + "\n");
    Serial3.print("G: " + args[2] + "\n");
    Serial3.print("B: " + args[3] + "\n");
    pixels.setPixelColor(args[0].toInt(), pixels.Color(args[1].toInt(), args[2].toInt(), args[3].toInt()));
    pixels.show();
  } else if (serialCmd == "LEDA") { // LEDA - Set all LEDs
    Serial3.print("OK\n");
    recvArgs(3);
    Serial3.print("R: " + args[0] + "\n");
    Serial3.print("G: " + args[1] + "\n");
    Serial3.print("B: " + args[2] + "\n");
    for(int i = 0; i < numPixels; i++) { // loop for all pixels
      pixels.setPixelColor(i, pixels.Color(args[0].toInt(), args[1].toInt(), args[2].toInt())); // set pixel
      pixels.show();
    }
  } else {
    Serial3.print("ERROR\n"); // ERROR - Error if command not recognized
  }
}