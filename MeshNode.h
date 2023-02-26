#pragma once
#include "ObjectNode.h"
#include "DirectXFramework.h"
#include <vector>

using namespace std;


class MeshNode : public SceneNode
{
public:
	//setting all the main values in the constructor.
	MeshNode(wstring name, Vector4 AmbientLightColor, shared_ptr<Mesh> _mesh) : SceneNode(name) {
		mesh = _mesh;
		_submeshCount = _mesh->GetSubMeshCount();
		_ambientLightColor = AmbientLightColor;
		_eyePosition = DirectXFramework::GetDXFramework()->GetEyePosition();
		device = DirectXFramework::GetDXFramework()->GetDevice();
		deviceContext = DirectXFramework::GetDXFramework()->GetDeviceContext();
		_directionalLightVector = DirectXFramework::GetDXFramework()->GetLightDirection();
		_directionalLightColour = DirectXFramework::GetDXFramework()->GetLightColour();
		_secondDirectionalLightVector = DirectXFramework::GetDXFramework()->GetSecondLightDirection();
		_secondDirectionalLightColour = DirectXFramework::GetDXFramework()->GetSecondLightColour();
		
	};
	~MeshNode(void) {};

	virtual bool Initialise(void) override;
	virtual void Render(void) override;
	virtual void Shutdown(void) override;
	
	
	size_t								_submeshCount;

private:

	//sets all of the functions and variables we will be using.

	int								_rotationAngle{ 0 };

	void BuildGeometryBuffers();
	void BuildTextureShaders();
	void BuildShaders();
	void BuildVertexLayout();
	void BuildConstantBuffer();
	void BuildRasteriserState();

	ComPtr<ID3D11Buffer>			_vertexBuffer;
	ComPtr<ID3D11Buffer>			_indexBuffer;
	size_t							_vertexCount;
	size_t							_indexCount;

	shared_ptr<SubMesh>				currentSubmesh;
	shared_ptr<Mesh>				mesh;

	ComPtr<ID3DBlob>				_vertexShaderByteCode = nullptr;
	ComPtr<ID3DBlob>				_pixelShaderByteCode = nullptr;
	ComPtr<ID3DBlob>				_texvertexShaderByteCode = nullptr;
	ComPtr<ID3DBlob>				_texpixelShaderByteCode = nullptr;
	ComPtr<ID3D11VertexShader>		_vertexShader;
	ComPtr<ID3D11PixelShader>		_pixelShader;
	ComPtr<ID3D11VertexShader>		_texvertexShader;
	ComPtr<ID3D11PixelShader>		_texpixelShader;


	shared_ptr<Material>			_material;
	
	ComPtr<ID3D11ShaderResourceView>_texture;
	ComPtr<ID3D11InputLayout>		_layout;
	ComPtr<ID3D11Buffer>			_constantBuffer;
	ComPtr<ID3D11Device>			device;
	ComPtr<ID3D11DeviceContext>		deviceContext;
	ComPtr<ID3D11RasterizerState>   _rasteriserState;


	Vector3							_eyePosition;
	Vector4							_ambientLightColor;
	Vector4							_directionalLightVector;
	Vector4							_directionalLightColour;
	Vector4							_secondDirectionalLightVector;
	Vector4							_secondDirectionalLightColour;
	
};
