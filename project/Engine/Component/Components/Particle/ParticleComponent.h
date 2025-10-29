#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* particle
#include "ParticleElement.h"

//* component
#include "../BaseComponent.h"
#include "../Camera/CameraComponent.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxGraphicsPipelineState.h>
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/System/Runtime/Performance/DeltaTimePoint.h>
#include <Engine/Content/InputGeometry/InputPrimitive.h>
#include <Engine/Preview/Asset/UAssetTexture.h>
#include <Engine/Preview/Asset/UAssetParameter.h>

//* lib
#include <Lib/Motion/Motion.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ParticleComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class ParticleComponent
	: public BaseComponent {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Parameter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Parameter {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		TimePointf<TimeUnit::second> time = { 0.0f };
		MotionT<Vector3f> scale           = MotionT<Vector3f>::CreateLinear(kUnit3<float>, kOrigin3<float>);
		MotionT<float> velocity           = MotionT<float>::CreateLinear(1.0f, 0.0f);
		MotionT<Color3f> color            = MotionT<Color3f>::CreateLinear(kWhite3<float>, kBlack3<float>);
		MotionT<float> transparent        = MotionT<float>::CreateLinear(1.0f, 0.0f);
		
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Instance structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Instance {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Update();

		//=========================================================================================
		// public variables
		//=========================================================================================

		DeltaTimePointf<TimeUnit::second> timer = {};

		bool isDelete = false;

		QuaternionTransform transform = {};
		Vector3f velocity             = kOrigin3<float>;

		Color3f color     = kWhite3<float>;
		float transparent = 1.0f;

		Parameter parameter = {};
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ParticleComponent(MonoBehaviour* behaviour);
	~ParticleComponent() = default;

	void ShowComponentInspector() override;

	void Init(uint32_t elementCount, BlendMode mode);

	void Update();

	//* particle option *//

	void SetPrimitive(InputPrimitive&& primitive);

	void SetFunction(const std::function<Parameter()>& function) { function_ = function; }

	void Emit(const Vector3f& position, const Vector3f& direction);

	void DrawParticle(const DirectXQueueContext* context, const CameraComponent* camera);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	UAssetParameter<UAssetTexture> albedoTexture_;
	UAssetParameter<UAssetTexture> transparentTexture_;

	std::function<Parameter()> function_ = []() { return Parameter(); }; //!< HACK!!!

	std::list<Instance> instances_;

	//* buffers *//

	std::unique_ptr<DxObject::DimensionBuffer<ParticleElement>> elements_;
	std::unique_ptr<DxObject::DimensionBuffer<TransformationMatrix>> matrices_;

	uint32_t elementCount_ = 0;

	// primtive mesh
	std::optional<InputPrimitive> primitive_;

	//* pipeline *//

	std::unique_ptr<DxObject::ReflectionGraphicsPipelineState> pipeline_;
	// HACK:

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreatePipeline(BlendMode mode);

	void UpdateInstance();

	void UpdateBuffers();

};
