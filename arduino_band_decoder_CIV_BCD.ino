#include <Arduino.h>

/*
  Band decoder for 2 radios with TRX control output for Arduino rev.0.4
-----------------------------------------------------------
  http://remoteqth.com/arduino-band-decoder.php
  2016-05 by OK1HRA

  Inspired by OK1CDJ code, Thanks!
  https://github.com/ok1cdj/ARDUINO-Bandecoder

  ___               _        ___ _____ _  _
 | _ \___ _ __  ___| |_ ___ / _ \_   _| || |  __ ___ _ __
 |   / -_) '  \/ _ \  _/ -_) (_) || | | __ |_/ _/ _ \ '  \
 |_|_\___|_|_|_\___/\__\___|\__\_\|_| |_||_(_)__\___/_|_|_|


    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

Features:
Support inputs
  * SERIAL terminal (ASCII)
  * ICOM CI-V
  * KENWOOD - CAT
  * YAESU BCD
  * ICOM ACC voltage
  * YAESU CAT - TRX since 2008 ascii format
  * YAESU CAT old binary format (tested on FT-817)

Outputs
  * 14 local relay

  Changelog
  ---------
  2017-09 multi output relay support in matrix table
  2016-07 REMOTE_RELAY TCP/IP remote relay - need install and configure TCP232 module
  2016-05 ICOM_CIV  add 'state machine' protection against collisions on CIV bus, TNX KF5SA and N2LEE
  2016-02 YAESU CAT in/output support old binary format (tested on FT-817)
  2016-01 YAESU CAT in/output support since 2015 ascii format
  2019-08 Large modifications for 2 radios from EC3TW

*/
//=====[ Inputs ]=============================================================================================

#define ICOM_CIV           // read frequency from CIV (icom_civ.h) ** you must enabled 'CI-V transceive' in TRX settings **
// #define KENWOOD_PC         // RS232 CAT (kenwood_pc.h)
// #define YAESU_CAT          // RS232 CAT (yaesu_cat.h) YAESU CAT since 2015 ascii format
// #define YAESU_CAT_OLD      // Old binary format RS232 CAT (yaesu_cat_old.h) <------- ** tested on FT-817 **
// #define YAESU_BCD          // TTL BCD in A  (yaesu_bcd.h)
// #define ICOM_ACC           // voltage 0-8V on pin4 ACC(2) connector - need calibrate in (icom_acc.h)

//=====[ Outputs ]============================================================================================

// #define SERIAL_echo        // Feedback on serial line in same baudrate, CVS format <[band],[freq]>\n
 #define BCD_OUT            // output 11-14 relay used as Yaesu BCD

//=====[ Settings ]===========================================================================================

 #define SERBAUD      9600  // [baud] Serial port in/out baudrate
 #define CIV_ADRESS   0x98  // CIV input HEX Icom adress (0x is prefix) Está desactivado en la línea 410-411
 #define CIV_ADR_OUT  0x56  // CIV output HEX Icom adress (0x is prefix)

