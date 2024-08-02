#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// c++
#include <string>
#include <unordered_set>

// imgui
#include <imgui.h>


////////////////////////////////////////////////////////////////////////////////////////////
// Attribute base class
////////////////////////////////////////////////////////////////////////////////////////////
class Attribute {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	~Attribute() { Term(); }

	void Term();

	//! @brief Attributeで選択された際のImGui更新処理
	virtual void SetAttributeImGui();

	const std::string& GetName() const { return name_; }

	/*char* GetNameData() { return name_.data(); }*/

	void SetAttributeName(const char* name) { name_ = name; }

	void SetAttributeName(const std::string& name) { name_ = name; }

	// テスト機能 //
	void SetAttributeNode(Attribute* other);

	void EraseAttributeNode(Attribute* other);

	const std::unordered_set<Attribute*>& GetNode() const { return nodes_; }

	bool IsOpenNode() const { return isOpenNode_; }

	void SetIsOpenNode(bool isOpenNode) { isOpenNode_ = isOpenNode; }

	void SetThisAttribute(const std::string& name);

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::string name_;

	bool isOpenNode_ = true;
	std::unordered_set<Attribute*> nodes_; //!< テスト機能

	//=========================================================================================
	// protected methods
	//=========================================================================================

};