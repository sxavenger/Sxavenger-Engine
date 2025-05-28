#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Asset/AssetStorage.h>
#include <Engine/Asset/Observer/AssetObserver.h>
#include <Engine/Component/Components/Transform/TransformComponent.h>
#include <Engine/Component/Components/Collider/ColliderComponent.h>
#include <Engine/Component/Components/Particle/ParticleComponent.h>
#include <Engine/Module/GameObject/GameObject.h>

////////////////////////////////////////////////////////////////////////////////////////////
// LeadParticle class
////////////////////////////////////////////////////////////////////////////////////////////
class LeadParticle
	: public GameObject {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Load() override;

	void Awake() override;

	void Start() override;

	void Update() override;

	//* inspectable *//

	void Inspectable() override;

	//* setter *//

	void SetTarget(const Vector3f& target);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* component *//

	ParticleComponent* component_  = nullptr;
	TransformComponent* transform_ = nullptr;
	ColliderComponent* collider_ = nullptr;

	Vector3f prePosition_;

	AssetObserver<AssetTexture> texture_;

	float range_ = 0.5f;

	Vector3f targetPosition_ = {};
	float speed_ = 0.2f;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void UpdateEmitter();

	void UpdateEmit();

	void Emit(const Vector3f& position);

};
