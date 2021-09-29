//
//  TremeloUnit.cpp
//  TremeloAUv2
//
//  Created by David Miller on 19/9/21.
//

#include "TremeloUnit.hpp"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
AUDIOCOMPONENT_ENTRY(AUBaseFactory, TremeloUnit)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// The COMPONENT_ENTRY macro is required for the MacOS Component Manager to recognize
// and use the audio unit.

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  TremeloUnit::TremeloUnit
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// The constructor for new TremeloUnit audio units.
TremeloUnit::TremeloUnit (AudioUnit component) : AUEffectBase(component) {
    
    // This method, defined in the AUBase superclass, ensures that the required audio unit
    // elements are created and initialised.
    CreateElements();
    
    // Invokes the use of an STL vector for parameter access.
    // See AuBase/AUScopeElement.cpp
    Globals()->UseIndexedParameters(kNumberOfParameters);
    
    // During instantiation, sets up the parameters according to their defaults.
    // The parameter defaults should correspond to the settings for the default factory preset.
    SetParameter(kParameter_Frequency, kDefaultValue_Tremelo_Freq);
    SetParameter(kParameter_Depth, kDefaultValue_Tremelo_Depth);
    SetParameter(kParameter_Waveform, kDefaultValue_Tremelo_Waveform);
    
    // During instantiation, sets the preset menu to indicate the default preset,
    // which corresponds to the default parameters. It's possible to set this a
    // fresh audio unit indicates the wrong preset, so be careful to get this right.
    SetAFactoryPresetAsCurrent(kPresets[kPreset_Default]);
    
    #if AU_DEBUG_DISPATCHER
        mDebugDispatcher = new AUDebugDispatcher(this);
    #endif
}

