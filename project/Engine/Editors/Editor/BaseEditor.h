#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>

//* lib
#include <Lib/Geometry/Vector2.h>

//* c++
#include <concepts>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class EditorEngine;

////////////////////////////////////////////////////////////////////////////////////////////
// Base Editor class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief Editorの基底クラス
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

	bool IsDisplay() const;

	bool& GetIsDisplay() { return isDisplay_; }

	EditorEngine* GetEditorEngine() const { return editor_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//=========================================================================================
	// protected methods
	//=========================================================================================

	//* layout *//

	static void MenuPadding();

	static const Vector2f& GetMenuPadding() { return kMenuPadding; }

	//* option *//

	void SetNextWindowDocking();

	uint32_t GetWindowFlag() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* editor engine *//

	EditorEngine* editor_ = nullptr;

	//* config *//

	bool isDisplay_ = true;

	//* parameter *//

	static inline const Vector2f kMenuPadding = { 320.0f, 0.0f };

};

////////////////////////////////////////////////////////////////////////////////////////////
// concept
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
concept BaseEditorConcept = std::derived_from<T, BaseEditor>;

SXAVENGER_ENGINE_NAMESPACE_END
