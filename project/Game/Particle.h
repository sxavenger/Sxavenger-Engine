#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DxObject/DxBufferResource.h>
#include <Engine/System/DxObject/DxShaderBlob.h>
#include <Engine/System/DxObject/DxGraphicsPipeline.h>
#include <Engine/Game/Behavior/BaseBehavior.h>
#include <Engine/Game/Model.h>
#include <Engine/Game/Transform.h>
#include <Engine/Game/InputAssembler.h>

//* lib
#include <Lib/Geometry/Vector4.h>
#include <Lib/Geometry/Vector2.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Particle class
////////////////////////////////////////////////////////////////////////////////////////////
class ParitcleCollection
	: public BaseBehavior {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void Term();

	void DrawAdaptive(_MAYBE_UNUSED const Camera3D* camera) override;

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// ParticleInfo sturcture
	////////////////////////////////////////////////////////////////////////////////////////////
	struct ParticleInfo {
		Color4f color;
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// ParticleElement structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct ParticleElement {
		QuaternionTransform transform;
		Color4f color;
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	Model* model_;

	//* buffer *//

	static const uint32_t kInstanceCount_ = 8;

	std::unique_ptr<DxObject::BufferResource<TransformationMatrix>> transformBuffer_;
	std::unique_ptr<DxObject::BufferResource<ParticleInfo>> infoBuffer_;

	std::list<ParticleElement> elements_;

	//* pipeline *//

	std::unique_ptr<DxObject::GraphicsBlob>     blob_;
	std::unique_ptr<DxObject::GraphicsPipeline> pipeline_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreatePipeline();
	void CreateBuffer();

};