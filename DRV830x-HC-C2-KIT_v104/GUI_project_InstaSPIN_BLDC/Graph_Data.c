#include "PeripheralHeaderIncludes.h"
#include "IQmathLib.h"
#include "Graph_Data.h"

#pragma DATA_SECTION(DLOG_4CH_buff1,"DLOG1");
_iq DLOG_4CH_buff1[256];
#pragma DATA_SECTION(DLOG_4CH_buff2,"DLOG2");
_iq DLOG_4CH_buff2[256];
#pragma DATA_SECTION(DLOG_4CH_buff3,"DLOG3");
_iq DLOG_4CH_buff3[256];

//Uint16 continuous = 1;
//Uint16 update_graphs = 1;

void Graph_Data_Init(struct GRAPH_DATA *g)
{
	g->task = WAIT_TRIGGER_LOW;
	g->write_ptr1 = &DLOG_4CH_buff1[0];
	g->write_ptr2 = &DLOG_4CH_buff2[0];
	g->write_ptr3 = &DLOG_4CH_buff3[0];
	
}

void Graph_Data_Update(struct GRAPH_DATA *g)
{
//if(update_graphs)
//{
	switch(g->task)
	{
		//wait for channel 1 to fall below the trigger level
		case WAIT_TRIGGER_LOW:
			if(*g->ch1_ptr < g->trig_value)
			{
				g->task++;
			}
			break;
		//wait for the rising edge
		case WAIT_TRIGGER_HIGH:
			if(*g->ch1_ptr > g->trig_value)
			{
				g->task++;
				g->cntr = 0;
				g->skip_cntr = 0;
			}
			break;
		case FILL_BUFFERS:
			g->skip_cntr++;
			if(g->skip_cntr == g->prescalar)
			{
				g->skip_cntr = 0;
				g->write_ptr1[g->cntr] = *g->ch1_ptr;
				g->write_ptr2[g->cntr] = *g->ch2_ptr;
				g->write_ptr3[g->cntr] = *g->ch3_ptr;
				g->cntr++;
				if(g->cntr == g->size)
				{
					g->task++;
					g->cntr = 0;
					g->skip_cntr = 0;
				}
			}
			break;
		case HOLDOFF:
				g->cntr++;
				if(g->cntr == g->holdoff)
				{
					g->task = WAIT_TRIGGER_LOW;
					g->cntr = 0;
					g->skip_cntr = 0;
//					if(continuous == 0)
//					{
//						update_graphs = 0;
//					}
				}
			break;
	}
//}
}
