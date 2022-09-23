#pragma once
#include <DirectXMath.h>
#include "Transform.h"
class Camera
{
private:
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;
	Transform transform;
	float frustumRadians;
	float nearPlane;
	float farPlane;
	float movementSpeed;
	float mouseLookSpeed;
public:
	Camera(float x, float y, float z, float aspectRatio);
	Camera(Transform transform, float aspectRatio);
	Camera(Transform transform, float aspectRatio, float frustumRadians, float nearPlane, float farPlane, float movementSpeed, float mouseLookSpeed);
	DirectX::XMFLOAT4X4 GetViewMatrix();
	DirectX::XMFLOAT4X4 GetProjectionMatrix();
	Transform GetTransform();
	void UpdateProjectionMatrix(float aspectRatio);
	void UpdateViewMatrix();
	void Update(float dt);
};

