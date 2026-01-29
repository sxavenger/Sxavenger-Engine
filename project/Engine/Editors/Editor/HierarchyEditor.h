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

	bool HierarchyFilter(EntityBehaviour* behaviour, const std::string& filter);
	void HierarchySelectable(EntityBehaviour* behaviour, const std::string& filter);

	void SetSelectedView(EntityBehaviour* behaviour);

};

SXAVENGER_ENGINE_NAMESPACE_END
