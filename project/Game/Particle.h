#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/Game/Behavior/ModelInstanceBehavior.h>

//* engine
#include <Engine/System/Performance.h>
#include <Engine/Beta/Curve.h>

//* c++
#include <list>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Particle class
////////////////////////////////////////////////////////////////////////////////////////////
class Particle
	: public ModelInstanceBehavior {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Particle()  = default;
	~Particle() { Term(); }

	void Init(const Vector3f& position, const Vector3f& velocity, const Curve* curve);

	void Term();

	void Update();

	bool IsDelete() const { return isDelete_; }

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// ParticleElement strcture
	////////////////////////////////////////////////////////////////////////////////////////////
	struct ParticleElement {
		QuaternionTransform transform;
		DeltaTimePoint      lifeTime;
		Vector3f            velocity;
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* element parameter *//

	bool isDelete_ = false;

	static const uint32_t kInstanceCount_ = 12;

	DeltaTimePoint current_ = {};
	DeltaTimePoint lifeTime_ = { 1.0f };

	std::vector<ParticleElement> elements_;

	const Curve* curve_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// ParticleCollection class
////////////////////////////////////////////////////////////////////////////////////////////
class ParticleCollection
	: public BaseBehavior {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ParticleCollection()  = default;
	~ParticleCollection() { Term(); }

	void Init();

	void Update();

	void Term();

	void CreateParticle(const Vector3f& position, const Vector3f& velocity);

	void SetAttributeImGui();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================
	
	std::list<std::unique_ptr<Particle>> particles_;

	std::unique_ptr<Curve> curve_;

};