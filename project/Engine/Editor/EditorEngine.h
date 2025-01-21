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
#include <typeindex>
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
	T* TryGetEditor();

	template <BaseEditorDerived T>
	void ExecuteEditorFunction(const std::function<void(T*)>& function);

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

	bool isEditorDisplay_ = true;

	//* editor *//

	std::unordered_map<std::type_index, std::unique_ptr<BaseEditor>> editors_;

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
#ifndef _RELEASE
	auto ptr = std::make_unique<T>(this);
	ptr->Init();
	editors_.try_emplace(typeid(T), std::move(ptr));
#endif // _RELEASE
}

template <BaseEditorDerived T>
inline void EditorEngine::RemoveEditor() {
	editors_.erase(typeid(T));
}

template <BaseEditorDerived T>
inline T* EditorEngine::GetEditor() {
	Assert(editors_.contains(typeid(T)), "editor is not found.");
	return dynamic_cast<T*>(editors_.at(typeid(T)).get());
}

template <BaseEditorDerived T>
inline T* EditorEngine::TryGetEditor() {
#ifndef _RELEASE
	if (editors_.contains(typeid(T))) {
		return dynamic_cast<T*>(editors_.at(typeid(T)).get());
	}
#endif // _RELEASE

	return nullptr;
}

template <BaseEditorDerived T>
inline void EditorEngine::ExecuteEditorFunction(const std::function<void(T*)>& function) {
	if (auto editor = TryGetEditor<T>()) {
		function(editor);
	}
}
