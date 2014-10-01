










// *********************
// Full commros:
// *********************













struct LoggerVarStruct
{
	UINT32 address;
	unsigned char size;
};

struct LoggerProbeStruct
{
	IEEE32BITFLOAT sampleTime;
	char* pName;
};

struct DataLogger
{
	INT64 m_triggerValue1;
	INT64 m_triggerValue2;
	INT64 m_trigger2Acutal_old;
	UINT32 m_bufferAddress;
	UINT32 m_bufferLength;
	void (*EnableLoggerUserCallback)(struct DataLogger* s);
	unsigned char* m_currentBufferLocation;
	struct LoggerProbeStruct* m_pProbes;
	struct LoggerVarStruct* m_pLoggerVariables;
	UINT32 m_dataStartAddress;	
	UINT32 m_noSamples;
	UINT32 m_triggerVarAddress_1;
	UINT32 m_triggerVarAddress_2;
	UINT32 m_noLoggedSamples;
	UINT16 m_noPreTriggedSamples;
	UINT16 m_noLoggedPreTriggedSamples;
	UINT16 m_samplerate;
	UINT16 m_samplecnt;
	unsigned char m_bIsTrigged;
	unsigned char m_bIsEnabled;
	unsigned char m_bIsDataAvailable;
	unsigned char m_bIsFirstSample;
	unsigned char m_triggerVarSize_1;
	unsigned char m_triggerVarSize_2;
	unsigned char m_triggerCondition;
	unsigned char m_noVariables;
	unsigned char m_maxNoVariables;
	unsigned char m_selProbe;
	unsigned char m_noProbes;
};

void AddProbes(struct DataLogger *pDataLogger,struct LoggerProbeStruct* m_pProbes,unsigned char number_of_probes);
void AddLoggerVariableBuffers(struct DataLogger *pDataLogger,struct LoggerVarStruct* pVarBuffer,unsigned char size);
void Datalogger(struct DataLogger *pDataLogger,unsigned char probe);
void Init(struct DataLogger *pDataLogger);

unsigned char DecodeCommands(struct DataLogger *pDataLogger,unsigned char* packet,unsigned char* transmitbuffer);
void loggdata(struct DataLogger *pDataLogger);

inline void DataloggerSetBuffer(struct DataLogger *pDataLogger,unsigned char* pBuffer,UINT32 length)
{
	pDataLogger->m_bufferAddress = (UINT32)pBuffer;
	pDataLogger->m_bufferLength = length;
}




enum DecoderStates
{
	CRCPROT_SYNCH,
	CRCPROT_LEN,
	CRCPROT_LSUM,
	CRCPROT_DATA,
	CRCPROT_ESUM,
	CRCPROT_ESUM2,
	CRCPROT_VALID
};

struct ByteProtocol
{
	enum DecoderStates state;
	INT16 pos;
	INT16 len;
	UINT16 CRC;
	UINT16 CRCPackage;
	UINT16 m_tableCrc16[16];
	unsigned char lastff;
	unsigned char buf[128];
};

void ByteProtocol_Init(struct ByteProtocol* pByteProtocol);
void ByteProtocol_Transmit(struct ByteProtocol* pByteProtocol,unsigned char* packet,unsigned char size, void (*TransmitByte)(unsigned char data));
inline unsigned char* ByteProtocol_GetBuffer(struct ByteProtocol* pByteProtocol) { return pByteProtocol->buf;}
inline unsigned char ByteProtocol_GetBufferLength(struct ByteProtocol* pByteProtocol) { return pByteProtocol->len; }
INT16 ByteProtocol_DecodeByte(struct ByteProtocol* pByteProtocol,unsigned char data);




// ******************************************
// struct Commros
//
// ******************************************
struct Commros
{

	struct DataLogger m_datalogger;

	struct ByteProtocol m_bytePacket;

	void (*UserProgStartFunction)();	
	void (*UserProgStopFunction)();		

	void (*TransmitByte)(unsigned char data);
	unsigned char (*DataAvailable)();
	unsigned char (*ReceiveByte)();

	void (*TransmitPacket)(unsigned char* data,unsigned char size);
	unsigned char (*ReceivePacket)(unsigned char** pPacket);

	unsigned char (*UserFunctions)(unsigned char* input,unsigned char* output,unsigned char packetsize);
	void (*RestoreInterrupts)();
	void (*DisableInterrupts)();
	void (*ResetProcessor)();
	void (*UserServiceRoutine)();




	UINT32 m_tableCrc32[16];

	unsigned char m_bIsProgramLoaded;
	unsigned char m_bIsProgramRunning;



	unsigned char m_decodeIndex;
	unsigned char m_inputBufferIndex;
	unsigned char m_inputBuffer[16];
	unsigned char transmitbuffer[128];
};

void InitCore(struct Commros* pCommros);
void ServiceRoutine(struct Commros* pCommros);
void SendResetMessage(struct Commros* pCommros,unsigned char ResetCode);
void DecodePacket(struct Commros* pCommros,unsigned char* packet,unsigned char byteoriented,unsigned char packetsize);
void EncodeAndTransmitData(struct Commros* pCommros,unsigned char* packet,unsigned char size,unsigned char byteoriented);
UINT32 DoCrc32(UINT32* pTableCrc32,unsigned char* startAddress,UINT32 length);

inline void SetByteProtocolCallBacks(struct Commros* pCommros,unsigned char (*dataAvailable)(),void (*transmitByte)(unsigned char data),unsigned char (*receiveByte)())
{
	pCommros->DataAvailable = dataAvailable;
	pCommros->TransmitByte = transmitByte;
	pCommros->ReceiveByte = receiveByte;
}



