#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <GraphicsRender.h>
#include <InputAssembler.h>
#include <Transform.h>
#include <Material.h>

//* object
#include <ObjectStructure.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DefaultGraphics enum
////////////////////////////////////////////////////////////////////////////////////////////
enum DefaultGraphicsType {
	kDefault_Texture,
	kDefault_LightingTexture,

	kCountOfDefaultGraphicsType
};

////////////////////////////////////////////////////////////////////////////////////////////
// DefaultGraphics class
////////////////////////////////////////////////////////////////////////////////////////////
class DefaultGraphics {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void Term();

	//* graphics option *//

	void DrawDefaultTexture(
		const InputAssembler<VertexData>& ia,
		const BaseTransformBuffer& transform, const ObjectMaterialBuffer& material,
		const D3D12_GPU_DESCRIPTOR_HANDLE& texture
	);

	void DrawDefaultLightingTexture(
		const InputAssembler<VertexData>& ia,
		const BaseTransformBuffer& transform, const ObjectMaterialBuffer& material,
		const D3D12_GPU_DESCRIPTOR_HANDLE& texture
	);

	//* singleton *//

	static DefaultGraphics* GetInstance();
	DefaultGraphics()  = default;
	~DefaultGraphics() = default;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	GraphicsRender renders_[kCountOfDefaultGraphicsType];

};