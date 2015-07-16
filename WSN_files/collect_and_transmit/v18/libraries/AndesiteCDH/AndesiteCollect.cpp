// ========
// Includes
// ========

#include "AndesiteCollect.h"



// //////////////////////////////////
// ///// ANDESITE COLLECT CLASS /////
// //////////////////////////////////

// Collect and store magnetometer data
void AndesiteCollect::mag() {
    
    // Check how many lines of data have been written to file
    if ( !_File.status() ) {
        Serial.println(F("ERROR: File state is bad, mag."));
        return;
    }
    
    // Update magnetometer readings
    DOF.readMag();
    
    // Write magnetometer data to file
    String data;    
    char temp[10] = {0};
    
    // Concatenate strings to write just one string to file
    data = "M";
    //dtostrf(acdh_adc_readData(.....),1,4,temp);
    dtostrf(DOF.calcMag(DOF.mx), 1, 4, temp);
    data += temp;
    data += ",";
    dtostrf(DOF.calcMag(DOF.my), 1, 4, temp);
    data += temp;
    data += ",";
    dtostrf(DOF.calcMag(DOF.mz), 1, 4, temp);
    data += temp;
    
    _File.write(data);
}



// Collect and store gyroscope data
void AndesiteCollect::gyro() {
    
    // Check how many lines of data have been written to file
    if ( !_File.status() ) {
        Serial.println(F("ERROR: File state is bad, gyro."));
        return;
    }
    
    // Update gyroscope readings
    // Serial.print(F("Collecting gyro data..."));
    DOF.readGyro();
    
    // Write gyroscope data to file
    String data;
    char temp[10] = {0};
    
    // Concatenate strings to write just one string to file
    data = "G";
    dtostrf(DOF.calcGyro(DOF.gx), 1, 4, temp);
    data += temp;
    data += ",";
    dtostrf(DOF.calcGyro(DOF.gy), 1, 4, temp);
    data += temp;
    data += ",";
    dtostrf(DOF.calcGyro(DOF.gz), 1, 4, temp);
    data += temp;
    
    _File.write(data);
}



// Collect and store GPS data
void AndesiteCollect::gps() {
    
    // Check how many lines of data have been written to file
    if ( !_File.status() ) {
        Serial.println(F("ERROR: File state is bad, gps."));
        return;
    }
    
    // Write GPS data to file
    String data;
    char temp[10] = {0};
    
    // Concatenate strings to write just one string to file
    data = "T";
    data += GPS.time.hour();
    data += ":";
    data += GPS.time.minute();
    data += ":";
    data += GPS.time.second();
    data += ".";
    data += GPS.time.centisecond();
    data += ",";
    
    dtostrf(GPS.location.lat(), 1, 4, temp);
    data += temp;
    data += ",";
    dtostrf(GPS.location.lng(), 1, 4, temp);
    data += temp;
    data += ",";
    dtostrf(GPS.altitude.meters(), 1, 4, temp);
    data += temp;
    
    _File.write(data);
}
