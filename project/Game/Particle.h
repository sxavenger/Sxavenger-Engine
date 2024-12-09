#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Performance.h>
#include <Engine/System/DxObject/DxBufferResource.h>
#include <Engine/System/DxObject/DxShaderBlob.h>
#include <Engine/System/DxObject/DxGraphicsPipeline.h>
#include <Engine/Game/Behavior/BaseBehavior.h>
#include <Engine/Game/Model.h>
#include <Engine/Game/Transform.h>
#include <Engine/Game/InputAssembler.h>

//* lib
#include <Lib/Geometry/Vector4.h>
#include <Lib/Adapter/Random/Random.h>
#include <Lib/Geometry/VectorComparison.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ParitcleCollection class
////////////////////////////////////////////////////////////////////////////////////////////
class ParitcleCollection
	: public BaseBehavior {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void Term();

	void Update();

	void DrawAdaptive(_MAYBE_UNUSED const Camera3D* camera) override;

	void SetAttributeImGui() override;

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
		Vector3f velocity;
		Color4f color;

		DeltaTimePoint lifeTime;
		DeltaTimePoint currentTime;
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Emitter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Emitter {
		EulerTransform transform;
		uint32_t count;
		DeltaTimePoint freqency;
		DeltaTimePoint freqencyTimer;
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// AccelerationField structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct AccelerationField {
		Vector3f acceleration;
		Vector3f min;
		Vector3f max;

		bool IsCollision(const Vector3f& point) {
			if (All(point >= min) && All(point <= max)) {
				return true;
			}

			return false;
		}
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	Model* model_;

	//* buffer *//

	static const uint32_t kMaxInstanceCount_ = 16;
	uint32_t instanceCount_ = 0;

	std::unique_ptr<DxObject::BufferResource<TransformationMatrix>> transformBuffer_;
	std::unique_ptr<DxObject::BufferResource<ParticleInfo>> infoBuffer_;

	std::list<ParticleElement> elements_;

	//* pipeline *//

	std::unique_ptr<DxObject::GraphicsBlob>     blob_;
	std::unique_ptr<DxObject::GraphicsPipeline> pipeline_;

	Color4f color_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	//* emitter *//

	Emitter emitter_ = {};

	//* field *//

	AccelerationField field_ = {};

	bool isApplyField_ = true;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreatePipeline();
	void CreateBuffer();

	void UpdateParticle();
	void UpdateEmitter();

	void MakeParticle(const Vector3f& transform);
	void Emit();
};