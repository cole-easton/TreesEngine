#include <stdio.h>
#include <iostream>
#include "Camera.h"
#include "Input.h"

using namespace DirectX;

Camera::Camera(float x, float y, float z, float aspectRatio) : Camera(new Transform(x, y, z, 0, 0, 0, 1, 1, 1), aspectRatio) {}

Camera::Camera(Transform* transform, float aspectRatio) : Camera(transform, aspectRatio, 1, 0.01, 1000) {}

Camera::Camera(Transform* transform, float aspectRatio, float frustumRadians, float nearPlane, float farPlane)
{
	this->transform = transform;
	this->frustumRadians = frustumRadians;
	this->nearPlane = nearPlane;
	this->farPlane = farPlane;
	this->movementSpeed = movementSpeed;
	this->mouseLookSpeed = mouseLookSpeed;
	XMStoreFloat4x4(&(this->viewMatrix), XMMatrixLookToLH(XMLoadFloat3(&(transform->GetPosition())), XMLoadFloat3(&(transform->GetForward())), XMLoadFloat3(&(transform->GetUp()))));
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

Transform* Camera::GetTransform()
{
	return transform;
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
	XMStoreFloat4x4(&(this->projectionMatrix), XMMatrixPerspectiveFovLH(frustumRadians, aspectRatio, nearPlane, farPlane));
}

void Camera::UpdateViewMatrix()
{
	XMStoreFloat4x4(&(this->viewMatrix), XMMatrixLookToLH(XMLoadFloat3(&(transform->GetPosition())), XMLoadFloat3(&(transform->GetForward())), XMLoadFloat3(&(transform->GetUp()))));
}

void Camera::Update(float dt)
{
	UpdateViewMatrix();
}
