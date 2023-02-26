#include"MeshNode.h"
#include"DirectXFramework.h"
#include "SimpleMath.h"
#define ShaderFileName			L"modelshader.hlsl"
#define TextureShaderFileName	L"modeltextureshader.hlsl"
#define PixelShaderName			"PS"
#define VertexShaderName		"VS"


struct CBuffer
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
	Vector4		_diffuseColour;
	Vector4		_specularColour;
	float		_opacity;
	Vector3		pad;

};
D3D11_INPUT_ELEMENT_DESC vertexDescription[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }


};
bool MeshNode::Initialise()
{
	BuildTextureShaders();
	BuildShaders();
	BuildVertexLayout();
	BuildConstantBuffer();
	BuildRasteriserState();

	return true;
}

void MeshNode::Render() {

	// Calculate the world x view x projection transformation
	for (int x = 0; x < _submeshCount; x++) {
		currentSubmesh = mesh->GetSubMesh(x);
		_material = currentSubmesh->GetMaterial();
		_vertexBuffer = currentSubmesh->GetVertexBuffer();
		_indexBuffer = currentSubmesh->GetIndexBuffer();
		_indexCount = currentSubmesh->GetIndexCount();
		_vertexCount = currentSubmesh->GetVertexCount();
		_texture = _material->GetTexture();
		
		Matrix completeTransformation = _cumulativeWorldTransformation * DirectXFramework::GetDXFramework()->GetViewTransformation() * DirectXFramework::GetDXFramework()->GetProjectionTransformation();
		// set the constant buffers.
		CBuffer constantBuffer;
		constantBuffer.WorldViewProjection = completeTransformation;
		constantBuffer.AmbientLightColour = _ambientLightColor;
		constantBuffer.WorldTransformation = _cumulativeWorldTransformation;
		constantBuffer.DirectionalLightVector = _directionalLightVector;
		constantBuffer.DirectionalLightColour = _directionalLightColour;
		constantBuffer.SecondDirectionalLightVector = _secondDirectionalLightVector;
		constantBuffer.SecondDirectionalLightColour = _secondDirectionalLightColour;
		constantBuffer.eyePosition = _eyePosition;

		// all the material properties can be sent in.
		constantBuffer.specularPower = _material->GetShininess();
		constantBuffer._diffuseColour = _material->GetDiffuseColour();
		constantBuffer._specularColour = _material->GetSpecularColour();
		constantBuffer._opacity = _material->GetOpacity();

		// Update the constant buffer. Note the layout of the constant buffer must match that in the shader
		deviceContext->VSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());
		deviceContext->PSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());

		deviceContext->UpdateSubresource(_constantBuffer.Get(), 0, 0, &constantBuffer, 0, 0);


		deviceContext->PSSetShaderResources(0, 1, _texture.GetAddressOf());
		
	
		// Specify the distance between vertices and the starting point in the vertex buffer
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		// Set the vertex buffer and index buffer we are going to use
		deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
		deviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		// Specify the layout of the polygons (it will rarely be different to this)
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Specify the layout of the input vertices.  This must match the layout of the input vertices in the shader
		deviceContext->IASetInputLayout(_layout.Get());

		//lets us use certain shaders depending if we have a texture.
		if (currentSubmesh->HasTexCoords()) {
			// Specify the vertex and pixel shaders we are going to use
			deviceContext->VSSetShader(_texvertexShader.Get(), 0, 0);
			deviceContext->PSSetShader(_texpixelShader.Get(), 0, 0);
		}
		else {
			deviceContext->VSSetShader(_vertexShader.Get(), 0, 0);
			deviceContext->PSSetShader(_pixelShader.Get(), 0, 0);
		}

		// Specify details about how the object is to be drawn
		deviceContext->RSSetState(_rasteriserState.Get());

		
		deviceContext->DrawIndexed(_indexCount, 0, 0);

	}
}

void MeshNode::BuildGeometryBuffers()
{
	// This method uses the arrays defined in Geometry.h
	// 
	// Setup the structure that specifies how big the vertex 
	// buffer should be
	D3D11_BUFFER_DESC vertexBufferDescriptor = { 0 };
	vertexBufferDescriptor.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDescriptor.ByteWidth = sizeof(Vertex) * _vertexCount;
	vertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDescriptor.CPUAccessFlags = 0;
	vertexBufferDescriptor.MiscFlags = 0;
	vertexBufferDescriptor.StructureByteStride = 0;

	// Now set up a structure that tells DirectX where to get the
	// data for the vertices from
	D3D11_SUBRESOURCE_DATA vertexInitialisationData = { 0 };
	vertexInitialisationData.pSysMem = &_vertexCount;

	// and create the vertex buffer
	//ComPtr<ID3D11Device> device = DirectXFramework::GetDXFramework()->GetDevice()
	ThrowIfFailed(device->CreateBuffer(&vertexBufferDescriptor, &vertexInitialisationData, _vertexBuffer.GetAddressOf()));

	// Setup the structure that specifies how big the index 
	// buffer should be
	D3D11_BUFFER_DESC indexBufferDescriptor = { 0 };
	indexBufferDescriptor.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDescriptor.ByteWidth = sizeof(UINT) * _indexCount;
	indexBufferDescriptor.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDescriptor.CPUAccessFlags = 0;
	indexBufferDescriptor.MiscFlags = 0;
	indexBufferDescriptor.StructureByteStride = 0;

	// Now set up a structure that tells DirectX where to get the
	// data for the indices from
	D3D11_SUBRESOURCE_DATA indexInitialisationData;
	indexInitialisationData.pSysMem = &_indexCount;

	// and create the index buffer
	ThrowIfFailed(device->CreateBuffer(&indexBufferDescriptor, &indexInitialisationData, _indexBuffer.GetAddressOf()));
}

