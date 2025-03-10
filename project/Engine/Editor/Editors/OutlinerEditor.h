#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "BaseEditor.h"

//* c++
#include <list>
#include <string>
#include <functional>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// Attribute class
////////////////////////////////////////////////////////////////////////////////////////////
class Attribute {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	using Container = std::list<Attribute*>;
	using Iterator  = Container::const_iterator;

	////////////////////////////////////////////////////////////////////////////////////////////
	// Status enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Status : uint8_t {
		Unregistered, //!< 未登録
		Outliner      //!< outlinerとして設定
	};

public:

	//=========================================================================================
	// public method
	//=========================================================================================

	Attribute() { SetToOutliner(); }
	~Attribute() { RemoveIterator(); }

	void ExecuteAttribute() { attributeFunc_(); }

	//* attribute option *//

	virtual void AttributeImGui() {}

	//* component option *//

	void SetToOutliner();

	void SetName(const std::string& name) { name_ = name; }

	void RemoveIterator();

	//* getter *//

	const std::string& GetName() const { return name_; }
	std::string& GetName() { return name_; }

	void SetAttributeFunc(const std::function<void()>& func) { attributeFunc_ = func; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::string name_ = "new attribute";

	std::function<void()> attributeFunc_ = [this]() { AttributeImGui(); };

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::optional<Iterator> iterator_;

	Status GetStatus() const;

	bool CheckIteratorEmpty() const;

};

////////////////////////////////////////////////////////////////////////////////////////////
// OutlinerEditor class
////////////////////////////////////////////////////////////////////////////////////////////
class OutlinerEditor
	: public BaseEditor {
public:

	//=========================================================================================
	// public method
	//=========================================================================================

	OutlinerEditor(EditorEngine* editor) : BaseEditor(editor) {}
	~OutlinerEditor() = default;

	void Init() override;

	void ShowMainMenu() override;

	void ShowWindow() override;

	//* option *//

	Attribute::Iterator SetAttribute(Attribute* attribute);

	void RemoveAttribute(const Attribute::Iterator& iterator);

	void CheckSelectClear(const Attribute::Iterator& iterator);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Attribute::Container outliner_;

	Attribute* selected_ = nullptr;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* show imgui component *//

	void ShowOutlinerWindow();
	void ShowAttributeWindow();

	//* sub method *//

	bool CheckSelected(const Attribute* attribute) const;
	void Selectable(const Attribute::Container& container);

};
