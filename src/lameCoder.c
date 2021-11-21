#include <string.h>
#include <stdint.h>
#include "lameCoder.h"

hip_t hip;

DLLEXPORT lame_global_flags* STDCALL lameCoder_initializeDefaultFlag()
{
    lame_global_flags* glf = lame_init();
    lame_init_params(glf);
    return glf;
}

DLLEXPORT lame_global_flags* STDCALL lameCoder_createFlag()
{
    lame_global_flags* glf = lame_init();
    return glf;
}

DLLEXPORT void STDCALL lameCoder_initializeFlag(lame_global_flags* glf)
{
    lame_init_params(glf);
}

DLLEXPORT void STDCALL lameCoder_closeFlag(lame_global_flags* glf)
{
    lame_close(glf);
}

DLLEXPORT int STDCALL lameCoder_encodeToMp3(lame_global_flags* glf, char* source, int sourceSize, char** destination, int* destinationSize)
{
    const int sampleLimit = 1024;
    const int readLimit = sampleLimit * 2;
    unsigned char* encodeBuffer;
    const int encodeBufferSize = readLimit + readLimit * 5 / 4 + 7200;
    char* buffer = NULL;
    int bufferSize = 0;
    char* reallocBuffer;
    int nb_read;
    int nb_write;
    int ret = 0;

    encodeBuffer = (unsigned char*)malloc(encodeBufferSize);
    if (!encodeBuffer)
    {
        ret = 1;
        goto fail;
    }
    while (sourceSize)
    {
        nb_read = readLimit ^ ((sourceSize ^ readLimit) & -(sourceSize < readLimit));
        sourceSize -= nb_read;
        nb_write = lame_encode_buffer(glf, source, source, sampleLimit, encodeBuffer, encodeBufferSize);
        source += nb_read;
        if (nb_write == 0)
        {
            continue;
        }
        if (nb_write < 0)
        {
            ret = -nb_write - (nb_write == -2);
            goto fail;
        }
        reallocBuffer = (char*)realloc(buffer, bufferSize + nb_write);
        if (!reallocBuffer)
        {
            ret = 1;
            goto fail;
        }
        buffer = reallocBuffer;
        memcpy(buffer + bufferSize, encodeBuffer, nb_write);
        bufferSize += nb_write;
    }

    nb_write = lame_encode_flush(glf, encodeBuffer, encodeBufferSize);
    if (nb_write)
    {
        reallocBuffer = (char*)realloc(buffer, bufferSize + nb_write);
        if (!reallocBuffer)
        {
            ret = 1;
            goto fail;
        }
        buffer = reallocBuffer;
        memcpy(buffer + bufferSize, encodeBuffer, nb_write);
        bufferSize += nb_write;
    }
    
    if (0)
    {
        fail:
        if (buffer)
        {
            free(buffer);
            buffer = NULL;
        }
        bufferSize = 0;
    }
    if (encodeBuffer)
    {
        free(encodeBuffer);
    }

    *destination = buffer;
    *destinationSize = bufferSize;
    return ret;
}

DLLEXPORT void STDCALL lameCoder_initializeDecoder()
{
    hip = hip_decode_init();
}

DLLEXPORT void STDCALL lameCoder_closeDecoder()
{
    hip_decode_exit(hip);
}

DLLEXPORT int STDCALL lameCoder_decodeToPcm(lame_global_flags* glf, char* source, int sourceSize, mp3data_struct* metadata, char** destination, int* destinationSize)
{
    unsigned char input[1024];
    short* output_l;
    short* output_r;
    size_t nb_read = 0;
    size_t nb_write = 0;
    char* buffer = NULL, *reallocBuffer;
    int bufferSize = 0, ret = 0;
    output_l = (short*)malloc(1152 * 16);
    output_r = (short*)malloc(1152 * 16);
    if (!output_l || !output_r)
    {
        ret = 1;
        goto fail;
    }
    while (sourceSize) 
    {
        nb_read = 1024 ^ ((sourceSize ^ 1024) & -(sourceSize < 1024));
        memcpy(input, source, nb_read);
        source += nb_read;
        sourceSize -= nb_read;
        nb_write = hip_decode_headers(hip, input, nb_read, output_l, output_r, metadata);
        if (nb_write == -1)
        {
            ret = 2;
            goto fail;
        }
        if (nb_write == 0)
        {
            continue;
        }
        nb_write *= sizeof(short);
        reallocBuffer = (char*)realloc(buffer, bufferSize + nb_write);
        if (!reallocBuffer)
        {
            ret = 1;
            goto fail;
        }
        buffer = reallocBuffer;
        memcpy(buffer + bufferSize, output_l, nb_write);
        bufferSize += nb_write;
    }
    nb_write = lame_encode_flush(glf, output_l, 8192);
    if (nb_write)
    {
        reallocBuffer = (char*)realloc(buffer, bufferSize + nb_write);
        if (!reallocBuffer)
        {
            ret = 1;
            goto fail;
        }
        buffer = reallocBuffer;
        memcpy(buffer + bufferSize, output_l, nb_write);
        bufferSize += nb_write;
    }
    if (0)
    {
        fail:
        if (buffer)
        {
            free(buffer);
            buffer = NULL;
        }
        bufferSize = 0;
    }
    if (output_l)
    {
        free(output_l);
    }
    if (output_r)
    {
        free(output_r);
    }

    *destination = buffer;
    *destinationSize = bufferSize;
    return ret;
}

