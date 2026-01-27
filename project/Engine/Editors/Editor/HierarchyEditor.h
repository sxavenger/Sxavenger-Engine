#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "BaseEditor.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/Configuration/Configuration.h>
#include <Engine/Components/Entity/EntityBehaviour.h>

//* lib
#include <Lib/Geometry/Color4.h>

//////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
//////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
// HierarchyEditor class
//////////////////////////////////////////////////////////////////////////////////////////
//! @brief Entity関連Editorクラス
class HierarchyEditor
	: public BaseEditor {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	HierarchyEditor(EditorEngine* editor) : BaseEditor(editor) {}
	~HierarchyEditor() = default;

	void Init() override;

	void ShowMainMenu() override;

	void ShowWindow() override;

	void LateUpdate() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* parameter *//

	static inline const Color4f disableColor_ = { 0.4f, 0.4f, 0.4f, 1.0f };

	//* scene parameter *//

	static inline const std::filesystem::path kSceneDirectory = kAssetsDirectory / "scene";

	std::string hierarchyBuf_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* show imgui component *//

	void ShowHierarchyMenu();

	void ShowActorMenu();
	void ShowSceneMenu();
	void ShowSummaryMenu();

	void ShowHierarchyWindow();

	//* helper method *//

	void ForEachBehaviourHierarchy(const EntityBehaviour::Hierarchy& hierarchy, const std::function<void(EntityBehaviour*)>& function);

	void HierarchySelectable(EntityBehaviour* behaviour);

	bool HierarchyFilter(EntityBehaviour* behaviour, const std::string& filter);
	void HierarchySelectableFilter(EntityBehaviour* behaviour, const std::string& filter);

	bool CheckSelected(EntityBehaviour* behaviour);

	void SetSelected(EntityBehaviour* behaviour);

	void SetSelectedView(EntityBehaviour* behaviour);

};

SXAVENGER_ENGINE_NAMESPACE_END
