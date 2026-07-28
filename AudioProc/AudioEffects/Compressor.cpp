#include "Compressor.h"

//***************************************************************************
//*                           Compressor                                    *
//*                                                                         *
//*  Audio dynamics processor: limiter or compressor.                       *
//*  Processes interleaved float buffers (range: -1.0 to +1.0).            *
//*                                                                         *
//*  Usage:                                                                 *
//*    Init(sampleRate);                                                    *
//*    SetMode(LIMITER);                                                    *
//*    Compress(buffer, channels, sampleCount);       // per buffer         *
//*    float dB = GetGainReduction();                 // for meter display  *
//*                                                                         *
//*  Stereo link: call SetLink() on both sides so both channels share       *
//*  the more aggressive gain reduction.                                    *
//*                                                                         *
//***************************************************************************

//************
// constructor 
//************
ttaudiofx::Compressor::Compressor(){  
  UpdateCoefficients();
}

//************************************************************
// Init
//
// resets state and recalculates time constants for sampleRate
//************************************************************
void ttaudiofx::Compressor::Init(const int sampleRate) {
  SampleRate           = sampleRate > 0 ? sampleRate : 44100;
  Gain                 = 1.0f;
  PeakHold             = 0.0f;
  HoldRemain           = 0;
  HoldSamples          = static_cast<int>(LimiterHoldMs * 0.001f * static_cast<float>(SampleRate) + 0.5f);
  GainReductionPeak    = 0.0f;
  ClippingHoldSamples  = static_cast<int>(SampleRate * 0.25);
  ClippingHoldRemain   = 0;
  ClippingConsecutive  = 0;
  UpdateCoefficients();
  LookAheadSamples  = static_cast<int>(LimiterAttackMs * 0.001f * static_cast<float>(SampleRate) * 10.0f + 0.5f);
  LookAheadBuffer.assign(LookAheadSamples > 0 ? LookAheadSamples : 1, 0.0f);
  LookAheadWritePos = 0;
}

//*****************************************************************
// SetMode
//
// switches processing mode and updates time constants accordingly
//***************************************************************** 
void ttaudiofx::Compressor::SetMode(const WorkType mode){
  Mode = mode;
  UpdateCoefficients();
}

//**********
// SetBypass
//**********
void ttaudiofx::Compressor::SetBypass(const bool bypass){
  this->Bypass = bypass;
  if (!bypass){
    ClippingHoldRemain  = 0;
    ClippingConsecutive = 0;
  }
}

//**********
// GetBypass
//**********
bool ttaudiofx::Compressor::GetBypass() const{
  return this->Bypass;
}

//***********
// IsLinked
//***********
bool ttaudiofx::Compressor::IsLinked() const{
  return Link != nullptr;
}

//********
// SetLink
//********
void ttaudiofx::Compressor::SetLink(Compressor* const link){ 
  this->Link = link;
}

//-------------------------------- LIMITER setters / getters --------------------------------

//********************
// SetLimiterThreshold
//********************
void ttaudiofx::Compressor::SetLimiterThreshold(const float dB){
  this->LimiterThresholdDb = dB;
}

//********************
// GetLimiterThreshold
//********************
float ttaudiofx::Compressor::GetLimiterThreshold() const{
  return this->LimiterThresholdDb;
}

//*****************
// SetLimiterAttack
//*****************
void ttaudiofx::Compressor::SetLimiterAttack(const float ms){
  this->LimiterAttackMs = ms;
  if (Mode == LIMITER) UpdateCoefficients();
}

//*****************
// GetLimiterAttack
//*****************
float ttaudiofx::Compressor::GetLimiterAttack() const{
  return this->LimiterAttackMs;
}

//******************
// SetLimiterRelease
//******************
void ttaudiofx::Compressor::SetLimiterRelease(const float ms){
  this->LimiterReleaseMs = ms;
  if (Mode == LIMITER) UpdateCoefficients();
}

