#include "StructIncludes.hlsli"
#include "LightingIncludes.hlsli"

cbuffer externalData : register(b0) {
	float4 color;
	float3 cameraPosition;
	Light lights[5];
}

Texture2D Albedo : register(t0);
Texture2D RoughnessMap : register(t1);
Texture2D NormalMap : register(t2);
Texture2D MetalnessMap : register(t3);
SamplerState Sampler : register(s0);

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	int texScale = 2;
	float3 unpackedNormal = NormalMap.Sample(Sampler, input.uv/texScale).rgb * 2 - 1;

	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);
	input.tangent = normalize(input.tangent - input.normal * dot(input.tangent, input.normal)); // Gram-Schmidt assumes T&N are normalized!
	float3 bitangent = cross(input.tangent, input.normal);
	float3x3 TBN = float3x3(input.tangent, bitangent, input.normal);
	input.normal = mul(unpackedNormal, TBN);

	float metalness = MetalnessMap.Sample(Sampler, input.uv / texScale).r;

	LightingInfo info;
	info.normal = input.normal;
	info.roughness = RoughnessMap.Sample(Sampler, input.uv / texScale).r;
	info.metalness = MetalnessMap.Sample(Sampler, input.uv / texScale).r;
	info.worldPosition = input.worldPosition;
	info.cameraPosition = cameraPosition;
	info.surfaceColor = color * pow(Albedo.Sample(Sampler, input.uv/texScale).rgb, 2.2);
	info.alpha = 1;

	float3 pixelColor = 0;
	for (int i = 0; i < 5; i++) {
		pixelColor += calculateTotalLighting(lights[i], info);

	}
	return float4(pow(pixelColor, 1.0f/2.2f), 1);

}