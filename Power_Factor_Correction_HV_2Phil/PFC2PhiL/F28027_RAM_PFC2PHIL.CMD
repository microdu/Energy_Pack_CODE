/*==================================================================================*/
/*	User specific Linker command file for running from RAM							*/
/*==================================================================================*/
/*	FILE:			F2806_RAM_PFC2PHIL.CMD                                              */
/*                                                                                  */
/*	Description:	Linker command file for User custom sections targetted to run   */
/*					from RAM.  			                                            */
/*                                                                                  */
/*  Target:  		TMS320F2806 or larger mem device                                */
/*                                                                                  */
/*	Version: 		1.00                                							*/
/*                                                                                  */
/*----------------------------------------------------------------------------------*/
/*  Copyright Texas Instruments © 2008                                			    */	
/*----------------------------------------------------------------------------------*/
/*  Revision History:                                                               */
/*----------------------------------------------------------------------------------*/
/*  Date	  | Description                                                         */
/*----------------------------------------------------------------------------------*/
/*  03/14/08  | Release 1.0  		New release.                                    */
/*----------------------------------------------------------------------------------*/

MEMORY
{
PAGE 0 :
   
	BEGIN		: origin = 0x000000,	length = 0x000002             
	RAMM0		: origin = 0x000002,	length = 0x0003FE
/*	progRAM		: origin = 0x008400,	length = 0x000c00*/
	progRAM		: origin = 0x0083B0,	length = 0x000C50

	RESET      	: origin = 0x3FFFC0, length = 0x000002
	BOOTROM    	: origin = 0x3FF000, length = 0x000FC0               

         
PAGE 1 : 

	RAMM1		: origin = 0x000400,	length = 0x000400
	dataRAM		: origin = 0x008000,	length = 0x0003B0	
}

 
SECTIONS
{

   codestart        : > BEGIN,      PAGE = 0
 /*  ramfuncs         : > progRAM       PAGE = 0  */
 	ramfuncs        : > RAMM0,       PAGE = 0
   .text            : > progRAM,     PAGE = 0

   .cinit           : > RAMM0,      PAGE = 0
   .pinit           : > RAMM0,      PAGE = 0
   .switch          : > RAMM0,      PAGE = 0
   .reset           : > RESET,      PAGE = 0, TYPE = DSECT
   
   .stack           : > RAMM1,      PAGE = 1

   .ebss            : > dataRAM,     PAGE = 1	
	
/*   .econst          : > dataRAM,     PAGE = 1 */     
   .econst          : > RAMM1,     PAGE = 1
   .esysmem         : > RAMM1,      PAGE = 1

   IQmath           : >  progRAM,   PAGE = 0
   IQmathTables     : >  BOOTROM, type = NOLOAD, PAGE = 0

   DLOG	 			: >	dataRAM,    PAGE = 1
}

    
  
SECTIONS
{

	ISRVariables					: > dataRAM				PAGE = 1
	/*************       DPLIB Sections C28x      ************************/
	/* ADCDRV_1ch section */
	ADCDRV_1ch_Section				: > dataRAM				PAGE = 1

	/* ADCDRV_4ch section */
	ADCDRV_4ch_Section				: > dataRAM				PAGE = 1
	
	/* CNTL_2P2Z section */
	CNTL_2P2Z_Section				: > dataRAM				PAGE = 1
	CNTL_2P2Z_InternalData			: > dataRAM				PAGE = 1
	CNTL_2P2Z_Coef					: > dataRAM				PAGE = 1
	
	/*DLOG_4CH section */
	DLOG_4ch_Section				: > dataRAM				PAGE = 1
	DLOG_BUFF						: > dataRAM				PAGE = 1
	
	/*MATH_EMAVG section */
	MATH_EMAVG_Section				: > dataRAM				PAGE = 1
	
	/*PFC_ICMD section*/
	PFC_ICMD_Section				: > dataRAM				PAGE = 1
	
	/*PFC_INVSQR section*/
	PFC_INVSQR_Section				: > dataRAM				PAGE = 1
	
	/* PWMDRV_1ch driver section */
	PWMDRV_1ch_Section				: > dataRAM				PAGE = 1
	
	/* PWMDRV_1chHiRes driver section */
	PWMDRV_1chHiRes_Section				: > dataRAM				PAGE = 1
	
	/* PWMDRV_PFC2PhiL driver section */
	PWMDRV_PFC2PhiL_Section			: > dataRAM				PAGE = 1
	
 	/* PWMDRV_PSFB driver section */
	PWMDRV_PSFB_Section				: > dataRAM				PAGE = 1
	
	/* ZeroNet_Section  */
	ZeroNet_Section					: > dataRAM				PAGE = 1
	

}
