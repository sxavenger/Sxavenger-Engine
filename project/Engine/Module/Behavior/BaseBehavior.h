#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/CXXAttributeConfig.h>

//* c++
#include <list>
#include <string>
#include <optional>
#include <bitset>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class BaseBehavior;
class SxavGraphicsFrame;

////////////////////////////////////////////////////////////////////////////////////////////
// BehaviorRenderingType enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class BehaviorRenderingType : uint8_t {
	kSystematic,
	kAdaptive,
	kLateAdaptive,
	kRaytracing,
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

	//* rendering option *//

	void SetRenderingFlag(BehaviorRenderingType type, bool isRendering = true) { renderingFlag_.set(static_cast<uint8_t>(type), isRendering); }

	//* derivative behaivor methods *//
	//* ImGui command

	void BaseAttributeImGui();
	virtual void SystemAttributeImGui() {} //!< system using
	virtual void SetAttributeImGui() {}    //!< user側で定義可能

	//* Draw methods

	virtual void DrawSystematic(_MAYBE_UNUSED const SxavGraphicsFrame*) {}
	virtual void DrawAdaptive(_MAYBE_UNUSED const SxavGraphicsFrame*) {}
	virtual void DrawLateAdaptive(_MAYBE_UNUSED const SxavGraphicsFrame*) {}


	//* getter *//

	const std::string& GetName() const { return name_; }

	bool CheckRenderingFlag(BehaviorRenderingType type) const { return renderingFlag_.test(static_cast<uint8_t>(type)); }

	const BehaviorContainer& GetChildren() const { return children_; }

	bool IsActive() const { return isActive_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::string name_ = "New Behavior";

	bool isActive_ = true; //!< TODO: methods define

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* iterator and node *//

	//!< listとして登録された場合, iteratorが保存される.
	//* outlinerとchildは両方となることはない.
	std::optional<BehaviorIterator> iterator_ = std::nullopt;

	//!< 自身の親
	BaseBehavior* parent_ = nullptr;

	//!< 自身が親となっているnode
	BehaviorContainer children_;

	//* rendering flag *//

	std::bitset<static_cast<uint8_t>(BehaviorRenderingType::kRaytracing) + 1> renderingFlag_ = {};

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CheckIterator();

};
