#include "StructIncludes.hlsli"

cbuffer externalData : register(b0) {
	matrix world;
	matrix worldInvTranspose;
	matrix view;
	matrix projection;
}

// --------------------------------------------------------
// The entry point (main method) for our vertex shader
// 
// - Input is exactly one vertex worth of data (defined by a struct)
// - Output is a single struct of data to pass down the pipeline
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
VertexToPixel main( VertexShaderInput input )
{
	// Set up output struct
	VertexToPixel output;

	matrix wvp = mul(projection, mul(view, world));
	output.screenPosition = mul(wvp, float4(input.localPosition, 1.0f));

	output.uv = input.uv;
	output.normal = mul(world, input.normal); //worldInvTranspose isn't working
	output.worldPosition = mul(world, float4(input.localPosition, 1)).xyz;
	output.tangent = mul(world, input.tangent);

	// Whatever we return will make its way through the pipeline to the
	// next programmable stage we're using (the pixel shader for now)
	return output;
}