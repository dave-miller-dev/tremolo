//
//  TremeloUnit.hpp
//  TremeloAUv2
//
//  Created by David Miller on 19/9/21.
//

#include "AUEffectBase.h"
#include "TremeloUnitVersion.h"

#if AU_DEBUG_DISPATCHER
    #include "AUDebugDispatcher.h"
#endif

#ifndef TremeloUnit_hpp
#define TremeloUnit_hpp

/* constants for parameters and factory presets */
#pragma mark ____TremeloUnit Parameter Constants

/// Provides the user interface name for the frequency parameter.
static CFStringRef kParamName_Tremelo_Freq          = CFSTR("Frequency");
/// Defines a constant default value for the Frequency parameter, anticipating a unit of hertz to be defined in the .cpp implementation file.
static constexpr float kDefaultValue_Tremelo_Freq   = 2.0;
/// Defines a constant for the minimum value for the Frequency parameter.
static constexpr float kMinimumValue_Tremelo_Freq   = 0.5;
/// Defines a constant for the maximum value for the Frequency parameter.
static constexpr float kMaximumValue_Tremelo_Freq   = 20.0;

static CFStringRef kParamName_Tremelo_Depth         = CFSTR("Depth");
static constexpr float kDefaultValue_Tremelo_Depth  = 50.0;
static constexpr float kMinimumValue_Tremelo_Depth  = 0.0;
static constexpr float kMaximumValue_Tremelo_Depth  = 100.0;

static CFStringRef kParamName_Tremelo_Waveform      = CFSTR("Waveform");
static constexpr int kSineWave_Tremelo_Waveform     = 1;
static constexpr int kSquareWave_Tremelo_Waveform   = 2;
static constexpr int kDefaultValue_Tremelo_Waveform = kSineWave_Tremelo_Waveform;

// Defines menu item names for the Waveform parameter.
static CFStringRef kMenuItem_Tremelo_Sine           = CFSTR("Sine");
static CFStringRef kMenuItem_Tremelo_Square         = CFSTR("Square");

enum Parameters {
    kParameter_Frequency    = 0,
    kParameter_Depth        = 1,
    kParameter_Waveform     = 2,
    kNumberOfParameters     = 3
};

#pragma mark ____TremeloUnit Factory Preset Constants

/// Define a constant for the frequency value for the "Slow and Gentle" factory preset.
static constexpr float kParameter_Preset_Frequency_Slow = 2.0;

/// Define a constant for the frequency value for the "Fast & Hard" factory preset.
static constexpr float kParameter_Preset_Frequency_Fast = 20.0;

/// Define a constant for the depth value for the "Slow and Gentle" factory preset.
static constexpr float kParameter_Preset_Depth_Slow     = 50.0;

/// Define a constant for the depth value for the "Fast & Hard" factory preset.
static constexpr float kParameter_Preset_Depth_Fast     = 90.0;

/// Define a constant for the waveform value for the "Slow and Gentle" factory preset.
static constexpr float kParameter_Preset_Waveform_Slow  = kSineWave_Tremelo_Waveform;

/// Define a constant for the waveform value for the "Fast & Hard" factory preset.
static constexpr float kParameter_Preset_Waveform_Fast  = kSquareWave_Tremelo_Waveform;

enum Presets {
    /// Defines a constant for the "Slow & Gentle" factory preset.
    kPreset_Slow = 0,
    /// Defines a constant for the "Fast & Hard" factory preset.
    kPreset_Fast = 1,
    /// Defines a constant representing the total number of factory presets.
    kNumberOfPresets = 2
};

// Defines an array containing two Core Foundation string objects. The objects contain
// values for the menu items in the user interface corresponding to the factory presets.
static AUPreset kPresets [kNumberOfPresets] = {
    {kPreset_Slow, CFSTR ("Slow & Gentle")},
    {kPreset_Fast, CFSTR ("Fast & Hard")}
};

