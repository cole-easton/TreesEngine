#include <stdio.h>
#include <iostream>
#include "Camera.h"
#include "Input.h"

using namespace DirectX;

Camera::Camera(float x, float y, float z, float aspectRatio) : Camera(Transform(x, y, z, 0, 0, 0, 1, 1, 1), aspectRatio) {}

Camera::Camera(Transform transform, float aspectRatio) : Camera(transform, aspectRatio, 1, 0.01, 1000, 4, 1) {}

Camera::Camera(Transform transform, float aspectRatio, float frustumRadians, float nearPlane, float farPlane, float movementSpeed, float mouseLookSpeed)
{
	this->transform = transform;
	this->frustumRadians = frustumRadians;
	this->nearPlane = nearPlane;
	this->farPlane = farPlane;
	this->movementSpeed = movementSpeed;
	this->mouseLookSpeed = mouseLookSpeed;
	XMStoreFloat4x4(&(this->viewMatrix), XMMatrixLookToLH(XMLoadFloat3(&(transform.GetPosition())), XMLoadFloat3(&(transform.GetForward())), XMLoadFloat3(&(transform.GetUp()))));
	XMStoreFloat4x4(&(this->projectionMatrix), XMMatrixPerspectiveFovLH(frustumRadians, aspectRatio, nearPlane, farPlane));
}

XMFLOAT4X4 Camera::GetViewMatrix()
{
	return viewMatrix;
}

XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return projectionMatrix;
}

Transform Camera::GetTransform()
{
	return transform;
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
	XMStoreFloat4x4(&(this->projectionMatrix), XMMatrixPerspectiveFovLH(frustumRadians, aspectRatio, nearPlane, farPlane));
}

void Camera::UpdateViewMatrix()
{
	XMStoreFloat4x4(&(this->viewMatrix), XMMatrixLookToLH(XMLoadFloat3(&(transform.GetPosition())), XMLoadFloat3(&(transform.GetForward())), XMLoadFloat3(&(transform.GetUp()))));
}

void Camera::Update(float dt)
{
	Input& input = Input::GetInstance();
	//I am intentionally not using else-ifs -- I want forward and backward to cancel and to be able to move diagonally
	if (input.KeyDown('W') || input.KeyDown(VK_UP)) {
		transform.Move(0, 0, movementSpeed * dt);
	}
	if (input.KeyDown('S') || input.KeyDown(VK_DOWN)) {
		transform.Move(0, 0, -movementSpeed * dt);
	}
	if (input.KeyDown('A')) {
		transform.Move(-movementSpeed * dt, 0, 0);
	}
	if (input.KeyDown('D')) {
		transform.Move(movementSpeed * dt, 0, 0);
	}
	if (input.KeyDown(VK_LEFT)) {
		transform.Turn(0, -dt, 0);
	}
	if (input.KeyDown(VK_RIGHT)) {
		transform.Turn(0, dt, 0);
	}
	if (input.KeyDown(VK_SPACE)) {
		transform.Translate(0, movementSpeed * dt, 0);
	}
	if (input.KeyDown('X')) {
		transform.Translate(0, -movementSpeed * dt, 0);
	}

	if (input.MouseLeftDown())
	{
		int cursorMovementX = input.GetMouseXDelta();    
		int cursorMovementY = input.GetMouseYDelta();
		transform.Turn(-cursorMovementY * mouseLookSpeed * dt, -cursorMovementX * mouseLookSpeed * dt, 0);
		XMFLOAT3 rotation = transform.GetRotation();
		if (rotation.y < -XM_PI / 2) {
			rotation.y = -XM_PI / 2;
		}
		else if (rotation.y > XM_PI / 2) {
			rotation.y = XM_PI / 2;
		}
		if (rotation.x < -XM_PI / 2) {
			rotation.x = -XM_PI / 2;
		}
		else if (rotation.x > XM_PI / 2) {
			rotation.x = XM_PI / 2;
		}
		transform.SetRotation(rotation.x, rotation.y, rotation.z);
	}
	UpdateViewMatrix();
}
