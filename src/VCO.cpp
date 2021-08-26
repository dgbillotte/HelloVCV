#include "plugin.hpp"


struct VCO : Module {
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

	float _phase = 0.f;
	float _gain = 5.f; // this results in signal in [-5V..5V]


	VCO() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

		// configure the frequency knob or "param":
		// min-val: 0.0
		// max-val: 10,000.0
		// default-val: 440.0
		// description that shows up in the UI
		configParam(FREQUENCY_PARAM, 0.f, 10000.f, 440.f, "Frequency in Hz");
	}

	/*
	 * This is where your plugin does all of its work.
	 */
	void process(const ProcessArgs& args) override {
		// get the base frequency from the frequency-knob
		float freq = params[FREQUENCY_PARAM].getValue();

		// calculate the site output 		
		float sine_output = sin(_phase);

		// write the sine output
		outputs[SINE_OUTPUT].setVoltage(sine_output * _gain);

		// calculate and write the square output
		float square_output = (sine_output >= 0) ? 1.f : -1.f;
		outputs[SQUARE_OUTPUT].setVoltage(square_output * _gain);

		// move the oscillator forward one step
		stepForward(freq, args.sampleRate);
	}

	/*
	 * I added this function just to keep process() short and to the point
	 */
	void stepForward(float freq, int sampleRate) {
		// calculate the phase increment
		float phase_increment = (2 * M_PI * freq) / sampleRate;

		// push the oscillator forward one step (the phase increment)
		_phase += phase_increment;
		if(_phase >= 2 * M_PI) {
			_phase -= 2 * M_PI;
		}
	}
};


struct VCOWidget : ModuleWidget {
	VCOWidget(VCO* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/VCO.svg")));

		// Add the screws to the UI. This is just for looks
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		// place a RoundLargeBlackKnob for the frequency knob ("param")
		addParam(createParamCentered<RoundLargeBlackKnob>(mm2px(Vec(10.792, 30.f)), module, VCO::FREQUENCY_PARAM));

		// place the output jacks
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.792, 95.f)), module, VCO::SINE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.792, 120.f)), module, VCO::SQUARE_OUTPUT));
	}
};


Model* modelVCO = createModel<VCO, VCOWidget>("VCO");