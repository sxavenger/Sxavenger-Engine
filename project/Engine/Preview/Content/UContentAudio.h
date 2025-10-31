#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
<<<<<<<< HEAD:project/Engine/Preview/Content/UContentAudio.h
//* ucontent
#include "UBaseContent.h"

////////////////////////////////////////////////////////////////////////////////////////////
// UContentAudio class
////////////////////////////////////////////////////////////////////////////////////////////
class UContentAudio
	: public UBaseContent {
========
//* scene
#include <Engine/Adapter/Scene/BaseScene.h>

//* engine
#include <Engine/System/Runtime/Performance/DeltaTimePoint.h>
#include <Engine/Component/Entity/MonoBehaviour.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ClearScene class
////////////////////////////////////////////////////////////////////////////////////////////
class ClearScene
	: public BaseScene {
>>>>>>>> a:project/Demo/Scene/ClearScene.h
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

<<<<<<<< HEAD:project/Engine/Preview/Content/UContentAudio.h
	UContentAudio()           = default;
	~UContentAudio() override = default;

	void AsyncLoad(_MAYBE_UNUSED const DirectXQueueContext* context) override;

	AsyncExecution GetAsyncExecution() const { return AsyncExecution::None; }

	void AttachUuid() override;

	//* content option *//

	void Load(const std::filesystem::path& filepath);

	const Uuid& GetId() const { return id_; }
========
	void Init() override;

	void Update() override;
>>>>>>>> a:project/Demo/Scene/ClearScene.h

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

<<<<<<<< HEAD:project/Engine/Preview/Content/UContentAudio.h
	Uuid id_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* helper methods *//

	void GetUuid();
========
	TimePointf<TimeUnit::second> time_       = { 2.0f };
	DeltaTimePointf<TimeUnit::second> timer_ = { 0.0f };

	std::unique_ptr<MonoBehaviour> fade_ = nullptr;
>>>>>>>> a:project/Demo/Scene/ClearScene.h

};
