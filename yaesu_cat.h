// yaesu_cat.h

while (Serial.available()) {
    rdYS="";
    Serial.readBytesUntil(lf, rdY, 38);         // fill array from serial
        if (rdY[0] == 73 && rdY[1] == 70){      // filter
            for (int i=5; i<=12; i++){          // 6-13 position to freq
                rdYS = rdYS + String(rdY[i]);   // append variable to string
            }
            freq = rdYS.toInt();

            //=====[ Frequency (Hz) to Band rules ]======================================
            //                                        you can expand rules up to 14 Bands
            
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
            //===========================================================================
            
            bandSET();                                              // set outputs relay

            #if defined(SERIAL_echo)
                serialEcho();
            #endif
        }
        memset(rdY, 0, sizeof(rdY));   // Clear contents of Buffer
}

