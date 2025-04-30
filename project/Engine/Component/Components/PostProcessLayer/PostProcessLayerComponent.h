#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"
#include "BasePostProcess.h"

//* c++
#include <list>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessLayerComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class PostProcessLayerComponent
	: public BaseComponent {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Tag enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Tag {
		None
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	PostProcessLayerComponent(MonoBehaviour* behaviour) : BaseComponent(behaviour) { }
	~PostProcessLayerComponent() override = default;

	void ShowComponentInspector() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::list<std::unique_ptr<BasePostProcess>> processes_;

	Tag tag_ = Tag::None;

};
