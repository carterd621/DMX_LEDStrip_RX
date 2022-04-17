#include "masterDefs.h"

char numMap[10] = {'0','1','2','3','4','5','6','7','8','9'};

void mainDisp(HT16K33 display, int address){
  display.clear();
  display.printChar('A',0);
  display.printChar(numMap[address/100],1);
  display.printChar(numMap[(address/10)%10],2);
  display.printChar(numMap[address%10],3);
  display.updateDisplay();
}

void errorDisp(HT16K33 display){
  display.clear();
  display.printChar('E',0);
  display.printChar('R',1);
  display.printChar('R',2);
  display.updateDisplay();
}

void modeDisp(HT16K33 display, int mode){
  display.clear();
  display.printChar('M',0);
  display.printChar(numMap[mode],1);
  display.updateDisplay();
}

void startDisp(HT16K33 display){
  display.clear();
  display.printChar('D',0);
  display.printChar('M',1);
  display.printChar('X',2);
  display.updateDisplay();
}

void rxDisp(HT16K33 display){
  display.clear();
  display.printChar('R',0);
  display.printChar('X',1);
  display.updateDisplay();
}

void DMXRXDisp(HT16K33 display, int value){
  display.clear();
  display.printChar('V',0);
  display.printChar(numMap[value/100],1);
  display.printChar(numMap[(value/10)%10],2);
  display.printChar(numMap[value%10],3);
  display.updateDisplay();
}