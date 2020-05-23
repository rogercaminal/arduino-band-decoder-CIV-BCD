#include <Arduino.h>

//=====[ Settings ]===========================================================================================

 #define SERBAUD      9600  // [baud] Serial port in/out baudrate
 #define CIV_ADRESS   0x98  // CIV input HEX Icom adress (0x is prefix) Está desactivado en la línea 410-411
 #define CIV_ADR_OUT  0x56  // CIV output HEX Icom adress (0x is prefix)

//============================================================================================================

int BAND1;
int BAND2;
long freq1 = 0;
long freq2 = 0;
byte rdI1[10];   //read data icom
byte rdI2[10];   //read data icom
String rdIS1;    //read data icom string
String rdIS2;    //read data icom string
byte incomingByte1 = 0;
byte incomingByte2 = 0;
int state1 = 1;  // state machine
int state2 = 1;  // state machine

int pin_k31_band0 = 50;
int pin_k31_band1 = 51;
int pin_k31_band2 = 52;
int pin_k31_band3 = 53;

int pin_k32_band0 = 31;
int pin_k32_band1 = 33;
int pin_k32_band2 = 35;
int pin_k32_band3 = 37;

int pin_ICE1_10m = 2;
int pin_ICE1_15m = 3;
int pin_ICE1_20m = 4;
int pin_ICE1_40m = 5;
int pin_ICE1_80m = 6;
int pin_ICE1_160m = 7;

int pin_ICE2_10m = 8;
int pin_ICE2_15m = 9;
int pin_ICE2_20m = 10;
int pin_ICE2_40m = 11;
int pin_ICE2_80m = 12;
int pin_ICE2_160m = 13;

bool radio1_use_CIV;
bool radio2_use_CIV;

//-------------------------------------------------
void setup() {
  Serial.begin(9600);
  Serial.println("Setting up ... \n");
  
  Serial1.begin(SERBAUD);
  Serial1.setTimeout(10);
  Serial2.begin(SERBAUD);
  Serial2.setTimeout(10);
  
  analogReference(EXTERNAL);

  pinMode(pin_k31_band0, INPUT_PULLUP);
  pinMode(pin_k31_band1, INPUT_PULLUP);
  pinMode(pin_k31_band2, INPUT_PULLUP);
  pinMode(pin_k31_band3, INPUT_PULLUP);

  pinMode(pin_k32_band0, INPUT_PULLUP);
  pinMode(pin_k32_band1, INPUT_PULLUP);
  pinMode(pin_k32_band2, INPUT_PULLUP);
  pinMode(pin_k32_band3, INPUT_PULLUP);

  pinMode(pin_ICE1_10m, OUTPUT);
  pinMode(pin_ICE1_15m, OUTPUT);
  pinMode(pin_ICE1_20m, OUTPUT);
  pinMode(pin_ICE1_40m, OUTPUT);
  pinMode(pin_ICE1_80m, OUTPUT);
  pinMode(pin_ICE1_160m, OUTPUT);

  pinMode(pin_ICE2_10m, OUTPUT);
  pinMode(pin_ICE2_15m, OUTPUT);
  pinMode(pin_ICE2_20m, OUTPUT);
  pinMode(pin_ICE2_40m, OUTPUT);
  pinMode(pin_ICE2_80m, OUTPUT);
  pinMode(pin_ICE2_160m, OUTPUT);

  radio1_use_CIV = (Serial1.available() > 0) ? true : false;
  radio2_use_CIV = (Serial2.available() > 0) ? true : false;

  Serial.println("Uses CIV in radio 1 : ");
  Serial.println(radio1_use_CIV);
  Serial.println("Uses CIV in radio 2 : ");
  Serial.println(radio2_use_CIV);
}

void loop() {
  if (radio1_use_CIV) read_band_ICOM_CIV(1); else read_band_K3(1);
  if (radio2_use_CIV) read_band_ICOM_CIV(2); else read_band_K3(2);

  Serial.println(BAND1);

  bandSET(1);
  bandSET(2);
}


