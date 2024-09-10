struct Pin
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};


float4 main(Pin input) : SV_TARGET
{
    return input.color;
}