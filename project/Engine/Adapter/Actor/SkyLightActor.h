#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Component/Entity/MonoBehaviour.h>
#include <Engine/Render/Scene/Environment/FEnvironmentMap.h>
#include <Engine/Preview/Asset/AssetTexture.h>
#include <Engine/Preview/Asset/AssetParameter.h>

//* component
#include <Engine/Component/Components/Light/Environment/SkyLightComponent.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SkyLightActor class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief skylightを表現するクラス
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

	void SetTexture(const AssetParameter<AssetTexture>& texture);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	FEnvironmentMap environment_;

	//* components *//

	SkyLightComponent* skyLightComponent_;

	//* texture *//

	AssetParameter<AssetTexture> texture_; //!< sky lightの環境テクスチャ

};
