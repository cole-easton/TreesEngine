#include "StructIncludes.hlsli"

TextureCube CubeMap : register(t0);
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
float4 main(SkyBoxVertexToPixel input) : SV_TARGET
{

	float3 pixelColor = CubeMap.Sample(Sampler, input.sampleDir);
	return float4(pixelColor, 1);

}