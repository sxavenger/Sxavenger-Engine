#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"
#include "../Transform/TransformComponent.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/Assets/Asset/AssetTexture.h>
#include <Engine/Assets/Asset/AssetParameter.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// DecalRendererComponent class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief デカールの描画component.
class DecalRendererComponent final
	: public BaseComponent {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// DecalRendererComponent class
	////////////////////////////////////////////////////////////////////////////////////////////

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DecalRendererComponent(EntityBehaviour* behaviour) : BaseComponent(behaviour) {}
	~DecalRendererComponent() override = default;

	void ShowComponentInspector() override;

	//* component option *//

	void SetTexture(const AssetParameter<AssetTexture>& texture) { texture_ = texture; }

	const AssetParameter<AssetTexture>& GetTexture() const { return texture_; }

	//* behaviour option *//

	//! @brief TransformComponentを取得
	const TransformComponent* GetTransform() const;

	//! @brief TransformComponentを取得
	//! @throw TransformComponentがEntityに存在しない場合に例外をスローする
	const TransformComponent* RequireTransform() const;

	//* json option *//

	json ParseToJson() const override;

	void InputJson(const json& data) override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	AssetParameter<AssetTexture> texture_ = nullptr;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void PushDecalLine() const;

};

SXAVENGER_ENGINE_NAMESPACE_END
