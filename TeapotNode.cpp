#include"TeapotNode.h"
#include"TeapotGeometry.h"
#include"DirectXFramework.h"

bool TeapotNode::Initialise()
{
	CreateVertexArray(vertexfloats);
	DefineNormals();
	BuildGeometryBuffers();
	BuildShaders();
	BuildVertexLayout();
	BuildConstantBuffer();
	BuildRasteriserState();

	return true;
}

void TeapotNode::Render() {

	// Calculate the world x view x projection transformation
	Matrix completeTransformation = _cumulativeWorldTransformation * DirectXFramework::GetDXFramework()->GetViewTransformation() * DirectXFramework::GetDXFramework()->GetProjectionTransformation();

	teaCBuffer constantBuffer;
	constantBuffer.WorldViewProjection = completeTransformation;
	constantBuffer.AmbientLightColour = _ambientLightColor;
	constantBuffer.WorldTransformation = _cumulativeWorldTransformation;
	constantBuffer.DirectionalLightVector = _directionalLightVector;
	constantBuffer.DirectionalLightColour = _directionalLightColour;
	constantBuffer.SecondDirectionalLightVector = _secondDirectionalLightVector;
	constantBuffer.SecondDirectionalLightColour = _secondDirectionalLightColour;
	constantBuffer.eyePosition = _eyePosition;
	constantBuffer.specularPower = 64.0f;

	// Update the constant buffer. Note the layout of the constant buffer must match that in the shader
	deviceContext->VSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());
	deviceContext->PSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());
	deviceContext->UpdateSubresource(_constantBuffer.Get(), 0, 0, &constantBuffer, 0, 0);

	// Now render the cube
	// Specify the distance between vertices and the starting point in the vertex buffer
	UINT stride = sizeof(teaVertex);
	UINT offset = 0;
	// Set the vertex buffer and index buffer we are going to use
	deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
	deviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// Specify the layout of the polygons (it will rarely be different to this)
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Specify the layout of the input vertices.  This must match the layout of the input vertices in the shader
	deviceContext->IASetInputLayout(_layout.Get());

	// Specify the vertex and pixel shaders we are going to use
	deviceContext->VSSetShader(_vertexShader.Get(), 0, 0);
	deviceContext->PSSetShader(_pixelShader.Get(), 0, 0);

	// Specify details about how the object is to be drawn
	deviceContext->RSSetState(_rasteriserState.Get());

	// Now draw the first Teapot
	deviceContext->DrawIndexed(ARRAYSIZE(teaindices), 0, 0);
}
void TeapotNode::CreateVertexArray(float vertexfloats[]) {
	for (int x = 0; x < size(teavertices); ++x) {

		teavertices[x].Position = Vector3(vertexfloats[x * 3], vertexfloats[x * 3 + 1], vertexfloats[x * 3 + 2]);
		teavertices[x].Normal = Vector3(vertexfloats[x * 3 + 3], vertexfloats[x * 3 + 4], vertexfloats[x * 3 + 5]);

		teavertices[x].Normal = Vector3(0, 0, 0);
	}
}
void TeapotNode::DefineNormals() {

	for (int i = 0; i < size(teacontributingCount); i += 1) {
		teacontributingCount[i] = 0;
	}

	float mNumTriangles = size(teaindices) / 3;
	// For each triangle in the mesh:
	for (UINT i = 0; i < mNumTriangles; ++i)
	{
		// indices of the ith triangle
		UINT i0 = teaindices[i * 3 + 0];
		UINT i1 = teaindices[i * 3 + 1];
		UINT i2 = teaindices[i * 3 + 2];
		// vertices of ith triangle
		teaVertex v0 = teavertices[i0];
		teaVertex v1 = teavertices[i1];
		teaVertex v2 = teavertices[i2];
		// compute face normal
		Vector3 e0 = v1.Position - v0.Position;
		Vector3 e1 = v2.Position - v0.Position;
		Vector3 faceNormal = e0.Cross(e1);
		// This triangle shares the following three vertices,
		// so add this face normal into the average of these
		// vertex normals.
		teavertices[i0].Normal += faceNormal;
		teavertices[i1].Normal += faceNormal;
		teavertices[i2].Normal += faceNormal;
		teacontributingCount[i0] += 1;
		teacontributingCount[i1] += 1;
		teacontributingCount[i2] += 1;
	}

	for (int i = 0; i < size(teavertices); i++) {
		teavertices[i].Normal /= teacontributingCount[i];
		teavertices[i].Normal.Normalize();
	}
}

void TeapotNode::BuildGeometryBuffers()
{
	// This method uses the arrays defined in Geometry.h
	// 
	// Setup the structure that specifies how big the vertex 
	// buffer should be
	D3D11_BUFFER_DESC vertexBufferDescriptor = { 0 };
	vertexBufferDescriptor.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDescriptor.ByteWidth = sizeof(teaVertex) * ARRAYSIZE(teavertices);
	vertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDescriptor.CPUAccessFlags = 0;
	vertexBufferDescriptor.MiscFlags = 0;
	vertexBufferDescriptor.StructureByteStride = 0;

	// Now set up a structure that tells DirectX where to get the
	// data for the vertices from
	D3D11_SUBRESOURCE_DATA vertexInitialisationData = { 0 };
	vertexInitialisationData.pSysMem = &teavertices;

	// and create the vertex buffer
	//ComPtr<ID3D11Device> device = DirectXFramework::GetDXFramework()->GetDevice()
	ThrowIfFailed(device->CreateBuffer(&vertexBufferDescriptor, &vertexInitialisationData, _vertexBuffer.GetAddressOf()));

	// Setup the structure that specifies how big the index 
	// buffer should be
	D3D11_BUFFER_DESC indexBufferDescriptor = { 0 };
	indexBufferDescriptor.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDescriptor.ByteWidth = sizeof(UINT) * ARRAYSIZE(teaindices);
	indexBufferDescriptor.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDescriptor.CPUAccessFlags = 0;
	indexBufferDescriptor.MiscFlags = 0;
	indexBufferDescriptor.StructureByteStride = 0;

	// Now set up a structure that tells DirectX where to get the
	// data for the indices from
	D3D11_SUBRESOURCE_DATA indexInitialisationData;
	indexInitialisationData.pSysMem = &teaindices;

	// and create the index buffer
	ThrowIfFailed(device->CreateBuffer(&indexBufferDescriptor, &indexInitialisationData, _indexBuffer.GetAddressOf()));
}

void TeapotNode::BuildShaders()
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

void TeapotNode::BuildVertexLayout()
{
	// Create the vertex input layout. This tells DirectX the format
	// of each of the vertices we are sending to it. The vertexDesc array is
	// defined in Geometry.h

	ThrowIfFailed(device->CreateInputLayout(teavertexDesc, ARRAYSIZE(teavertexDesc), _vertexShaderByteCode->GetBufferPointer(), _vertexShaderByteCode->GetBufferSize(), _layout.GetAddressOf()));
}

void TeapotNode::BuildConstantBuffer()
{
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(teaCBuffer);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	ThrowIfFailed(device->CreateBuffer(&bufferDesc, NULL, _constantBuffer.GetAddressOf()));
}

void TeapotNode::Shutdown() {
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

void TeapotNode::BuildRasteriserState()
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