//******************
// GetLimiterRelease
//******************
float ttaudiofx::Compressor::GetLimiterRelease() const{
  return this->LimiterReleaseMs;
}

//------------------------------ COMPRESSOR setters / getters  ------------------------------

//*****************
// SetCompThreshold
//*****************
void ttaudiofx::Compressor::SetCompThreshold(const float dB){
  this->CompThresholdDb = dB;
}

//*****************
// GetCompThreshold
//*****************
float ttaudiofx::Compressor::GetCompThreshold() const{
  return this->CompThresholdDb;
}

//*************
// SetCompRatio
//*************
void ttaudiofx::Compressor::SetCompRatio(const float ratio){
  this->CompRatio = ratio;
}

//*************
// GetCompRatio
//*************
float ttaudiofx::Compressor::GetCompRatio() const{
  return this->CompRatio;
}

//**************
// SetCompAttack
//**************
void ttaudiofx::Compressor::SetCompAttack(const float ms){
  this->CompAttackMs = ms;
  if (Mode == COMPRESSOR) UpdateCoefficients();
}

//**************
// GetCompAttack
//**************
float ttaudiofx::Compressor::GetCompAttack() const{
  return this->CompAttackMs;
}

//***************
// SetCompRelease
//***************
void ttaudiofx::Compressor::SetCompRelease(const float ms){
  this->CompReleaseMs = ms;
  if (Mode == COMPRESSOR) UpdateCoefficients();
}

//***************
// GetCompRelease
//***************
float ttaudiofx::Compressor::GetCompRelease() const{
  return this->CompReleaseMs;
}



//************************************************************************************************* 
// PreAnalyze 
//
// scans the buffer for the worst-case target gain; call on ALL channels before any Compress() call
//************************************************************************************************* 
void ttaudiofx::Compressor::PreAnalyze(float* const buffer, const int sampleCount, const int channelOffset) {
  if (Bypass || sampleCount <= 0 || channelOffset <= 0){
    BufferTargetGain = 1.0f;
    return;
  }
  float minTarget = 1.0f;
  for (int s = 0; s < sampleCount; ++s){
    const float peak   = std::abs(buffer[s * channelOffset]);
    const float target = ComputeTargetGain(peak);
    if (target < minTarget) minTarget = target;
  }
  BufferTargetGain = minTarget;
}

