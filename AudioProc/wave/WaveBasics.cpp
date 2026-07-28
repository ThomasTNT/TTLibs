#include "WaveBasics.h"

//************************************************************************
//
// enums and its string representations
//
//************************************************************************
std::string ttaudio::NameOfWaveFormat(const ttaudio::WaveFormat format){
 switch (format){
    case ttaudio::NORMAL8:   return "8 bit";
    case ttaudio::NORMAL16:  return "16 bit";
    case ttaudio::NORMAL24:  return "24 bit";
    case ttaudio::STUDER20:  return "studer 20 bit";
    case ttaudio::STUDER24:  return "studer 24 bit";
    case ttaudio::STUDER24L: return "studer 24L";
    case ttaudio::NORMAL32:  return "32 bit";
    case ttaudio::FLOAT32:   return "32 bit float ";
    case ttaudio::UINT8:     return "8 bit unsigned";
    case ttaudio::INVALID:   return "????";
    default:              return "----";
  }
}

//************************************************************************
//
// these are some utility functions for handling wave files and audio data
//
//************************************************************************

//********************
// HELP: char4ToString
//********************
std::string ttaudio::char4ToString(const char* value){
  char temp[5];
  for (int i=0; i<4; i++) temp[i] = value[i];
  temp[4] = '\0';
  return std::string(temp);
}


// ----------------- convert functions for wave file valus ---------------

//*********************************
// CONVERT MAIN CONVERSION FUNCTION
// converts any type to any type
//*********************************
U32 ttaudio::Convert(const U32 valueIn, const WaveFormat formatIn, const WaveFormat formatOut){
  if (formatIn == formatOut) return valueIn;
  if (formatIn == ttaudio::FLOAT32) return ConvertIntFromFloat(FloatOfU32Bytes(valueIn), formatOut);
  if (formatOut == ttaudio::FLOAT32) return U32BytesOfFloat(ConvertFloatFromInt(valueIn, formatIn));
  return ConvertIntFromInt(valueIn, formatIn, formatOut);
}

//*****************************************
// CONVERT MAIN CONVERSION FUNCTION
//
// convert any int format to any int format
//*****************************************
U32 ttaudio::ConvertIntFromInt(const U32 valueIn, const WaveFormat formatIn, const WaveFormat formatOut){

  if (formatIn == formatOut) return valueIn;

  switch (formatIn){
    case ttaudio::NORMAL32:
      switch (formatOut){
        case ttaudio::NORMAL24: return valueIn >> 8;
        case ttaudio::NORMAL16: return valueIn >> 16;
        case ttaudio::NORMAL8:  return valueIn >> 24;
        case ttaudio::UINT8:    return 0;
      }
    case ttaudio::NORMAL24:
      switch (formatOut){
        case ttaudio::NORMAL32: return valueIn << 8;
        case ttaudio::NORMAL16: return valueIn >> 8;
        case ttaudio::NORMAL8:  return valueIn >> 16;
        case ttaudio::UINT8:    return 0;
      }
    case ttaudio::NORMAL16:
      switch (formatOut){
        case ttaudio::NORMAL32: return valueIn << 16;
        case ttaudio::NORMAL24: return valueIn << 8;
        case ttaudio::NORMAL8:  return valueIn >> 8;
        case ttaudio::UINT8:    return 0;
      }
    case ttaudio::NORMAL8:
      switch (formatOut){
        case ttaudio::NORMAL32: return valueIn >> 24;
        case ttaudio::NORMAL24: return valueIn << 16;
        case ttaudio::NORMAL16: return valueIn << 8;
        case ttaudio::UINT8:    return 0;
      }
    case ttaudio::UINT8:
      switch (formatOut){
        case ttaudio::NORMAL32: return 0;
        case ttaudio::NORMAL24: return 0;
        case ttaudio::NORMAL16: return 0;
        case ttaudio::NORMAL8:  return 0;
      }
  }
  return 0;
}

