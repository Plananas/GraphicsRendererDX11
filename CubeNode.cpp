#include"CubeNode.h"
#include"CubeGeometry.h"
#include"DirectXFramework.h"

bool CubeNode::Initialise()
{
	DefineNormals();
	BuildGeometryBuffers();
	BuildShaders();
	BuildVertexLayout();
	BuildConstantBuffer();
	BuildRasteriserState();

	return true;
}

void CubeNode::Render() {

	// Calculate the world x view x projection transformation
	Matrix completeTransformation = _cumulativeWorldTransformation * DirectXFramework::GetDXFramework()->GetViewTransformation() * DirectXFramework::GetDXFramework()->GetProjectionTransformation();

	CubeCBuffer constantBuffer;
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
	UINT stride = sizeof(CubeVertex);
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

	// Now draw the first cube
	deviceContext->DrawIndexed(ARRAYSIZE(Cubeindices), 0, 0);
}

void CubeNode::DefineNormals() {

	for (int i = 0; i < size(CubecontributingCount); i += 1) {
		CubecontributingCount[i] = 0;
	}

	float mNumTriangles = size(Cubeindices) / 3;
	// For each triangle in the mesh:
	for (UINT i = 0; i < mNumTriangles; ++i)
	{
		// indices of the ith triangle
		UINT i0 = Cubeindices[i * 3 + 0];
		UINT i1 = Cubeindices[i * 3 + 1];
		UINT i2 = Cubeindices[i * 3 + 2];
		// vertices of ith triangle
		CubeVertex v0 = Cubevertices[i0];
		CubeVertex v1 = Cubevertices[i1];
		CubeVertex v2 = Cubevertices[i2];
		// compute face normal
		Vector3 e0 = v1.Position - v0.Position;
		Vector3 e1 = v2.Position - v0.Position;
		Vector3 faceNormal = e0.Cross(e1);
		// This triangle shares the following three vertices,
		// so add this face normal into the average of these
		// vertex normals.
		Cubevertices[i0].Normal += faceNormal;
		Cubevertices[i1].Normal += faceNormal;
		Cubevertices[i2].Normal += faceNormal;
		CubecontributingCount[i0] += 1;
		CubecontributingCount[i1] += 1;
		CubecontributingCount[i2] += 1;
	}

	for (int i = 0; i < size(Cubevertices); i++) {
		Cubevertices[i].Normal /= CubecontributingCount[i];
		Cubevertices[i].Normal.Normalize();
	}
}

void CubeNode::BuildGeometryBuffers()
{
	// This method uses the arrays defined in Geometry.h
	// 
	// Setup the structure that specifies how big the vertex 
	// buffer should be
	D3D11_BUFFER_DESC vertexBufferDescriptor = { 0 };
	vertexBufferDescriptor.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDescriptor.ByteWidth = sizeof(CubeVertex) * ARRAYSIZE(Cubevertices);
	vertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDescriptor.CPUAccessFlags = 0;
	vertexBufferDescriptor.MiscFlags = 0;
	vertexBufferDescriptor.StructureByteStride = 0;

	// Now set up a structure that tells DirectX where to get the
	// data for the vertices from
	D3D11_SUBRESOURCE_DATA vertexInitialisationData = { 0 };
	vertexInitialisationData.pSysMem = &Cubevertices;

	// and create the vertex buffer
	//ComPtr<ID3D11Device> device = DirectXFramework::GetDXFramework()->GetDevice()
	ThrowIfFailed(device->CreateBuffer(&vertexBufferDescriptor, &vertexInitialisationData, _vertexBuffer.GetAddressOf()));

	// Setup the structure that specifies how big the index 
	// buffer should be
	D3D11_BUFFER_DESC indexBufferDescriptor = { 0 };
	indexBufferDescriptor.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDescriptor.ByteWidth = sizeof(UINT) * ARRAYSIZE(Cubeindices);
	indexBufferDescriptor.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDescriptor.CPUAccessFlags = 0;
	indexBufferDescriptor.MiscFlags = 0;
	indexBufferDescriptor.StructureByteStride = 0;

	// Now set up a structure that tells DirectX where to get the
	// data for the indices from
	D3D11_SUBRESOURCE_DATA indexInitialisationData;
	indexInitialisationData.pSysMem = &Cubeindices;

	// and create the index buffer
	ThrowIfFailed(device->CreateBuffer(&indexBufferDescriptor, &indexInitialisationData, _indexBuffer.GetAddressOf()));
}

void CubeNode::BuildShaders()
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

void CubeNode::BuildVertexLayout()
{
	// Create the vertex input layout. This tells DirectX the format
	// of each of the vertices we are sending to it. The vertexDesc array is
	// defined in Geometry.h

	ThrowIfFailed(device->CreateInputLayout(cubevertexDescription, ARRAYSIZE(cubevertexDescription), _vertexShaderByteCode->GetBufferPointer(), _vertexShaderByteCode->GetBufferSize(), _layout.GetAddressOf()));
}

void CubeNode::BuildConstantBuffer()
{
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(CubeCBuffer);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	ThrowIfFailed(device->CreateBuffer(&bufferDesc, NULL, _constantBuffer.GetAddressOf()));
}

void CubeNode::Shutdown() {
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

void CubeNode::BuildRasteriserState()
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

