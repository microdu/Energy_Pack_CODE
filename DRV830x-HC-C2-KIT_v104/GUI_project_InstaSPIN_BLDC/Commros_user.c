#include "PeripheralHeaderIncludes.h"
//#include "DSP280x_Device.h"     // DSP280x Headerfile Include File
//#include "DSP280x_Examples.h"   // DSP280x Examples Include File

// ***********************************
//          Commros header files
// ***********************************
#include "Commros_user.h"

struct Commros commros;

// ******************************************************
// bool SciDataAvailable()
//
//  This function is used to check if there is any data
//  in the serial port receive fifo.
//
// Return values:
//  true : There is one or more bytes in the fifo buffer
//  false: The fifo buffer is empty
// ******************************************************
unsigned char SCIDataAvailable()
{
	return ((SciaRegs.SCIFFRX.all & 0x1F00)!=0);
}


// ******************************************************
// unsigned char SCIReceiveByte()
//
//  This function is used to receive a single byte from
//  serial ports receive fifo buffer.
//
// Return values:
//  The first (oldest) byte in the fifo buffer
// ******************************************************
unsigned char SCIReceiveByte()
{
	return SciaRegs.SCIRXBUF.all;
}


// ******************************************************
// void SCITransmitByte(unsigned char data)
//
//  This function is used to transmit a single byte on
//  the serial port.
//
// Parameters:
//  data: The byte to transmit.
// ******************************************************
void SCITransmitByte(unsigned char data)
{
	//1. If the fifo buffer is full we should wait
	while((SciaRegs.SCIFFTX.all &0x1F00)>0x0300) ;
			//kickdog();

	//2. Write the data byte to the fifo buffer
	SciaRegs.SCITXBUF=data;
}

void InitSciaGpio()
{
   EALLOW;

/* Enable internal pull-up for the selected pins */
// Pull-ups can be enabled or disabled disabled by the user.
// This will enable the pullups for the specified pins.

	GpioCtrlRegs.GPAPUD.bit.GPIO28 = 0;    // Enable pull-up for GPIO28 (SCIRXDA)
//	GpioCtrlRegs.GPAPUD.bit.GPIO7 = 0;     // Enable pull-up for GPIO7  (SCIRXDA)

	GpioCtrlRegs.GPAPUD.bit.GPIO29 = 0;	   // Enable pull-up for GPIO29 (SCITXDA)
//	GpioCtrlRegs.GPAPUD.bit.GPIO12 = 0;	   // Enable pull-up for GPIO12 (SCITXDA)

/* Set qualification for selected pins to asynch only */
// Inputs are synchronized to SYSCLKOUT by default.
// This will select asynch (no qualification) for the selected pins.

	GpioCtrlRegs.GPAQSEL2.bit.GPIO28 = 3;  // Asynch input GPIO28 (SCIRXDA)
//	GpioCtrlRegs.GPAQSEL1.bit.GPIO7 = 3;   // Asynch input GPIO7 (SCIRXDA)

/* Configure SCI-A pins using GPIO regs*/
// This specifies which of the possible GPIO pins will be SCI functional pins.

	GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 1;   // Configure GPIO28 for SCIRXDA operation
//	GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 2;    // Configure GPIO7  for SCIRXDA operation

	GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 1;   // Configure GPIO29 for SCITXDA operation
//	GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 2;   // Configure GPIO12 for SCITXDA operation

    EDIS;
}

void SetupSerialPort()
{

	// *** Calculate the BRR ***
	//
	//          CPUCLK>>LOW_SPEED_CLOCK_DIVIDER
	//	BRR = ----------------------------------  - 1
	//                    8*BAUDRATE


#ifdef DSP2803x_DEVICE_H
	UINT32 BRR = (15000000UL)/(115200<<3)-1;		//Low speed clock = 15 MHz, baudrate = 115200 baud
#endif
#ifdef F2806x_DEVICE_H
	UINT32 BRR = (20000000UL)/(115200<<3)-1;		//Low speed clock = 20 MHz, baudrate = 115200 baud
#endif


	InitSciaGpio();

	SciaRegs.SCICTL1.all=0x0003;	// Reset SCI
	SciaRegs.SCIFFCT.all=0x0000;
	SciaRegs.SCIFFTX.all=0xE000;	// Enable transmit FIFO
	SciaRegs.SCIFFRX.all=0x2000;	// Enable receive FIFO

	SciaRegs.SCICTL2.all=0x0000;

	// Set the baud rate
	SciaRegs.SCIHBAUD=(UINT16)(BRR>>8) & 0xFF;
	SciaRegs.SCILBAUD=(UINT16)BRR & 0xFF;

	SciaRegs.SCIPRI.all=0x0018;
	SciaRegs.SCICCR.all=0x0007;		// 8 bit character length, No parity, 1 stop bit
	SciaRegs.SCICTL1.all=0x0023;	// Enable the SCI
}

// *****************************************************
// InitCommros()
//
//  This function is used to initalize commros and the
//  neccessary communication hardware.
// *****************************************************
void InitCommros()
{
	//Initalize commros and reset all internal variables
	InitCore(&commros);

	//Map serial port functions to commros
	SetByteProtocolCallBacks(&commros,&SCIDataAvailable,&SCITransmitByte,&SCIReceiveByte);

	//Initialize the serial port so it is ready for receiving and transmitting data
	SetupSerialPort();

	//Inform anyone listening on the serial port that the we are awake
	SendResetMessage(&commros,0);
}
