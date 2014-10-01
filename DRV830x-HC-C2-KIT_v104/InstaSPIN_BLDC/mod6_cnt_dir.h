/* =================================================================================
File name:        MOD6_CNT_DIR.H  (IQ version)                  
                    
Originator:	Digital Control Systems Group
			Texas Instruments

Description: 
Header file containing constants, data type definitions, and 
macro definitions for the MOD6CNT module.
=====================================================================================
 History:
-------------------------------------------------------------------------------------
 06-08-2011	Version 1.0                                             
------------------------------------------------------------------------------*/
#ifndef __MOD6_CNT_DIR_H__
#define __MOD6_CNT_DIR_H__

typedef struct { Uint32  TrigInput;   	// Input: Modulo 6 counter trigger input - Q0 (0x00000000 or 0x00007FFF)
				 _iq	 CntDirection;	// Count UP >= 0 or DOWN < 0
				 Uint32  Counter;	    // Output: Modulo 6 counter output - Q0 (0,1,2,3,4,5)			
		  	  	 
				 } MOD6CNTDIR;	            

typedef MOD6CNTDIR *MOD6CNTDIR_handle;
/*-----------------------------------------------------------------------------
Default initalizer for the MOD6CNT object.
-----------------------------------------------------------------------------*/                     
#define MOD6CNTDIR_DEFAULTS { 0, \
						   _IQ(0.0), \
						   0 }

/*------------------------------------------------------------------------------
	MOD6_CNT Macro Definition
------------------------------------------------------------------------------*/


#define MOD6CNTDIR_MACRO(v)												\
																		\
	if (v.TrigInput > 0)												\
	{																	\
		if (v.CntDirection >= 0)										\
		{																\
			if(v.Counter == 5)											\
			{															\
				v.Counter = 0;											\
			}															\
			else														\
			{															\
				v.Counter++;											\
			}															\
		}																\
		else															\
		{																\
			if(v.Counter == 0)											\
			{															\
				v.Counter = 5;											\
			}															\
			else														\
			{															\
				v.Counter--;											\
			}															\
		}																\
	}

#endif // __MOD_6CNT_DIR_H__ 
