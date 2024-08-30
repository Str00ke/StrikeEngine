
cbuffer cbPerObject : register(b0)
{
    float4x4 gWorldViewProj;
};

struct VertexIn
{
    float3 posL : POSITION;
    float4 color : COLOR;
    //float2 texCoord : TEXCOORD;
};

struct VertexOut
{
    float4 posH : SV_POSITION;
    float4 color : COLOR;
    //float2 texCoord : TEXCOORD;
};



VertexOut VS(VertexIn _vIn)
{
    VertexOut vOut;

    // Transform to homogeneous clip space
    vOut.posH = mul(float4(_vIn.posL, 1.0f), gWorldViewProj);
    //vOut.posH.w = 4.5;
    //vOut.posH = float4(_vIn.posL.xyz, 1.0);
    //vOut.posH.z = 0.0f;

    // Pass vertex color into the pixel shader
    vOut.color = _vIn.color;

    //vOut.texCoord = _vIn.texCoord;

    return vOut;
}

