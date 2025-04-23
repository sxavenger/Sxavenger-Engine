#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/System/DirectX/DxObject/DxGraphicsPipelineState.h>
#include <Engine/Content/InputGeometry/InputPrimitive.h>
#include <Engine/Component/Components/Camera/CameraComponent.h>

//* lib
#include <Lib/Geometry/Vector3.h>
#include <Lib/Transform/Transform.h>

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
	//* nothing
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ParticleComponent(MonoBehaviour* behaviour) : BaseComponent(behaviour) {}
	~ParticleComponent() override = default;

	void Init(uint32_t count);

	void Update();


	void SetPrimitive(InputPrimitive&& primitive);

	void Emit(const Vector3f& position);

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
	std::list<QuaternionTransform>                                   transforms_;

	// material


	// intermediate
	uint32_t instance_ = 0;

	//* HACK: pipeline *//

	std::unique_ptr<DxObject::ReflectionGraphicsPipelineState> pipeline_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void AssingBuffer();
};
