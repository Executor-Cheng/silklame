#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "endian.h"
#include "dllexport.h"
#include "SKP_Silk_SDK_API.h"
#include "SKP_Silk_SigProc_FIX.h"

DLLEXPORT int STDCALL silkCoder_encodeToSilk(char* source, int sourceSize, unsigned int tencent, int fs_Hz, int maxInternalSampleRate, int packetSize, int packetLossPercentage, int useInBandFEC, int useDTX, int complexity, int bitRate, char** destination, int* destinationSize);

DLLEXPORT int STDCALL silkCoder_decodeToPcm(char* source, int sourceSize, int fs_hz, float loss, char** destination, int* destinationSize);

static int DetermineLost(const short* nBytesPerPacket, char* payload, unsigned char* FECpayload, short* nBytes, char** payloadToDec);

static int InternalDecode(int lost, void* psDec, SKP_SILK_SDK_DecControlStruct* decControl, const short* nBytesPerPacket, short* out, char* payloadToDec, char* payload, char** payloadEnd, short nBytes, char** output, int* outputSize);
