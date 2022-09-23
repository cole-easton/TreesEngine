#pragma once
#include <wrl/client.h>
#include <memory>
#include "Mesh.h"
#include "Camera.h"
#include "SimpleShader.h"
class SkyBox
{
private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeMapSRV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	Mesh* skyMesh;
	std::shared_ptr<SimplePixelShader> pixelShader;
	std::shared_ptr<SimpleVertexShader> vertexShader;
public:
	SkyBox(Mesh* skyMesh, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeMapSRV, std::shared_ptr<SimpleVertexShader> vertexShader, std::shared_ptr<SimplePixelShader> pixelShader, Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState, Microsoft::WRL::ComPtr<ID3D11Device> device);
	void Draw(std::shared_ptr<Camera> camera, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
	~SkyBox();
};

