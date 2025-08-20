#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Component/Entity/MonoBehaviour.h>
#include <Engine/Render/Scene/Environment/FEnvironmentMap.h>
#include <Engine/Preview/Asset/UAssetTexture.h>
#include <Engine/Preview/Asset/UAssetParameter.h>

//* component
#include <Engine/Component/Components/Light/Environment/SkyLightComponent.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SkyLightActor class
////////////////////////////////////////////////////////////////////////////////////////////
class SkyLightActor
	: public MonoBehaviour {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SkyLightActor() = default;
	~SkyLightActor() { Term(); }

	void Init(const Vector2f& size);

	void Term();

	void Update(bool isWait = false);

	void Inspectable() override;

	//* option *//

	void SetTexture(const UAssetParameter<UAssetTexture>& texture);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	FEnvironmentMap environment_;

	//* components *//

	SkyLightComponent* skyLightComponent_;

	//* texture *//

	UAssetParameter<UAssetTexture> texture_; //!< sky lightの環境テクスチャ

};
