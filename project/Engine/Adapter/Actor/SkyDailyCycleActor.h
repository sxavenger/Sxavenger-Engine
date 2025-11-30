#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Component/Entity/MonoBehaviour.h>
#include <Engine/Preview/Asset/AssetTexture.h>
#include <Engine/Preview/Asset/AssetParameter.h>

//* component
#include <Engine/Component/Components/Transform/TransformComponent.h>
#include <Engine/Component/Components/Light/Environment/SkyLightComponent.h>
#include <Engine/Component/Components/Light/Environment/SkyAtmosphereComponent.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SkyDailyCycleActor class
////////////////////////////////////////////////////////////////////////////////////////////
class SkyDailyCycleActor
	: public MonoBehaviour {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SkyDailyCycleActor();

	void Update();

	void SetNightEnvironmentTexture(const AssetParameter<AssetTexture>& texture);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* components *//

	TransformComponent* transform_;
	SkyLightComponent* skyLight_;
	SkyAtmosphereComponent* skyAtmosphere_;

	//* parameters *//

	float dayIntensity_   = 1.0f;
	float nightIntensity_ = 1.0f;

};
