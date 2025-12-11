#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editors
#include "Editors/BaseEditor.h"

//* engine
#include <Engine/System/Utility/Logger.h>

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

	//! @brief Editorの登録
	//! @tparam T 登録するEditorの型
	template <BaseEditorDerived T>
	void RegisterEditor();

	//! @brief Editorの削除
	//! @tparam T 削除するEditorの型
	template <BaseEditorDerived T>
	void RemoveEditor();

	//! @brief Editorの取得
	//! @tparam T 取得するEditorの型
	//! @retval ptr     取得したEditor
	//! @retval nullptr 登録されていない
	template <BaseEditorDerived T>
	T* GetEditor();

	//! @brief Editorの取得
	//! @tparam T 取得するEditorの型
	//! @throw Editorが登録されていない場合, 例外スローする.
	template <BaseEditorDerived T>
	T* RequireEditor();

	//! @brief Editorに対して関数を実行
	//! @tparam T 対象のEditorの型
	//! @param function 実行関数
	//! @note Editorが登録されていない場合, 何も実行されない.
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

	static inline const std::string kEditorName_ = "Sxavenger Engine Editor";

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

	Exception::Assert(editor != nullptr, "require editor not registered.");
	return editor;
}

template <BaseEditorDerived T>
inline void EditorEngine::ExecuteEditorFunction(const std::function<void(T*)>& function) {
	if (auto editor = this->GetEditor<T>()) {
		function(editor);
	}
}
