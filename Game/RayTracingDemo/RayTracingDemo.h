#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// Sxavenger
#include <Sxavenger.h>
#include <Attribute.h>
#include <Console.h>

// _DXROBJECT 3.0
#include "DxrExportGroup.h"
#include "DxrAccelerationStructure.h"
#include "DxrShaderBlob.h"
#include "DxrShaderBlobManager.h"
#include "DxrStateObject.h"
#include "DxrShaderTable.h"

// _DXOBJECT
#include "DxBufferResource.h"

#include "RayTracingCamera.h"

#include "Model.h"

////////////////////////////////////////////////////////////////////////////////////////////
// RayTracingDemo class
////////////////////////////////////////////////////////////////////////////////////////////
class RayTracingDemo
	: public Attribute {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void Term();

	void Update();

	void Draw();

	void SetAttributeImGui() override;

private:

	//=========================================================================================
	// private varables
	//=========================================================================================


	std::unique_ptr<_DXROBJECT ShaderBlobManager> manager_;
	std::unique_ptr<_DXROBJECT ShaderBlob> blob_;

	std::unique_ptr<_DXROBJECT ExportGroup> raygen_;
	std::unique_ptr<_DXROBJECT BufferRecord> raygenRecord_;
	std::unique_ptr<DummyTexture> output_; //!< UAVTexure

	std::unique_ptr<_DXROBJECT ExportGroup> miss_;
	std::unique_ptr<_DXROBJECT BufferRecord> missRecord_;
	std::unique_ptr<DxObject::BufferResource<Color4f>> missColor_;
	

	std::unique_ptr<_DXROBJECT StateObject> stateObject_;

	std::unique_ptr<_DXROBJECT ShaderTable> table_;

	std::unique_ptr<_DXROBJECT TopLevelAS> tlas_;


	std::unique_ptr<_DXROBJECT ExportGroup> hitgroup_;
	std::unique_ptr<Model> model_;
	std::unique_ptr<_DXROBJECT BottomLevelAS> blas_;
	EulerTransform transform_;

	std::unique_ptr<RayTracingCamera> camera_;

};