#include "lame.h"

__declspec(dllexport) lame_global_flags* __stdcall lameCoder_initializeDefaultFlag();

__declspec(dllexport) lame_global_flags* __stdcall lameCoder_createFlag();

__declspec(dllexport) void __stdcall lameCoder_initializeFlag(lame_global_flags* glf);

__declspec(dllexport) void __stdcall lameCoder_close(lame_global_flags* glf);

__declspec(dllexport) int __stdcall lameCoder_encode(lame_global_flags* glf, char* source, int sourceSize, char** destination, int* destinationSize);

__declspec(dllexport) void __stdcall lameCoder_initDecoder();

__declspec(dllexport) void __stdcall lameCoder_closeDecoder();

__declspec(dllexport) int __stdcall lameCoder_decode(lame_global_flags* glf, char* source, int sourceSize, mp3data_struct* metadata, char** destination, int* destinationSize);

__declspec(dllexport) void __stdcall lameCoder_setInputSamplerate(lame_global_flags* glf, int inputSamplerate);

__declspec(dllexport) void __stdcall lameCoder_setInputChannelNum(lame_global_flags* glf, int inputChannels);

__declspec(dllexport) void __stdcall lameCoder_setOutputSamplerate(lame_global_flags* glf, int outputSamplerate);

__declspec(dllexport) void __stdcall lameCoder_setOutputBitrate(lame_global_flags* glf, int outputBitrate);

__declspec(dllexport) void __stdcall lameCoder_setInputScale(lame_global_flags* glf, float scale);

__declspec(dllexport) void __stdcall lameCoder_setOutputChannelMode(lame_global_flags* glf, int outputChannelMode);

__declspec(dllexport) void __stdcall lameCoder_setVbrMode(lame_global_flags* glf, int mode);

__declspec(dllexport) void __stdcall lameCoder_setQuality(lame_global_flags* glf, int quality);

__declspec(dllexport) void __stdcall lameCoder_setVbrQuality(lame_global_flags* glf, int quality);

__declspec(dllexport) void __stdcall lameCoder_setVbrBitrate(lame_global_flags* glf, int abrBitrateInKbps);

__declspec(dllexport) void __stdcall lameCoder_setLowpassFrequency(lame_global_flags* glf, int frequency);

__declspec(dllexport) void __stdcall lameCoder_setHighpassFrequency(lame_global_flags* glf, int frequency);

__declspec(dllexport) void __stdcall lameCoder_setTags(lame_global_flags* glf, const char* titleTag, const char* artistTag, const char* albumTag, const char* yearTag, const char* commentTag);