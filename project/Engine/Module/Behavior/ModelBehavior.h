#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include "TransformBehavior.h"

//* engine
#include <Engine/Asset/AssetObserver.h>
#include <Engine/Asset/ModelAnimator/Model/Model.h>
#include <Engine/Module/Material/MaterialComponent.h>

//* c++
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// ModelBehavior class
////////////////////////////////////////////////////////////////////////////////////////////
class ModelBehavior
	: public TransformBehavior, public MaterialComponent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ModelBehavior()  { Init(); }
	~ModelBehavior() = default;

	void Init();

	//* derivative behaivor methods *//
	//* ImGui command

	virtual void SystemAttributeImGui() override;

	//* Draw methods

	virtual void DrawSystematic(_MAYBE_UNUSED const SxavGraphicsFrame* frame) override;
	virtual void DrawAdaptive(_MAYBE_UNUSED const SxavGraphicsFrame* frame) override;
	virtual void DrawLateAdaptive(_MAYBE_UNUSED const SxavGraphicsFrame* frame) override;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::optional<AssetObserver<Model>> model_;
	// HACK: modelがloadされていることが前提となってる.

};
