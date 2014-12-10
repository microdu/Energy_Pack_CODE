#ifndef CLASHARED_C_H_
#define CLASHARED_C_H_

#define PI	3.1415;

#ifndef DSP28_DATA_TYPES
#define DSP28_DATA_TYPES
typedef short           int16;
typedef long            int32;
typedef unsigned short    Uint16;
typedef unsigned long   Uint32;
typedef float           float32;
typedef long double     float64;
#endif

#include "CLAmath.h"
#include "CLARKE_CLA.h"
#include "PARK_CLA.h"
#include "iPARK_CLA.h"
#include "SVGEN_CLA.h"
#include "RAMP_GEN_CLA.h"
#include "RAMP_CTL_CLA.h"
#include "VOLT_CALC_CLA.h"
#include "SPEED_EST_CLA.h"
#include "PI_CLA.h"
#include "SMO_CONST_CLA.h"
#include "SMOPOS_CLA.h"

#include "PWMDRV_DAC_CLA.h"
#include "PWMDRV_3phInv_CLA.h"

#ifdef __TMS320C28XX__
#define C28x_volatile volatile
#else
#define C28x_volatile
#endif

/*------------------------------------------------------------------------------
Following is the list of the Build Level choices.
------------------------------------------------------------------------------*/
#define LEVEL1  1      		// Module check out (do not connect the motors) 
#define LEVEL2  2           // Verify ADC, park/clarke, calibrate the offset 
#define LEVEL3  3           // Verify closed current(torque) loop and PIDs and speed measurement
#define LEVEL4  4           // Verify speed estimation and rotor position est.
#define LEVEL5  5           // Verify close speed loop sensorless

/*------------------------------------------------------------------------------
This line sets the BUILDLEVEL to one of the available choices.
------------------------------------------------------------------------------*/
#define   DMC_CLA_BUILDLEVEL LEVEL1

extern volatile float SpeedRef;
extern volatile int lsw;
extern volatile float IdRef;
extern volatile float IqRef;
extern volatile float VdTesting;
extern volatile float VqTesting;
extern volatile float EnableFlag;

//Task 2 (C) Variables

//Task 3 (C) Variables

//Task 4 (C) Variables

//Task 5 (C) Variables

//Task 6 (C) Variables

//Task 7 (C) Variables

//Task 8 (C) Variables

//Common (C) Variables

//CLA C Tasks defined in Cla1Tasks_C.cla
interrupt void Cla1Task1();
interrupt void Cla1Task2();
interrupt void Cla1Task3();
interrupt void Cla1Task4();
interrupt void Cla1Task5();
interrupt void Cla1Task6();
interrupt void Cla1Task7();
interrupt void Cla1Task8();
#endif /*CLASHARED_C_H_*/
