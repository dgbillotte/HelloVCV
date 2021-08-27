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
<details>
<summary>
the basic structure of a module in VCV Rack:
</summary>

```c++
#include "plugin.hpp"

// your sub-struct of Module holds all of the logic for your module
struct VCO : Module {

    // define UI components in enums like this
	enum ParamIds {
		FREQUENCY_PARAM,
		NUM_PARAMS
	};
    // ...

	VCO() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		// configure params and do setup here
	}

	void process(const ProcessArgs& args) override {
        // args.sampleRate is useful sometimes

        // do all audio processing work here
	}
};

// your sub-struct of ModuleWidget is where you layout the UI of the widget
struct VCOWidget : ModuleWidget {
	VCOWidget(VCO* module) {
		setModule(module);

		// set the background image
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/VCO.svg")));

		// place components with calls like these
		addParam(createParamCentered<RoundLargeBlackKnob>(
            mm2px(Vec(10.792, 30.f)), module, VCO::FREQUENCY_PARAM));
		addOutput(createOutputCentered<PJ301MPort>(
            mm2px(Vec(10.792, 95.f)), module, VCO::SINE_OUTPUT));
	}
};

// this binds your module together into something Rack can work with
Model* modelVCO = createModel<VCO, VCOWidget>("VCO");
```
</details>

<details>
<summary>
the enums that define all of the UI components in a module:
</summary>

```c++
	enum ParamIds {
		FREQUENCY_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		NUM_INPUTS
	};
	enum OutputIds {
		SINE_OUTPUT,
		SQUARE_OUTPUT,
		NUM_OUTPUTS
	};
    enum LightIds {
		NUM_LIGHTS
	};
```
</details>

<details>
<summary>
how to configure params with min, max, and default values:

</summary>

```c++
	VCO() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

		/* 
		 * configure the frequency knob:
		 *   min-val: 0.f
		 *   max-val: 10000.f
		 *   default-val: 440.f
		 *   text-for-ui: ...
		 */
		configParam(FREQUENCY_PARAM, 0.f, 10000.f, 440.f, "Frequency in Hz");
	}
```

</details>

<details>
<summary>
how to get values from the module's parameters with the getValue() method:
</summary>

```c++
    // get the base frequency from the frequency-knob
    float freq = params[FREQUENCY_PARAM].getValue();
```
</details>

<details>
<summary>
how to write to the outputs with the setVoltage() method:

</summary>

```c++
    // calculate and write the sine output 		
    float sine_output = _gain * sin(_phase);
    outputs[SINE_OUTPUT].setVoltage(sine_output);
```
</details>

<details>
<summary>
More importantly is the phase loop that is setup to define and track the 
progress of the oscillator as it runs through its cycles. The logic for the
phase loop is contained in the incrementPhase() method... (read on)

</summary>

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

The basic idea is that we need to run a cycle that is exactly the length of one period of the waveform that we want to produce. Since we know that the sin() function repeats on the interval of [0..2PI] radians, we can define that as the length of our cycle. Now we need to determine how quickly we run through the cycle, this is called the phase-increment value and will have units of cycles/sample or radians/sample where 1 cycle = 2PI radians.

Given the sample-rate in samples/second and a target oscillator frequency in cycles/second, frequency/sample-rate gives cycles/sample. Multiplying that by 2PI produces radians/sample which is our phase increment value:
```c++
    float phase_increment = (_2PI * freq) / sampleRate;
```

From there, we add the phase_increment to the current phase. If that value is greater than 2PI, we have passed our interval and we need to wrap it back around to the beginning, which we do by subtracting 2PI from it. Since the exact end of a cycle will not likely ever be exactly on a sample boundary, we have to carry the little difference forward to the next cycle, otherwise discontinuities and aliasing will be introduced into the signal.

With this basic loop in place we can use any function that repeats on 2PI to produce our waveform.


</details>