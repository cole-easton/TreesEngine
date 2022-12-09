#pragma once

#include "DXCore.h"
#include "Lights.h"

#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include <memory>
#include <vector>

#include "SimpleShader.h"
#include "Mesh.h"
#include "MeshEntity.h"
#include "Camera.h"
#include "Skybox.h"

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

private:

	// Should we use vsync to limit the frame rate?
	bool vsync;

	// Shaders and shader-related constructs
	std::shared_ptr<SimplePixelShader> perturbationShader;
	std::shared_ptr<SimplePixelShader> basicLightingShader;
	std::shared_ptr<SimplePixelShader> transparencyShader;
	std::shared_ptr<SimplePixelShader> skyBoxPixelShader;
	std::shared_ptr<SimpleVertexShader> vertexShader;
	std::shared_ptr<SimpleVertexShader> skyBoxVertexShader;

	std::shared_ptr<Camera> camera;
	Transform* camTransform;

	Mesh* planeMesh;
	Mesh* sphereMesh;
	Mesh* cubeMesh;
	Mesh* tree1Mesh;
	Mesh* tree2Mesh;

	SkyBox* skyBox;

	std::vector<std::shared_ptr<MeshEntity>> trees;
	std::shared_ptr<MeshEntity> tree2instance1;
	std::shared_ptr<MeshEntity> player;
	std::shared_ptr<MeshEntity> ground;

	std::vector<std::shared_ptr<MeshEntity>> meshEntities;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> barkAlbedo;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> barkRoughness;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> barkNormals;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> barkMetalness;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> birchAlbedo;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> birchRoughness;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> birchNormals;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> grassAlbedo;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> grassRoughness;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> grassNormals;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> alumAlbedo;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> alumRoughness;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> alumNormals;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> alumMetalness;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skyBoxTex;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> refractionRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> refractionSRV;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> gammaCorrectionRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> gammaCorrectionSRV;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

	Material* bark;
	Material* birch;
	Material* aluminum;
	Material* grass;

	ID3D11BlendState* transparencyBlendState;

	std::vector<Light> lights;

	DirectX::XMFLOAT3 ambientColor;

	// Initialization helper methods - feel free to customize, combine, etc.
	void TestLSystem();
	void LoadShaders(); 
	void CreateBasicGeometry();
	void SetLights();
	void ResizeOnePostProcessResource(Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& rtv, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv);
	void replaceAll(std::string& str, const std::string& from, const std::string& to);

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//    Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr
	

};

