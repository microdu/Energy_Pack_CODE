#ifndef COMMROS_USER_H_INCLUDED
#define COMMROS_USER_H_INCLUDED

#include "ProcessorDependent.h"
#include "commros_full.h"


extern struct Commros commros;

unsigned char SCIDataAvailable();
unsigned char SCIReceiveByte();
void SCITransmitByte(unsigned char data);
void SetupSerialPort();
void InitCommros();


#endif//\#ifndef COMMROS_USER_H_INCLUDED

