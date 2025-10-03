#pragma once


//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Asset/AssetStorage.h>
#include <Engine/Asset/Observer/AssetObserver.h>
#include <Engine/Component/Components/Transform/TransformComponent.h>
#include <Engine/Component/Components/Particle/ParticleComponent.h>
#include <Engine/Module/GameObject/GameObject.h>

////////////////////////////////////////////////////////////////////////////////////////////
// RainParticle class
////////////////////////////////////////////////////////////////////////////////////////////
class RainParticle
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

	AssetObserver<AssetTexture> texture_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateParticle();

	void Emit();

};
