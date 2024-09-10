Texture2D diffuseTexture;
SamplerState SampleType;

struct Pin
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
};

float4 main(Pin input) : SV_TARGET
{
    float4 textureColor = diffuseTexture.Sample(SampleType, input.tex);

	return textureColor;
}