# HelloVCV: A Starting Point

HelloVCV is a simple plugin for VCV Rack to illustrate how to get started
doing audio programming with C++ and VCV. Below are a listing of the modules
contained in the plugin, instructions to build it, and some discussion about the methods used within each module. It was developed with v1.6.1 of the
VCV Rack SDK.

## Pre-Requsites
See my [article](TBD) for a quick run through of how to:
- Install VCV Rack
- Setup and test your build environment


## How to build and install the plugin
```bash
> git clone git@github.com:dgbillotte/HelloVCV.git
> cd HelloVCV
> make install
> # restart VCV Rack
```

## Modules Contained
As of now, this plugin contains a simple oscillator called VCO.

### VCO : A Simple Oscillator
This is almost the simplest oscillator one could write while still being
slightly interesting. It does have a Hz knob to twiddle... and **TWO** different
output waveforms (sine and square-wave).

In this module you can see:
- the basic structure of a module in VCV Rack
- the enums that define all of the UI components in a module
- how to get values from the module's parameters with the getValue() method
- how to write to the outputs with the setVoltage() method

More importantly is the phase loop that is setup to define and track the 
progress of the oscillator as it runs through its cycles. The logic for the
phase loop is contained in the incrementPhase() method.

```c++
void incrementPhase(float freq, float sampleRate) {
    // calculate the phase increment
    float phase_increment = (_2PI * freq) / sampleRate;

    // push the oscillator forward one step
    _phase += phase_increment;
    if(_phase >= _2PI) {
        _phase -= _2PI;
    }
}
```

Some things to note:
- _phase represents the current position in the phase loop
- _phase is never reset to 0.f after initialization, but gets 2PI subtracted from it so that the little bits left over don't get lost.
- the interval of the cycle is [0..2PI]

The basic idea is that we need to run a cycle that is exactly the length of the frequency of the waveform that we want to produce. Since we know that the sin() function repeats on the interval of [0..2PI] radians, we can define that as the length of our cycle. Now we need to determine how quickly we run through the cycle, this is called the phase-increment value and will have units of cycles/sample or radians/sample where 1 cycle = 2PI radians.

Given the sample-rate in samples/second and a target oscillator frequency in cycles/second, frequency/sample-rate gives cycles/sample. Multiplying that by 2PI produces radians/sample which is our phase increment value:
```c++
float phase_increment = (_2PI * freq) / sampleRate;
```

From there, we add the phase_increment to the current_phase. If that value is greater than 2PI, we have passed our interval and we need to wrap it back around to the beginning, which we do by subtracting 2PI from it. Since the exact end of a cycle will not likely ever be exactly on a sample boundary, we have to carry the little difference forward to the next cycle, otherwise discontinuities and aliasing will be introduced into the signal.

With this basic loop in place we can use any function that repeats on 2PI to produce our waveform.

