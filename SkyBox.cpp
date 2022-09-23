#include "SkyBox.h"
#include "DXCore.h"

SkyBox::SkyBox(Mesh* skyMesh, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeMapSRV, std::shared_ptr<SimpleVertexShader> vertexShader, std::shared_ptr<SimplePixelShader> pixelShader, Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState, Microsoft::WRL::ComPtr<ID3D11Device> device)
{
	this->skyMesh = skyMesh;
	this->cubeMapSRV = cubeMapSRV;
	this->samplerState = samplerState;
	this->vertexShader = vertexShader;
	this->pixelShader = pixelShader;
	D3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.CullMode = D3D11_CULL_FRONT;
	device->CreateRasterizerState(&rastDesc, rasterizerState.GetAddressOf());

	D3D11_DEPTH_STENCIL_DESC stencilDesc = {};
	stencilDesc.DepthEnable = true;
	stencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	device->CreateDepthStencilState(&stencilDesc, depthStencilState.GetAddressOf());
}

void SkyBox::Draw(std::shared_ptr<Camera> camera, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
	context->RSSetState(rasterizerState.Get());
	context->OMSetDepthStencilState(depthStencilState.Get(), 0);
	vertexShader->SetShader();
	vertexShader->SetMatrix4x4("view", camera->GetViewMatrix());
	vertexShader->SetMatrix4x4("projection", camera->GetProjectionMatrix());
	vertexShader->CopyAllBufferData();
	pixelShader->SetShader();
	pixelShader->SetSamplerState("Sampler", samplerState);
	pixelShader->SetShaderResourceView("CubeMap", cubeMapSRV);
	pixelShader->CopyAllBufferData();
	
	skyMesh->Draw();
	context->RSSetState(0);
	context->OMSetDepthStencilState(0, 0);
}

SkyBox::~SkyBox()
{
}
