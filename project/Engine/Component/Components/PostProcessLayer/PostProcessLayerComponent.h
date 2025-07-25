#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"
#include "../Camera/CameraComponent.h"
#include "../Transform/TransformComponent.h"

//* process
#include "BasePostProcess.h"
#include "PostProcessLocalExposure.h"
#include "PostProcessAutoExposure.h"
#include "PostProcessBloom.h"
#include "PostProcessDoF.h"
#include "PostProcessLUT.h"
#include "PostProcessGrayScale.h"
#include "PostProcessRadialBlur.h"
#include "PostProcessChromaticAberration.h"

//* engine
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* c++
#include <list>
#include <memory>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class FRenderTargetBuffer;

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
		Global,
		Volume,
		Local,
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

	void Process(const DirectXQueueContext* context, FRenderTargetBuffer* buffer, const CameraComponent* camera);

	//* tag option *//

	void SetTag(Tag tag) { tag_ = tag; }

	Tag GetTag() const { return tag_; }

	//* process option *//

	template <PostProcessConcept _Ty>
	_Ty* AddPostProcess();

	//* behaviour component option *//

	const TransformComponent* GetTransform() const; //!< volume時のみ使用.

	bool IsInsideVolume(const Vector3f& position) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================


	//* main process *//

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