//*******************************************
// CONVERT MAIN CONVERSION FUNCTION
//
// convert float F32 format to any int format
//*******************************************
U32 ttaudio::ConvertIntFromFloat(const float valueIn, const WaveFormat formatOut){
  switch (formatOut){
    case ttaudio::NORMAL32: return Int32ByFloat32(valueIn);
    case ttaudio::NORMAL24: return Int24ByFloat32(valueIn);
    case ttaudio::NORMAL16: return Int16ByFloat32(valueIn);
    case ttaudio::NORMAL8:  return Int8ByFloat32(valueIn);
    case ttaudio::UINT8:    return UInt8ByFloat32(valueIn);
  }
  return 0;
}

//*******************************************
// CONVERT MAIN CONVERSION FUNCTION
//
// convert any int format to float F32 format
//*******************************************
float ttaudio::ConvertFloatFromInt(const U32 valueIn, const WaveFormat formatIn){
  switch (formatIn){
    case ttaudio::NORMAL32: return Float32ByInt32(valueIn);
    case ttaudio::NORMAL24: return Float32ByInt24(valueIn);
    case ttaudio::NORMAL16: return Float32ByInt16(valueIn);
    case ttaudio::NORMAL8:  return Float32ByInt8(valueIn);
    case ttaudio::UINT8:    return Float32ByUInt8(valueIn);
    default: return 0.0f;
  }
}

//**********************************************
// CONVERT float32ByInt24
// int 32 (in U32) -> float (1.0 ... 0 ... -1.0)
//**********************************************
float ttaudio::Float32ByInt32(const U32 value){
  const int intValue = value <= 2147483647 ? static_cast<int>(value) : static_cast<int>(value) - 0xFFFFFFFF;
  return static_cast<float>(static_cast<float>(intValue) / 2147483648.0f);
}

//**********************************************
// CONVERT float32ByInt24
// int 24 (in U32) -> float (1.0 ... 0 ... -1.0)
//**********************************************
float ttaudio::Float32ByInt24(const U32 value){
  const int intValue = value <= 0x7FFFFF ? static_cast<int>(value) : static_cast<int>(value) - 0xFFFFFF;
  return static_cast<float>(static_cast<float>(intValue) / 8388608.0f);
}

//**********************************************
// CONVERT float32ByInt16
// int 16 (in U32) -> float (1.0 ... 0 ... -1.0)
//**********************************************
float ttaudio::Float32ByInt16(const U32 value){
  const int intValue = value <= 32767 ? static_cast<int>(value) : static_cast<int>(value) - 0xFFFF;
  return static_cast<float>(static_cast<float>(intValue) / 32768.0f);
}

//**********************************************
// CONVERT float32ByInt24
// int 8 (in U32) -> float (1.0 ... 0 ... -1.0)
//**********************************************
float ttaudio::Float32ByInt8(const U32 value){
  const int intValue = value <= 127 ? static_cast<int>(value) : static_cast<int>(value) - 0xFF;
  return static_cast<float>(static_cast<float>(intValue) / 128.0f);
}

//**********************************************
// CONVERT float32ByInt24
// uint 8 (in U32) -> float (1.0 ... 0 ... -1.0)
//**********************************************
float ttaudio::Float32ByUInt8(const U32 value){
  const int intValue = static_cast<int>(value) - 128;
  return static_cast<float>(static_cast<float>(intValue) / 128.0);
}

//**********************************************
// CONVERT Int32ByFloat32
// float (1.0 ... 0 ... -1.0) -> int 32 (in U32)
//**********************************************
U32 ttaudio::Int32ByFloat32(const float value){
  const float big = (value * 2147483648.0f);
  return big >= 0 ? static_cast<unsigned int>(big) : static_cast<unsigned int>(static_cast<int>(big) + 0xFFFFFFFF);
}

//**********************************************
// CONVERT float32ByInt24
// float (1.0 ... 0 ... -1.0) -> int 24 (in U32)
//**********************************************
U32 ttaudio::Int24ByFloat32(const float value){
  const float big = (value * 8388608.0f);
  return big >= 0 ? static_cast<unsigned int>(big) : static_cast<unsigned int>(static_cast<int>(big) + 0xFFFFFF);
}

//**********************************************
// CONVERT float32ByInt16
// float (1.0 ... 0 ... -1.0) -> int 16 (in U32)
//**********************************************
U32 ttaudio::Int16ByFloat32(const float value){
  const float big = (value * 32768.0f);
  return big >= 0 ? static_cast<unsigned int>(big) : static_cast<unsigned int>(static_cast<int>(big) + 0xFFFF);
}

