#pragma once

#include <cmath>
#include <vector>


namespace ttaudiofx {

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
class Compressor {

  public:

    enum WorkType { LIMITER, COMPRESSOR };

    //constructor
    Compressor();

    //must be called once before processing begins
    //resets internal state and recalculates time constants
    void Init(const int sampleRate);


    //switches between LIMITER (brickwall at 0 dBFS) and
    //COMPRESSOR (threshold -12 dBFS, ratio 4:1)
    void SetMode(const WorkType mode);

 
    // when bypassed, audio passes through unmodified
    void SetBypass(const bool bypass);
    bool GetBypass() const;
    bool IsLinked() const;

    //links to another compressor for stereo-link gain sharing;
    //both compressors adopt the more aggressive gain reduction
    //pass nullptr to unlink
    void SetLink(Compressor* const link);

    //scans the buffer and stores the worst-case target gain for this buffer;
    //must be called on ALL linked channels before any Compress() call so the
    //link can use the current buffer's target rather than the previous buffer's smoothed gain
    void PreAnalyze(float* const buffer, const int sampleCount, const int channelOffset = 1);

    //processes a single-channel sample stream in-place
    //buffer        : pointer to the first sample of the channel
    //sampleCount   : number of samples to process
    //channelOffset : step between consecutive samples (1 for packed, numChannels for interleaved buffer)
    void Compress(float* const buffer, const int sampleCount, const int channelOffset = 1);

    //returns the peak gain reduction in dB since the last call,
    //then resets the peak; suitable for driving TTClipMeter
    //note: called from GUI thread while audio thread may write —
    //       practically safe on x86 for a single float
    float GetGainReduction();

    //returns true if >= 3 consecutive samples exceeded 1.0f recently;
    //indicator stays active for 2 seconds after the last clipping event
    bool GetClipping() const;

    // LIMITER parameter setters / getters
    void  SetLimiterThreshold(const float dB);
    float GetLimiterThreshold() const;
    void  SetLimiterAttack(const float ms);
    float GetLimiterAttack() const;
    void  SetLimiterRelease(const float ms);
    float GetLimiterRelease() const;

    // COMPRESSOR parameter setters / getters
    void  SetCompThreshold(const float dB);
    float GetCompThreshold() const;
    void  SetCompRatio(const float ratio);
    float GetCompRatio() const;
    void  SetCompAttack(const float ms);
    float GetCompAttack() const;
    void  SetCompRelease(const float ms);
    float GetCompRelease() const;

  private:

    //recalculates attack/release coefficients from Mode and SampleRate
    void UpdateCoefficients();

    //returns the target linear gain for the given inter-channel peak level
    float ComputeTargetGain(const float peakLevel) const;

    WorkType    Mode              = LIMITER;
    bool        Bypass            = true;
    Compressor* Link              = nullptr;

    int         SampleRate        = 44100;

    //current smoothed gain (linear; 1.0 = unity, < 1.0 = reduction)
    float       Gain              = 1.0f;

    //peak gain reduction in dB since last GetGainReduction() call
    float       GainReductionPeak    = 0.0f;

    //worst-case target gain across the current buffer; written by PreAnalyze, read by linked partner's Compress
    float       BufferTargetGain     = 1.0f;

    //clipping detection: >= 3 consecutive samples >= 1.0f trigger a 2-second hold
    int         ClippingHoldSamples  = 88200;  //placeholder; overwritten by Init
    int         ClippingHoldRemain   = 0;
    int         ClippingConsecutive  = 0;

    //per-sample smoothing coefficients (derived from attack/release times)
    float       AttackCoeff       = 1.0f;
    float       ReleaseCoeff      = 0.0f;

    //peak-hold detector: instant attack, fixed hold, then smooth release
    //holds the detected peak level for HoldSamples before allowing release,
    //preventing gain from tracking individual half-waves of a sustained low-frequency tone
    float       PeakHold          = 0.0f;
    int         HoldRemain        = 0;
    int         HoldSamples       = 0;    //overwritten by Init

    //LIMITER parameters
    float       LimiterThresholdDb = 0.0f;
    float       LimiterAttackMs    = 0.2f;   //attack time in ms; 0 = instant
    float       LimiterReleaseMs   = 80.0f;
    float       LimiterHoldMs      = 5.0f;

    //COMPRESSOR parameters
    float       CompThresholdDb    = -12.0f;
    float       CompRatio          = 4.0f;
    float       CompAttackMs       = 5.0f;
    float       CompReleaseMs      = 100.0f;

    //look-ahead delay buffer for LIMITER mode:
    //gain is computed from the incoming (future) signal and applied to a delayed copy,
    //giving the attack time to act before the transient arrives at the output
    std::vector<float> LookAheadBuffer;
    int                LookAheadWritePos = 0;
    int                LookAheadSamples  = 0;
};

} // end of namespace ttaudiofx
