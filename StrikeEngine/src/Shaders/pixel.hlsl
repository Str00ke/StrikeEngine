
struct VertexOut
{
    float4 posH : SV_POSITION;
    float4 color : COLOR;
    //float2 texCoord : TEXCOORD;
};


float4 PS(VertexOut _vOut) : SV_Target
{
    return _vOut.color;
}