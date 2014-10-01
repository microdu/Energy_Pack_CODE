#ifndef GRAPH_DATA_H_
#define GRAPH_DATA_H_

#define WAIT_TRIGGER_LOW	0
#define WAIT_TRIGGER_HIGH	1
#define FILL_BUFFERS		2
#define HOLDOFF				3

struct GRAPH_DATA	{
	_iq *ch1_ptr;
	_iq *ch2_ptr;
	_iq *ch3_ptr;
	_iq *write_ptr1;
	_iq *write_ptr2;
	_iq *write_ptr3;
	_iq trig_value;
	Uint16 holdoff;
	Uint16 cntr;
	Uint16 size;
	Uint16 prescalar;
	Uint16 skip_cntr;
	Uint16 task;
};

void Graph_Data_Init(struct GRAPH_DATA *g);
void Graph_Data_Update(struct GRAPH_DATA *g);

extern _iq DLOG_4CH_buff1[256];
extern _iq DLOG_4CH_buff2[256];
extern _iq DLOG_4CH_buff3[256];

#endif /*GRAPH_DATA_H_*/
