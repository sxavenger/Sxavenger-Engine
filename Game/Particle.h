#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxObject
// CS
#include <DxCSBlob.h>
#include <DxCSPipelineState.h>
#include <DxCSBufferResource.h>

// Graphics
#include <DxGraphicsBlob.h>
#include <DxGraphicsPipeline.h>

// Model
#include <Model.h>

// c++
#include <memory>

// Geometry
#include <Vector4.h>
#include <Vector3.h>
#include <Matrix4x4.h>

// rendering
#include <DefferedRendering.h>

// attribtue
#include <Attribute.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Particle class
////////////////////////////////////////////////////////////////////////////////////////////
class Particle
	: public Attribute {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Particle() = default;

	~Particle() { Term(); }

	void Init(DefferedRendering* defferdRendering);

	void Term();

	void Update();

	void Draw();

	void SetAttributeImGui() override;

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// ParticleData structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct ParticleData {
		Vector4f  color;
		Matrix4x4 worldMatrix = Matrix4x4::MakeIdentity();

		Vector3f tranlate;
		Vector3f velocity;
		Vector3f acceleration;
		float mass;

		Vector3f rotate;
		Vector3f addRotate;

		int isUpdate;
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	// CS
	std::unique_ptr<DxObject::CSBlob>                         csBlob_;
	std::unique_ptr<DxObject::CSPipelineState>                csPipelineState_;
	std::unique_ptr<DxObject::CSBufferResource<ParticleData>> csBuffer_;

	std::unique_ptr<DxObject::BufferPtrResource<int>> boolBuffer_; //!< 仮

	// Graphics
	std::unique_ptr<DxObject::GraphicsBlob>     graphicsBlob_;

	DxObject::GraphicsPipelineDesc pipelineDesc_;
	std::unique_ptr<DxObject::GraphicsPipeline> graphicsPipeline_;

	// IA
	std::unique_ptr<Model> model_;

	DefferedRendering* deffered_;

	// blendMode
	static const std::string blendModeNames_[kCountOfBlendMode];

	int isInit_ = true;

};
