
cbuffer ConstantBuffer
{
	matrix	worldViewProjection;
	matrix  worldTransformation;
	float4	ambientLightColour;
	float4  directionalLightColour;
	float4  directionalLightVector;
	float4  secondDirectionalLightVector;
	float4  secondDirectionalLightColour;
	float3  eyePosition;
	float	specularPower;

	float4	_diffuseColour;
	float4	_specularColour;
	float	_opacity;
	float3	pad;
};
Texture2D Texture;
SamplerState ss;

struct VertexIn
{
	float3 InputPosition : POSITION;
	float3 Normal		 : NORMAL;
	float2 TexCoord		 : TEXCOORD;
};

struct VertexOut
{
	float4 OutputPosition	: SV_POSITION;
	float4 Normal			: TEXCOORD0;
	float4 WorldPosition	: TEXCOORD1;
	float2 TexCoord			: TEXCOORD3;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.TexCoord = vin.TexCoord;
	// Transform to homogeneous clip space.
	vout.OutputPosition = mul(worldViewProjection, float4(vin.InputPosition, 1.0f));



	//pass these over to the pixel shader
	vout.Normal = float4(vin.Normal, 0);
	vout.WorldPosition = float4(vin.InputPosition, 1.0f);
	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{

	float4 adjustedNormal = normalize(mul(worldTransformation, pin.Normal));
	float4 vectorToLight1 = normalize(-directionalLightVector);
	float4 vectorToLight2 = normalize(-secondDirectionalLightVector);
	float4 finalColour = ambientLightColour;

	/////Diffuse/////

	float4 diffuseBrightness = saturate(dot(adjustedNormal, vectorToLight1) * directionalLightColour);
	float4 diffuseBrightness2 = saturate(dot(adjustedNormal, vectorToLight2) * secondDirectionalLightColour);

	finalColour += (diffuseBrightness + diffuseBrightness2) * _diffuseColour;


	/////Blinn Phong/////

	float4 viewDir = normalize(float4(eyePosition, 1.0f) - pin.WorldPosition);
	float4 halfwayDir = normalize(vectorToLight1 + viewDir);
	float  spec = pow(abs(dot(adjustedNormal, halfwayDir)), specularPower);
	float4 specular = saturate((float4(1.0f, 1.0f, 1.0f, 1.0f) * spec) * directionalLightColour);

	float4 viewDir2 = normalize(float4(eyePosition, 0) - pin.WorldPosition);
	float4 halfwayDir2 = normalize(vectorToLight2 + viewDir2);
	float  spec2 = pow(abs(dot(adjustedNormal, halfwayDir2)), specularPower);
	float4 specular2 = saturate((float4(1.0f, 1.0f, 1.0f, 1.0f) * spec2) * secondDirectionalLightColour);

	finalColour += (specular + specular2) * _specularColour;
	return finalColour;
}

