#pragma once
#include "ObjectNode.h"
#include "DirectXFramework.h"

using namespace std;

// class CubeNode;

class TextureCubeNode : public ObjectNode
{
public:

	TextureCubeNode(wstring name, Vector4 AmbientLightColor) : ObjectNode(name, AmbientLightColor) {};
	~TextureCubeNode(void) {};

	virtual bool Initialise(void) override;

	virtual void Render(void) override;
	virtual void Shutdown(void) override;

	ComPtr<ID3D11ShaderResourceView> _texture;


private:


	//takes the functions defined in Object node and overrides them to do what it needs.
	//Ideally the object node could do all this for us to save coding space however I will need to think of a way to implement this.
	
	virtual void BuildGeometryBuffers() override;
	virtual void BuildShaders() override;
	virtual void BuildVertexLayout() override;
	virtual void BuildConstantBuffer() override;
	virtual void BuildRasteriserState() override;

	void DefineNormals();
	void BuildTexture();
};