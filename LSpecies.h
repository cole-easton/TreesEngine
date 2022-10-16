#pragma once
#include <string>
#include<functional>
#include <vector>
#include "LState.h"

class LSpecies
{
private:
	std::function<std::string(std::string)> iterator;
	std::string axiom;
	float deltaInclination;
	float deltaAzimuth;
	float initialThickness;
	float thicknessDecay;
	float initialLimbLength;
	float limbLengthDecay;
	std::vector<LState> states;

public: 
	LSpecies(std::function<std::string(std::string)> iterator, std::string axiom, float deltaInclination, float deltaAzimuth, float initialThickness, float thicknessDecay, float initialLimbLength, float limbLengthDecay);
	std::string Grow(int iterations);
};