#pragma mark ____Parameters

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// TremeloUnit::GetParameterInfo
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Called by the audio unit's view; provides information needed for the view to display the
// audio units parameters.
ComponentResult TremeloUnit::GetParameterInfo(AudioUnitScope inScope,
                                              AudioUnitParameterID inParameterID,
                                              AudioUnitParameterInfo &outParameterInfo) {
    ComponentResult result = noErr;
    
    // Adds two flags to all parameters for the audio unit, indicating to the host application
    // that it should consider all the audio units parameters to be readable and writable.
    outParameterInfo.flags = kAudioUnitParameterFlag_IsWritable | kAudioUnitParameterFlag_IsReadable;
    
    // All three parameters of this audio unit are in the "global" scope.
    if (inScope == kAudioUnitScope_Global) {
        
        switch (inParameterID) {
                
            case kParameter_Frequency:
                // Invoked when the view needs information for the kTremelo_Param_Frequency
                // parameter; defines how to represent this parameter in the user interface.
                AUBase::FillInParameterName(outParameterInfo, kParamName_Tremelo_Freq, false);
                
                // Sets the unit of measurement for the Frequency parameter;
                outParameterInfo.unit           = kAudioUnitParameterUnit_Hertz;
                // Sets the minimum value for the Frequency parameter;
                outParameterInfo.minValue       = kMinimumValue_Tremelo_Freq;
                // Sets the maxmim value for the Frequency parameter.
                outParameterInfo.maxValue       = kMaximumValue_Tremelo_Freq;
                // Sets the default value for the Frequency parameter.
                outParameterInfo.defaultValue   = kDefaultValue_Tremelo_Freq;
                // Adds a flag to indicate to the host that it should a logarithmic
                // control for the Frequency parameter.
                outParameterInfo.flags          |= kAudioUnitParameterFlag_DisplayLogarithmic;
                break;
                
            case kParameter_Depth:
                AUBase::FillInParameterName(outParameterInfo, kParamName_Tremelo_Depth, false);
                outParameterInfo.unit           = kAudioUnitParameterUnit_Percent;
                outParameterInfo.minValue       = kMinimumValue_Tremelo_Depth;
                outParameterInfo.maxValue       = kMaximumValue_Tremelo_Depth;
                outParameterInfo.defaultValue   = kDefaultValue_Tremelo_Depth;
                break;
                
            case kParameter_Waveform:
                AUBase::FillInParameterName(outParameterInfo, kParamName_Tremelo_Waveform, false);
                outParameterInfo.unit           = kAudioUnitParameterUnit_Indexed;
                // Sets the unit of measurement for the Waveform parameter to "indexed",
                // allowing it to be displayed as a pop-up menu in the gerneic view. The following
                // three statements ser the min, max, and default values for the depth parameter.
                // All three are required for peoper functioning of the parameters user interface.
                outParameterInfo.minValue       = kSineWave_Tremelo_Waveform;
                outParameterInfo.maxValue       = kSquareWave_Tremelo_Waveform;
                outParameterInfo.defaultValue   = kSineWave_Tremelo_Waveform;
                break;
                
            default:
                result = kAudioUnitErr_InvalidParameter;
                break;
        }
    } else {
        result = kAudioUnitErr_InvalidParameter;
    }
    return result;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//TremeloUnit::GetParameterValueStrings
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Provides the strings for the Waveform pop-up menu in the generic view.
ComponentResult TremeloUnit::GetParameterValueStrings(AudioUnitScope inScope,
                                                      AudioUnitParameterID inParameterID,
                                                      CFArrayRef *outStrings) {
    
    // This method applies only to the Waveform parameter in the Global Scope.
    if ((inScope == kAudioUnitScope_Global) && (inParameterID == kParameter_Waveform)) {
        
        // When this method gets called by the AUBase::DispatchGetPropertInfo method, which
        // provides a NULL value for the outStrings parameter, just return without error.
        if (outStrings == NULL) return noErr;
        
        // Define an array for our menu item pop-up names.
        CFStringRef strings [] = {
            kMenuItem_Tremelo_Sine,
            kMenuItem_Tremelo_Square
        };
        
        //Create a new immutable array containing the menu item names, and places the array
        // in the outStrings output parameter.
        *outStrings = CFArrayCreate(NULL,
                                    (const void **) strings,
                                    (sizeof(strings) / sizeof(strings[0])),
                                    NULL);
        return noErr;
    }
    return kAudioUnitErr_InvalidParameter;
}

#pragma mark ____Properties

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Tremelo::GetPropertyInfo
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This audio unit doesn't define any custom properties, so it uses this generic code for
// this method.
ComponentResult TremeloUnit::GetPropertyInfo(AudioUnitPropertyID inID,
                                             AudioUnitScope inScope,
                                             AudioUnitElement inElement,
                                             UInt32 &outDataSize,
                                             Boolean &outWritable) {
    return AUEffectBase::GetPropertyInfo(inID, inScope, inElement, outDataSize, outWritable);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// TremeloUnit::GetProperty
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This audio unit doesn't define any custom properties, so it uses this generic code for
// this method.
ComponentResult TremeloUnit::GetProperty(AudioUnitPropertyID inID,
                                         AudioUnitScope inScope,
                                         AudioUnitElement inElement,
                                         void *outData) {
    return AUEffectBase::GetProperty(inID, inScope, inElement, outData);
}

#pragma mark ____Factory Presets

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//    TremoloUnit::GetPresets
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// For users to be able to use the factory presets you define, you must add a generic
// implementation of the GetPresets method. The code here works for any audio unit that can
// support factory presets.

// The GetPresets method accepts a single parameter, a pointer to a CFArrayRef object. This
// object holds the factory presets array generated by this method. The array contains just
// factory preset numbers and names. The host application uses this array to set up its
// factory presets menu and when calling the NewFactoryPresetSet method.

ComponentResult TremeloUnit::GetPresets(CFArrayRef *outData) const {
    
    // Check whether factory presets are implemented for this audio unit.
    if (outData == NULL) return noErr;
    
    // Instantiates a mutable Core Foundation array to fold the factory presets.
    CFMutableArrayRef presetsArray = CFArrayCreateMutable(NULL, kNumberOfPresets, NULL);
    
    // Fills the Factory presets array with the values from the definitions in the TremeloUnit.h file.
    for (int i = 0; i < kNumberOfPresets; i++) {
        CFArrayAppendValue(presetsArray, &kPresets[i]);
    }
    
    // Stores the factory presets array at the outData location.
    *outData = (CFArrayRef) presetsArray;
    return noErr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//    TremoloUnit::NewFactoryPresetSet
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// The NewFactoryPresetSet method defines all the factory presets for an audio unit. Basically,
// for each preset, it invokes a series of SetParameter calls.

OSStatus TremeloUnit::NewFactoryPresetSet(const AUPreset &inNewFactoryPreset) {
    
    SInt32 chosenPreset = inNewFactoryPreset.presetNumber;
    
    if (chosenPreset == kPreset_Slow || kPreset_Fast) {
        
        for (int i = 0; i < kNumberOfPresets; i++) {
            if (chosenPreset == kPresets[i].presetNumber) {
                // The settings for Factory preset "Slow & Gentle".
                switch (chosenPreset) {
                    case kPreset_Slow:
                        SetParameter(kParameter_Frequency, kParameter_Preset_Frequency_Slow);
                        SetParameter(kParameter_Depth, kParameter_Preset_Depth_Slow);
                        SetParameter(kParameter_Waveform, kParameter_Preset_Waveform_Slow);
                        break;
                // The settings for factory preset "Fast & Hard".
                    case kPreset_Fast:
                        SetParameter(kParameter_Frequency, kParameter_Preset_Frequency_Fast);
                        SetParameter(kParameter_Depth, kParameter_Preset_Depth_Fast);
                        SetParameter(kParameter_Waveform, kParameter_Preset_Waveform_Fast);
                        break;
                }
                SetAFactoryPresetAsCurrent(kPresets[i]);
                return noErr;
            }
        }
    }
    return kAudioUnitErr_InvalidProperty;
}

#pragma mark ____TremeloUnit DSP Kernel
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//    TremoloUnit::TremoloUnitKernel::TremoloUnitKernel()
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This is the constructor for the TremoloUnitKernel helper class, which holds the DSP code
//  for the audio unit. TremoloUnit is an n-to-n audio unit; one kernel object gets built for
//  each channel in the audio unit.
//
// The first line of the method consists of the constructor method declarator and constructor-
//  initializer. In addition to calling the appropriate superclasses, this code initializes two
//  member variables:
//
// mCurrentScale:        a factor for correlating points in the current wave table to
//                        the audio signal sampling frequency. to produce the desired
//                        tremolo frequency
// mSamplesProcessed:    a global count of samples processed. it allows the tremolo effect
//                        to be continuous over data input buffer boundaries
//
// (In the Xcode template, the header file contains the call to the superclass constructor.)
TremeloUnit::TremeloUnitKernel::TremeloUnitKernel(AUEffectBase *inAudioUnit) : AUKernelBase(inAudioUnit),
mSamplesProcessed(0), mCurrentScale(0) {
    
    // Generates a wave table that represents one cycle of a sine wave, normalized so that
    //  that it never goes negative and it ranges from 0 to 1; this sine wave represents
    // how to vary the volume during one cycle of tremelo.
    for (int i = 0; i < kWaveArraySize; i++) {
        double radians = i * 2.0 * M_PI / kWaveArraySize;
        mSine[i] = (sin(radians) + 1.0) * 0.5;
    }
    
    // Does the same for a psuedo square wave, with nice rounded corners to avoid pops.
    for (int i  = 0; i < kWaveArraySize; i++) {
        double radians = i * 2.0 * M_PI / kWaveArraySize;
        radians = radians + 0.32; // Push the wave over for a smoother start.
        mSquare[i] = (
                      sin(radians) + // Sums the odd harmonics, scaled for a nice final waveform.
                      0.3 * sin(3 * radians) +
                      0.15 * sin(5 * radians) +
                      0.075 * sin(7 * radians) +
                      0.0375 * sin(9 * radians) +
                      0.01875 * sin(11 * radians) +
                      0.009375 * sin(13 * radians) +
                      0.8           // Shifts the value fo it doesn't go negative
                      ) * 0.63;     // Scales the wave so the peak value is close
                                    //  to unity gain.
    }
    // Gets the samples per second of the audio stream from the host provided to audio unit.
    // Obtaining the value here in the constructor assumes the sample rate will not change
    // during the instanitation of the audio unit.
    mSampleFrequency = GetSampleRate();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//    TremoloUnit::TremoloUnitKernel::Reset()
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Because we're calculating each output sample based on a unique input sample, there's no
// need to clear any buffers. We simply reinitialize the variables that were initialized on
// instantiation of the kernel object.
void TremeloUnit::TremeloUnitKernel::Reset() {
    mCurrentScale = 0;
    mSamplesProcessed = 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//    TremoloUnit::TremoloUnitKernel::Process
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This method contains the DSP code.
void TremeloUnit::TremeloUnitKernel::Process(const Float32 *inSourceP,  // The audio sample input buffer.
                                             Float32 *inDestP,          // The audio sample output buffer
                                             UInt32 inSamplesToProcess, // The number of samples in the input buffer
                                             UInt32 inNumChannels,      // The number of input channels. This is always equal to 1
                                                                        //  there is always one kernel object instaniated
                                                                        //  per channel of audio.
                                             bool &ioSilence)           // A boolean flag indicating whether the input to the audio
                                                                        //  unit consists of silence, with a TRUE value indicating
                                                                        //  silence.
{
    // Ignores the request to perform the Process method if the input to the audio unit is silence.
    if (!ioSilence) {
        
        // Assigns a pointer variable to the start if the audio sample input buffer.
        const Float32 *sourceP = inSourceP;
        
        Float32 *destP = inDestP;           // A pointer variable to the start of the audio sample output buffer.
        Float32 inputSample;                // The current input sample to process.
        Float32 outputSample;               // The current output sample resulting from one iteration of
                                            //  the processing loop.
        Float32 tremeloFrequency;           // The tremelo frquency requested by the user via the audio units view.
        Float32 tremeloDepth;               // The tremelo depth requested by the user via the audio unit's view.
        Float32 samplesPerTremeloCycle;     // The number of audio samples in one cycle of the tremelo waveform.
        Float32 rawTremeloGain;             // The tremelo gain for the current audio sample, as stored in the wave table.
        Float32 tremeloGain;                // The adjusted tremelo gain of the current audio sample, considering the
                                            //  depth parameter value.
        
        int tremeloWaveform;                // The tremelo waveform type requested by the user via the audio unit's view.
        
        // Once per input buffer, get the tremelo frequency (in Hz) from the user via the audio unit's view.
        tremeloFrequency    = GetParameter(kParameter_Frequency);
        // Once per input buffer, get the tremelo depth (in percent %) from the user via the audio unit's view.
        tremeloDepth        = GetParameter(kParameter_Depth);
        // Once per input buffer, get the tremelo waveform type from the user via the audio unit's view.
        tremeloWaveform     = (int) GetParameter(kParameter_Waveform);
        
        // Assigns a pointer to the wave table for the user selected tremelo wave form.
        if (tremeloWaveform == kSineWave_Tremelo_Waveform) {
            waveArrayPointer = &mSine[0];
        } else {
            waveArrayPointer = &mSquare[0];
        }
        
        // Performs bounds checking on the parameters.
        if (tremeloFrequency < kMinimumValue_Tremelo_Freq) {
            tremeloFrequency = kMinimumValue_Tremelo_Freq;
        }
        if (tremeloFrequency > kMaximumValue_Tremelo_Freq) {
            tremeloFrequency = kMaximumValue_Tremelo_Freq;
        }
        
        if (tremeloDepth    < kMinimumValue_Tremelo_Depth) {
            tremeloDepth    = kMinimumValue_Tremelo_Depth;
        }
        if (tremeloDepth    > kMaximumValue_Tremelo_Depth) {
            tremeloDepth    = kMaximumValue_Tremelo_Depth;
        }
        
        // Calculate the number of audio sample per cycle of tremelo frequency.
        samplesPerTremeloCycle = mSampleFrequency / tremeloFrequency;
        
        // Calculate the scaling factor to use for applying the wave to the current sampling
        //  frequency and tremelo frequency.
        mNextScale = kWaveArraySize / samplesPerTremeloCycle;
        /*
            An explanation of the scaling factor (mNextScale)
            -------------------------------------------------
            Say that the audio sample frequency is 10 kHz and that the tremolo frequency is
            10.0 Hz. the number of audio samples per tremolo cycle is then 1,000.
            
            For a wave table of length 1,000, the scaling factor is then unity (1.0). This means
            that the wave table happens to be the exact size needed for each point in the table
            to correspond to exactly one sample.

            If the tremolo frequency slows to 1.0 Hz, then the number of samples per tremolo
            cycle rises to 10,000. The scaling factor is then 0.1. This means that every 10th
            element of the wave table array corresponds to a sample.
            
            If the tremolo frequency increases to 20 Hz, the samples per tremolo cycle lowers to
            500. The scaling factor is then 1,000/500 = 2.0. In this case, two samples in a row
            need to make use of the same point in the wave table.
        */
        
        // The sample processing loop; processes the current batch of samples, one sample at a time.
        for (int i = 0; i < inSamplesToProcess; i++) {
            
            // The following statement calculates the position in the wave table ("index") to
            // use for the current sample. This position, along with the calculation of
            // mNextScale, is the only subtle math for this audio unit.
            //
            // "index" is the position marker in the wave table. The wave table is an array;
            //        index varies from 0 to kWaveArraySize.
            //
            //    "index" is also the number of samples processed since the last
            //    counter reset, divided by the number of samples that play during one pass
            //    through the wave table, modulo the size of the wave table (see "An explanation...",
            //  above).
            
            int index = static_cast<long>(mSamplesProcessed * mCurrentScale) % kWaveArraySize;
            
            // If the user has moved the tremolo frequency slider, changes the scale factor
            // at the next positive zero crossing of the tremolo sine wave and resets the
            // mSamplesProcessed value so it stays in sync with the index position.
            if ((mNextScale != mCurrentScale) && (index == 0)) {
                mCurrentScale = mNextScale;
                mSamplesProcessed = 0;
            }
            
            // If the audio unit runs for a long time without the user moving the
            // tremolo frequency slider, resets the mSamplesProcessed value at the
            // next positive zero crossing of the tremolo sine wave.
            if ((mSamplesProcessed >= sampleLimit) && (index == 0)) {
                mSamplesProcessed = 0;
            }
            
            // Gets the raw tremelo gain from the appropiate wave table.
            rawTremeloGain = waveArrayPointer[index];
            
            // Calculates the finel tremelo gain according to the depth setting.
            tremeloGain = (rawTremeloGain * tremeloDepth - tremeloDepth + 100.0) * 0.01;
            
            // Gets the next input sample.
            inputSample = *sourceP;
            
            // Calculates the next output sample.
            outputSample = (inputSample * tremeloGain);
            
            // Stores the output sample in the output buffer.
            *destP = outputSample;
            
            // Advance to the next sample location in the input and output buffer.
            sourceP += 1;
            destP += 1;
            
            // Advance the global samples counter.
            mSamplesProcessed += 1;
        }
    }
}
