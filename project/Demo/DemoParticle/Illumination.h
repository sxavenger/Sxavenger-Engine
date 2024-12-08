#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/Module/Behavior/InstanceBehavior.h>

//* engine
#include <Engine/System/DirectX/DxObject/DxGraphicsPipelineState.h>
#include <Engine/Asset/AssetLibrary/Model/AssetModel.h>
#include <Engine/Module/VisualLayer/VisualDoF.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Illmaination class
////////////////////////////////////////////////////////////////////////////////////////////
class Illmination
	: public InstanceBehavior {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Illmination()  = default;
	~Illmination() = default;

	void Init();

	//* derivative behaivor methods *//

	void SetAttributeImGui();

	void DrawAdaptive(_MAYBE_UNUSED const SxavGraphicsFrame* frame) override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static const uint32_t kInstanceCount_ = 12;

	std::shared_ptr<AssetModel> model_;

	MaterialComponent material_;
	MaterialComponent materialPoint_;

	std::unique_ptr<DxObject::ReflectionGraphicsPipelineState> stateShape_;
	std::unique_ptr<DxObject::ReflectionGraphicsPipelineState> statePoint_;

	std::unique_ptr<VisualDoF> dof_;


};