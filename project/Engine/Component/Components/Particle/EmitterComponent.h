#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"
#include "../Transform/TransformComponent.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/System/DirectX/DxObject/DxComputePipelineState.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>
#include <Engine/System/Runtime/Performance/DeltaTimePoint.h>

////////////////////////////////////////////////////////////////////////////////////////////
// EmitterComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class EmitterComponent final
	: public BaseComponent {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Emitter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Emitter {
		//!< Sphere Emitter想定
		// TODO: 他のEmitterも実装する
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init();

		void Seed();

		//=========================================================================================
		// public variables
		//=========================================================================================

		Vector3f seed;
		uint32_t count;
		float radius;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	EmitterComponent(MonoBehaviour* behaviour) : BaseComponent(behaviour) { Init(); }
	~EmitterComponent() override = default;

	void Init();

	//* emitter option *//

	void Update(const DirectXQueueContext* context);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<DxObject::DimensionBuffer<Emitter>> emitter_;

	TimePointf<TimeUnit::second> time_       = {};
	DeltaTimePointf<TimeUnit::second> timer_ = {};


	std::unique_ptr<DxObject::ReflectionComputePipelineState> pipeline_; //!< HACK


	//=========================================================================================
	// private methods
	//=========================================================================================

	void EmitGPUParticle(const DirectXQueueContext* context);
	void EmitCPUParticle();

	//* helepr methods *//

	const TransformComponent* RequireTransform() const;

};
