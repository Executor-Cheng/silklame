#include "lame.h"
#include "dllexport.h"
#include "codingBuffer.h"

DLLEXPORT lame_global_flags* STDCALL lameCoder_initializeDefaultFlag();

DLLEXPORT lame_global_flags* STDCALL lameCoder_createFlag();

DLLEXPORT void STDCALL lameCoder_initializeFlag(lame_global_flags* glf);

DLLEXPORT void STDCALL lameCoder_closeFlag(lame_global_flags* glf);

DLLEXPORT int STDCALL lameCoder_encodeToMp3(lame_global_flags* glf, char* source, int sourceSize, char** destination, int* destinationSize);

DLLEXPORT void STDCALL lameCoder_initializeDecoder();

DLLEXPORT void STDCALL lameCoder_closeDecoder();

DLLEXPORT int STDCALL lameCoder_decodeToPcm(lame_global_flags* glf, char* source, int sourceSize, mp3data_struct* metadata, char** destination, int* destinationSize);

DLLEXPORT void STDCALL lameCoder_setInputSamplerate(lame_global_flags* glf, int inputSamplerate);

DLLEXPORT void STDCALL lameCoder_setInputChannelNum(lame_global_flags* glf, int inputChannels);

DLLEXPORT void STDCALL lameCoder_setOutputSamplerate(lame_global_flags* glf, int outputSamplerate);

DLLEXPORT void STDCALL lameCoder_setOutputBitrate(lame_global_flags* glf, int outputBitrate);

DLLEXPORT void STDCALL lameCoder_setInputScale(lame_global_flags* glf, float scale);

DLLEXPORT void STDCALL lameCoder_setOutputChannelMode(lame_global_flags* glf, int outputChannelMode);

DLLEXPORT void STDCALL lameCoder_setVbrMode(lame_global_flags* glf, int mode);

DLLEXPORT void STDCALL lameCoder_setQuality(lame_global_flags* glf, int quality);

DLLEXPORT void STDCALL lameCoder_setVbrQuality(lame_global_flags* glf, int quality);

DLLEXPORT void STDCALL lameCoder_setVbrBitrate(lame_global_flags* glf, int abrBitrateInKbps);

DLLEXPORT void STDCALL lameCoder_setLowpassFrequency(lame_global_flags* glf, int frequency);

DLLEXPORT void STDCALL lameCoder_setHighpassFrequency(lame_global_flags* glf, int frequency);

DLLEXPORT void STDCALL lameCoder_setTags(lame_global_flags* glf, const char* titleTag, const char* artistTag, const char* albumTag, const char* yearTag, const char* commentTag);