DLLEXPORT void STDCALL lameCoder_setInputSamplerate(lame_global_flags* glf, int inputSamplerate)
{
    lame_set_in_samplerate(glf, inputSamplerate);
}

DLLEXPORT void STDCALL lameCoder_setInputChannelNum(lame_global_flags* glf, int inputChannels)
{
    lame_set_num_channels(glf, inputChannels);
}

DLLEXPORT void STDCALL lameCoder_setOutputSamplerate(lame_global_flags* glf, int outputSamplerate)
{
    lame_set_out_samplerate(glf, outputSamplerate);
}

DLLEXPORT void STDCALL lameCoder_setOutputBitrate(lame_global_flags* glf, int outputBitrate)
{
    lame_set_brate(glf, outputBitrate);
}

DLLEXPORT void STDCALL lameCoder_setInputScale(lame_global_flags* glf, float scale)
{
    lame_set_scale(glf, scale);
}

DLLEXPORT void STDCALL lameCoder_setOutputChannelMode(lame_global_flags* glf, int outputChannelMode)
{
    switch (outputChannelMode)
    {
        case 0:
            lame_set_mode(glf, STEREO);
            break;
        case 1:
            lame_set_mode(glf, JOINT_STEREO);
            break;
        case 3:
            lame_set_mode(glf, MONO);
            break;
        default:
            lame_set_mode(glf, NOT_SET);
            break;
    }
}

DLLEXPORT void STDCALL lameCoder_setVbrMode(lame_global_flags* glf, int mode)
{
    switch (mode)
    {
        case 2:
            lame_set_VBR(glf, vbr_rh);
            break;
        case 3:
            lame_set_VBR(glf, vbr_abr);
            break;
        case 4:
            lame_set_VBR(glf, vbr_mtrh);
            break;
        case 6:
            lame_set_VBR(glf, vbr_default);
            break;
        default:
            lame_set_VBR(glf, vbr_off);
            break;
    }
}

DLLEXPORT void STDCALL lameCoder_setQuality(lame_global_flags* glf, int quality)
{
    lame_set_quality(glf, quality);
}

DLLEXPORT void STDCALL lameCoder_setVbrQuality(lame_global_flags* glf, int quality)
{
    lame_set_VBR_q(glf, quality);
}

DLLEXPORT void STDCALL lameCoder_setVbrBitrate(lame_global_flags* glf, int abrBitrateInKbps)
{
    lame_set_VBR_mean_bitrate_kbps(glf, abrBitrateInKbps);
}

DLLEXPORT void STDCALL lameCoder_setLowpassFrequency(lame_global_flags* glf, int frequency)
{
    lame_set_lowpassfreq(glf, frequency);
}

DLLEXPORT void STDCALL lameCoder_setHighpassFrequency(lame_global_flags* glf, int frequency)
{
    lame_set_highpassfreq(glf, frequency);
}

DLLEXPORT void STDCALL lameCoder_setTags(lame_global_flags* glf, const char* titleTag, const char* artistTag, const char* albumTag, const char* yearTag, const char* commentTag)
{
    id3tag_init(glf);
    if (titleTag)
    {
        id3tag_set_title(glf, titleTag);
    }
    if (artistTag)
    {
        id3tag_set_artist(glf, artistTag);
    }
    if (albumTag)
    {
        id3tag_set_album(glf, albumTag);
    }
    if (yearTag)
    {
        id3tag_set_year(glf, yearTag);
    }
    if (commentTag)
    {
        id3tag_set_comment(glf, commentTag);
    }
}