// Defines a constant representing the default factory present - "Slow & Gentle"
static constexpr int kPreset_Default = kPreset_Slow;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// TremeloUnit class
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#pragma mark ____TremeloUnit
class TremeloUnit : public AUEffectBase {
    
public:
    TremeloUnit (AudioUnit component);
    
#if AU_DEBUG_DISPATCHER
    virtual ~TremeloUnit () { delete mDebugDispatcher; }
#endif
    
    virtual AUKernelBase *NewKernel () { return new TremeloUnitKernel(this); }
    
    virtual ComponentResult GetParameterValueStrings(AudioUnitScope inScope,
                                                     AudioUnitParameterID inParameterID,
                                                     CFArrayRef *outStrings);
    
    virtual ComponentResult GetParameterInfo(AudioUnitScope inScope,
                                             AudioUnitParameterID inParameterID,
                                             AudioUnitParameterInfo &outParameterInfo);
    
    virtual ComponentResult GetPropertyInfo(AudioUnitPropertyID inID,
                                            AudioUnitScope inScope,
                                            AudioUnitElement inElement,
                                            UInt32 &outDataSize,
                                            Boolean &outWritable);
    
    virtual ComponentResult GetProperty(AudioUnitPropertyID inID,
                                        AudioUnitScope inScope,
                                        AudioUnitElement inElement,
                                        void *outData);
    
    // Report that the AudioUnit supports the kAudioUnitProperty_TailTime property.
    virtual bool SupportsTail() { return true; }
    
    //provide the AudioUnit version information
    virtual ComponentResult Version () { return kTremoloUnitVersion; }
    
    // Declaration for the GetPresets method (for seeting up the factory presets),
    // overriding the method from the AUBase superclass.
    virtual ComponentResult GetPresets (CFArrayRef *outData) const;
    
    /* Declaration for the for the NewFactoryPresetSet method (for setting a factory preset
      when requested by the host application), overriding the method from the
     AUBase superclass. */
    virtual OSStatus NewFactoryPresetSet(const AUPreset &inNewFactoryPreset);
    
    
protected:
    class TremeloUnitKernel : public AUKernelBase {
    public:
        TremeloUnitKernel (AUEffectBase *inAudioUnit);
        
        /* *Required* overrides for the process method from the AUBase superclass
         for this effect. Processes one channel of interleaved samples. */
        virtual void Process(const Float32 *inSourceP,
                             Float32 *inDestP,
                             UInt32 inFramesToProcess,
                             UInt32 inNumChannels, /* equal to 1 */
                             bool &ioSilence
                             );
        
        virtual void Reset ();
        
    private:
        enum    {kWaveArraySize = 2000};    // The number of points in the wave table.
        float   mSine [kWaveArraySize];     // The wave table for the tremelo sine wave.
        float   mSquare [kWaveArraySize];   // The wave table for the tremelo square wave.
        float   *waveArrayPointer;          // Points to the wave table to use for the current audio input buffer.
        Float32 mSampleFrequency;           // The 'sample rate' of the audio signal being processed.
        long    mSamplesProcessed;          // The number of sample since the audio unit starting rendering
                                            //  or since this variable was reset to 0. We have to keep track
                                            //  of this because we vary the tremelo constinuously and
                                            //  independently of the input buffer size.
        enum    {sampleLimit = (int) 10E6}; // To keep the value of mSamplesProcessed within a reasonable limit.
                                            //  10E6 is equivalent to the number of sample in 100 seconds of 96Khz audio.
        float   mCurrentScale;              // There are two scaling factors to allow the audio unit to switch to a new
                                            //  scaling factor at the beginning of the tremelo waveform, no matter when
                                            //  the user changes the tremelo frequency. mCurrentScale is the scaling
                                            //  factor in use.
        float   mNextScale;                 // The scaling factor that the user most recently requested, by moving the
                                            //  tremelo frequency slider.
    };
};

#endif /* TremeloUnit_hpp */
