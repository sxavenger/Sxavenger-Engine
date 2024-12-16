#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/Module/Behavior/TransformBehavior.h>

//* engine
#include <Engine/System/Runtime/Performance/DeltaTimePoint.h>

//* particle
#include "BaseParticle.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Emitter class
////////////////////////////////////////////////////////////////////////////////////////////
class Emitter
	: public TransformBehavior {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Emitter()  = default;
	~Emitter() = default;

	void Init();

	void Term();

	void Update();

	void SetAttributeImGui() override;

	virtual void DrawLateAdaptive(_MAYBE_UNUSED const SxavGraphicsFrame* frame) override;

	//* setter *//

	void SetParticle(BaseParticle* particle) { particle_ = particle; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external

	BaseParticle* particle_ = nullptr;

	//* AABB emitter
	Vector3f min_ = { -1.0f, -1.0f, -1.0f };
	Vector3f max_ = { 1.0f, 1.0f, 1.0f };

	DeltaTimePoint<TimeUnit::s> emitTime_ = { 0.2f };
	DeltaTimePoint<TimeUnit::s> emitTimer_;

	uint32_t emitCount_ = 1;

};
