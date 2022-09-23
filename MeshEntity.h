#pragma once
#include <memory>
#include "Transform.h"
#include "Camera.h"
#include "Mesh.h"
#include "Material.h"

class MeshEntity
{
private:
	Mesh * pMesh;
	Material * pMaterial;
	Transform transform;
public: 
	MeshEntity(Mesh * mesh, Material * material);
	Mesh * GetMesh();
	Transform * const GetTransform();
	Material * GetMaterial();
	void SetMaterial(Material * material);
	void Draw(std::shared_ptr<Camera> camera, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
};

