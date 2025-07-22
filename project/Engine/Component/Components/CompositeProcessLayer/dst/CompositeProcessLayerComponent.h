#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"
#include "BaseCompositeProcess.h"
#include "../Camera/CameraComponent.h"

//* process
#include "CompositeProcessLUT.h"

//* engine
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* c++
#include <list>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// CompositeProcessLayerComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class CompositeProcessLayerComponent
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

	using Container = std::list<std::unique_ptr<BaseCompositeProcess>>;
	using Iterator  = Container::iterator;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	CompositeProcessLayerComponent(MonoBehaviour* behaviour) : BaseComponent(behaviour) { }
	~CompositeProcessLayerComponent() override = default;

	void ShowComponentInspector() override;

	void Process(const DirectXQueueContext* context, FRenderTargetTextures* textures);

	//* process option *//

	template <CompositeProcessConcept _Ty>
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

template <CompositeProcessConcept _Ty>
inline _Ty* CompositeProcessLayerComponent::AddPostProcess() {
	auto process = std::make_unique<_Ty>();
	process->Init();

	_Ty* result = process.get();
	processes_.emplace(processes_.end(), std::move(process));

	return result;
}
