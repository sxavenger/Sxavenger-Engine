#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "BaseEditor.h"

//* component
#include "../EditorComponent/AttributeComponent.h"

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
	~OutlinerEditor() { Clear(); }

	void Init() override;

	void ShowMainMenu() override;

	void ShowWindow() override;

	//* option *//

	AttributeComponent::Iterator SetAttribute(AttributeComponent* attribute);

	void RemoveAttribute(const AttributeComponent::Iterator& iterator);

	void Clear();

	void CheckSelectClear(const AttributeComponent::Iterator& iterator);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	AttributeComponent::Container outliner_;

	std::optional<AttributeComponent::Iterator> selectIterator_  = std::nullopt;
	const AttributeComponent::Container*        selectContainer_ = nullptr;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* show imgui component *//

	void ShowOutlinerWindow();
	void ShowAttributeWindow();

	//* sub method *//

	bool CheckSelected(const AttributeComponent::Iterator& iterator) const;
	void Selectable(const AttributeComponent::Container& container);

};
