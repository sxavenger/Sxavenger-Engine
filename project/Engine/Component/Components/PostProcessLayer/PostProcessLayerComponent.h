#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"
#include "BasePostProcess.h"
#include "../Camera/CameraComponent.h"

//* process
#include "PostProcessLocalExposure.h"
#include "PostProcessAutoExposure.h"
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
		None,
		Local,
		Global
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

	//* tag option *//

	void SetTag(Tag tag) { tag_ = tag; }

	Tag GetTag() const { return tag_; }

	//* process option *//

	template <PostProcessConcept _Ty>
	_Ty* AddPostProcess();


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

template <PostProcessConcept _Ty>
inline _Ty* PostProcessLayerComponent::AddPostProcess() {
	auto process = std::make_unique<_Ty>();
	process->Init();

	_Ty* result = process.get();
	processes_.emplace(processes_.end(), std::move(process));

	return result;
}
