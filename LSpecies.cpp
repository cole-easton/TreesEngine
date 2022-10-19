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
	DirectX::XMStoreFloat4x4(&initRotation, DirectX::XMMatrixRotationRollPitchYaw(0, DirectX::XM_PIDIV2, 0));
	LState state(DirectX::XMFLOAT3(0, 0, 0), initRotation, initialThickness, initialLimbLength);
	std::vector<LState>* savedStates = new std::vector<LState>();
	const int numSides = 5;
	unsigned int fCount = 0;
	for (int i = 0; i < rule.length(); ++i) {
		if (rule[i] == 'F') {
			++fCount;
		}
	}
	Vertex* vertices = new Vertex[40];
	unsigned int* indices = new unsigned int[40];
	unsigned int vertexIndex = 0;
	unsigned int indexIndex = 0;
	for (unsigned int i = 0; i < rule.length(); ++i) {
		switch (rule[i])
		{
		case 'F':
			DirectX::XMFLOAT3 left = DirectX::XMFLOAT3(state.direction._11, state.direction._12, state.direction._13);
			DirectX::XMFLOAT3 forward = DirectX::XMFLOAT3(state.direction._31, state.direction._32, state.direction._33);
			//construct ring of verts around current draw pos
			for (int j = 0; j < numSides; j++) {
				Vertex vert = {};
				DirectX::XMStoreFloat3(&vert.Position, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&state.position), DirectX::XMVector3Transform(DirectX::XMVectorScale(DirectX::XMLoadFloat3(&left),state.thickness/2), DirectX::XMMatrixRotationRollPitchYaw(0, 0, DirectX::XM_2PI * ((float)j) / numSides))));
				vertices[vertexIndex++] = vert;
			}
			//move draw position forward by length
			DirectX::XMStoreFloat3(&state.position, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&state.position), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&forward), state.length)));
			// construct ring of verts around new draw pos
			for (unsigned int j = 0; j < numSides; j++) {
				Vertex vert = {};
				DirectX::XMStoreFloat3(&vert.Position, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&state.position), DirectX::XMVector3Transform(DirectX::XMVectorScale(DirectX::XMLoadFloat3(&left), state.thickness / 2), DirectX::XMMatrixRotationRollPitchYaw(0, 0, DirectX::XM_2PI * ((float)j) / numSides))));
				vertices[vertexIndex++] = vert;
			}
			for (unsigned int j = vertexIndex - 2 * numSides; j < vertexIndex - numSides; ++j) {
				indices[indexIndex++] = j;
				indices[indexIndex++] = j+numSides;
				indices[indexIndex++] = (j+numSides+1==vertexIndex)?vertexIndex-numSides+1:j+numSides+1;

				indices[indexIndex++] = j;
				indices[indexIndex++] = (j + numSides + 1 == vertexIndex) ? vertexIndex - numSides + 1 : j + numSides + 1;
				indices[indexIndex++] = (j+1==vertexIndex-numSides+1)?vertexIndex-2*numSides:j+1;
			} 
			break;
		case '+':
			DirectX::XMStoreFloat4x4(&state.direction, DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationRollPitchYaw(deltaInclination, 0, 0), DirectX::XMLoadFloat4x4(&state.direction)));
			break;
		case '-':
			DirectX::XMStoreFloat4x4(&state.direction, DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationRollPitchYaw(-deltaInclination, 0, 0), DirectX::XMLoadFloat4x4(&state.direction)));
			break;
		case '>':
			DirectX::XMStoreFloat4x4(&state.direction, DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationRollPitchYaw(0, 0, deltaInclination), DirectX::XMLoadFloat4x4(&state.direction)));
			break;
		case '<':
			DirectX::XMStoreFloat4x4(&state.direction, DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationRollPitchYaw(0, 0, -deltaInclination), DirectX::XMLoadFloat4x4(&state.direction)));
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
	return new Mesh(vertices, vertexIndex, indices, indexIndex, device, context);
	delete[] vertices;
	delete[] indices;
	delete savedStates;
}