//-----------------------------------------------------
// R E L A Y s
//-----------------------------------------------------
void bandSET(int radio) { 
  // For some reason the relays work the other way around: 0V --> ON. Therefore, do the opposite.
  if (BAND1 == 1) {
    digitalWrite(pin_ICE1_160m, LOW);
    digitalWrite(pin_ICE1_80m, HIGH);
    digitalWrite(pin_ICE1_40m, HIGH);
    digitalWrite(pin_ICE1_20m, HIGH);
    digitalWrite(pin_ICE1_15m, HIGH);
    digitalWrite(pin_ICE1_10m, HIGH);
  }
  else if (BAND1 == 2) {
    digitalWrite(pin_ICE1_160m, HIGH);
    digitalWrite(pin_ICE1_80m, LOW);
    digitalWrite(pin_ICE1_40m, HIGH);
    digitalWrite(pin_ICE1_20m, HIGH);
    digitalWrite(pin_ICE1_15m, HIGH);
    digitalWrite(pin_ICE1_10m, HIGH);
  }
  else if (BAND1 == 3) {
    digitalWrite(pin_ICE1_160m, HIGH);
    digitalWrite(pin_ICE1_80m, HIGH);
    digitalWrite(pin_ICE1_40m, LOW);
    digitalWrite(pin_ICE1_20m, HIGH);
    digitalWrite(pin_ICE1_15m, HIGH);
    digitalWrite(pin_ICE1_10m, HIGH);
  }
  else if (BAND1 == 5) {
    digitalWrite(pin_ICE1_160m, HIGH);
    digitalWrite(pin_ICE1_80m, HIGH);
    digitalWrite(pin_ICE1_40m, HIGH);
    digitalWrite(pin_ICE1_20m, LOW);
    digitalWrite(pin_ICE1_15m, HIGH);
    digitalWrite(pin_ICE1_10m, HIGH);
  }
  else if (BAND1 == 7) {
    digitalWrite(pin_ICE1_160m, HIGH);
    digitalWrite(pin_ICE1_80m, HIGH);
    digitalWrite(pin_ICE1_40m, HIGH);
    digitalWrite(pin_ICE1_20m, HIGH);
    digitalWrite(pin_ICE1_15m, LOW);
    digitalWrite(pin_ICE1_10m, HIGH);
  }
  else if (BAND1 == 9) {
    digitalWrite(pin_ICE1_160m, HIGH);
    digitalWrite(pin_ICE1_80m, HIGH);
    digitalWrite(pin_ICE1_40m, HIGH);
    digitalWrite(pin_ICE1_20m, HIGH);
    digitalWrite(pin_ICE1_15m, HIGH);
    digitalWrite(pin_ICE1_10m, LOW);
  }

  if (BAND2 == 1) {
    digitalWrite(pin_ICE2_160m, LOW);
    digitalWrite(pin_ICE2_80m, HIGH);
    digitalWrite(pin_ICE2_40m, HIGH);
    digitalWrite(pin_ICE2_20m, HIGH);
    digitalWrite(pin_ICE2_15m, HIGH);
    digitalWrite(pin_ICE2_10m, HIGH);
  }
  else if (BAND2 == 2) {
    digitalWrite(pin_ICE2_160m, HIGH);
    digitalWrite(pin_ICE2_80m, LOW);
    digitalWrite(pin_ICE2_40m, HIGH);
    digitalWrite(pin_ICE2_20m, HIGH);
    digitalWrite(pin_ICE2_15m, HIGH);
    digitalWrite(pin_ICE2_10m, HIGH);
  }
  else if (BAND2 == 3) {
    digitalWrite(pin_ICE2_160m, HIGH);
    digitalWrite(pin_ICE2_80m, HIGH);
    digitalWrite(pin_ICE2_40m, LOW);
    digitalWrite(pin_ICE2_20m, HIGH);
    digitalWrite(pin_ICE2_15m, HIGH);
    digitalWrite(pin_ICE2_10m, HIGH);
  }
  else if (BAND2 == 5) {
    digitalWrite(pin_ICE2_160m, HIGH);
    digitalWrite(pin_ICE2_80m, HIGH);
    digitalWrite(pin_ICE2_40m, HIGH);
    digitalWrite(pin_ICE2_20m, LOW);
    digitalWrite(pin_ICE2_15m, HIGH);
    digitalWrite(pin_ICE2_10m, HIGH);
  }
  else if (BAND2 == 7) {
    digitalWrite(pin_ICE2_160m, HIGH);
    digitalWrite(pin_ICE2_80m, HIGH);
    digitalWrite(pin_ICE2_40m, HIGH);
    digitalWrite(pin_ICE2_20m, HIGH);
    digitalWrite(pin_ICE2_15m, LOW);
    digitalWrite(pin_ICE2_10m, HIGH);
  }
  else if (BAND2 == 9) {
    digitalWrite(pin_ICE2_160m, HIGH);
    digitalWrite(pin_ICE2_80m, HIGH);
    digitalWrite(pin_ICE2_40m, HIGH);
    digitalWrite(pin_ICE2_20m, HIGH);
    digitalWrite(pin_ICE2_15m, HIGH);
    digitalWrite(pin_ICE2_10m, LOW);
  }
}

