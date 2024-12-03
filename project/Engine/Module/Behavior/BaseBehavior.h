#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/CXXAttributeConfig.h>

//* c++
#include <list>
#include <memory>
#include <string>
#include <optional>
#include <variant>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class BaseBehavior;
class SxavGraphicsFrame;

////////////////////////////////////////////////////////////////////////////////////////////
// BehaviorRenderingFlag enum
////////////////////////////////////////////////////////////////////////////////////////////
enum BehaviorRenderingFlag {
	kBehaviorRendering_None         = 0,
	kBehaviorRendering_Systematic   = 1 << 0,
	kBehaviorRendering_Adaptive     = 1 << 1,
	kBehaviorRendering_LateAdaptive = 1 << 2,
	kBehaviorRendering_Raytracing   = 1 << 3,
};

////////////////////////////////////////////////////////////////////////////////////////////
// using
////////////////////////////////////////////////////////////////////////////////////////////
using BehaviorContainer = std::list<BaseBehavior*>;          //!< behavior list
using BehaviorIterator  = BehaviorContainer::const_iterator; //!< behavior iterator

////////////////////////////////////////////////////////////////////////////////////////////
// BaseBehavior class
////////////////////////////////////////////////////////////////////////////////////////////
class BaseBehavior {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BaseBehavior()  = default;
	~BaseBehavior() { Term(); }

	void Term();

	//* behavior option *//

	void SetToConsole();
	void SetToConsole(const std::string& name);

	void SetName(const std::string& name) { name_ = name; }

	void SetChild(BaseBehavior* child);
	void EraseChild(BaseBehavior* child);

	void ResetIterator();

	//* derivative behaivor methods *//
	//* ImGui command
	virtual void SystemAttributeImGui() {} //!< system using
	virtual void SetAttributeImGui() {}    //!< user側で定義可能

	//* Draw methods

	virtual void DrawSystematic(_MAYBE_UNUSED const SxavGraphicsFrame*) {}
	virtual void DrawAdaptive(_MAYBE_UNUSED const SxavGraphicsFrame*) {}
	virtual void DrawLateAdaptive(_MAYBE_UNUSED const SxavGraphicsFrame*) {}

	//* getter *//

	const std::string& GetName() const { return name_; }

	const uint32_t GetRenderingFlag() const { return renderingFlag_; }

	const BehaviorContainer& GetChildren() const { return children_; }

	const bool IsActive() const { return isActive_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::string name_ = "New Behavior";

	uint32_t renderingFlag_ = kBehaviorRendering_None;
	bool isActive_          = true; //!< TODO: methods define

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//!< listとして登録された場合, iteratorが保存される.
	//* outlinerとchildは両方となることはない.
	std::optional<BehaviorIterator> iterator_ = std::nullopt;

	//!< 自身の親
	BaseBehavior* parent_ = nullptr;

	//!< 自身が親となっているnode
	BehaviorContainer children_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CheckIterator();

};