//=====[ Sets band -->  to output in MATRIX table ]===========================================================

        boolean matrix[15][15] = { /*

        Band 0 --> */ { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, /*  If inputs out of range, or WATCHDOG timeout
\       Band 1 --> */ { 1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, /*
 \      Band 2 --> */ { 0,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, /*
  \     Band 3 --> */ { 0,  0,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, /*
   \    Band 4 --> */ { 0,  0,  0,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, /*
    \   Band 5 --> */ { 0,  0,  0,  0,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, /*
     \  Band 6 --> */ { 0,  0,  0,  0,  0,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0 }, /*
IN    ) Band 7 --> */ { 0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0 }, /*
     /  Band 8 --> */ { 0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0 }, /*
    /   Band 9 --> */ { 0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0 }, /*
   /    Band 10 -> */ { 0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0 }, /*
  /     Band 11 -> */ { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0 }, /*
 /      Band 12 -> */ { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0 }, /*
/       Band 13 -> */ { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0 }, /*
        Band 14 -> */ { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0 }, /*
                        |   |   |   |   |   |   |   |   |   |   |   |   |   |
                        V   V   V   V   V   V   V   V   V   V   V   V   V   V
                     -----------------------------------------------------------
                     |  1   2   3   4   5   6   7   8   9  10  11  12  13  14  |
                     -----------------------------------------------------------
                                          OUTPUTS RELAY*/
        };
//============================================================================================================

/*                     Arduino Ports / in-out layout
-------------------------------------------------------------------------------
        PIND       |       PINB        |         PINC         | Arduino Ports
   6 5 4 3 2 1 x x | x x x 11 10 9 8 7 | x x x 16 15 14 13 12 | switch number
-------------------------------------------------------------------------------
*/

int key[2][16] = {
//           PIND                PINB             PINC
      { 2, 3, 4, 5, 6, 7,   0, 1, 2, 3, 4,   0, 1, 2, 3, 4 }, // bit in port
      { 3, 3, 3, 3, 3, 3,   1, 1, 1, 1, 1,   2, 2, 2, 2, 2 }, // Ports
};

int bitNR;
int dataD;
int dataB;
int dataC;
int BAND;
int BANDs;
long freq = 0;

#if defined(ICOM_ACC)
    const int AD = A7;
    int VALUE = 0;
    int prevVALUE=0;
    float VOLTAGE = 0;
    int band = 0;
    int counter = 0;
#endif
#if defined(YAESU_BCD)
    boolean BCD1;
    boolean BCD2;
    boolean BCD3;
    boolean BCD4;
    int bandBCD;
#endif
#if defined(KENWOOD_PC) || defined(YAESU_CAT)
    int lf = 59;  // 59 = ;
#endif
#if defined(KENWOOD_PC)
    char rdK[37];   //read data kenwood
    String rdKS;    //read data kenwood string
#endif
#if defined(YAESU_CAT)
    char rdY[37];   //read data yaesu
    String rdYS;    //read data yaesu string
#endif
#if defined(YAESU_CAT_OLD)
    byte rdYO[37];   //read data yaesu
    String rdYOS;    //read data yaesu string
#endif
#if defined(ICOM_CIV)
    int fromAdress = 14;              // 0E
    byte rdI[10];   //read data icom
    String rdIS;    //read data icom string
    long freqPrev1;
    byte incomingByte = 0;
    int state = 1;  // state machine
#endif
#if defined(BCD_OUT)
    char BCDout;
#endif


void setup() {
    #if defined(SERIAL_echo) || defined(KENWOOD_PC) || defined(ICOM_CIV) || defined(YAESU_CAT)
        Serial.begin(SERBAUD);
        Serial.setTimeout(10);
    #endif
    #if defined(YAESU_CAT_OLD)
        Serial.begin(SERBAUD, SERIAL_8N2);
        Serial.setTimeout(10);
    #endif
    #if defined(KENWOOD_PC) || defined(YAESU_CAT)
//        CATdata.reserve(200);          // reserve bytes for the CATdata
    #endif
    analogReference(EXTERNAL);
    //      bit #  76543210
    DDRD = DDRD | B11111100; // D7-2 output, 1-0 without changing (RX/TX)
    DDRB =        B00111111; // bit7-6 not use, D13-8 output
    DDRC =        B00000111; // A7-3 input, A2-0 output
}

void loop() {
    dataD = B00000000;
    dataB = B00000000;
    dataC = B00000000;
    #if defined(ICOM_ACC)
        #include "icom_acc.h"
    #endif
    #if defined(ICOM_CIV)
        #include "icom_civ.h"
    #endif
    #if defined(YAESU_BCD)
        #include "yaesu_bcd.h"
    #endif
    #if defined(KENWOOD_PC)
        #include "kenwood_pc.h"
    #endif
    #if defined(YAESU_CAT)
        #include "yaesu_cat.h"
    #endif
    #if defined(YAESU_CAT_OLD)
        #include "yaesu_cat_old.h"
    #endif
}

//=====[ Output relay ]=======================================================================================

void bandSET() {                                               // set outputs by BAND variable
    #if defined(BCD_OUT)
        BANDs=10;                                              // BANDs dependency to BCD output reserved
    #else
        BANDs=14;
    #endif
    for (bitNR=0; bitNR<BANDs; bitNR++){
      // port D
      if(key[1][bitNR]==3){
        if(matrix[BAND][bitNR]==1){
          dataD = dataD | (1<<key[0][bitNR]);               // 0-th bit to 1
        }
      }
      // port B
      if(key[1][bitNR]==1){
        if(matrix[BAND][bitNR]==1){
          dataB = dataB | (1<<key[0][bitNR]);               // 0-th bit to 1
        }
      }
      // port C
      if(key[1][bitNR]==2){
        if(matrix[BAND][bitNR]==1){
          dataC = dataC | (1<<key[0][bitNR]);               // 0-th bit to 1
        }
      }
    }
    PORTD = dataD;
    PORTB = dataB;
    PORTC = dataC;
    #if defined(BCD_OUT)
        bcdOut();
    #endif
}

//=====[ Output serial ]======================================================================================

void serialEcho() {
    Serial.print("<");
    Serial.print(BAND);
    Serial.print(",");
    Serial.print(freq);
    Serial.println(">");
    Serial.flush();
}

//=====[ Output BCD ]=========================================================================================

#if defined(BCD_OUT)
    void bcdOut(){
        boolean BCDmatrixOUT[4][11] = { /*
        --------------------------------------------------------------------
        Band # to output relay   0   1   2   3   4   5   6   7   8   9  10
        (Yaesu BCD)                 160 80  40  30  20  17  15  12  10  6m
        --------------------------------------------------------------------
                                 |   |   |   |   |   |   |   |   |   |   |
                                 V   V   V   V   V   V   V   V   V   V   V
                            */ { 0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0 }, /* --> Relay 11
                            */ { 0,  0,  1,  1,  0,  0,  1,  1,  0,  0,  1 }, /* --> Relay 12
                            */ { 0,  0,  0,  0,  1,  1,  1,  1,  0,  0,  0 }, /* --> Relay 13
                            */ { 0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1 }, /* --> Relay 14
        */};
        if (BCDmatrixOUT[0][BAND] == 1){ digitalWrite(12, HIGH); }else{ digitalWrite(12, LOW);;}
        if (BCDmatrixOUT[1][BAND] == 1){ digitalWrite(A0, HIGH); }else{ digitalWrite(A0, LOW);;}
        if (BCDmatrixOUT[2][BAND] == 1){ digitalWrite(A1, HIGH); }else{ digitalWrite(A1, LOW);;}
        if (BCDmatrixOUT[3][BAND] == 1){ digitalWrite(A2, HIGH); }else{ digitalWrite(A2, LOW);;}

// EA7KE usar estos para cambiar el estado lógico de salidas
//        if (BCDmatrixOUT[0][BAND] == 1){ digitalWrite(12, LOW); }else{ digitalWrite(12, HIGH);;}
//        if (BCDmatrixOUT[1][BAND] == 1){ digitalWrite(A0, LOW); }else{ digitalWrite(A0, HIGH);;}
//        if (BCDmatrixOUT[2][BAND] == 1){ digitalWrite(A1, LOW); }else{ digitalWrite(A1, HIGH);;}
//        if (BCDmatrixOUT[3][BAND] == 1){ digitalWrite(A2, LOW); }else{ digitalWrite(A2, HIGH);;}
    }
#endif

//=====[ Icom CIV ]===========================================================================================

#if defined(ICOM_CIV)

    int icomSM(byte b){      // state machine
        // This filter solves read from 0x00 0x05 0x03 commands and 00 E0 F1 address used by software
        switch (state) {
            case 1: if( b == 0xFE ){ state = 2; rdI[0]=b; }; break;
            case 2: if( b == 0xFE ){ state = 3; rdI[1]=b; }else{ state = 1;}; break;
            // addresses that use different software 00-trx, e0-pc-ale, winlinkRMS, f1-winlink trimode
            case 3: if( b == 0x00 || b == 0xE0 || b == 0xF1 ){ state = 4; rdI[2]=b;                             // choose command $03
              }else if( b == CIV_ADRESS ){ state = 6; rdI[2]=b;}else{ state = 1;}; break;                       // or $05

         //   case 4: if( b == CIV_ADRESS ){ state = 5; rdI[3]=b; }else{ state = 1;}; break;                      // select command $03 EA7KE
            case 4: if( 0 == 0 ){ state = 5; rdI[3]=b; }else{ state = 1;}; break;                      // select command $03 - EA7KE Valida la igualdad para que funcione con todas las direcciones C-IV
         
            case 5: if( b == 0x00 || b == 0x03 ){state = 8; rdI[4]=b; }else{ state = 1;}; break;

            case 6: if( b == 0x00 || b == 0xE0 || b == 0xF1 ){ state = 7; rdI[3]=b; }else{ state = 1;}; break;  // select command $05
            case 7: if( b == 0x00 || b == 0x05 ){ state = 8; rdI[4]=b; }else{ state = 1;}; break;

            case 8: if( b <= 0x99 ){state = 9; rdI[5]=b; }else{state = 1;}; break;
            case 9: if( b <= 0x99 ){state = 10; rdI[6]=b; }else{state = 1;}; break;
           case 10: if( b <= 0x99 ){state = 11; rdI[7]=b; }else{state = 1;}; break;
           case 11: if( b <= 0x99 ){state = 12; rdI[8]=b; }else{state = 1;}; break;
           case 12: if( b <= 0x99 ){state = 13; rdI[9]=b; }else{state = 1;}; break;
           case 13: if( b == 0xFD ){state = 1; rdI[10]=b; }else{state = 1; rdI[10] = 0;}; break;
        }
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
#endif
