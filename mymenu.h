#ifndef MYMENU_h
#define MYMENU_h


#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "mymenuelement.h"

class MYMENU {
  public:
    MYMENU(std::string title, MYMENUELEMENT elements[]); // base type

    /*//char begin();
    char begin(int sdaPin, int sclPin);
      // call pressure.begin() to initialize BMP280 before use
      // returns 1 if success, 0 if failure (i2C connection problem.)
        
    short getOversampling(void);
    char  setOversampling(short oss);
    
    char startMeasurment(void);
      // command BMP280 to start a pressure measurement
      // oversampling: 0 - 3 for oversampling value
      // returns (number of ms to wait) for success, 0 for fail
    
    char calcTemperature(double &T, double &uT);
      // calculation the true temperature from the given uncalibrated Temperature 
      
    char calcPressure(double &P, double uP);
      //calculation for measuring pressure.
      
    double sealevel(double P, double A);
      // convert absolute pressure to sea-level pressure 
      // P: absolute pressure (mbar)
      // A: current altitude (meters)
      // returns sealevel pressure in mbar

    double altitude(double P, double P0);
      // convert absolute pressure to altitude (given baseline pressure; sea-level, runway, etc.)
      // P: absolute pressure (mbar)
      // P0: fixed baseline pressure (mbar)
      // returns signed altitude in meters

    char getError(void);
      // If any library command fails, you can retrieve an extended
      // error code using this command. Errors are from the wire library: 
      // 0 = Success
      // 1 = Data too long to fit in transmit buffer
      // 2 = Received NACK on transmit of address
      // 3 = Received NACK on transmit of data
      // 4 = Other error
      
    char getTemperatureAndPressure(double& T,double& P);
*/
  private:
    std::string  title;
    MYMENUELEMENT elements[];
  /*
    char readCalibration();
      // Retrieve calibration data from device:
      // The BMP280 includes factory calibration data stored on the device.
      // Each device has different numbers, these must be retrieved and
      // used in the calculations when taking measurements.

    char readInt(char address, double &value);
      // read an signed int (16 bits) from a BMP280 register
      // address: BMP280 register address
      // value: external signed int for returned value (16 bits)
      // returns 1 for success, 0 for fail, with result in value

    char readUInt(char address, double &value);
      // read an unsigned int (16 bits) from a BMP280 register
      // address: BMP280 register address
      // value: external unsigned int for returned value (16 bits)
      // returns 1 for success, 0 for fail, with result in value

    char readBytes(unsigned char *values, char length);
      // read a number of bytes from a BMP280 register
      // values: array of char with register address in first location [0]
      // length: number of bytes to read back
      // returns 1 for success, 0 for fail, with read bytes in values[] array
      
    char writeBytes(unsigned char *values, char length);
      // write a number of bytes to a BMP280 register (and consecutive subsequent registers)
      // values: array of char with register address in first location [0]
      // length: number of bytes to write
      // returns 1 for success, 0 for fail
    
    char getUnPT(double &uP, double &uT); 
      //get uncalibrated UP and UT value.
  
        
    //int dig_T2 , dig_T3 , dig_T4 , dig_P2 , dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9; 
    //unsigned int dig_P1 , dig_T1 ;
    double dig_T1, dig_T2 , dig_T3 , dig_T4 , dig_P1, dig_P2 , dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9; 
    short oversampling, oversampling_t;
    double t_fine;
    char error;
    */
};




#endif
