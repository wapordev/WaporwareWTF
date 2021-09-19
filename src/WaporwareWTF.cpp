#include <cmath>
#include <math.h>
#include <algorithm>
#include "plugin.hpp"

typedef float (*waveFunctions) (float phase);

float lerp(float a,float b,float t){
	return a + t * (b - a);
}

float sinwave(float phase){
	return std::sin(2.f * M_PI * phase);
}

float trianglewave(float phase){
	phase+=.25;
	if (phase >= 0.5f)
			phase -= 1.f;
	return 5*abs(phase)-1.25;
}

float sawwave(float phase){
	return phase*2;
}

float squarewave(float phase){
	return (round(phase/2-.5)+.5)*2;
}

struct WaporwareWTF : Module {
	enum ParamIds {
		DUAL_PARAM,
		INVERT_PARAM,
		NOISE_PARAM,
		NOISEMIX_PARAM,
		FRONT_PARAM,
		WIDTH_PARAM,
		REAR_PARAM,
		FRONTMOD_PARAM,
		WIDTHMOD_PARAM,
		REARMOD_PARAM,
		FM_PARAM,
		PITCH_PARAM,
		FINE_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		FRONT_INPUT,
		WIDTH_INPUT,
		REAR_INPUT,
		FM_INPUT,
		OCT_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		NOISE_OUTPUT,
		FRONT_OUTPUT,
		OUT_OUTPUT,
		REAR_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	WaporwareWTF() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(DUAL_PARAM, 0.f, 1.f, 0.f, "");
		configParam(INVERT_PARAM, 0.f, 1.f, 0.f, "");
		configParam(NOISE_PARAM, 0.f, 1.f, 0.f, "");
		configParam(NOISEMIX_PARAM, 0.f, 1.f, 0.f, "");
		configParam(FRONT_PARAM, 0.f, 0.99f, 0.f, "");
		configParam(WIDTH_PARAM, 0.f, 1.f, 0.f, "");
		configParam(REAR_PARAM, 0.f, 1.f, 0.f, "");
		configParam(FRONTMOD_PARAM, 0.f, 1.f, 0.f, "");
		configParam(WIDTHMOD_PARAM, 0.f, 1.f, 0.f, "");
		configParam(REARMOD_PARAM, 0.f, 1.f, 0.f, "");
		configParam(FM_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PITCH_PARAM, 0.f, 1.f, 0.f, "");
		configParam(FINE_PARAM, 0.f, 1.f, 0.5f, "");
	}

	float phase = 0.f;
	float noiseRate = 0.f;
	float noise = 0.f;

	void process(const ProcessArgs& args) override {
		waveFunctions waves[] = {sinwave, trianglewave, sawwave, squarewave};
		// Compute the frequency from the pitch parameter and input

		float pitch = params[PITCH_PARAM].getValue() + (params[FINE_PARAM].getValue()-.5)/6 + inputs[FM_INPUT].getVoltage()*params[FM_PARAM].getValue();
		pitch += inputs[OCT_INPUT].getVoltage();
		pitch = clamp(pitch, -4.f, 4.f);
		// The default pitch is C4 = 261.6256f
		float freq = dsp::FREQ_C4 * std::pow(2.f, pitch);

		// Accumulate the phase
		phase += freq * args.sampleTime;
		if (phase >= 0.5f)
			phase -= 1.f;

		float frontSlide = clamp(params[FRONT_PARAM].getValue()+inputs[FRONT_INPUT].getVoltage()*params[FRONTMOD_PARAM].getValue(),0.f,0.99f);
		float rearSlide = clamp(params[REAR_PARAM].getValue()+inputs[REAR_INPUT].getVoltage()*params[REARMOD_PARAM].getValue(),0.f,0.99f);
		int numWaves = ((sizeof(waves)/sizeof(waves[0]))-1);
		int iwaveFront = floor(frontSlide * numWaves);
		int iwaveRear = floor(rearSlide * numWaves);
		float front = 5*lerp(waves[iwaveFront](phase),waves[iwaveFront+1](phase),(frontSlide - floor(frontSlide*numWaves)/numWaves)*numWaves);
		float rear = 0;
		if (rearSlide!=1){
			rear = 5*lerp(waves[iwaveRear](phase),waves[iwaveRear+1](phase),(rearSlide - floor(rearSlide*numWaves)/numWaves)*numWaves);
		} else{
			rear = inputs[REAR_INPUT].getVoltage();
		}
		

		noiseRate += params[NOISE_PARAM].getValue() * 10000 * args.sampleTime;
		if (noiseRate >= 0.5f){
			noiseRate -= 1.f;
			noise = rand()/(RAND_MAX/10);
			noise = noise - 5;
			}
		if (params[INVERT_PARAM].getValue() >= .5) {
			rear = 0-rear;
		}
		bool window = false;
		float width = clamp(params[WIDTH_PARAM].getValue()+inputs[WIDTH_INPUT].getVoltage()*params[WIDTHMOD_PARAM].getValue(),0.f,1.f);;
		if (params[DUAL_PARAM].getValue() < .5) {
			window = phase < 0 + width/2 and phase > 0 - width/2;
		}else{
			window = (phase > -0.25 - width/4 and phase < -0.25 + width/4) or (phase > 0.25 - width/4 and phase < 0.25 + width/4);
		}
		outputs[NOISE_OUTPUT].setVoltage(noise);
		outputs[FRONT_OUTPUT].setVoltage(front);
		outputs[REAR_OUTPUT].setVoltage(rear);
		outputs[OUT_OUTPUT].setVoltage(front*(not window)+lerp(rear, noise, params[NOISEMIX_PARAM].getValue())*(window));
	}
};


struct WaporwareWTFWidget : ModuleWidget {
	WaporwareWTFWidget(WaporwareWTF* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/WaporwareWTF.svg")));

		addParam(createParamCentered<CKSS>(mm2px(Vec(15.24, 38.55)), module, WaporwareWTF::DUAL_PARAM));
		addParam(createParamCentered<CKSS>(mm2px(Vec(45.72, 38.55)), module, WaporwareWTF::INVERT_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(71.12, 38.55)), module, WaporwareWTF::NOISE_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(81.28, 38.55)), module, WaporwareWTF::NOISEMIX_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24, 57.825)), module, WaporwareWTF::FRONT_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(45.72, 57.825)), module, WaporwareWTF::WIDTH_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(76.2, 57.825)), module, WaporwareWTF::REAR_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24, 77.1)), module, WaporwareWTF::FRONTMOD_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(45.72, 77.1)), module, WaporwareWTF::WIDTHMOD_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(76.2, 77.1)), module, WaporwareWTF::REARMOD_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24, 96.375)), module, WaporwareWTF::FM_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(45.72, 96.375)), module, WaporwareWTF::PITCH_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(76.2, 96.375)), module, WaporwareWTF::FINE_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.16, 122.075)), module, WaporwareWTF::FRONT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(20.32, 122.075)), module, WaporwareWTF::WIDTH_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(30.48, 122.075)), module, WaporwareWTF::REAR_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(40.64, 122.075)), module, WaporwareWTF::FM_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(50.8, 122.075)), module, WaporwareWTF::OCT_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(76.2, 109.225)), module, WaporwareWTF::NOISE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(66.04, 122.075)), module, WaporwareWTF::FRONT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(76.2, 122.075)), module, WaporwareWTF::OUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(86.36, 122.075)), module, WaporwareWTF::REAR_OUTPUT));
	}
};


Model* modelWaporwareWTF = createModel<WaporwareWTF, WaporwareWTFWidget>("WaporwareWTF");