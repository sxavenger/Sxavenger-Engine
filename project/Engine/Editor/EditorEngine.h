#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editors
#include "Editors/BaseEditor.h"

//* external
#include <imgui.h>

//* c++
#include <string>
#include <unordered_map>
#include <typeinfo>
#include <memory>
#include <functional>

////////////////////////////////////////////////////////////////////////////////////////////
// EditorEngine class
////////////////////////////////////////////////////////////////////////////////////////////
class EditorEngine {
public:

	//=========================================================================================
	// public method
	//=========================================================================================

	EditorEngine()  = default;
	~EditorEngine() = default;

	void Init();

	void Term();

	void UpdateEditor();

	//* imgui opiton *//

	ImGuiWindowFlags GetWindowFlag() const { return windowFlag_; }

	void SetNextWindowDocking() const;

	//* editor option *//

	template <BaseEditorDerived T>
	void RegisterEditor();

	template <BaseEditorDerived T>
	void RemoveEditor();

	template <BaseEditorDerived T>
	T* GetEditor();

	template <BaseEditorDerived T>
	T* RequireEditor();

	template <BaseEditorDerived T>
	void ExecuteEditorFunction(const std::function<void(T*)>& function);

	//* engine option *//

	bool IsEditorDisplay() const { return isEditorDisplay_; }

	//* singleton *//

	static EditorEngine* GetInstance();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static const std::string kEditorName_;

	//* imgui *//

	ImGuiID          dockingId_  = NULL;
	ImGuiWindowFlags windowFlag_ = NULL;

	//* config *//

	bool isEditorDisplay_ = false;

	//* editor *//

	std::unordered_map<const std::type_info*, std::unique_ptr<BaseEditor>> editors_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* show imgui component *//

	void ShowMainMenu();

	void ShowWindow();

	//* sub method *//

	static void MenuDummy();

	void ShowEditorMenu();

};

//=========================================================================================
// static variable
//=========================================================================================
static EditorEngine* const sEditorEngine = EditorEngine::GetInstance();

////////////////////////////////////////////////////////////////////////////////////////////
// EditorEngine class template method
////////////////////////////////////////////////////////////////////////////////////////////

template <BaseEditorDerived T>
inline void EditorEngine::RegisterEditor() {
#ifdef _DEVELOPMENT

	constexpr const std::type_info* type = &typeid(T);

	if (editors_.contains(type)) {
		return;
	}

	auto ptr = std::make_unique<T>(this);
	ptr->Init();
	editors_.emplace(type, std::move(ptr));
#endif // _DEVELOPMENT
}

template <BaseEditorDerived T>
inline void EditorEngine::RemoveEditor() {
	editors_.erase(&typeid(T));
}

template <BaseEditorDerived T>
inline T* EditorEngine::GetEditor() {
#ifdef _DEVELOPMENT

	constexpr const std::type_info* type = &typeid(T);

	if (editors_.contains(type)) {
		return static_cast<T*>(editors_.at(type).get());
	}
#endif // _DEVELOPMENT

	return nullptr;
}

template <BaseEditorDerived T>
inline T* EditorEngine::RequireEditor() {
	T* editor = this->GetEditor<T>();

	Assert(editor != nullptr, "require editor not registered.");
	return editor;
}

template <BaseEditorDerived T>
inline void EditorEngine::ExecuteEditorFunction(const std::function<void(T*)>& function) {
	if (auto editor = this->GetEditor<T>()) {
		function(editor);
	}
}
