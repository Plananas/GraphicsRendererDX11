#pragma once
#include "ObjectNode.h"
#include "DirectXFramework.h"
#include <vector>

using namespace std;


class CubeNode : public ObjectNode
{
public:

	CubeNode(wstring name, Vector4 AmbientLightColor) : ObjectNode(name, AmbientLightColor) {};
	~CubeNode(void) {};

	virtual bool Initialise(void) override;
	virtual void Render(void) override;
	virtual void Shutdown(void) override;

private:


	int								_rotationAngle{ 0 };

	virtual void BuildGeometryBuffers() override;
	virtual void BuildShaders() override;
	virtual void BuildVertexLayout() override;

	virtual void DefineNormals() override;
	virtual void BuildConstantBuffer() override;
	virtual void BuildRasteriserState() override;
};
