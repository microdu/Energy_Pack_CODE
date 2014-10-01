//###########################################################################
// Description:
//
// This example calls the sine per unit function(CLAsinPU) of the CLA
// math library on a test vector and compares the result to the
// equivalent "math.h" routine
//
//###########################################################################
// $TI Release: CLA Math Library for CLA C Compiler V4.00 $
// $Release Date: December 10, 2011 $
//###########################################################################

#include "DSP28x_Project.h"
#include "CLAShared.h"
#include "math.h"

#define CLA1_RAM0_ENABLE	0
#define CLA1_RAM1_ENABLE	1

#define EPSILON				1e-2

// Constants
#define WAITSTEP	asm(" RPT #255||NOP")

//Global Variables
//Task 1 (C) Variables
float y[BUFFER_SIZE];
#pragma DATA_SECTION(fAngle,"CpuToCla1MsgRAM")
float fAngle;
#pragma DATA_SECTION(fResult,"Cla1ToCpuMsgRAM")
float fResult;

//Golden Test Values
float test_values[BUFFER_SIZE] = {
-6.283185, -6.184237, -6.085289, -5.986342, -5.887394, 
-5.788446, -5.689498, -5.590551, -5.491603, -5.392655, 
-5.293707, -5.194759, -5.095812, -4.996864, -4.897916, 
-4.798968, -4.700020, -4.601073, -4.502125, -4.403177, 
-4.304229, -4.205282, -4.106334, -4.007386, -3.908438, 
-3.809490, -3.710542, -3.611595, -3.512647, -3.413699, 
-3.314751, -3.215803, -3.116856, -3.017908, -2.918960, 
-2.820012, -2.721065, -2.622117, -2.523169, -2.424221, 
-2.325273, -2.226326, -2.127378, -2.028430, -1.929482, 
-1.830534, -1.731586, -1.632639, -1.533691, -1.434743, 
-1.335795, -1.236848, -1.137900, -1.038952, -0.9400041, 
-0.8410563, -0.7421085, -0.6431607, -0.5442129, -0.4452651, 
-0.3463173, -0.2473695, -0.1484217, -0.04947390, 0.04947390, 
0.1484217, 0.2473695, 0.3463173, 0.4452651, 0.5442129, 
0.6431607, 0.7421085, 0.8410563, 0.9400041, 1.038952, 
1.137900, 1.236848, 1.335795, 1.434743, 1.533691, 
1.632639, 1.731586, 1.830534, 1.929482, 2.028430, 
2.127378, 2.226326, 2.325273, 2.424221, 2.523169, 
2.622117, 2.721065, 2.820012, 2.918960, 3.017908, 
3.116856, 3.215803, 3.314751, 3.413699, 3.512647, 
3.611595, 3.710542, 3.809490, 3.908438, 4.007386, 
4.106334, 4.205282, 4.304229, 4.403177, 4.502125, 
4.601073, 4.700020, 4.798968, 4.897916, 4.996864, 
5.095812, 5.194759, 5.293707, 5.392655, 5.491603, 
5.590551, 5.689498, 5.788446, 5.887394, 5.986342, 
6.085289, 6.184237,	6.283185
};

float res_expected[BUFFER_SIZE]={
2.449294e-16, 0.09878642, 0.1966064, 0.2925031, 0.3855383, 
0.4748020, 0.5594208, 0.6385669, 0.7114661, 0.7774054, 
0.8357394, 0.8858978, 0.9273896, 0.9598091, 0.9828392, 
0.9962543, 0.9999235, 0.9938107, 0.9779758, 0.9525737, 
0.9178528, 0.8741529, 0.8219014, 0.7616096, 0.6938671, 
0.6193367, 0.5387476, 0.4528881, 0.3625982, 0.2687610, 
0.1722946, 0.07414275, -0.02473443, -0.1233696, -0.2207980, 
-0.3160663, -0.4082426, -0.4964253, -0.5797516, -0.6574063, 
-0.7286299, -0.7927254, -0.8490661, -0.8971006, -0.9363590, 
-0.9664573, -0.9871011, -0.9980884, -0.9993117, -0.9907590, 
-0.9725141, -0.9447554, -0.9077544, -0.8618732, -0.8075605, 
-0.7453477, -0.6758435, -0.5997276, -0.5177448, -0.4306971, 
-0.3394361, -0.2448544, -0.1478774, -0.04945372, 0.04945372, 
0.1478774, 0.2448544, 0.3394361, 0.4306971, 0.5177448, 
0.5997276, 0.6758435, 0.7453477, 0.8075605, 0.8618732, 
0.9077544, 0.9447554, 0.9725141, 0.9907590, 0.9993117, 
0.9980884, 0.9871011, 0.9664573, 0.9363590, 0.8971006, 
0.8490661, 0.7927254, 0.7286299, 0.6574063, 0.5797516, 
0.4964253, 0.4082426, 0.3160663, 0.2207980, 0.1233696, 
0.02473443, -0.07414275, -0.1722946, -0.2687610, -0.3625982, 
-0.4528881, -0.5387476, -0.6193367, -0.6938671, -0.7616096, 
-0.8219014, -0.8741529, -0.9178528, -0.9525737, -0.9779758, 
-0.9938107, -0.9999235, -0.9962543, -0.9828392, -0.9598091, 
-0.9273896, -0.8858978, -0.8357394, -0.7774054, -0.7114661, 
-0.6385669, -0.5594208, -0.4748020, -0.3855383, -0.2925031, 
-0.1966064, -0.09878642, -2.449294e-16
};

