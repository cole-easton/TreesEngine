#include "LSpecies.h"
#include "LState.h"
#include "Vertex.h"
#include <vector>

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
	std::string rule = axiom;
	for (int i = 0; i < iterations; ++i) {
		rule = iterator(rule);
	}
	return rule;
}

Mesh* LSpecies::Build(const std::string& rule, Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
	DirectX::XMFLOAT4X4 initRotation;
	DirectX::XMStoreFloat4x4(&initRotation, DirectX::XMMatrixRotationRollPitchYaw(DirectX::XM_PIDIV2, 0, 0));
	LState state(DirectX::XMFLOAT3(0, 0, 0), initRotation, initialThickness, initialLimbLength);
	std::vector<LState>* savedStates = new std::vector<LState>();
	const int numSides = 5;
	unsigned int fCount = 0;
	for (int i = 0; i < rule.length(); ++i) {
		if (rule[i] == 'F') {
			++fCount;
		}
	}
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	unsigned int vertexIndex = 0;
	for (unsigned int i = 0; i < rule.length(); ++i) {
		const DirectX::XMFLOAT3 forward = DirectX::XMFLOAT3(state.direction._13, state.direction._23, state.direction._33);
		DirectX::XMFLOAT3 oldForward;
		if (savedStates->size() >= 1) {
			LState prev = savedStates->back();
			oldForward = DirectX::XMFLOAT3(prev.direction._13, prev.direction._23, prev.direction._33);
		}
		switch (rule[i])
		{
		case 'F':
			const DirectX::XMFLOAT3 right = DirectX::XMFLOAT3(state.direction._11, state.direction._21, state.direction._31);
			//construct ring of verts around current draw pos
			for (int j = 0; j < numSides; j++) {
				Vertex vert = {};
				DirectX::XMStoreFloat3(&vert.Position, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&state.position), DirectX::XMVector3Transform(DirectX::XMVectorScale(DirectX::XMLoadFloat3(&right),state.thickness/2), DirectX::XMMatrixRotationRollPitchYaw(0, DirectX::XM_2PI * ((float)j) / numSides, 0))));
				vertices.push_back(vert);
			}
			//move draw position forward by length
			DirectX::XMStoreFloat3(&state.position, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&state.position), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&forward), state.length)));
			// construct ring of verts around new draw pos
			for (unsigned int j = 0; j < numSides; j++) {
				Vertex vert = {};
				DirectX::XMStoreFloat3(&vert.Position, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&state.position), DirectX::XMVector3Transform(DirectX::XMVectorScale(DirectX::XMLoadFloat3(&right), state.thickness / 2), DirectX::XMMatrixRotationRollPitchYaw(0, DirectX::XM_2PI * ((float)j) / numSides, 0))));
				vertices.push_back(vert);
			}
			for (unsigned int j = vertexIndex; j < vertexIndex + numSides; ++j) {
				indices.push_back(j);
				indices.push_back((j == vertexIndex + numSides - 1) ? vertexIndex + numSides : j + numSides + 1);
				indices.push_back(j + numSides);
				
				indices.push_back(j);
				indices.push_back((j == vertexIndex + numSides - 1) ? vertexIndex : j + 1);
				indices.push_back((j == vertexIndex + numSides - 1) ? vertexIndex + numSides : j + numSides + 1);
			} 

			vertexIndex += numSides * 2;
			break;
		case '+':
			DirectX::XMStoreFloat4x4(&state.direction, DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationRollPitchYaw(0, 0, deltaInclination), DirectX::XMLoadFloat4x4(&state.direction)));
			break;
		case '-':
			DirectX::XMStoreFloat4x4(&state.direction, DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationRollPitchYaw(0, 0, -deltaInclination), DirectX::XMLoadFloat4x4(&state.direction)));
			break;
		case '>':
			DirectX::XMStoreFloat4x4(&state.direction, DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&oldForward), deltaAzimuth), DirectX::XMLoadFloat4x4(&state.direction)));
			break;
		case '<':
			DirectX::XMStoreFloat4x4(&state.direction, DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&oldForward), -deltaAzimuth), DirectX::XMLoadFloat4x4(&state.direction)));
			break;
		case '[':
				savedStates->push_back(state);
				break;
		case ']':
			state = savedStates->back();
			savedStates->pop_back();
			break;
		case '#':
			state.thickness *= thicknessDecay;
			break;
		case '$':
			state.length *= limbLengthDecay;
		default:
			break;
		}
	}
	delete savedStates;
	Mesh* mesh = new Mesh(&vertices[0], vertexIndex, &indices[0], indices.size(), device, context);
	return mesh;
}


