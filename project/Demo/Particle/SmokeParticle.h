#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Asset/AssetCollection.h>
#include <Engine/Asset/Observer/AssetObserver.h>
#include <Engine/Component/Components/Transform/TransformComponent.h>
#include <Engine/Component/Components/Particle/ParticleComponent.h>
#include <Engine/Module/GameObject/GameObject.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SmokeParticle class
////////////////////////////////////////////////////////////////////////////////////////////
class SmokeParticle
	: public GameObject {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Load() override;

	void Awake() override;

	void Start() override;

	void Update() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	ParticleComponent* component_ = nullptr;

	static inline const size_t kSubComponentCount = 1;
	std::array<std::unique_ptr<MonoBehaviour>, kSubComponentCount> subParticles_;
	std::array<ParticleComponent*, kSubComponentCount> subComponents_;

	AssetObserver<AssetTexture> textures_[2];

	

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateParticle();

	void Emit();

};
