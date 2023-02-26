#include "DirectXApp.h"
#include "TeapotNode.h"
#include "CubeNode.h"
#include "TextureCubeNode.h"
#include "ResourceManager.h"
#include "MeshNode.h"
DirectXApp app;

void DirectXApp::CreateSceneGraph()
{
	
	SceneGraphPointer sceneGraph = GetSceneGraph();
	// Now add nodes to the scene graph
	//Create a sceneGraph for the robot and add it to the world scene graph.
	//shared_ptr<ResourceManager>

	shared_ptr<ResourceManager> manager = GetResourceManager();

	//Teapots//
	shared_ptr<SceneGraph> TeapotSceneGraph = make_shared<SceneGraph>(L"Teapots");

	shared_ptr<TeapotNode> Teapot = make_shared<TeapotNode>(L"Teapot", Vector4(0.01f, 0.01f, 0.01f, 1.0f));
	Teapot->SetWorldTransform(Matrix::CreateScale(Vector3(7.0f, 7.0f, 7.0f)) * Matrix::CreateTranslation(Vector3(-0.0f, 50.0f, 0.0f)));
	TeapotSceneGraph->Add(Teapot);

	shared_ptr<TeapotNode> Teapot2 = make_shared<TeapotNode>(L"Teapot2", Vector4(0.01f, 0.01f, 0.01f, 1.0f));
	Teapot2->SetWorldTransform(Matrix::CreateScale(Vector3(5.0f, 5.0f, 5.0f)) * Matrix::CreateTranslation(Vector3(20.0f, 15.0f, 0.0f)));
	TeapotSceneGraph->Add(Teapot2);

	shared_ptr<TeapotNode> Teapot3 = make_shared<TeapotNode>(L"Teapot3", Vector4(0.01f, 0.01f, 0.01f, 1.0f));
	Teapot3->SetWorldTransform(Matrix::CreateScale(Vector3(5.0f, 5.0f, 5.0f)) * Matrix::CreateTranslation(Vector3(-20.0f, 20.0f, 0.0f)));
	TeapotSceneGraph->Add(Teapot3);

	sceneGraph->Add(TeapotSceneGraph);
	////////////////////
	// 
	//Robot//
	shared_ptr<SceneGraph> Robot = make_shared<SceneGraph>(L"Robot");

	shared_ptr<CubeNode> Body = make_shared<CubeNode>(L"Body", Vector4(Colors::Red));
	Body->SetWorldTransform(Matrix::CreateScale(Vector3(5.0f, 8.0f, 2.5f)) * Matrix::CreateTranslation(Vector3(0.0f, 20.0f, 0)));
	Robot->Add(Body);

	shared_ptr<CubeNode> Head = make_shared<CubeNode>(L"Head", Vector4(Colors::Green));
	Head->SetWorldTransform(Matrix::CreateScale(Vector3(3.0f, 3.0f, 3.0f)) * Matrix::CreateTranslation(Vector3(0.0f, 31.0f, 0)));
	Robot->Add(Head);

	shared_ptr<CubeNode> LeftLeg = make_shared<CubeNode>(L"LeftLeg", Vector4(Colors::Blue));
	LeftLeg->SetWorldTransform(Matrix::CreateScale(Vector3(1.0f, 7.5f, 1.0f)) * Matrix::CreateTranslation(Vector3(-4.0f, 4.5f, 0)));
	Robot->Add(LeftLeg);

	shared_ptr<CubeNode> RightLeg = make_shared<CubeNode>(L"RightLeg", Vector4(Colors::Blue));
	RightLeg->SetWorldTransform(Matrix::CreateScale(Vector3(1.0f, 7.5f, 1.0f)) * Matrix::CreateTranslation(Vector3(4.0f, 4.5f, 0)));
	Robot->Add(RightLeg);

	shared_ptr<CubeNode> LeftArm = make_shared<CubeNode>(L"LeftArm", Vector4(Colors::Yellow));
	LeftArm->SetWorldTransform(Matrix::CreateScale(Vector3(1.0f, 8.5f, 1.0f)) * Matrix::CreateTranslation(Vector3(-6.0f, 19.0f, 0)));
	Robot->Add(LeftArm);

	shared_ptr<CubeNode> RightArm = make_shared<CubeNode>(L"RightArm", Vector4(Colors::Yellow));
	RightArm->SetWorldTransform(Matrix::CreateScale(Vector3(1.0f, 8.5f, 1.0f)) * Matrix::CreateTranslation(Vector3(6.0f, 19.0f, 0)));
	Robot->Add(RightArm);

	sceneGraph->Add(Robot);	
	////////////////////
	// 
	//Floor//
	shared_ptr<SceneGraph> FloorSceneGraph = make_shared<SceneGraph>(L"FloorSceneGraph");
	shared_ptr<CubeNode> Floor = make_shared<CubeNode>(L"Floor", Vector4(Colors::DarkGreen));
	Floor->SetWorldTransform(Matrix::CreateScale(Vector3(100.0f, 0.5f, 100.0f)) * Matrix::CreateTranslation(Vector3(0.0f, -10.0f, 0)));
	FloorSceneGraph->Add(Floor);
	sceneGraph->Add(FloorSceneGraph);
	///////////////////
	//
	//
	//Textured Cubes//
	shared_ptr<SceneGraph> BlockSceneGraph = make_shared<SceneGraph>(L"Block");
	
	shared_ptr<TextureCubeNode> Box1 = make_shared<TextureCubeNode>(L"Box1", Vector4(0.1f, 0.1f, 0.1f, 1.0f));
	Box1->SetWorldTransform(Matrix::CreateScale(Vector3(5.0f, 5.0f, 5.0f)) * Matrix::CreateRotationY(2) * Matrix::CreateTranslation(Vector3(40.0f, 0.0f, 40.0f)));
	BlockSceneGraph->Add(Box1);

	shared_ptr<TextureCubeNode> Box2 = make_shared<TextureCubeNode>(L"Box2", Vector4(0.1f, 0.1f, 0.1f, 1.0f));
	Box2->SetWorldTransform(Matrix::CreateScale(Vector3(5.0f, 5.0f, 5.0f)) * Matrix::CreateRotationY(3) * Matrix::CreateTranslation(Vector3(50.0f, 0.0f, 30.0f)));
	BlockSceneGraph->Add(Box2);

	sceneGraph->Add(BlockSceneGraph);
	//////////////////
	// 
	//Models//
	shared_ptr<Mesh> _mesh = manager->GetMesh(L"airplane.x");
	shared_ptr<MeshNode> plane = make_shared<MeshNode>(L"Plane", Vector4(0.01f, 0.01f, 0.01f, 1.0f), _mesh);
	plane->SetWorldTransform(Matrix::CreateRotationX(6.5) * Matrix::CreateRotationY(5) * Matrix::CreateScale(Vector3(5.0f, 5.0f, 5.0f)) * Matrix::CreateTranslation(Vector3(0.0f, 25.0f, 50.0f)));
	sceneGraph->Add(plane);

	shared_ptr<Mesh> Crash = manager->GetMesh(L"crashbandicoot.obj");
	shared_ptr<MeshNode> crash = make_shared<MeshNode>(L"crash", Vector4(0.7f, 0.7f, 0.7f, 1.0f), Crash);
	crash->SetWorldTransform(Matrix::CreateRotationY(1) * Matrix::CreateScale(Vector3(0.20f, 0.20f, 0.20f)));
	sceneGraph->Add(crash);

}

void DirectXApp::UpdateSceneGraph()
{
	
	_rotationAngle += 0.01;

	//grab all of the nodes from the main scene graph.
	SceneGraphPointer sceneGraph = GetSceneGraph();
	sceneGraph->SetWorldTransform(Matrix::CreateRotationY(_rotationAngle / 2));



	SceneNodePointer  Robot = sceneGraph->Find(L"Robot");
	SceneNodePointer Teapots = sceneGraph->Find(L"Teapots");
	SceneNodePointer Crash = sceneGraph->Find(L"Crash");

	Teapots->SetWorldTransform(Matrix::CreateRotationY(_rotationAngle/2));
	

	Robot->SetWorldTransform(Matrix::CreateRotationY(6) * Matrix::CreateTranslation(Vector3(35.0f, -3.0f, 25.0f)));

}
