#include <DDSTextureLoader.h>
#include <algorithm> //for std::sort
#include "Game.h"
#include "Vertex.h"
#include "Input.h"
#include "Lights.h"
#include "Sphere.h"
#include "WICTextureLoader.h"
#include "LSpecies.h"
#include <iostream>

// Needed for a helper function to read compiled shader files from the hard drive
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		   // The application's handle
		"DirectX Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true),			   // Show extra stats (fps) in title bar?
	vsync(false)
{
#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	delete metalHatchMaterial;
	delete transparentMaterial;
	delete cubeMesh;
	delete tree1Mesh;
	delete skyBox;
	delete tree1Mesh;
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	CreateBasicGeometry();
	TestLSystem();
	SetLights();
	camera = std::make_shared<Camera>(Transform(0, 0, -10, 0, 0, 0, 1, 1, 1), (float)this->width / this->height);
	ambientColor = XMFLOAT3(0.15f, 0.15f, 0.25f);
	
	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D11_BLEND_DESC bd = {};
	bd.AlphaToCoverageEnable = false;
	bd.IndependentBlendEnable = false;
	bd.RenderTarget[0].BlendEnable = true;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	device->CreateBlendState(&bd, &transparencyBlendState);
}

void Game::TestLSystem() {
	LSpecies* species1 = new LSpecies([this](std::string rule) {
		this->replaceAll(rule, "X", "F[-[#$FX]<[#$FX]<[#$FX]]");
		return rule;
		}, std::string("X"), DirectX::XM_PI/6, 2*DirectX::XM_PI/3, 0.3f, 0.7f, 1.5f, 0.8f);
	std::string rule = species1->Grow(3);
	printf(rule.c_str());
	tree1Mesh = species1->Build(rule, device, context);
	tree1instance1 = std::make_shared<MeshEntity>(tree1Mesh, metalHatchMaterial);
	tree1instance1->GetTransform()->SetPosition(0, -2, 0);
	meshEntities.push_back(tree1instance1);
	delete species1;
}


// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	vertexShader = std::make_shared<SimpleVertexShader>(device, context, GetFullPathTo_Wide(L"VertexShader.cso").c_str());    
	skyBoxVertexShader = std::make_shared<SimpleVertexShader>(device, context, GetFullPathTo_Wide(L"SkyBoxVertexShader.cso").c_str());
	skyBoxPixelShader = std::make_shared<SimplePixelShader>(device, context, GetFullPathTo_Wide(L"SkyBoxPixelShader.cso").c_str());
	basicLightingShader = std::make_shared<SimplePixelShader>(device, context, GetFullPathTo_Wide(L"BasicLightingPixelShader.cso").c_str());
	transparencyShader = std::make_shared<SimplePixelShader>(device, context, GetFullPathTo_Wide(L"TransparencyPixelShader.cso").c_str());
	transparencyShader = std::make_shared<SimplePixelShader>(device, context, GetFullPathTo_Wide(L"PerturbationShader.cso").c_str());
}

void Game::SetLights() {
	Light directionalLight1 = {};
	directionalLight1.type = LIGHT_TYPE_DIRECTIONAL;
	directionalLight1.color = XMFLOAT3(1, 1, 1);
	directionalLight1.direction = XMFLOAT3(1, 0, 0);
	directionalLight1.intensity = 0.6f;

	Light directionalLight2 = {};
	directionalLight2.type = LIGHT_TYPE_DIRECTIONAL;
	directionalLight2.color = XMFLOAT3(1, 1, 1);
	directionalLight2.direction = XMFLOAT3(-1, -1, 0);
	directionalLight2.intensity = 0.9f;

	Light directionalLight3 = {};
	directionalLight3.type = LIGHT_TYPE_DIRECTIONAL;
	directionalLight3.color = XMFLOAT3(1, 1, 1);
	directionalLight3.direction = XMFLOAT3(0, 0, 1);
	directionalLight3.intensity = 1.3f;

	Light pointLight1 = {};
	pointLight1.type = LIGHT_TYPE_POINT;
	pointLight1.color = XMFLOAT3(1, 1, 1);
	pointLight1.position = XMFLOAT3(-4, 1.5, 0);
	pointLight1.intensity = 1;
	pointLight1.range = 3;

	Light pointLight2 = {};
	pointLight2.type = LIGHT_TYPE_POINT;
	pointLight2.color = XMFLOAT3(1, 1, 1);
	pointLight2.position = XMFLOAT3(6, -2, -2);
	pointLight2.intensity = 0.8f;
	pointLight2.range = 3;

	lights.push_back(directionalLight1);
	lights.push_back(directionalLight2);
	lights.push_back(directionalLight3);
	lights.push_back(pointLight1);
	lights.push_back(pointLight2);
}

// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/metalhatch_albedo.tif").c_str(), 0, metalHatchTex.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/metalhatch_roughness.tif").c_str(), 0, metalHatchRoughness.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/metalhatch_normal.tif").c_str(), 0, metalHatchNormal.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/metalhatch_metalness.tif").c_str(), 0, metalHatchMetalness.GetAddressOf());
	CreateDDSTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/SunnyCubeMap.dds").c_str(), 0, skyBoxTex.GetAddressOf());

	D3D11_SAMPLER_DESC desc = {};
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.Filter = D3D11_FILTER_ANISOTROPIC;
	desc.MaxAnisotropy = 8; //1-16, higher is slower -- possibly adjust this later
	desc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&desc, samplerState.GetAddressOf());

	metalHatchMaterial = new Material(XMFLOAT4(1, 1, 1, 1), vertexShader, basicLightingShader);
	transparentMaterial = new Material(XMFLOAT4(1, 0, 0, 0.1f), vertexShader, transparencyShader, 0.1f);

	metalHatchMaterial->AddTextureSRV("Albedo", metalHatchTex);
	metalHatchMaterial->AddTextureSRV("RoughnessMap", metalHatchRoughness);
	metalHatchMaterial->AddTextureSRV("NormalMap", metalHatchNormal);
	metalHatchMaterial->AddTextureSRV("MetalnessMap", metalHatchMetalness);
	metalHatchMaterial->AddSampler("Sampler", samplerState); //can't call ut SamplerState because thats an HLSL keyword
	
	cubeMesh = new Mesh(GetFullPathTo("../../Assets/Models/cube.obj").c_str(), device, context);

	skyBox = new SkyBox(cubeMesh, skyBoxTex, skyBoxVertexShader, skyBoxPixelShader, samplerState, device);
}


// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();
	if(camera) camera->UpdateProjectionMatrix((float)this->width / this->height);

	ResizeOnePostProcessResource(refractionRTV, refractionSRV);
	ResizeOnePostProcessResource(gammaCorrectionRTV, gammaCorrectionSRV);
}

//stackoverflow.com/questions/3418231/replace-part-of-a-string-with-another-string
void Game::replaceAll(std::string& str, const std::string& from, const std::string& to) {
	if (from.empty())
		return;
	size_t start_pos = 0;
	std::string result;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
}

// Adapted from code by Chris Casciolli 
// https ://github.com/vixorien/ggp-demos/blob/main/16%20-%20Bloom%20Post%20Process/Game.cpp
void Game::ResizeOnePostProcessResource(
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& rtv,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv)
{
	rtv.Reset();
	srv.Reset();

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.ArraySize = 1;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; 
	textureDesc.CPUAccessFlags = 0;
	textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	textureDesc.MipLevels = 1;
	textureDesc.MiscFlags = 0;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> ppTexture;
	device->CreateTexture2D(&textureDesc, 0, ppTexture.GetAddressOf());

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	device->CreateRenderTargetView(ppTexture.Get(), &rtvDesc, rtv.ReleaseAndGetAddressOf());

	//null description gives srv access to the whole resource
	device->CreateShaderResourceView(ppTexture.Get(), 0, srv.ReleaseAndGetAddressOf());
}


// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	for (int i = 0; i < meshEntities.size(); ++i) {
		meshEntities.at(i)->GetTransform()->Turn(0, 0.3f * deltaTime, 0);
	}
	camera->Update(deltaTime);

	XMFLOAT3 camPos = camera->GetTransform().GetPosition();

	// Example input checking: Quit if the escape key is pressed
	if (Input::GetInstance().KeyDown(VK_ESCAPE))
		Quit();
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV.Get(), color);
	context->ClearDepthStencilView(
		depthStencilView.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	// We can't do this in Material or MeshEntity because it can't be done to just any shader, just this one in particular
	basicLightingShader->SetFloat3("cameraPosition", camera->GetTransform().GetPosition());
	basicLightingShader->SetData("lights", &lights[0], sizeof(Light) * (int)lights.size());

	for (int i = 0; i < meshEntities.size(); ++i) {
		std::shared_ptr<MeshEntity> currentEntity = meshEntities.at(i);
		//should work fine without checking (just potentially unneccessary setting), does this help or hurt performance?
		if (currentEntity->GetMaterial()->GetPixelShader() == basicLightingShader) {
			currentEntity->GetMaterial()->BindResources();
			currentEntity->GetMaterial()->GetPixelShader()->SetFloat("roughness", currentEntity->GetMaterial()->GetRoughness());
			currentEntity->GetMaterial()->GetPixelShader()->SetFloat3("ambientColor", ambientColor);

		}
		currentEntity->Draw(camera, context);
	}
	skyBox->Draw(camera, context); //after drawing objects

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(vsync ? 1 : 0, 0);

	// Due to the usage of a more sophisticated swap chain,
	// the render target must be re-bound after every call to Present()
	context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthStencilView.Get());
}