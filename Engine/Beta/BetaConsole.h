#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <string>
#include <list>
#include <optional>
#include <format>
#include <memory>
#include <deque>

//* imgui
#include <imgui.h>

//* DxObject
#include <Engine/System/DxObject/DxShaderBlob.h>
#include <Engine/System/DxObject/DxGraphicsPipeline.h>

//* engine
#include <Engine/System/Texture.h>
#include <Engine/Game/Camera3D.h>
#include <Engine/Game/DebugCamera3D.h>

//* Beta
#include "DepthRenderTarget.h"
#include "CineCamera.h"

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class BaseBehavior;

////////////////////////////////////////////////////////////////////////////////////////////
// BetaConsolePipelineType enum
////////////////////////////////////////////////////////////////////////////////////////////
enum BetaConsolePipelineType {
	kConsoleLocal,
	kDefault_Diffuse,
	kDefaultMesh_Diffuse,

	kCountOfPipelineType
};

////////////////////////////////////////////////////////////////////////////////////////////
// BetaConsolePipeline class
////////////////////////////////////////////////////////////////////////////////////////////
class BetaConsolePipeline {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void Term();

	void SetPipeline(BetaConsolePipelineType type);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<DxObject::GraphicsBlob>     blobs_[kCountOfPipelineType];
	std::unique_ptr<DxObject::GraphicsPipeline> pipelines_[kCountOfPipelineType];

	//=========================================================================================
	// private methods
	//=========================================================================================

};

////////////////////////////////////////////////////////////////////////////////////////////
// BetaConsole class
////////////////////////////////////////////////////////////////////////////////////////////
class BetaConsole final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief 初期化処理
	void Init();

	//! @brief 終了処理
	void Term();

	//! @brief 更新処理
	void Update();

	//! @brief 描画処理
	void Draw();

	//* console docking *//

	//! @brief 現在のwindowをconsoleにdockingする
	void DockingConsole() const;

	//* MonoBehavior option *//

	void SetBehavior(BaseBehavior* behavior);
	void RemoveSelectedBehavior(BaseBehavior* behavior);
	void RemoveBehavior(BaseBehavior* behavior);

	//* console option *//

	void Log(const std::string& log, const Color4f& color = kDefaultColor);

	//* console accsessor *//

	bool IsUpdateRequired() const { return isUpdateRequired_; }

	DepthRenderTarget* GetLocalRenderTarget() const { return localRenderTarget_.get(); }
	DepthRenderTarget* GetSceneRenderTarget() const { return sceneRenderTarget_.get(); }
	DepthRenderTarget* GetGameRenderTarget() const { return gameRenderTarget_.get(); }

	Camera3D* GetLocalCamera() const { return localCamera_.get(); }
	CineCamera* GetGameCamera() const { return gameCamera_.get(); }

	Camera3D* GetDisplayCamera() const { return displayCamera_; }

	//* console rendering pipeline accessor *//

	void SetPipeline(BetaConsolePipelineType type);

	void DisplayLocalRenderTarget(uint32_t indentNum);

	//* singleton *//

	static BetaConsole* GetInstance();
	BetaConsole()  = default;
	~BetaConsole() = default;

	//=========================================================================================
	// public variables
	//=========================================================================================

	static const Color4f kCommentOutColor;
	static const Color4f kErrorColor;
	static const Color4f kWarningColor;
	static const Color4f kDefaultColor;


private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// LogData structture
	////////////////////////////////////////////////////////////////////////////////////////////
	struct LogData {
		std::string log;
		Color4f color;
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* MonoBehavior *//

	std::list<BaseBehavior*>                                behaviors_;
	std::optional<std::list<BaseBehavior*>::const_iterator> selectedBehavior_ = std::nullopt;

	//* config *//
	//* Console config
	bool isDisplayConsole_ = true;
	bool isOutputImGuiIni_ = false;

	//* ImGui config
	ImGuiID          dockingID_   = 0;
	ImGuiWindowFlags windowFlags_ = 0;

	//* Rendering *//
	//* screen
	std::unique_ptr<DepthRenderTarget> gameRenderTarget_;
	std::unique_ptr<CineCamera>        gameCamera_;

	std::unique_ptr<DepthRenderTarget> sceneRenderTarget_;
	std::unique_ptr<DebugCamera3D>     sceneCamera_;

	//* local
	std::unique_ptr<DepthRenderTarget> localRenderTarget_;
	std::unique_ptr<DebugCamera3D>     localCamera_;

	Camera3D* displayCamera_ = nullptr; //!< scene cameraまたはgame cameraが入る

	BetaConsolePipeline pipeline_;

	//* log *//

	std::deque<LogData> logDatas_;
	const uint32_t      kMaxLog_ = 30;

	//* process *//

	bool isUpdateRequired_ = true; //!< 最終的なプロセス管理

	//! [optional]
	//! nullopt:   通常状態(制限なく更新処理をする)
	//! has_value: 何回更新処理をするか<frame>(0の場合は更新処理を止める)
	std::optional<uint32_t> updateLimit_ = std::nullopt;

	//* parameter *//

	static const std::string kConsoleName_;
	static const std::string kImGuiIniFilepath_;


	//=========================================================================================
	// private methods
	//=========================================================================================

	//* displays methods *//

	void DisplayMainMenu();

	void DisplayGame();
	void DisplayScene();

	void DisplayOutliner();  //!< selectable MonoBehavior
	void DisplayAttribute(); //!< selected MonoBehavior

	void DisplayLog();
	void DisplaySystem();

	void DisplayPerformance();

	void DisplayAsset();

	//* console option methods *//

	void InitConfig();

	void InitRenderTarget();
	void TermRenderTarget();

	void DrawBehavior(BaseBehavior* behavior);

	//* assistance methods *//

	void DisplayTextureImGuiFullWindow(const BaseTexture*       texture);
	void DisplayTextureImGuiFullWindow(const DepthRenderTarget* texture);
	void DisplayTextureImGui(const BaseTexture* texture,       const Vector2f& displaySize);
	void DisplayTextureImGui(const DepthRenderTarget* texture, const Vector2f& displaySize);

	bool IsSelectedBehavior(BaseBehavior* behavior);
	void SelectableBehavior(const std::list<BaseBehavior*>::const_iterator& behaviorIt);

};

static BetaConsole* const sBetaConsole = BetaConsole::GetInstance();