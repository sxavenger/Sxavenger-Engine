#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#include <imgui.h>

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
	~RenderingConsole() = default;

	void Init();

	void Term();

	void UpdateConsole();

	//* behavior option *//

	void SetBehavior(BaseBehavior* behavior);

	void RemoveSelectedBehavior(BaseBehavior* behavior);

	void RemoveBehavior(BaseBehavior* behavior);


private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* behavior *//
	//* container
	std::list<BaseBehavior*>                                behaviors_;

	//* selected
	const std::list<BaseBehavior*>*                         selectedBehaviorTable_ = nullptr;      //!< selectされてるbahevior container
	std::optional<std::list<BaseBehavior*>::const_iterator> selectedBehavior_      = std::nullopt; //!< selectされているbehavior



	//=========================================================================================
	// private methods
	//=========================================================================================

	//* display methods *//

	void DisplayOutliner();
	void DisplayAttribute();

	//* behavior methods *//

	bool IsSelectedBehavior(BaseBehavior* behavior);
	void SelectableBehavior(const std::list<BaseBehavior*>::const_iterator& it, const std::list<BaseBehavior*>& table);

};