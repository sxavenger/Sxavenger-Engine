#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxObject
// CS
#include <DxCSBlob.h>
#include <DxCSPipelineState.h>
#include <DxCSBufferResource.h>
#include <DxCSDimensionBuffer.h>

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

////////////////////////////////////////////////////////////////////////////////////////////
// Particle class
////////////////////////////////////////////////////////////////////////////////////////////
class Particle
	: public Attribute {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Particle() { Init(); }

	~Particle() { Term(); }

	void Init();

	void Term();

	void Update();

	void Draw();

	void SetAttributeImGui() override;

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// ParticleData structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct ParticleData {
		Matrix4x4 worldMatrix = Matrix4x4::MakeIdentity();
		Vector4f  color;
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	// CS
	std::unique_ptr<DxObject::CSBlob>                         csBlob_;
	std::unique_ptr<DxObject::CSPipelineState>                csPipelineState_;
	std::unique_ptr<DxObject::CSBufferResource<ParticleData>> csBuffer_;

	// Graphics
	std::unique_ptr<DxObject::GraphicsBlob>     graphicsBlob_;
	std::unique_ptr<DxObject::GraphicsPipeline> graphicsPipeline_;

	// IA
	std::unique_ptr<Model> model_;

};
