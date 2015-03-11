////////////////////////////////////////////////////////////////////////////////
// Filename: color.fx
////////////////////////////////////////////////////////////////////////////////

/////////////
// GLOBALS //
/////////////
matrix ModelToWorldMatrix;
matrix WorldToViewMatrix;
matrix ViewToProjMatrix;
matrix ModelToWorldNormalMatrix;

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType ColorVertexShader(VertexInputType input)
{
    PixelInputType output;
    
	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;
	
	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, ModelToWorldMatrix);
    output.position = mul(output.position, WorldToViewMatrix);
    output.position = mul(output.position, ViewToProjMatrix);
    
	// Store the input color for the pixel shader to use.
    output.color.xyz = mul(input.normal, (float3x3)ModelToWorldNormalMatrix).xyz;
	output.color.w = 1.0f;
	
	return output;
}

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 ColorPixelShader(PixelInputType input) : SV_Target
{
    return input.color;
}

////////////////////////////////////////////////////////////////////////////////
// Technique
////////////////////////////////////////////////////////////////////////////////
technique10 RenderPass
{
    pass pass0
    {
        SetVertexShader(CompileShader(vs_4_0, ColorVertexShader()));
        SetPixelShader(CompileShader(ps_4_0, ColorPixelShader()));
        SetGeometryShader(NULL);
    }
}