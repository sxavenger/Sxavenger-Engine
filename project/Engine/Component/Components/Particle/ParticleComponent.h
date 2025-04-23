#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/System/DirectX/DxObject/DxGraphicsPipelineState.h>
#include <Engine/System/Runtime/Performance/TimePoint.h>
#include <Engine/System/Runtime/Performance/DeltaTimePoint.h>
#include <Engine/Content/InputGeometry/InputPrimitive.h>
#include <Engine/Component/Components/Camera/CameraComponent.h>

//* lib
#include <Lib/Geometry/Vector3.h>
#include <Lib/Transform/Transform.h>
#include <Lib/Motion/Motion.h>

//* c++
#include <memory>
#include <functional>
#include <list>

////////////////////////////////////////////////////////////////////////////////////////////
// ParticleComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class ParticleComponent final
	: public BaseComponent {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Particle structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Particle {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Update();

		bool IsDelete() const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		//* time
		DeltaTimePointf<TimeUnit::second> time;
		TimePointf<TimeUnit::second> lifeTime;

		//* transform
		QuaternionTransform transform;
		MotionT<Vector3f> velocity;


	private:
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ParticleComponent(MonoBehaviour* behaviour) : BaseComponent(behaviour) {}
	~ParticleComponent() override = default;

	void Init(uint32_t count);

	void Update();


	void SetPrimitive(InputPrimitive&& primitive);

	Particle& Emit(const Vector3f& position);

	void DrawParticle(const DirectXThreadContext* context, const CameraComponent* camera);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* emitter paraemter *//

	//std::function<Vector3f()> emitFunction_;

	//* particle parameter *//

	//* particle *//

	//- primitive mesh
	//- buffer
	//- transform
	//- material
	//- intermediate

	// primtive mesh
	std::optional<InputPrimitive> primitive_;

	// transforms
	std::unique_ptr<DxObject::DimensionBuffer<TransformationMatrix>> matrices_;

	// material


	// intermediate
	uint32_t instance_ = 0;

	// element
	std::list<Particle> particles_;

	//* HACK: pipeline *//

	std::unique_ptr<DxObject::ReflectionGraphicsPipelineState> pipeline_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void AssingBuffer();
};