void MeshNode::BuildTextureShaders()
{
	DWORD shaderCompileFlags = 0;
#if defined( _DEBUG )
	shaderCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ComPtr<ID3DBlob> compilationMessages = nullptr;

	//Compile vertex shader
	HRESULT hr = D3DCompileFromFile(TextureShaderFileName,
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		VertexShaderName, "vs_5_0",
		shaderCompileFlags, 0,
		_texvertexShaderByteCode.GetAddressOf(),
		compilationMessages.GetAddressOf());

	if (compilationMessages.Get() != nullptr)
	{
		// If there were any compilation messages, display them
		MessageBoxA(0, (char*)compilationMessages->GetBufferPointer(), 0, 0);
	}
	// Even if there are no compiler messages, check to make sure there were no other errors.
	ThrowIfFailed(hr);
	ThrowIfFailed(device->CreateVertexShader(_texvertexShaderByteCode->GetBufferPointer(), _texvertexShaderByteCode->GetBufferSize(), NULL, _texvertexShader.GetAddressOf()));

	// Compile pixel shader
	hr = D3DCompileFromFile(TextureShaderFileName,
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		PixelShaderName, "ps_5_0",
		shaderCompileFlags, 0,
		_texpixelShaderByteCode.GetAddressOf(),
		compilationMessages.GetAddressOf());

	if (compilationMessages.Get() != nullptr)
	{
		// If there were any compilation messages, display them
		MessageBoxA(0, (char*)compilationMessages->GetBufferPointer(), 0, 0);
	}
	ThrowIfFailed(hr);
	ThrowIfFailed(device->CreatePixelShader(_texpixelShaderByteCode->GetBufferPointer(), _texpixelShaderByteCode->GetBufferSize(), NULL, _texpixelShader.GetAddressOf()));
}
void MeshNode::BuildShaders()
{
	DWORD shaderCompileFlags = 0;
#if defined( _DEBUG )
	shaderCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ComPtr<ID3DBlob> compilationMessages = nullptr;

	//Compile vertex shader
	HRESULT hr = D3DCompileFromFile(ShaderFileName,
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		VertexShaderName, "vs_5_0",
		shaderCompileFlags, 0,
		_vertexShaderByteCode.GetAddressOf(),
		compilationMessages.GetAddressOf());

	if (compilationMessages.Get() != nullptr)
	{
		// If there were any compilation messages, display them
		MessageBoxA(0, (char*)compilationMessages->GetBufferPointer(), 0, 0);
	}
	// Even if there are no compiler messages, check to make sure there were no other errors.
	ThrowIfFailed(hr);
	ThrowIfFailed(device->CreateVertexShader(_vertexShaderByteCode->GetBufferPointer(), _vertexShaderByteCode->GetBufferSize(), NULL, _vertexShader.GetAddressOf()));

	// Compile pixel shader
	hr = D3DCompileFromFile(ShaderFileName,
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		PixelShaderName, "ps_5_0",
		shaderCompileFlags, 0,
		_pixelShaderByteCode.GetAddressOf(),
		compilationMessages.GetAddressOf());

	if (compilationMessages.Get() != nullptr)
	{
		// If there were any compilation messages, display them
		MessageBoxA(0, (char*)compilationMessages->GetBufferPointer(), 0, 0);
	}
	ThrowIfFailed(hr);
	ThrowIfFailed(device->CreatePixelShader(_pixelShaderByteCode->GetBufferPointer(), _pixelShaderByteCode->GetBufferSize(), NULL, _pixelShader.GetAddressOf()));
}

void MeshNode::BuildVertexLayout()
{
	// Create the vertex input layout. This tells DirectX the format
	// of each of the vertices we are sending to it. The vertexDesc array is
	// defined in Geometry.h

	ThrowIfFailed(device->CreateInputLayout(vertexDescription, ARRAYSIZE(vertexDescription), _vertexShaderByteCode->GetBufferPointer(), _vertexShaderByteCode->GetBufferSize(), _layout.GetAddressOf()));
}

void MeshNode::BuildConstantBuffer()
{
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(CBuffer);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	ThrowIfFailed(device->CreateBuffer(&bufferDesc, NULL, _constantBuffer.GetAddressOf()));
}

void MeshNode::Shutdown() {
	if (_vertexBuffer) {
		_vertexBuffer->Release();


	}
	if (_indexBuffer) {
		_indexBuffer->Release();

	}
	if (_constantBuffer) {
		_constantBuffer->Release();

	}
	
}

void MeshNode::BuildRasteriserState()
{	// Set default and wireframe rasteriser states
	D3D11_RASTERIZER_DESC rasteriserDesc;
	rasteriserDesc.CullMode = D3D11_CULL_BACK;
	rasteriserDesc.FrontCounterClockwise = false;
	rasteriserDesc.DepthBias = 0;
	rasteriserDesc.SlopeScaledDepthBias = 0.0f;
	rasteriserDesc.DepthBiasClamp = 0.0f;
	rasteriserDesc.DepthClipEnable = true;
	rasteriserDesc.ScissorEnable = false;
	rasteriserDesc.MultisampleEnable = false;
	rasteriserDesc.AntialiasedLineEnable = false;
	rasteriserDesc.FillMode = D3D11_FILL_SOLID;
	ThrowIfFailed(device->CreateRasterizerState(&rasteriserDesc, _rasteriserState.GetAddressOf()));
}