//****************************************************************************************
// Compress
//
// processes one channel within a buffer in-place
// channelOffset = 1 for packed single-channel, = numChannels for interleaved buffer
// In LIMITER mode: gain is computed from the incoming (non-delayed) signal and applied
// to a look-ahead-delayed copy, so attack time is spent BEFORE the peak arrives at output.
//****************************************************************************************
void ttaudiofx::Compressor::Compress(float* const buffer, const int sampleCount, const int channelOffset) {
  if (sampleCount <= 0 || channelOffset <= 0) return;

  const bool useLookAhead = (Mode == LIMITER && LookAheadSamples > 0);

  for (int s = 0; s < sampleCount; ++s) {
    float& outSample      = buffer[s * channelOffset];
    const float inSample  = outSample;
    const float peak      = inSample >= 0.0f ? inSample : -inSample;

    float workSample;
    if (useLookAhead) {
      workSample                          = LookAheadBuffer[LookAheadWritePos];
      LookAheadBuffer[LookAheadWritePos]  = inSample;
      LookAheadWritePos = (LookAheadWritePos + 1) % LookAheadSamples;
    } else {
      workSample = inSample;
    }

    if (Bypass){
      //clipping detection only when unprotected (>= 3 consecutive samples required)
      if (peak >= 1.0f){
        if (++ClippingConsecutive >= 3){
          ClippingHoldRemain = ClippingHoldSamples;
        }
      }
      else{
        ClippingConsecutive = 0;
      }
      if (useLookAhead) outSample = workSample;
      continue;
    }

    //peak-hold detector: instant attack, fixed hold window, then smooth release
    //the hold prevents release during zero crossings of sustained low-frequency tones,
    //which would otherwise cause audible gain modulation (Kratzen)
    if (peak > PeakHold){
      PeakHold   = peak;
      HoldRemain = HoldSamples;
    }
    else if (HoldRemain > 0){
      --HoldRemain;
    }
    else{
      PeakHold += ReleaseCoeff * (peak - PeakHold);
    }

    float targetGain = ComputeTargetGain(PeakHold);

    //stereo link: take the more aggressive target from the partner's pre-analyzed buffer target
    if (Link && Link->BufferTargetGain < targetGain){
      targetGain = Link->BufferTargetGain;
    }

    if (targetGain < Gain){
      Gain = AttackCoeff * Gain + (1.0f - AttackCoeff) * targetGain;
    }
    else{
      Gain = ReleaseCoeff * Gain + (1.0f - ReleaseCoeff) * targetGain;
    }

    if (Gain > 1.0f)    Gain = 1.0f;
    if (Gain < 0.0001f) Gain = 0.0001f;  //-80 dB floor

    if (Gain < 1.0f){
      const float reductionDb = -20.0f * std::log10(Gain);
      if (reductionDb > GainReductionPeak) GainReductionPeak = reductionDb;
    }

    outSample = workSample * Gain;
    if (outSample >  1.0f) outSample =  1.0f;
    if (outSample < -1.0f) outSample = -1.0f;
  }

  if (ClippingHoldRemain > sampleCount){
    ClippingHoldRemain -= sampleCount;
  }
  else if (ClippingHoldRemain > 0){
    ClippingHoldRemain = 0;
  }
}

//**************************************************************** 
// GetGainReduction
//
// returns and resets the peak gain reduction since the last call
//**************************************************************** 
float ttaudiofx::Compressor::GetGainReduction() {
  const float peak  = GainReductionPeak;
  GainReductionPeak = 0.0f;
  return peak;
}

//*******************************************************************************
// GetClipping
//
// true while the 2-second hold after >= 3 consecutive clipping samples is active
//******************************************************************************* 
bool ttaudiofx::Compressor::GetClipping() const {
  return ClippingHoldRemain > 0;
}

//****************************************************************************
// UpdateCoefficients
//
// coeff = 1 - exp(-1 / (time_s * sampleRate));  0 ms -> coeff = 1.0 (instant)
//****************************************************************************
void ttaudiofx::Compressor::UpdateCoefficients() {
  const float sr        = static_cast<float>(SampleRate);
  const float attackMs  = Mode == LIMITER ? LimiterAttackMs  : CompAttackMs;
  const float releaseMs = Mode == LIMITER ? LimiterReleaseMs : CompReleaseMs;
  //convention matches the gain formula: Gain = coeff*Gain + (1-coeff)*targetGain
  //coeff = exp(-1/tau): close to 1 = slow, 0 = instant
  AttackCoeff  = attackMs > 0.0f
                 ? std::exp(-1.0f / (attackMs  * 0.001f * sr))
                 : 0.0f;
  ReleaseCoeff = std::exp(-1.0f / (releaseMs * 0.001f * sr));
}

//********************************************* ComputeTargetGain ************************************************
float ttaudiofx::Compressor::ComputeTargetGain(const float peakLevel) const {
  if (peakLevel <= 0.0f) return 1.0f;

  const float levelDb = 20.0f * std::log10(peakLevel);

  if (Mode == LIMITER) {
    if (levelDb > LimiterThresholdDb) {
      return std::pow(10.0f, (LimiterThresholdDb - levelDb) / 20.0f);
    }
  }
  else {
    if (levelDb > CompThresholdDb) {
      const float gainDb = (CompThresholdDb - levelDb) * (1.0f - 1.0f / CompRatio);
      return std::pow(10.0f, gainDb / 20.0f);
    }
  }
  return 1.0f;
}

