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

private:

	//=========================================================================================
	// private variables
	//=========================================================================================


	//* component *//

	ParticleComponent* component_  = nullptr;
	TransformComponent* transform_ = nullptr;

	Vector3f prePosition_;
	Vector3f position_;

	AssetObserver<AssetTexture> texture_;

	float range_ = 0.5f;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void Emit(const Vector3f& position);

};
