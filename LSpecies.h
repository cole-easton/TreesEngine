#pragma once
#include <string>
#include<functional>
#include <vector>
#include "LState.h"
#include "Mesh.h"

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

public: 
	LSpecies(std::function<std::string(std::string)> iterator, std::string axiom, float deltaInclination, float deltaAzimuth, float initialThickness, float thicknessDecay, float initialLimbLength, float limbLengthDecay);
	std::string Grow(int iterations);
	Mesh* Build(const std::string& rule, Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
};

