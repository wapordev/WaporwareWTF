#pragma once
#include <rack.hpp>


using namespace rack;

// Declare the Plugin, defined in plugin.cpp
extern Plugin* pluginInstance;

// Declare each Model, defined in each module source file
// extern Model* modelMyModule;
extern Model* modelWaporwareWTF;

////////////////////////////////////////////// KNOBS //////////////////////////////////////////////

struct WaporwareRedKnob : RoundKnob {
	WaporwareRedKnob() {
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/WaporwareRedKnob.svg")));
	}
	void onChange(const event::Change &e) override {
		RoundKnob::onChange(e);
	}
};

struct WaporwareGreenKnob : RoundKnob {
	WaporwareGreenKnob() {
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/WaporwareGreenKnob.svg")));
	}
	void onChange(const event::Change &e) override {
		RoundKnob::onChange(e);
	}
};

struct WaporwareBlueKnob : RoundKnob {
	WaporwareBlueKnob() {
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/WaporwareBlueKnob.svg")));
	}
	void onChange(const event::Change &e) override {
		RoundKnob::onChange(e);
	}
};

struct WaporwareGrayKnob : RoundKnob {
	WaporwareGrayKnob() {
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/WaporwareGrayKnob.svg")));
	}
	void onChange(const event::Change &e) override {
		RoundKnob::onChange(e);
	}
};

struct WaporwareGrayKnobSmall : RoundKnob {
	WaporwareGrayKnobSmall() {
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/WaporwareGrayKnobSmall.svg")));
	}
	void onChange(const event::Change &e) override {
		RoundKnob::onChange(e);
	}
};