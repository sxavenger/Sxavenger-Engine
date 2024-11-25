#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
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
class Camera; //!< 基底のcameraに合わせる

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
using BehaviorOutliner = std::list<std::shared_ptr<BaseBehavior>>; //!< Consoleでのcontainer
using BehaviorNode     = std::list<BaseBehavior*>;                 //!< Node関係のlist
using BehaviorIterator = std::variant<BehaviorOutliner::const_iterator, BehaviorNode::const_iterator>; //!< erase用iterator

////////////////////////////////////////////////////////////////////////////////////////////
// BaseBehavior class
////////////////////////////////////////////////////////////////////////////////////////////
class BaseBehavior {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BaseBehavior()  = default;
	~BaseBehavior() = default;

	//* behavior option *//



	void SetName(const std::string& name) { name_ = name; }

	void SetChild(BaseBehavior* child);
	void RemoveChild(BaseBehavior* child);

	//* derivative behaivor methods *//
	//* ImGui command
	virtual void SystemAttributeImGui() {} //!< system using
	virtual void SetAttributeImGui() {}    //!< user側で定義可能

	//* getter *//

	const std::string& GetName() const { return name_; }

	const uint32_t GetRenderingFlag() const { return renderingFlag_; }

	const BehaviorNode& GetChildren() const { return children_; }

	const bool IsActive() const { return isActive_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::string name_ = "New Behavior";

	uint32_t renderingFlag_ = kBehaviorRendering_None;
	bool isActive_          = true;

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
	BehaviorNode children_;
	
};