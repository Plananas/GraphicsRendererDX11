#pragma once
#include "SceneNode.h"
#include "DirectXFramework.h"

using namespace std;


class ObjectNode : public SceneNode
{
public:
	//gets all the values we need from the framework in the constructor.
	ObjectNode(wstring name, Vector4 AmbientLightColor) : SceneNode(name) {
		device = DirectXFramework::GetDXFramework()->GetDevice();
		deviceContext = DirectXFramework::GetDXFramework()->GetDeviceContext();
		_ambientLightColor = AmbientLightColor;
		_eyePosition = DirectXFramework::GetDXFramework()->GetEyePosition();
		_directionalLightVector = DirectXFramework::GetDXFramework()->GetLightDirection();
		_directionalLightColour = DirectXFramework::GetDXFramework()->GetLightColour();
		_secondDirectionalLightVector = DirectXFramework::GetDXFramework()->GetSecondLightDirection();
		_secondDirectionalLightColour = DirectXFramework::GetDXFramework()->GetSecondLightColour();
	};
	~ObjectNode(void) {};

	//this class is used by our objects so that all the common values are defined in one single area.

protected:
	virtual void DefineNormals() = 0;
	virtual void BuildGeometryBuffers() = 0;
	virtual void BuildShaders() = 0;
	virtual void BuildVertexLayout() = 0;
	virtual void BuildConstantBuffer() = 0;
	virtual void BuildRasteriserState() = 0;

	ComPtr<ID3D11Buffer>			_vertexBuffer;
	ComPtr<ID3D11Buffer>			_indexBuffer;

	ComPtr<ID3DBlob>				_vertexShaderByteCode = nullptr;
	ComPtr<ID3DBlob>				_pixelShaderByteCode = nullptr;
	ComPtr<ID3D11VertexShader>		_vertexShader;
	ComPtr<ID3D11PixelShader>		_pixelShader;

	ComPtr<ID3D11InputLayout>		_layout;
	ComPtr<ID3D11Buffer>			_constantBuffer;
	ComPtr<ID3D11Device>			device;
	ComPtr<ID3D11DeviceContext>		deviceContext;
	ComPtr<ID3D11RasterizerState>   _rasteriserState;

	
	int								_rotationAngle{ 0 };
	Vector3							_eyePosition;
	Vector4							_ambientLightColor;
	Vector4							_directionalLightVector;
	Vector4							_directionalLightColour;
	Vector4							_secondDirectionalLightVector;
	Vector4							_secondDirectionalLightColour;
};