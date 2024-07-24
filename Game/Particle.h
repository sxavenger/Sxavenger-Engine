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

// IA
#include <Model.h>
#include <DrawMethod.h>

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

	void Init();

	void Term();

	void Update();

	void Draw();

	void SetAttributeImGui() override;

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// ParticleCS structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct ParticleCS {
		Color4f color;
		Vector3f scale;
		Vector3f translate;
		Vector3f velocity;
		float currentTime;
		float lifeTime;
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// EmitterSphere structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct EmitterSphere {
		Vector3f position;   //!< 位置
		float radius;        //!< 射出半径
		uint32_t count;      //!< 発射数
		float frequency;     //!< 発射間隔
		float frequencyTime; //!< 発射間隔調整用時間
		int isEmit;          //!< 発射許可(bool)
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* CS *//
	std::unique_ptr<DxObject::CSBlob>          csInitBlob_;
	std::unique_ptr<DxObject::CSPipelineState> csInitPipeline_;

	std::unique_ptr<DxObject::CSBlob>          csBlob_;
	std::unique_ptr<DxObject::CSPipelineState> csPipeline_;

	//* buffers *//
	static const uint32_t kParticleNum = 1024;

	std::unique_ptr<DxObject::CSBufferResource<ParticleCS>> particleBuffer_;
	std::unique_ptr<DxObject::BufferResource<uint32_t>>     informationBuffer_;

	std::unique_ptr<DxObject::BufferResource<EmitterSphere>> emitterBuffer_;

	//* Graphics *//
	std::unique_ptr<DxObject::GraphicsBlob>     blob_;
	std::unique_ptr<DxObject::GraphicsPipeline> pipeline_;

	//* IA *//
	DrawData plane_;

};
