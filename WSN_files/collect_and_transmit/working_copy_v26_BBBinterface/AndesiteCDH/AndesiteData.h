// Header guard
#ifndef ANDESITEDATA_H
#define ANDESITEDATA_H

// ========
// Includes
// ========

#include <SdFat.h>
#include <TinyGPS.h>

#include <SPI.h>
#include <Wire.h>
#include <SFE_LSM9DS0.h>

// Global variables
extern SdFat       SD;
extern LSM9DS0     DOF;
extern TinyGPSPlus GPS;

#endif
