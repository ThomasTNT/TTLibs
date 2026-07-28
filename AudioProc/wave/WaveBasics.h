#pragma once

#include <string>
#include <climits>

//data types
typedef unsigned char  U8;
typedef unsigned short U16;
typedef unsigned long  U32;
typedef float          F32;

namespace ttaudio{

//************************************************************************
//
// enums and its string representations
//
//************************************************************************
enum WaveFormat { NORMAL8,    //normal (signed)  8 bit GND == 0
                  NORMAL16,   //normal (signed) 16 bit GND == 0
                  NORMAL24,   //normal (signed) 24 bit GND == 0
                  STUDER20,   //special format from studer 2x24bit into 5 bytes
                  STUDER24,   //special 24bit format from studer - byte order is different instead of (0(MSB),1,2|3(MSB),4,5) -> 1,0,3,2,5,4
                  STUDER24L,  //special 24bit format from studer (don't know what is was exactly)
                  NORMAL32,   //normal (signed) 32 bit GND == 0
                  FLOAT32,    //32 bit float +1 .... -1 GND == 0
                  UINT8,      //unsigned 8bit GND == 128
                  INVALID,    //indicates format is invalid
                  UNKNOWN };  //indicates format is not set yet

enum MixingType {OVERWRITE, ADD, MIX, SKIP};

std::string NameOfWaveFormat(const WaveFormat format);

//************************************************************************
//
// these are some utility functions for handling wave files and audio data
//
//************************************************************************

std::string char4ToString(const char* value);

//HELP CONVERT
inline float FloatOfU32Bytes(const U32 floatvalue){
  return *reinterpret_cast<const float*>(&floatvalue);
}

//HELP CONVERT
inline U32 U32BytesOfFloat(const float value){
  return *reinterpret_cast<const U32*>(&value);
}

//************************************************************************
//
// convert functions for wave file valus
//
//************************************************************************

//CONVERT MAIN CONVERSION FUNCTION
U32 Convert(const U32 valueIn, const WaveFormat formatIn, const WaveFormat formatOut);

//CONVERT MAIN CONVERSION FUNCTION
U32 ConvertIntFromInt(const U32 valueIn, const WaveFormat formatIn, const WaveFormat formatOut);

//CONVERT MAIN CONVERSION FUNCTION
U32 ConvertIntFromFloat(const float valueIn, const WaveFormat formatOut);

//CONVERT MAIN CONVERSION FUNCTION
float ConvertFloatFromInt(const U32 valueIn, const WaveFormat formatIn);


// CONVERT float32ByInt24
// int 32 (in U32) -> float (1.0 ... 0 ... -1.0)
float Float32ByInt32(const U32 value);

// CONVERT float32ByInt24
// int 24 (in U32) -> float (1.0 ... 0 ... -1.0)
float Float32ByInt24(const U32 value);

// CONVERT float32ByInt16
// int 16 (in U32) -> float (1.0 ... 0 ... -1.0)
float Float32ByInt16(const U32 value);

// CONVERT float32ByInt24
// int 8 (in U32) -> float (1.0 ... 0 ... -1.0)
float Float32ByInt8(const U32 value);

// CONVERT float32ByInt16
// uint 8 (in U32) -> float (1.0 ... 0 ... -1.0)
float Float32ByUInt8(const U32 value);

// CONVERT Int32ByFloat32
// float (1.0 ... 0 ... -1.0) -> int 32 (in U32)
U32 Int32ByFloat32(const float value);

// CONVERT Int24ByFloat32
// float (1.0 ... 0 ... -1.0) -> int 24 (in U32)
U32 Int24ByFloat32(const float value);

// CONVERT Int16ByFloat32
// float (1.0 ... 0 ... -1.0) -> int 16 (in U32)
U32 Int16ByFloat32(const float value);

// CONVERT Int8ByFloat32
// float (1.0 ... 0 ... -1.0) -> int 8 (in U32)
U32 Int8ByFloat32(const float value);

// CONVERT UInt8ByFloat32
// float (1.0 ... 0 ... -1.0) -> uint 8 (in U32)
U32 UInt8ByFloat32(const float value);

//************************************************************************
//
// mathematical functions
//
//************************************************************************

// CALC dB -> linear factor
double GainBydB(const double dB);


// CALC dB -> linear factor
double dBByGain(const double gain);

//************************************************************************
//
// Mixing
//
//************************************************************************

//GAIN
U32 GainFloat(const U32 value, const double gain);

//GAIN
U32 GainInt32(const U32 value, const double gain);

//GAIN
U32 GainInt24(const U32 value, const double gain);

//GAIN
U32 GainInt16(const U32 value, const double gain);

//GAIN
U32 GainInt8(const U32 value, const double gain);

//GAIN
U32 GainUint8(const U32 value, const double gain);


//MIXING adds newvalue to oldvalue
U32 AddAudio(const U32 oldvalue, const U32 newvalue, const WaveFormat format);

//MIXING mixes newvalue to oldvalue
U32 MixAudio(const U32 oldvalue, const U32 newvalue, const WaveFormat format, const double gain);

//MIXING adds newvalue to oldvalue
U32 AddAudioFloat(const U32 oldvalue, const U32 newvalue);

//MIXING adds newvalue to oldvalue
U32 AddAudioInt32(const U32 oldvalue, const U32 newvalue);

//MIXING adds newvalue to oldvalue
U32 AddAudioInt24(const U32 oldvalue, const U32 newvalue);

//MIXING adds newvalue to oldvalue
U32 AddAudioInt16(const U32 oldvalue, const U32 newvalue);

//MIXING adds newvalue to oldvalue
U32 AddAudioInt8(const U32 oldvalue, const U32 newvalue);

//MIXING adds newvalue to oldvalue
U32 AddAudioUint8(const U32 oldvalue, const U32 newvalue);



} //end of namespace ttaudio
