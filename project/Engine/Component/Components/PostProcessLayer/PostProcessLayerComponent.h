#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"
#include "BasePostProcess.h"
#include "../Camera/CameraComponent.h"

//* process
#include "PostProcessExposure.h"
#include "PostProcessBloom.h"
#include "PostProcessDoF.h"

//* engine
#include <Engine/System/DirectX/DirectXContext.h>

//* c++
#include <list>
#include <memory>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class FRenderTargetTextures;

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessLayerComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class PostProcessLayerComponent
	: public BaseComponent {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Tag enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Tag {
		None
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	using Container = std::list<std::unique_ptr<BasePostProcess>>;
	using Iterator  = Container::iterator;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	PostProcessLayerComponent(MonoBehaviour* behaviour) : BaseComponent(behaviour) { }
	~PostProcessLayerComponent() override = default;

	void ShowComponentInspector() override;

	void Process(const DirectXThreadContext* context, FRenderTargetTextures* textures, const CameraComponent* camera);

	//* process option *//

	template <PostProcessConcept T>
	Iterator AddPostProcess();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Container processes_;

	Tag tag_ = Tag::None;

};

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessLayerComponent class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <PostProcessConcept T>
inline PostProcessLayerComponent::Iterator PostProcessLayerComponent::AddPostProcess() {
	auto process = std::make_unique<T>();
	process->Init();
	return processes_.emplace(processes_.end(), std::move(process));
}
