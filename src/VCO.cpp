#include "plugin.hpp"


struct VCO : Module {
	enum ParamIds {
		FREQUENCY_PARAM,
		FREQUENCY_CV_ATTN_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		VOCT_INPUT,
		FREQUENCY_CV_INPUT,
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

	VCO() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(FREQUENCY_PARAM, 0.f, 1.f, 0.f, "");
		configParam(FREQUENCY_CV_ATTN_PARAM, 0.f, 1.f, 0.f, "");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct VCOWidget : ModuleWidget {
	VCOWidget(VCO* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/VCO.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.792, 19.932)), module, VCO::FREQUENCY_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.792, 54.451)), module, VCO::FREQUENCY_CV_ATTN_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.792, 38.597)), module, VCO::VOCT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.792, 65.871)), module, VCO::FREQUENCY_CV_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.792, 108.329)), module, VCO::SINE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.792, 119.482)), module, VCO::SQUARE_OUTPUT));
	}
};


Model* modelVCO = createModel<VCO, VCOWidget>("VCO");