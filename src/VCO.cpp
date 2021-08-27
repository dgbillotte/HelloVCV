#include "plugin.hpp"

/*
 * Your sub-struct of Module contains all of the audio
 * processing or generation code. 
 */
struct VCO : Module {
	/*
	 * These enums represent all of the UI components in your module.
	 * ParamIds: knobs, sliders, switches, or other non-jack inputs or "parameters"
	 * InputIds: input jacks
	 * OutputIds: output jacks
	 * LightIds: LEDs
	 */
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

	/*
	 * These are member variables specific to this module
	 * _phase: keeps track of where the oscillator is at in its cycle
	 * _gain: how much to amplify the [-1..1] output of our oscillator
	 * _2PI: because process() uses it 3x per call
	 */
	float _phase = 0.f;
	float _gain = 5.f; // this results in a signal in [-5V..5V]
	float _2PI = 2.f * M_PI;

	/*
	 * This gets called when your module is added to the on-screen rack.
	 * Configure params and do any startup stuff that you shouldn't 
	 * be doing in process().
	 */
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

	/*
	 * process() gets called once for every sample period (44,100 times/sec
	 * or better) and is where your plugin does all of its audio processing
	 * or generation sorcery.
	 * 
	 * If you dilly dally in here or do stuff like write out log messages
	 * (which is really useful sometimes...)
	 * you will hear it as nasty crackles in the output :-(
	 * 
	 * If you have to do stuff other than process the audio sample, you
	 * should do it in another thread, but that's another example module...
	 */
	void process(const ProcessArgs& args) override {
		// get the base frequency from the frequency-knob
		float freq = params[FREQUENCY_PARAM].getValue();

		// calculate and write the sine output 		
		float sine_output = _gain * sin(_phase);
		outputs[SINE_OUTPUT].setVoltage(sine_output);

		// calculate and write the square output
		float square_output = _gain * ((sine_output >= 0) ? 1.f : -1.f);
		outputs[SQUARE_OUTPUT].setVoltage(square_output);

		// the rest of the function is to move the oscillator one step forward
		incrementPhase(freq, args.sampleRate);
	}

	/*
	 * I added this function just to keep process() short and to the point
	 * and to illustrate that you can break process() up and put the
	 * sub-functions here.
	 */ 
	void incrementPhase(float freq, float sampleRate) {
		// calculate the phase increment
		float phase_increment = (_2PI * freq) / sampleRate;

		// push the oscillator forward one step
		_phase += phase_increment;
		if(_phase >= _2PI) {
			_phase -= _2PI;
		}
	}
};

/*
 * The ModuleWidget sub-struct is where you layout all of the visual
 * aspects of your module.
 */
struct VCOWidget : ModuleWidget {
	VCOWidget(VCO* module) {
		setModule(module);

		// this loads the SVG image that is your module's background
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/VCO.svg")));

		// Add the screws to the UI. This is just for looks
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		// place a RoundLargeBlackKnob for the frequency knob
		addParam(createParamCentered<RoundLargeBlackKnob>(mm2px(Vec(10.792, 30.f)), module, VCO::FREQUENCY_PARAM));

		// place the output jacks
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.792, 95.f)), module, VCO::SINE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.792, 120.f)), module, VCO::SQUARE_OUTPUT));
	}
};

Model* modelVCO = createModel<VCO, VCOWidget>("VCO");