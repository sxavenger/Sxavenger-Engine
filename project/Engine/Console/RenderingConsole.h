#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Game/SxavengerPipeline/RenderingPipeline.h>
#include <Engine/Game/SxavengerPipeline/SxavengerFrame.h>

//* c++
#include <list>
#include <optional>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class BaseBehavior;

////////////////////////////////////////////////////////////////////////////////////////////
// RenderingConsole class
////////////////////////////////////////////////////////////////////////////////////////////
class RenderingConsole {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	RenderingConsole()  = default;
	~RenderingConsole() = default;

	void Init();

	void Term();

	void UpdateConsole();

	void RenderSystematic(SxavengerFrame* frame);
	void RenderAdaptive(SxavengerFrame* frame);

	//* behavior option *//

	void SetBehavior(BaseBehavior* behavior);

	void RemoveSelectedBehavior(BaseBehavior* behavior);

	void RemoveBehavior(BaseBehavior* behavior);

	//* pipeline option *//

	void SetRenderingPipeline(RenderingPipelineType type) const;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* window *//

	bool isDisplayRenderingConsole_ = true;

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// behavior container using
	////////////////////////////////////////////////////////////////////////////////////////////
	using BehaviorContainer = std::list<BaseBehavior*>;

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* behavior *//
	//* container
	BehaviorContainer behaviors_;

	//* selected
	const BehaviorContainer*                         selectedBehaviorTable_ = nullptr;      //!< selectされてるbahevior container
	std::optional<BehaviorContainer::const_iterator> selectedBehavior_      = std::nullopt; //!< selectされているbehavior

	//* pipeline *//

	std::unique_ptr<RenderingPipeline> renderingPipeline_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* display methods *//

	void DisplayOutliner();
	void DisplayAttribute();

	//* behavior methods *//

	bool IsSelectedBehavior(BaseBehavior* behavior);
	void SelectableBehavior(const BehaviorContainer::const_iterator& it, const BehaviorContainer& table);

	void DrawSystematicBehavior(BaseBehavior* behavior, SxavengerFrame* frame);
	void DrawAdaptiveBehavior(BaseBehavior* behavior, SxavengerFrame* frame);

};