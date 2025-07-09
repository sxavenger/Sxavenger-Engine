#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <concepts>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class EditorEngine;

////////////////////////////////////////////////////////////////////////////////////////////
// Base Editor class
////////////////////////////////////////////////////////////////////////////////////////////
class BaseEditor {
public:

	//=========================================================================================
	// public method
	//=========================================================================================

	BaseEditor(EditorEngine* editor) : editor_(editor) {}
	virtual ~BaseEditor() = default;

	virtual void Init() = 0;

	virtual void ShowMainMenu() = 0;

	virtual void ShowWindow() = 0;

	virtual void LateUpdate() {}

	//* getter *//

	bool IsDisplay() const { return isDisplay_; }

	bool& GetIsDisplay() { return isDisplay_; }

	EditorEngine* GetEditorEngine() const { return editor_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* external *//

	EditorEngine* editor_ = nullptr;

	//* config *//

	bool isDisplay_ = true;

	//=========================================================================================
	// protected methods
	//=========================================================================================

	//* layout *//

	static void MenuPadding();

	//* option *//

	void SetNextWindowDocking();

	uint32_t GetWindowFlag() const;

};

////////////////////////////////////////////////////////////////////////////////////////////
// concept
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
concept BaseEditorDerived = std::derived_from<T, BaseEditor>;
