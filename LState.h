#pragma once

#include <DirectXMath.h>

struct LState {
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4X4 direction;
	float thickness;
	float length;
	LState(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3, float thickness, float length) :
		position(position), direction(direction), thickness(thickness), length(length) {};
};