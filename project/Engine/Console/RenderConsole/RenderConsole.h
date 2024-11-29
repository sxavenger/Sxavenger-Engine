#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* console
#include "RenderBehavior.h"

//* engine
#include <Engine/Module/Behavior/BaseBehavior.h>

//* c++
#include <optional>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class Console;

////////////////////////////////////////////////////////////////////////////////////////////
// RenderConsole class
////////////////////////////////////////////////////////////////////////////////////////////
class RenderConsole {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	RenderConsole()  = default;
	~RenderConsole() = default;

	void Init(Console* console);

	void Term();

	void UpdateConsole();

	//* behavior container option *//

	BehaviorIterator SetBehavior(BaseBehavior* behavior);

	void EraseBehavior(const BehaviorIterator& iterator);

	void ResetBehavior();

	void RemoveAttributeBehavior(const BehaviorIterator& iterator);

	void ResetAttributeBehavior();

	void RemoveUniqueBehavior();

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	bool isDisplayRenderConsole_ = true;

	//=========================================================================================
	// protected methods
	//=========================================================================================

	//* menu *//

	void ShowRenderConsoleMenu();
	void ShowBehaviorMenu();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	Console* console_ = nullptr;

	//* behavior *//

	BehaviorContainer outliner_;

	std::optional<BehaviorIterator> attributeIterator_ = std::nullopt; //!< 現在選択されているitretor
	const BehaviorContainer*        attributeTable_    = nullptr;      //!< 選択されているiteratorのコンテナ

	//* render behavior *//

	std::list<std::unique_ptr<RenderBehavior>> behaviors_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* window *//

	void DisplayOutliner();
	void DisplayAttribute();

	//* behavior methods *//

	bool IsSelectedBehavior(BaseBehavior* behavior);
	void SelectableBehavior(const BehaviorContainer& container);

	void UpdateUniqueRemove();


	static void MenuDummy();

};