//**********************************************
// CONVERT float32ByInt24
// float (1.0 ... 0 ... -1.0) -> int 8 (in U32)
//**********************************************
U32 ttaudio::Int8ByFloat32(const float value){
  const float big = (value * 128.0f);
  return big >= 0 ? static_cast<unsigned int>(big) : static_cast<unsigned int>(static_cast<int>(big) + 0xFF);
}

//**********************************************
// CONVERT float32ByInt16
// float (1.0 ... 0 ... -1.0) -> uint 8 (in U32)
//**********************************************
U32 ttaudio::UInt8ByFloat32(const float value){
  const float big = (value * 128.0f);
  return (big >= 0 ? static_cast<unsigned int>(big) : static_cast<unsigned int>(static_cast<int>(big) + 0xFF)) + 128;
}

// --------------------- mathematical functions  -----------------------

// CALC dB -> linear factor
double ttaudio::GainBydB(const double dB){
  return pow(10.0, (dB / 20.0));
}


// CALC dB -> linear factor
double ttaudio::dBByGain(const double gain){
  return 20.0 * log10(gain);
}

// ------------------------------ Mixing --------------------------------

//*****
// GAIN
//*****
U32 ttaudio::GainFloat(const U32 value, const double gain){
  const float valueF = FloatOfU32Bytes(value) * static_cast<float>(gain);
  const U32 result = U32BytesOfFloat(valueF);
  return result;
}

//*****
// GAIN
//*****
U32 ttaudio::GainInt32(const U32 value, const double gain){
  const float valueF = Float32ByInt32(value) * static_cast<float>(gain);
  const U32 result = Int32ByFloat32(valueF);
  return result;
}

//*****
// GAIN
//*****
U32 ttaudio::GainInt24(const U32 value, const double gain){
  const float valueF = Float32ByInt24(value) * static_cast<float>(gain);
  const U32 result = Int24ByFloat32(valueF);
  return result;
}

//*****
// GAIN
//*****
U32 ttaudio::GainInt16(const U32 value, const double gain){
  const float valueF = Float32ByInt16(value) * static_cast<float>(gain);
  const U32 result = Int16ByFloat32(valueF);
  return result;
}

//*****
// GAIN
//*****
U32 ttaudio::GainInt8(const U32 value, const double gain){
  const float valueF = Float32ByInt8(value) * static_cast<float>(gain);
  const U32 result = Int8ByFloat32(valueF);
  return result;
}

//*****
// GAIN
//*****
U32 ttaudio::GainUint8(const U32 value, const double gain){
  const float valueF = Float32ByUInt8(value) * static_cast<float>(gain);
  const U32 result = UInt8ByFloat32(valueF);
  return result;
}


//**************************************
// MIXING MAIN adds newvalue to oldvalue
//**************************************
U32 ttaudio::AddAudio(const U32 oldvalue, const U32 newvalue, const WaveFormat format){
  switch (format){
    case ttaudio::FLOAT32:  return AddAudioFloat(oldvalue, newvalue);
    case ttaudio::NORMAL32: return AddAudioInt32(oldvalue, newvalue);
    case ttaudio::NORMAL24: return AddAudioInt24(oldvalue, newvalue);
    case ttaudio::NORMAL16: return AddAudioInt16(oldvalue, newvalue);
    case ttaudio::NORMAL8:  return AddAudioInt8 (oldvalue, newvalue);
    case ttaudio::UINT8:    return AddAudioUint8(oldvalue, newvalue);
    default: return AddAudioFloat(oldvalue, newvalue);
  }
}

//***************************************
// MIXING MAIN mixes newvalue to oldvalue
//***************************************
U32 ttaudio::MixAudio(const U32 oldvalue, const U32 newvalue, const WaveFormat format, const double gain){
  //TODO
  return 0;
}


//**********************************
// MIXING adds newvalue to oldvalue
//**********************************
U32 ttaudio::AddAudioFloat(const U32 oldvalue, const U32 newvalue){
  const float oldvalueF = FloatOfU32Bytes(oldvalue);
  if (oldvalueF == 0.0f) return newvalue;
  const float newvalueF = FloatOfU32Bytes(newvalue);
  float sumF = oldvalueF + newvalueF;
  if (sumF >  1.0) sumF =  1.0;
  if (sumF < -1.0) sumF = -1.0;
  const U32 result = U32BytesOfFloat(sumF);
  return result;
}

