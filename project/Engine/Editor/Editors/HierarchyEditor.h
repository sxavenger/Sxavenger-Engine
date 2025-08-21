#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "BaseEditor.h"

//* engine
#include <Engine/System/Config/SxavengerConfig.h>
#include <Engine/Component/Entity/MonoBehaviour.h>

//* lib
#include <Lib/Geometry/Color4.h>

//////////////////////////////////////////////////////////////////////////////////////////
// HierarchyEditor class
//////////////////////////////////////////////////////////////////////////////////////////
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

	static inline const std::filesystem::path kSceneDirectory_ = kAssetsDirectory / "scene";

	std::filesystem::path sceneFileName_;
	std::string           sceneNameBuf_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* show imgui component *//

	void ShowHierarchyMenu();

	void ShowActorMenu();
	void ShowSceneMenu();

	void ShowHierarchyWindow();

	//* helper method *//

	void ForEachBehaviourHierarchy(const MonoBehaviour::Hierarchy& hierarchy, const std::function<void(MonoBehaviour*)>& function);

	void HierarchySelectable(MonoBehaviour* behaviour);

	bool CheckSelected(MonoBehaviour* behaviour);

	void SetSelected(MonoBehaviour* behaviour);

	void SetSelectedView(MonoBehaviour* behaviour);

};
