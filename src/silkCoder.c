#include "silkCoder.h"
#include "SKP_Silk_SDK_API.h"
#include "SKP_Silk_SigProc_FIX.h"

static SKP_int32 rand_seed = 1;

/*
0: Success
1: Out of memory
2: Invalid argument
*/
DLLEXPORT int STDCALL silkCoder_encodeToSilk(char* source, int sourceSize, unsigned int tencent, int sourceRate, int maxInternalSampleRate, int packetSize, int packetLossPercentage, int useInBandFEC, int useDTX, int complexity, int bitRate, char** destination, int* destinationSize)
{
    int counter;
    short nBytes;
    unsigned char payload[MAX_BYTES_PER_FRAME * MAX_INPUT_FRAMES];
    short in[FRAME_LENGTH_MS * MAX_API_FS_KHZ * MAX_INPUT_FRAMES];
    int encSizeBytes;
    void* psEnc = NULL;
    int samplesSinceLastPacket, expectedPacketSize;
    const int frameSizeReadFromFile_ms = 20;
    SKP_SILK_SDK_EncControlStruct encControl;
    SKP_SILK_SDK_EncControlStruct encStatus;
    int32_t ret = 0;
    coding_buffer buffer;

    if (coding_buffer_initialize(&buffer, 32 * 1024))
    {
        ret = 1;
        goto fail;
    }
    if (sourceRate > MAX_API_FS_KHZ * 1000 || sourceRate < 0)
    {
        ret = 2;
        goto fail;
    }
    SKP_Silk_SDK_Get_Encoder_Size(&encSizeBytes);
    psEnc = malloc(encSizeBytes);
    if (!psEnc)
    {
        ret = 1;
        goto fail;
    }
    coding_buffer_ensuredappend(&buffer, (!tencent) + "#!SILK_V3", 9 + tencent);
    SKP_Silk_SDK_InitEncoder(psEnc, &encStatus);
    encControl.API_sampleRate = sourceRate;
    encControl.maxInternalSampleRate = maxInternalSampleRate;
    encControl.packetSize = packetSize;
    encControl.packetLossPercentage = packetLossPercentage;
    encControl.useInBandFEC = useInBandFEC;
    encControl.useDTX = useDTX;
    encControl.complexity = complexity;
    encControl.bitRate = bitRate & -!!bitRate; // (bitRate > 0 ? bitRate : 0);
    samplesSinceLastPacket = 0;

    while (1) 
    {
        counter = nBytes = (frameSizeReadFromFile_ms * (size_t)sourceRate) / 1000;
        nBytes *= sizeof(SKP_int16);
        if ((sourceSize -= nBytes) < 0) 
        {
            break;
        } 
        memcpy(in, source, nBytes);
        source += nBytes;
#ifdef _SYSTEM_IS_BIG_ENDIAN
        swap_endian(in, nBytes);
#endif
        nBytes = MAX_BYTES_PER_FRAME * MAX_INPUT_FRAMES;
        SKP_Silk_SDK_Encode(psEnc, &encControl, in, counter, payload, &nBytes);
        expectedPacketSize = (1000 * encControl.packetSize) / encControl.API_sampleRate;
        samplesSinceLastPacket += counter;
        if (((1000 * samplesSinceLastPacket) / sourceRate) == expectedPacketSize)
        {
            if (coding_buffer_ensure_capacity(&buffer, buffer.length + nBytes + (!tencent) * 2, 2))
            {
                ret = 1;
                goto fail;
            }
#ifdef _SYSTEM_IS_BIG_ENDIAN
            *(short*)((char*)buffer.ptr + buffer.length) = swap_short_endian(nBytes);
#else
            *(short*)((char*)buffer.ptr + buffer.length) = nBytes;
#endif
            buffer.length += sizeof(short);
            coding_buffer_ensuredappend(&buffer, payload, nBytes);
            samplesSinceLastPacket = 0;
        }
    }
    if (!tencent)
    {
#ifdef _SYSTEM_IS_BIG_ENDIAN
        *(short*)((char*)buffer.ptr + buffer.length) = swap_short_endian(nBytes);
#else
        *(short*)((char*)buffer.ptr + buffer.length) = nBytes;
#endif
        buffer.length += sizeof(short);
    }
    if (0)
    {
        fail:
        coding_buffer_free(&buffer);
    }
    if (psEnc)
    {
        free(psEnc);
    }
    *destination = buffer.ptr;
    *destinationSize = buffer.length;
    return ret;
}