//**********************************
// MIXING adds newvalue to oldvalue
//**********************************
U32 ttaudio::AddAudioInt32(const U32 oldvalue, const U32 newvalue){
  if (oldvalue == 0) return newvalue;
  const int intValueOld = oldvalue <= 0x7FFFFFFF ? static_cast<int>(oldvalue) : static_cast<int>(oldvalue) - 0xFFFFFFFF;
  const int intValueNew = newvalue <= 0x7FFFFFFF ? static_cast<int>(newvalue) : static_cast<int>(newvalue) - 0xFFFFFFFF;
  int intValueResult = intValueOld + intValueNew;
  if (intValueResult >  0x7FFFFFFF) intValueResult =  0x7FFFFFFF;
  if (intValueResult < INT_MIN) intValueResult = INT_MIN;
  const U32 result = (intValueResult > 0) ?  static_cast<U32>(intValueResult) : static_cast<U32>(intValueResult + 0xFFFFFFFF);
  return result;
}

//**********************************
// MIXING adds newvalue to oldvalue
//**********************************
U32 ttaudio::AddAudioInt24(const U32 oldvalue, const U32 newvalue){
  if (oldvalue == 0) return newvalue;
  const int intValueOld = oldvalue <= 0x7FFFFF ? static_cast<int>(oldvalue) : static_cast<int>(oldvalue) - 0xFFFFFF;
  const int intValueNew = newvalue <= 0x7FFFFF ? static_cast<int>(newvalue) : static_cast<int>(newvalue) - 0xFFFFFF;
  int intValueResult = intValueOld + intValueNew;
  if (intValueResult >  0x7FFFFF) intValueResult =  0x7FFFFF;
  if (intValueResult < -0x800000) intValueResult = -0x800000;
  const U32 result = (intValueResult > 0) ?  static_cast<U32>(intValueResult) : static_cast<U32>(intValueResult + 0xFFFFFF);
  return result;
}

//**********************************
// MIXING adds newvalue to oldvalue
//**********************************
U32 ttaudio::AddAudioInt16(const U32 oldvalue, const U32 newvalue){
  if (oldvalue == 0) return newvalue;
  const int intValueOld = oldvalue <= 0x7FFF ? static_cast<int>(oldvalue) : static_cast<int>(oldvalue) - 0xFFFF;
  const int intValueNew = newvalue <= 0x7FFF ? static_cast<int>(newvalue) : static_cast<int>(newvalue) - 0xFFFF;
  int intValueResult = intValueOld + intValueNew;
  if (intValueResult >  0x7FFF) intValueResult =  0x7FFF;
  if (intValueResult < -0x8000) intValueResult = -0x8000;
  const U32 result = (intValueResult > 0) ?  static_cast<U32>(intValueResult) : static_cast<U32>(intValueResult + 0xFFFF);
  return result;
}

//**********************************
// MIXING adds newvalue to oldvalue
//**********************************
U32 ttaudio::AddAudioInt8(const U32 oldvalue, const U32 newvalue){
  if (oldvalue == 0) return newvalue;
  const int intValueOld = oldvalue <= 0x7F ? static_cast<int>(oldvalue) : static_cast<int>(oldvalue) - 0xFF;
  const int intValueNew = newvalue <= 0x7F ? static_cast<int>(newvalue) : static_cast<int>(newvalue) - 0xFF;
  int intValueResult = intValueOld + intValueNew;
  if (intValueResult >  0x7F) intValueResult =  0x7F;
  if (intValueResult < -0x80) intValueResult = -0x80;
  const U32 result = (intValueResult > 0) ?  static_cast<U32>(intValueResult) : static_cast<U32>(intValueResult + 0xFF);
  return result;
}

//**********************************
// MIXING adds newvalue to oldvalue
//**********************************
U32 ttaudio::AddAudioUint8(const U32 oldvalue, const U32 newvalue){
  if (oldvalue == 128) return newvalue;
  //--TODO ...
  return 0;
}
