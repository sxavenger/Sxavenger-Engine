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
	// Sphere structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Sphere {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		Vector3f Emit() const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		float radius = 1.0f;

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

	TimePointf<TimeUnit::second> time_       = {};
	DeltaTimePointf<TimeUnit::second> timer_ = {};

	//=========================================================================================
	// private methods
	//=========================================================================================

	const TransformComponent* RequireTransform() const;

};
