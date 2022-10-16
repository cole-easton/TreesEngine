#include "LSpecies.h"
//
LSpecies::LSpecies(std::function<std::string(std::string)> iterator, std::string axiom, float deltaInclination, float deltaAzimuth, float initialThickness, float thicknessDecay, float initialLimbLength, float limbLengthDecay) {
	this->iterator = iterator;
	this->axiom = axiom;
	this->deltaInclination = deltaInclination;
	this->deltaAzimuth = deltaAzimuth;
	this->thicknessDecay = thicknessDecay;
	this->limbLengthDecay = limbLengthDecay;
	this->initialLimbLength = initialLimbLength;
	this->initialThickness = initialThickness;
}
std::string LSpecies::Grow(int iterations) {
	states.push_back(LState(DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 1, 0), initialThickness, initialLimbLength));
	std::string rule = axiom;
	for (int i = 0; i < iterations; ++i) {
		rule = iterator(rule);
	}
	return rule;
}