/*
0: Success
1: Out of memory
2: Invalid data
*/
DLLEXPORT int STDCALL silkCoder_decodeToPcm(char* source, int sourceSize, int fs_hz, float loss, char** destination, int* destinationSize)
{
    const int payloadSize = MAX_BYTES_PER_FRAME * MAX_INPUT_FRAMES * (MAX_LBRR_DELAY + 1);
    const int outSize = sizeof(SKP_int16) * ((FRAME_LENGTH_MS * MAX_API_FS_KHZ) << 1) * MAX_INPUT_FRAMES;
    int i;
    short nBytes;
    char* payload = NULL, * payloadPtr, * payloadToDecode;
    short fecPayload[MAX_BYTES_PER_FRAME * MAX_INPUT_FRAMES];
    short nBytesPerPacket[MAX_LBRR_DELAY + 1];
    short* out = NULL;
    int decSizeBytes;
    void* psDec = NULL;
    int lost;
    SKP_SILK_SDK_DecControlStruct decControl;
    int ret = 0;
    coding_buffer buffer;
    buffer.ptr = NULL;

    if ((sourceSize -= 9) < 0)
    {
        ret = 2;
        goto fail;
    }
    if (!memcmp(source, "#!SILK_V3", 9))
    {
        source += 9;
    }
    else if (--sourceSize >= 0 && !memcmp(source, "#!SILK_V3", 10))
    {
        source += 10;
    }
    else
    {
        ret = 2;
        goto fail;
    }
    if (coding_buffer_initialize(&buffer, 512 * 1024))
    {
        ret = 1;
        goto fail;
    }

    decControl.API_sampleRate = fs_hz + (24000 & -(fs_hz == 0));
    decControl.framesPerPacket = 1;
    SKP_Silk_SDK_Get_Decoder_Size(&decSizeBytes);
    psDec = malloc(decSizeBytes);
    if (!psDec)
    {
        ret = 1;
        goto fail;
    }
    SKP_Silk_SDK_InitDecoder(psDec);
    payload = (char*)malloc(payloadSize);
    if (!payload)
    {
        ret = 1;
        goto fail;
    }
    payloadPtr = payload;
    for (i = 0; i < MAX_LBRR_DELAY; i++)
    {
        if ((sourceSize -= 2) < 0)
        {
            ret = 2;
            goto fail;
        }
        nBytes = *(short*)source;
        source += 2;
#ifdef _SYSTEM_IS_BIG_ENDIAN
        nBytes = swap_short_endian(nBytes);
#endif
        if ((sourceSize -= nBytes) < 0)
        {
            ret = 2;
            goto fail;
        }
        memcpy(payloadPtr, source, nBytes);
        source += nBytes;
        nBytesPerPacket[i] = nBytes;
        payloadPtr += nBytes;
    }
    out = (short*)malloc(outSize);
    if (!out)
    {
        ret = 1;
        goto fail;
    }
    while (1)
    {
        if ((sourceSize -= 2) < 0)
        {
            break;
        }
        nBytes = *(short*)source;
        source += 2;
#ifdef _SYSTEM_IS_BIG_ENDIAN
        nBytes = swap_short_endian(nBytes);
#endif
        if (nBytes < 0)
        {
            ret = 2;
            goto fail;
        }
        if ((sourceSize -= nBytes) < 0)
        {
            ret = 2;
            goto fail;
        }
        memcpy(payloadPtr, source, nBytes);
        source += nBytes;
        rand_seed = SKP_RAND(rand_seed);
        if ((((float)((rand_seed >> 16) + (1 << 15))) / 65535.0f >= (loss / 100.0f)))
        {
            nBytesPerPacket[MAX_LBRR_DELAY] = nBytes;
            payloadPtr += nBytes;
        }
        else
        {
            nBytesPerPacket[MAX_LBRR_DELAY] = 0;
        }
        lost = DetermineLost(&nBytesPerPacket, payload, &fecPayload, &nBytes, &payloadToDecode);
        ret = InternalDecode(lost, psDec, &decControl, &nBytesPerPacket, out, payloadToDecode, payload, &payloadPtr, nBytes, &buffer);
    }
    for (i = 0; i < MAX_LBRR_DELAY; i++)
    {
        lost = DetermineLost(&nBytesPerPacket, payload, &fecPayload, &nBytes, &payloadToDecode);
        ret = InternalDecode(lost, psDec, &decControl, &nBytesPerPacket, out, payloadToDecode, payload, &payloadPtr, nBytes, &buffer);
    }
    if (0)
    {
    fail:
        coding_buffer_free(&buffer);
    }
    if (psDec)
    {
        free(psDec);
    }
    if (payload)
    {
        free(payload);
    }
    if (out)
    {
        free(out);
    }
    *destination = buffer.ptr;
    *destinationSize = buffer.length;
    return ret;
}

