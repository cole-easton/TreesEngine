#include <DirectXMath.h>
#include "MeshEntity.h"

using namespace DirectX;

MeshEntity::MeshEntity(Mesh* mesh, Material * material)
{
	pMesh = mesh;
	pMaterial = material;
	transform = Transform();
}

Mesh* MeshEntity::GetMesh()
{
	return pMesh;
}

Transform* const MeshEntity::GetTransform()
{
	return &transform;
}

Material* MeshEntity::GetMaterial()
{
	return pMaterial;
}

void MeshEntity::SetMaterial(Material* material)
{
	pMaterial = material;
}

void MeshEntity::Draw(std::shared_ptr<Camera> camera, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
	std::shared_ptr<SimpleVertexShader> vs = pMaterial->GetVertexShader(); 
	vs->SetMatrix4x4("world", transform.GetWorldMatrix()); 
	vs->SetMatrix4x4("worldInvTranspose", transform.GetWorldInverseTransposeMatrix());
	vs->SetMatrix4x4("view", camera->GetViewMatrix());            
	vs->SetMatrix4x4("projection", camera->GetProjectionMatrix()); 
	vs->CopyAllBufferData();

	std::shared_ptr<SimplePixelShader> ps = pMaterial->GetPixelShader();
	ps->SetFloat4("colorTint", pMaterial->GetColorTint());
	ps->CopyAllBufferData();

	pMaterial->GetVertexShader()->SetShader();
	pMaterial->GetPixelShader()->SetShader();
	pMesh->Draw();
}
