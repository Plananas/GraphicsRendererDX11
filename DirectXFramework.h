#pragma once
#include <vector>
#include "Framework.h"
#include "DirectXCore.h"
#include "SceneGraph.h"
#include "ResourceManager.h"

class DirectXFramework : public Framework
{
public:
	DirectXFramework();
	DirectXFramework(unsigned int width, unsigned int height);

	virtual void CreateSceneGraph();
	virtual void UpdateSceneGraph();

	bool Initialise();
	void Update();
	void Render();
	void OnResize(WPARAM wParam);
	void Shutdown();

	static DirectXFramework *			GetDXFramework();

	inline SceneGraphPointer			GetSceneGraph() { return _sceneGraph; }
	inline shared_ptr<ResourceManager>	GetResourceManager() { return _resourceManager; }

	inline ComPtr<ID3D11Device>			GetDevice() { return _device; }
	inline ComPtr<ID3D11DeviceContext>	GetDeviceContext() { return _deviceContext; }
	inline Vector3						GetEyePosition() { return _eyePosition; }

	inline Vector4						GetLightDirection() { return _directionalLightVector; }
	inline Vector4						GetLightColour() { return _directionalLightColour; }
	inline Vector4						GetSecondLightDirection() { return _secondDirectionalLightVector; }
	inline Vector4						GetSecondLightColour() { return _secondDirectionalLightColour; }


	const Matrix&						GetViewTransformation() const;
	const Matrix&						GetProjectionTransformation() const;

	void								MoveCameraX(float direction);
	void								SetBackgroundColour(Vector4 backgroundColour);

private:
	ComPtr<ID3D11Device>				_device;
	ComPtr<ID3D11DeviceContext>			_deviceContext;
	ComPtr<IDXGISwapChain>				_swapChain;
	ComPtr<ID3D11Texture2D>				_depthStencilBuffer;
	ComPtr<ID3D11RenderTargetView>		_renderTargetView;
	ComPtr<ID3D11DepthStencilView>		_depthStencilView;

	D3D11_VIEWPORT						_screenViewport{ 0 };

	Vector3								_eyePosition;
	Vector3								_focalPointPosition;
	Vector3								_upVector;
	
	Vector4								_directionalLightVector;
	Vector4								_secondDirectionalLightVector;
	Vector4								_directionalLightColour;
	Vector4								_secondDirectionalLightColour;

	Matrix								_viewTransformation;
	Matrix								_projectionTransformation;

	SceneGraphPointer					_sceneGraph;
	shared_ptr<ResourceManager>			_resourceManager;

	float							    _backgroundColour[4];

	bool GetDeviceAndSwapChain();
};