//-----------------------------------------------------
// I C O M s
//-----------------------------------------------------
int icomSM(int radio, byte b){      // state machine

  if (radio == 1) {
  // This filter solves read from 0x00 0x05 0x03 commands and 00 E0 F1 address used by software
    switch (state1) {
      case 1: if( b == 0xFE ){ state1 = 2; rdI1[0]=b; }; break;
      case 2: if( b == 0xFE ){ state1 = 3; rdI1[1]=b; }else{ state1 = 1;}; break;
      // addresses that use different software 00-trx, e0-pc-ale, winlinkRMS, f1-winlink trimode
      case 3: if( b == 0x00 || b == 0xE0 || b == 0xF1 ){ state1 = 4; rdI1[2]=b;                             // choose command $03
              }else if( b == CIV_ADRESS ){ state1 = 6; rdI1[2]=b;}else{ state1 = 1;}; break;                       // or $05

      //   case 4: if( b == CIV_ADRESS ){ state1 = 5; rdI1[3]=b; }else{ state1 = 1;}; break;                      // select command $03 EA7KE
      case 4: if( 0 == 0 ){ state1 = 5; rdI1[3]=b; }else{ state1 = 1;}; break;                      // select command $03 - EA7KE Valida la igualdad para que funcione con todas las direcciones C-IV
         
      case 5: if( b == 0x00 || b == 0x03 ){state1 = 8; rdI1[4]=b; }else{ state1 = 1;}; break;

      case 6: if( b == 0x00 || b == 0xE0 || b == 0xF1 ){ state1 = 7; rdI1[3]=b; }else{ state1 = 1;}; break;  // select command $05
      case 7: if( b == 0x00 || b == 0x05 ){ state1 = 8; rdI1[4]=b; }else{ state1 = 1;}; break;

      case 8: if( b <= 0x99 ){state1 = 9; rdI1[5]=b; }else{state1 = 1;}; break;
      case 9: if( b <= 0x99 ){state1 = 10; rdI1[6]=b; }else{state1 = 1;}; break;
      case 10: if( b <= 0x99 ){state1 = 11; rdI1[7]=b; }else{state1 = 1;}; break;
      case 11: if( b <= 0x99 ){state1 = 12; rdI1[8]=b; }else{state1 = 1;}; break;
      case 12: if( b <= 0x99 ){state1 = 13; rdI1[9]=b; }else{state1 = 1;}; break;
      case 13: if( b == 0xFD ){state1 = 1; rdI1[10]=b; }else{state1 = 1; rdI1[10] = 0;}; break;
     }
  } // end radio 1
  else if (radio == 2) {
  // This filter solves read from 0x00 0x05 0x03 commands and 00 E0 F1 address used by software
    switch (state2) {
      case 1: if( b == 0xFE ){ state2 = 2; rdI2[0]=b; }; break;
      case 2: if( b == 0xFE ){ state2 = 3; rdI2[1]=b; }else{ state2 = 1;}; break;
      // addresses that use different software 00-trx, e0-pc-ale, winlinkRMS, f1-winlink trimode
      case 3: if( b == 0x00 || b == 0xE0 || b == 0xF1 ){ state2 = 4; rdI2[2]=b;                             // choose command $03
              }else if( b == CIV_ADRESS ){ state2 = 6; rdI2[2]=b;}else{ state2 = 1;}; break;                       // or $05

      //   case 4: if( b == CIV_ADRESS ){ state2 = 5; rdI2[3]=b; }else{ state2 = 1;}; break;                      // select command $03 EA7KE
      case 4: if( 0 == 0 ){ state2 = 5; rdI2[3]=b; }else{ state2 = 1;}; break;                      // select command $03 - EA7KE Valida la igualdad para que funcione con todas las direcciones C-IV
         
      case 5: if( b == 0x00 || b == 0x03 ){state2 = 8; rdI2[4]=b; }else{ state2 = 1;}; break;

      case 6: if( b == 0x00 || b == 0xE0 || b == 0xF1 ){ state2 = 7; rdI2[3]=b; }else{ state2 = 1;}; break;  // select command $05
      case 7: if( b == 0x00 || b == 0x05 ){ state2 = 8; rdI2[4]=b; }else{ state2 = 1;}; break;

      case 8: if( b <= 0x99 ){state2 = 9; rdI2[5]=b; }else{state2 = 1;}; break;
      case 9: if( b <= 0x99 ){state2 = 10; rdI2[6]=b; }else{state2 = 1;}; break;
      case 10: if( b <= 0x99 ){state2 = 11; rdI2[7]=b; }else{state2 = 1;}; break;
      case 11: if( b <= 0x99 ){state2 = 12; rdI2[8]=b; }else{state2 = 1;}; break;
      case 12: if( b <= 0x99 ){state2 = 13; rdI2[9]=b; }else{state2 = 1;}; break;
      case 13: if( b == 0xFD ){state2 = 1; rdI2[10]=b; }else{state2 = 1; rdI2[10] = 0;}; break;
     }
  } // end radio 2
}

