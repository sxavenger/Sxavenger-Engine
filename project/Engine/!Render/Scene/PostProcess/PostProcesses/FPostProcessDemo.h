#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* process
#include "../FPostProcess.h"

//* engine
#include <Engine/Content/TextureBuffer/UnorderedTexture.h>
#include <Engine/Module/Pipeline/CustomComputePipeline.h>
#include <Engine/Editor/EditorComponent/AttributeComponent.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FPostProcessDemo class
////////////////////////////////////////////////////////////////////////////////////////////
class FPostProcessDemo
	: public FPostProcess, public AttributeComponent {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// struct Parameter
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Parameter {
		float radius;
		float maxRadius;
		float angleBias;
		float strength;
		float filtter;
		Vector2f scale;
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FPostProcessDemo()          = default;
	virtual ~FPostProcessDemo() = default;

	void Init();

	//* process *//

	virtual void Process(const FPostProcess::ProcessContext& context) override;

	virtual void AttributeImGui() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	bool isActive_ = true;

	std::unique_ptr<CustomReflectionComputePipeline> pipeline_;
	std::unique_ptr<CustomReflectionComputePipeline> pipeline1_;

	std::unique_ptr<DxObject::DimensionBuffer<Parameter>> parameter_;

	std::unique_ptr<UnorderedTexture> buffer_;

};
