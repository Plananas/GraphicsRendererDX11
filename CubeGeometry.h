#pragma once

#include "SimpleMath.h"
#define ShaderFileName		L"shader.hlsl"
#define VertexShaderName	"VS"
#define PixelShaderName		"PS"


// Format of the constant buffer. This must match the format of the
// cbuffer structure in the shader

struct CubeCBuffer
{
	Matrix		WorldViewProjection;
	Matrix		WorldTransformation;
	Vector4		AmbientLightColour;
	Vector4		DirectionalLightColour;
	Vector4		DirectionalLightVector;
	Vector4     SecondDirectionalLightVector;
	Vector4     SecondDirectionalLightColour;
	Vector3		eyePosition;
	float		specularPower;
};

// Structure of a single vertex.  This must match the
// structure of the input vertex in the shader

struct CubeVertex
{
	Vector3		Position;
	Vector3		Normal;
};

// The description of the vertex that is passed to CreateInputLayout.  This must
// match the format of the vertex above and the format of the input vertex in the shader

D3D11_INPUT_ELEMENT_DESC cubevertexDescription[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

};

// This example uses hard-coded vertices and indices for a cube. Usually, you will load the verticesa and indices from a model file. 
// We will see this later in the module. 


CubeVertex Cubevertices[] =
{
	{ Vector3(-1.0f, -1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f)},    // Front Face
	{ Vector3(1.0f, -1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f) },
	{ Vector3(-1.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f) },
	{ Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f)  },

	{ Vector3(-1.0f, -1.0f, -1.0f), Vector3(0.0f, 0.0f, 0.0f)},    // Back Face
	{ Vector3(-1.0f, 1.0f, -1.0f), Vector3(0.0f, 0.0f, 0.0f) },
	{ Vector3(1.0f, -1.0f, -1.0f), Vector3(0.0f, 0.0f, 0.0f) },
	{ Vector3(1.0f, 1.0f, -1.0f), Vector3(0.0f, 0.0f, 0.0f)  },

	{ Vector3(-1.0f, 1.0f, -1.0f), Vector3(0.0f, 0.0f, 0.0f) },    // Top Face
	{ Vector3(-1.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f)  },
	{ Vector3(1.0f, 1.0f, -1.0f), Vector3(0.0f, 0.0f, 0.0f)  },
	{ Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f)   },

	{ Vector3(-1.0f, -1.0f, -1.0f), Vector3(0.0f, 0.0f, 0.0f)},    // Bottom Face
	{ Vector3(1.0f, -1.0f, -1.0f), Vector3(0.0f, 0.0f, 0.0f) },
	{ Vector3(-1.0f, -1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f) },
	{ Vector3(1.0f, -1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f)  },

	{ Vector3(1.0f, -1.0f, -1.0f), Vector3(0.0f, 0.0f, 0.0f) },    // Right Face
	{ Vector3(1.0f, 1.0f, -1.0f), Vector3(0.0f, 0.0f, 0.0f)  },
	{ Vector3(1.0f, -1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f)  },
	{ Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f)   },

	{ Vector3(-1.0f, -1.0f, -1.0f), Vector3(0.0f, 0.0f, 0.0f)},    // Left Face
	{ Vector3(-1.0f, -1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f) },
	{ Vector3(-1.0f, 1.0f, -1.0f), Vector3(0.0f, 0.0f, 0.0f) },
	{ Vector3(-1.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f)  }
};

int CubecontributingCount[ARRAYSIZE(Cubevertices)];

UINT Cubeindices[] = {
			0, 1, 2,       // side 1
			2, 1, 3,
			4, 5, 6,       // side 2
			6, 5, 7,
			8, 9, 10,      // side 3
			10, 9, 11,
			12, 13, 14,    // side 4
			14, 13, 15,
			16, 17, 18,    // side 5
			18, 17, 19,
			20, 21, 22,    // side 6
			22, 21, 23,
};