static int DetermineLost(const short* nBytesPerPacket, char* payload, unsigned char* FECpayload, short* nBytes, char** payloadToDec)
{
    int i, nBytesFEC;
    if (*nBytesPerPacket == 0)
    {
        for (i = 1; i <= MAX_LBRR_DELAY; i++)
        {
            if (nBytesPerPacket[i] > 0)
            {
                SKP_Silk_SDK_search_for_LBRR(payload, nBytesPerPacket[i], i, FECpayload, &nBytesFEC);
                if (nBytesFEC > 0)
                {
                    *payloadToDec = FECpayload;
                    *nBytes = nBytesFEC;
                    return 0;
                }
            }
            payload += nBytesPerPacket[i];
        }
        return 1;
    }
    *nBytes = nBytesPerPacket[0];
    *payloadToDec = payload;
    return 0;
}

static int InternalDecode(int lost, void* psDec, SKP_SILK_SDK_DecControlStruct* decControl, const short* nBytesPerPacket,
    short* out, char* payloadToDec, char* payload, char** payloadEnd, short nBytes, coding_buffer* buffer)
{
    const int payloadSize = MAX_BYTES_PER_FRAME * MAX_INPUT_FRAMES * (MAX_LBRR_DELAY + 1);
    /* Silk decoder */
    short length;
    int totalLength = 0, totalBytes;
    int i, frames;
    char* outptr = out;
    if (!lost)
    {
        /* No Loss: Decode all frames in the packet */
        frames = 0;
        do
        {
            /* Decode 20 ms */
            if (SKP_Silk_SDK_Decode(psDec, decControl, 0, payloadToDec, nBytes, out, &length))
            {
                return 2;
            }
            frames++;
            outptr += length;
            totalLength += length;
            if (frames > MAX_INPUT_FRAMES)
            {
                outptr = out;
                totalLength = 0;
                frames = 0;
            }
        } while (decControl->moreInternalDecoderFrames);
    }
    else
    {
        /* Loss: Decode enough frames to cover one packet duration */
        for (i = 0; i < decControl->framesPerPacket; i++)
        {
            /* Generate 20 ms */
            if (SKP_Silk_SDK_Decode(psDec, decControl, 1, payloadToDec, nBytes, outptr, &length))
            {
                return 2;
            }
            outptr += length;
            totalLength += length;
        }
    }

    /* Write output to file */
#ifdef _SYSTEM_IS_BIG_ENDIAN
    swap_endian(out, totalLength);
#endif
    totalLength *= sizeof(short);
    if (coding_buffer_append(buffer, out, totalLength))
    {
        return 1;
    }

    /* Update buffer */
    totalBytes = 0;
    for (i = 1; i <= MAX_LBRR_DELAY; i++)
    {
        totalBytes += nBytesPerPacket[i];
    }
    /* Check if the received totBytes is valid */
    if (totalBytes < 0 || totalBytes > payloadSize)
    {
        return 2;
    }
    memmove(payload, payload + nBytesPerPacket[0], totalBytes * sizeof(char));
    *payloadEnd -= nBytesPerPacket[0];
    memmove(nBytesPerPacket, &nBytesPerPacket[1], MAX_LBRR_DELAY * sizeof(short));
    return 0;
}
