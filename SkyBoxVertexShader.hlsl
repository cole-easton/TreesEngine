#include "StructIncludes.hlsli"

cbuffer externalData : register(b0) {
	matrix view;
	matrix projection;
}


SkyBoxVertexToPixel main(VertexShaderInput input) 
{
	matrix viewNoTranslate = view;
	viewNoTranslate._14 = 0;
	viewNoTranslate._24 = 0;
	viewNoTranslate._34 = 0;

	SkyBoxVertexToPixel output;
	output.position = mul(projection, mul(viewNoTranslate, input.localPosition));
	output.position.z = output.position.w;
	output.sampleDir = input.localPosition;
	return output;
}
