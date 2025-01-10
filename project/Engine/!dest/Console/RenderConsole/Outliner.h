#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* console
#include "RenderBehavior.h"

//* engine
#include <Engine/Module/Behavior/BaseBehavior.h>

//* external
#include <imgui.h>

//* c++
#include <list>
#include <optional>
#include <memory>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class Console;

////////////////////////////////////////////////////////////////////////////////////////////
// Outliner class
////////////////////////////////////////////////////////////////////////////////////////////
class Outliner {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Outliner()  = default;
	~Outliner() = default;

	void Init(Console* console);

	void Term();

	//* behavior option *//

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

	//* unique console behavior *//

	std::list<std::unique_ptr<RenderBehavior>> behaviors_;

	//=========================================================================================
	// protected methods
	//=========================================================================================

	//* process *//

	void UpdateUniqueRemove();

	//* window *//

	void DisplayOutliner();

	void DisplayAttribute();

	//* draw *//
 
	void DrawSystematicBehavior(SxavGraphicsFrame* frame, const BehaviorContainer& container);

	void DrawAdaptiveBehavior(SxavGraphicsFrame* frame, const BehaviorContainer& container);

	void DrawLateAdaptiveBehavior(SxavGraphicsFrame* frame, const BehaviorContainer& container);

	//* getter *//

	const BehaviorContainer& GetOutliner() const { return outliner_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	Console* console_ = nullptr;

	//* behavior *//

	BehaviorContainer outliner_;

	std::optional<BehaviorIterator> attributeIterator_ = std::nullopt; //!< 現在選択されているitretor
	const BehaviorContainer* attributeTable_           = nullptr;      //!< 選択されているiteratorのコンテナ

	//* parameter *//

	static const ImVec4 kBehaviorDisableColor_; //!< behavior disable color

	//=========================================================================================
	// private methods
	//=========================================================================================

	bool IsSelectedBehavior(BaseBehavior* behavior);

	void SelectableBehavior(const BehaviorContainer& container);

};
