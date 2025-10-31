#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
<<<<<<<< HEAD:project/Engine/Render/Pass/FRenderPassContext.h
//* render
#include "FBaseRenderPass.h"

//* c++
#include <list>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassContext class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderPassContext {
========
//* scene
#include <Engine/Adapter/Scene/BaseScene.h>

//* engine
#include <Engine/Adapter/Actor/PerformanceActor.h>

//* demo
#include <Demo/Object/PerspectiveCamera.h>
#include <Demo/Entity/Player.h>
#include <Demo/Object/CollectibleItems.h>

////////////////////////////////////////////////////////////////////////////////////////////
// GameScene class
////////////////////////////////////////////////////////////////////////////////////////////
class GameScene
	: public BaseScene {
>>>>>>>> a:project/Demo/Scene/GameScene.h
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

<<<<<<<< HEAD:project/Engine/Render/Pass/FRenderPassContext.h
	//* render option *//

	void Render(const DirectXQueueContext* context, const FBaseRenderPass::Config& config);

	//* pass option *//
========
	void Init() override;
>>>>>>>> a:project/Demo/Scene/GameScene.h

	void Emplace(std::unique_ptr<FBaseRenderPass>&& pass);

	template <FRenderPass T>
	void Add() { Emplace(std::make_unique<T>()); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

<<<<<<<< HEAD:project/Engine/Render/Pass/FRenderPassContext.h
	std::list<std::unique_ptr<FBaseRenderPass>> passes_;
========
	//* game objects *//

	std::unique_ptr<MonoBehaviour> skylight_;

	std::unique_ptr<PerformanceActor> performance_;

	std::unique_ptr<PerspectiveCamera> camera_;
	std::unique_ptr<Player> player_;

	std::unique_ptr<CollectibleItems> items_;

	std::unique_ptr<MonoBehaviour> demoText_;
	std::unique_ptr<MonoBehaviour> text_;
>>>>>>>> a:project/Demo/Scene/GameScene.h

	//=========================================================================================
	// public methods
	//=========================================================================================

<<<<<<<< HEAD:project/Engine/Render/Pass/FRenderPassContext.h
	static FBaseRenderPass::Config ApplyConfig(const FBaseRenderPass::Config& config);
========
	void SetCollisionCallback();
>>>>>>>> a:project/Demo/Scene/GameScene.h

};