unsigned int hexToDec(String hexString) {
  unsigned int decValue = 0;
  int nextInt;
  for (int i = 0; i < hexString.length(); i++) {
    nextInt = int(hexString.charAt(i));
    if (nextInt >= 48 && nextInt <= 57) nextInt = map(nextInt, 48, 57, 0, 9);
    if (nextInt >= 65 && nextInt <= 70) nextInt = map(nextInt, 65, 70, 10, 15);
    if (nextInt >= 97 && nextInt <= 102) nextInt = map(nextInt, 97, 102, 10, 15);
      nextInt = constrain(nextInt, 0, 15);
      decValue = (decValue * 16) + nextInt;
  }
  return decValue;
}

int convert_freq_to_int(long freq) {
  int BAND = 0;
       if (freq >=   1810000 && freq <=   2000000 )  {BAND=1;}  // 160m
  else if (freq >=   3500000 && freq <=   3800000 )  {BAND=2;}  //  80m
  else if (freq >=   7000000 && freq <=   7200000 )  {BAND=3;}  //  40m
  else if (freq >=  10100000 && freq <=  10150000 )  {BAND=4;}  //  30m
  else if (freq >=  14000000 && freq <=  14350000 )  {BAND=5;}  //  20m
  else if (freq >=  18068000 && freq <=  18168000 )  {BAND=6;}  //  17m
  else if (freq >=  21000000 && freq <=  21450000 )  {BAND=7;}  //  15m
  else if (freq >=  24890000 && freq <=  24990000 )  {BAND=8;}  //  12m
  else if (freq >=  28000000 && freq <=  29700000 )  {BAND=9;}  //  10m
  else if (freq >=  50000000 && freq <=  52000000 ) {BAND=10;}  //   6m
  else if (freq >= 144000000 && freq <= 146000000 ) {BAND=11;}  //   2m
  else {BAND=0;}                                                // out of range

  return BAND;
}

void read_band_ICOM_CIV(int radio) {
  if (radio == 1) {
    if (Serial1.available() > 0) {
        incomingByte1 = Serial1.read();
        icomSM(radio, incomingByte1);
        rdIS1="";
        if(rdI1[10]==0xFD){                    // state machine end
            for (int i=9; i>=5; i-- ){
                if (rdI1[i] < 10) { // leading zero
                    rdIS1 = rdIS1 + 0;
                }
                rdIS1 = rdIS1 + String(rdI1[i], HEX);  // append BCD digit from HEX variable to string
            }
            freq1 = rdIS1.toInt();
        
            BAND1 = convert_freq_to_int(freq1);
      }
    }
  } // end radio 1
  else if (radio == 2) {
    if (Serial2.available() > 0) {
        incomingByte2 = Serial2.read();
        icomSM(radio, incomingByte2);
        rdIS2="";
        if(rdI2[10]==0xFD){                    // state machine end
            for (int i=9; i>=5; i-- ){
                if (rdI2[i] < 10) { // leading zero
                    rdIS2 = rdIS2 + 0;
                }
                rdIS2 = rdIS2 + String(rdI2[i], HEX);  // append BCD digit from HEX variable to string
            }
            freq2 = rdIS2.toInt();

            BAND2 = convert_freq_to_int(freq2);
      }
    }
  } // end radio 2
}


//-----------------------------------------------------
// E L E C R A F T   -   K 3
//-----------------------------------------------------

void read_band_K3(int radio) {
  bool band0 = (radio == 1) ? digitalRead(pin_k31_band0) : digitalRead(pin_k32_band0);
  bool band1 = (radio == 1) ? digitalRead(pin_k31_band1) : digitalRead(pin_k32_band1);
  bool band2 = (radio == 1) ? digitalRead(pin_k31_band2) : digitalRead(pin_k32_band2);
  bool band3 = (radio == 1) ? digitalRead(pin_k31_band3) : digitalRead(pin_k32_band3);

  int band = 0;
  if ((band3 == LOW) && (band2 == LOW) && (band1 == LOW) && (band0 == HIGH))        band = 1;
  else if ((band3 == LOW) && (band2 == LOW) && (band1 == HIGH) && (band0 == LOW))   band = 2;
  else if ((band3 == LOW) && (band2 == LOW) && (band1 == HIGH) && (band0 == HIGH))  band = 3;
  else if ((band3 == LOW) && (band2 == HIGH) && (band1 == LOW) && (band0 == HIGH))  band = 5;
  else if ((band3 == LOW) && (band2 == HIGH) && (band1 == HIGH) && (band0 == HIGH)) band = 7;
  else if ((band3 == HIGH) && (band2 == LOW) && (band1 == LOW) && (band0 == HIGH))  band = 9;

  //Serial.println(radio);
  //Serial.println(band3);
  //Serial.println(band2);
  //Serial.println(band1);
  //Serial.println(band0);
  //Serial.println(band);
  //Serial.println("");

  if (radio == 1) BAND1 = band;
  else if (radio == 2) BAND2 = band;
}

