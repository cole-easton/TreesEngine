#pragma once
#include <DirectXMath.h>

class Transform
{
private:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 rotation;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 worldInverseTranspose;
	bool dirty;
public:
	Transform();
	Transform(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale);
	Transform(float posX, float posY, float posZ, float pitch, float yaw, float roll, float scaleX, float scaleY, float scaleZ);
	void SetPosition(float x, float y, float z);
	void SetRotation(float pitch, float yaw, float roll);
	void SetScale(float x, float y, float z);
	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetRotation(); 
	DirectX::XMFLOAT3 GetScale();
	DirectX::XMFLOAT3 GetForward();
	DirectX::XMFLOAT3 GetRight();
	DirectX::XMFLOAT3 GetUp(); //in case the transform falls asleep /s
	DirectX::XMFLOAT4X4 GetWorldMatrix();
	DirectX::XMFLOAT4X4 GetWorldInverseTransposeMatrix();
	void Translate(float x, float y, float z);
	void Move(float x, float y, float z); //I used an engine for years that used this translate/move distinction to mean absolute/relative
	void Turn(float pitch, float yaw, float roll);
	void Scale(float x, float y, float z);
};

