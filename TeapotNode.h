#pragma once
#include "ObjectNode.h"
#include "DirectXFramework.h"
#include <vector>

using namespace std;


class TeapotNode : public ObjectNode
{
public:

	TeapotNode(wstring name, Vector4 AmbientLightColor) : ObjectNode(name, AmbientLightColor) {};
	~TeapotNode(void) {};

	virtual bool Initialise(void) override;
	virtual void Render(void) override;
	virtual void Shutdown(void) override;

private: 

	//takes the functions defined in Object node and overrides them to do what it needs.
	//Ideally the object node could do all this for us to save coding space however I will need to think of a way to implement this.



	virtual void BuildGeometryBuffers() override;
	virtual void BuildShaders() override;
	virtual void BuildVertexLayout() override;
	virtual void DefineNormals() override;
	virtual void BuildConstantBuffer() override;
	virtual void BuildRasteriserState() override;

	int								_rotationAngle{ 0 };

	//Create vertex array for the list of floats.
	void CreateVertexArray(float vertexfloats[]);
};
