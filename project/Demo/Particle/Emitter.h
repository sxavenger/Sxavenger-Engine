#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/Module/Behavior/TransformBehavior.h>

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
	~Emitter() { Term(); }

	void Init();

	void Term();

	void Update();

	void SetAttributeImGui() override;

	virtual void DrawLateAdaptive(_MAYBE_UNUSED const SxavGraphicsFrame* frame) override;

	//* setter *//

	void SetParticle(BaseParticle* particle) { particle_ = particle; }

	//* getter *//

	const Vector3f GetEmitPosition() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	BaseParticle* particle_ = nullptr;

	//* AABB emitter
	Vector3f max_ = { 8.0f, 1.0f, 8.0f };
	Vector3f min_ = { -8.0f, -1.0f, -8.0f };

	DeltaTimePoint<TimeUnit::s> emitTime_ = { 0.2f };
	DeltaTimePoint<TimeUnit::s> emitTimer_;

	int32_t emitCount_ = 1;

};