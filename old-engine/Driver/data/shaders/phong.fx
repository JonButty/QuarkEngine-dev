matrix ModelToWorldMatrix;
matrix WorldToViewMatrix;
matrix ViewToProjMatrix;
matrix ModelToWorldNormalMatrix;

float4 CameraPos;
float4 AmbientColor;
float4 DiffuseColor;
float4 LightPos;
float SpecularPower;
float4 SpecularColor;

struct VertexInputType
{
    float4 position : POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 viewDirection : TEXCOORD1;
    float3 lightDirection : TEXCOORD2;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType ColorVertexShader(VertexInputType input)
{
	PixelInputType output;
    float4 worldPosition;

    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, ModelToWorldMatrix);
    output.position = mul(output.position, WorldToViewMatrix);
    output.position = mul(output.position, ViewToProjMatrix);
        
    // Calculate the normal vector against the world matrix only.
    output.normal.xyz = mul(input.normal, (float3x3)ModelToWorldNormalMatrix).xyz;
	
    // Normalize the normal vector.
    output.normal = normalize(output.normal);
	
    // Calculate the position of the vertex in the world.
    worldPosition = mul(input.position, ModelToWorldMatrix);

    // Determine the viewing direction based on the position of the camera and the position of the vertex in the world.
    output.viewDirection = CameraPos.xyz - worldPosition.xyz;
	output.lightDirection = LightPos.xyz -  worldPosition.xyz;
	
    // Normalize the viewing direction vector.
    output.viewDirection = normalize(output.viewDirection);

    return output;
}

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 ColorPixelShader(PixelInputType input) : SV_Target
{
    input.normal = normalize(input.normal);
	input.viewDirection = normalize(input.viewDirection);
	
	float3 lightDir;
	lightDir = normalize(input.lightDirection);
	
	float lDotN = max(0.0,dot(lightDir,input.normal));
	float3 diffuse;
	diffuse.xyz = lDotN * DiffuseColor.xyz;
	
	float3 ambient;
	ambient.xyz = AmbientColor.xyz * DiffuseColor.xyz;
	
	float3 r = 2 * (dot(lightDir,input.normal) * input.normal) - lightDir;
	r = normalize(r);
	
	float3 specular;
	specular = lDotN * SpecularColor.xyz * pow(max(0.0,dot(r,input.viewDirection)),SpecularPower);
	
    float4 color;
	color.xyz = ambient.xyz + diffuse.xyz + specular.xyz;
	color.w = 1.f;
	
    return color;
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