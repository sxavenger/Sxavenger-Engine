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
class RenderingConsole final {
	//!< system console で unique_ptrとして保持させる予定
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	RenderingConsole()  = default;
	~RenderingConsole() { Term(); }

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

	void SetPipeline(RenderingPipelineType type) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* behavior *//
	//* container
	std::list<BaseBehavior*> behaviors_;

	//* selected
	const std::list<BaseBehavior*>*                         selectedBehaviorTable_ = nullptr;      //!< selectされてるbahevior container
	std::optional<std::list<BaseBehavior*>::const_iterator> selectedBehavior_      = std::nullopt; //!< selectされているbehavior

	//* pipeline *//

	RenderingPipeline pipeline_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* display methods *//

	void DisplayOutliner();
	void DisplayAttribute();

	//* behavior methods *//

	bool IsSelectedBehavior(BaseBehavior* behavior);
	void SelectableBehavior(const std::list<BaseBehavior*>::const_iterator& it, const std::list<BaseBehavior*>& table);

	void DrawSystematicBehavior(BaseBehavior* behavior, SxavengerFrame* frame);
	void DrawAdaptiveBehavior(BaseBehavior* behavior, SxavengerFrame* frame);

};