//Linker defined vars
extern Uint16 Cla1Prog_Start;

//CLA ISRs
interrupt void cla1_task1_isr( void );


//! Main Function
void main(void)
{
   //! Step 1: Setup the system clock
   /*! Disable the watchdog timer, initialize the system clock,
    *  PLL and configure the peripheral clock.
    */
	InitSysCtrl();

   //! Step 2: Initialize PIE control
   /*! Intialize PIE control, disable all interrupts and
    * then copy over the PIE Vector table from BootROM to RAM
    */
   DINT;
   InitPieCtrl();
   IER = 0x00000000;
   IFR = 0x00000000;
   InitPieVectTable();
	
   /*! Assign user defined ISR to the PIE vector table */
	EALLOW;
    PieVectTable.CLA1_INT1  = &cla1_task1_isr;
	EDIS;

    /*! Compute all CLA task vectors */ 
    EALLOW;
    Cla1Regs.MVECT1 = ((Uint16)Cla1Task1 - (Uint16)&Cla1Prog_Start);
    EDIS;
   
    //! Step 3 : Mapping CLA tasks
    /*! All tasks are enabled and will be started by an ePWM trigger
     *  Map CLA program memory to the CLA and enable software breakpoints
     */
    EALLOW;
	Cla1Regs.MPISRCSEL1.bit.PERINT1SEL 	= CLA_INT1_NONE;
	Cla1Regs.MIER.all 		 		    = 0x00FF;
	EDIS;	
    
 
    /*! Enable CLA interrupts at the group and subgroup levels */
    PieCtrlRegs.PIEIER11.all       = 0xFFFF;
    IER = (M_INT11 );
    EINT;   // Enable Global interrupt INTM
    ERTM;   // Enable Global realtime interrupt DBGM

    /*!Switch the CLA program space to the CLA and enable software forcing
     * Also switch over CLA data ram 0 and 1
     */
    EALLOW;
    Cla1Regs.MMEMCFG.bit.PROGE 	= 1;
    Cla1Regs.MCTL.bit.IACKE	= 1;
    Cla1Regs.MMEMCFG.bit.RAM0E	= CLA1_RAM0_ENABLE;
    Cla1Regs.MMEMCFG.bit.RAM1E	= CLA1_RAM1_ENABLE;
    EDIS;	

    // Invoke Task(s)
    test_run();

    // Report Results
    test_report();

	//Forever loop
	while(1){
	 asm(" NOP");
	}

}

//C28 C tasks
void test_run(void)
{
	int i;
	for(i=0;i<BUFFER_SIZE;i++)
	{
	 fAngle = test_values[i]/(TWO_PI); //Angle per unit circle i.e per 2PI radians
	 Cla1ForceTask1andWait();
     WAITSTEP;
	 y[i] = fResult; 
	}
}

void test_report(void)
{
	unsigned int i;
	unsigned int pass_count=0;
	unsigned int fail_count=0;
	float fError[BUFFER_SIZE];
	float fErrMetric;
	for(i=0;i<BUFFER_SIZE;i++){
	  fError[i] = fabs(res_expected[i]-y[i]);
	  fErrMetric = fError[i];
	  if( fErrMetric < EPSILON){
		pass_count++;
	  }else{
		fail_count++;
	  }
	}
	if(fail_count) test_error(); 
}

void test_error( void )
{
	asm(" ESTOP0");	
}



//CLA ISR's
interrupt void cla1_task1_isr( void)
{
        PieCtrlRegs.PIEACK.bit.ACK11 = 1;
}

