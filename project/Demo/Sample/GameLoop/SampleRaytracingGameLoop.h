#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/System/Runtime/GameLoop/GameLoop.h>

//* engine
#include <Engine/System/Window/GameWindow.h>
#include <Engine/Asset/AssetCollection.h>

//* test !render
#include <Engine/System/DirectX/DxrObject/DxrAccelerationStructure.h>
#include <Engine/System/DirectX/DxrObject/DxrStateObjectContext.h>
#include <Engine/Content/TextureBuffer/UnorderedTexture.h>
#include <Engine/Render/Scene/Actor/Camera/CameraActors/ACineCameraActor.h>
#include <Engine/Render/Scene/Actor/Camera/CameraActors/APivotCameraActor.h>
#include <Engine/Render/FPresenter.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SampleRaytracingGameLoop class
////////////////////////////////////////////////////////////////////////////////////////////
class SampleRaytracingGameLoop
	: public GameLoop::Interface {
public:

	//=========================================================================================
	// public method
	//=========================================================================================

	void Init(GameLoop::Context* context) override;

	void Term() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::shared_ptr<GameWindow> main_;

	std::unique_ptr<DxrObject::RaytracingBlob> raygeneration_;
	std::unique_ptr<DxrObject::ExportGroup>    raygenerationExpt_;

	std::unique_ptr<DxrObject::RaytracingBlob> miss_;
	std::unique_ptr<DxrObject::ExportGroup>    missExpt_;

	std::unique_ptr<DxrObject::RaytracingBlob> hitgroup_;
	std::unique_ptr<DxrObject::ExportGroup>    hitgroupExpt_;

	std::shared_ptr<AssetModel> model_;
	std::shared_ptr<AssetModel> floor_;

	std::unique_ptr<DxrObject::TopLevelAS> topLevelAS_;

	std::unique_ptr<DxrObject::StateObjectContext> stateObject_;

	std::unique_ptr<ACineCameraActor> camera_;
	std::unique_ptr<UnorderedTexture> texture_;

	std::unique_ptr<APivotCameraActor> camera2_;
	std::unique_ptr<UnorderedTexture> texture2_;

	bool isView1_ = true;

	FPresenter presenter_;

	//=========================================================================================
	// private method
	//=========================================================================================

	void InitSystem();

	void TermSystem();

	void UpdateSystem();

	void DrawSystem();

};
