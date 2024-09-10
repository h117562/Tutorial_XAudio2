cbuffer MatrixBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
}

struct Vin
{
    float4 pos : POSITION;
    float2 texcoord : TEXCOORD;
};

struct VOut
{
    float4 pos : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

VOut main(Vin input)
{
	VOut output;

	output.pos = mul(input.pos, World);
	output.pos = mul(output.pos, View);
	output.pos = mul(output.pos, Projection);
	output.texcoord = input.texcoord;

	return output;
}