#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Game/SxavengerPipeline/RenderingPipeline.h>
#include <Engine/Game/SxavengerPipeline/SxavengerFrame.h>
#include <Engine/Game/RaytracingScene.h>
#include <Engine/Game/SxavengerPipeline/RaytracingPipeline.h>

//* c++
#include <list>
#include <optional>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class BaseBehavior;

////////////////////////////////////////////////////////////////////////////////////////////
// RenderingConsole class
////////////////////////////////////////////////////////////////////////////////////////////
class RenderingConsole {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	RenderingConsole()  = default;
	~RenderingConsole() = default;

	void Init();

	void Term();

	void UpdateConsole();

	void RenderSystematic(SxavengerFrame* frame);
	void RenderAdaptive(SxavengerFrame* frame);
	void RenderLateAdaptive(SxavengerFrame* frame);

	void SetupRaytracing();
	void RenderRaytracing(SxavengerFrame* frame);

	//* behavior option *//

	void SetBehavior(BaseBehavior* behavior);

	void RemoveSelectedBehavior(BaseBehavior* behavior);

	void RemoveBehavior(BaseBehavior* behavior);

	//* pipeline option *//

	void SetRenderingPipeline(RenderingPipelineType type) const;

	//* getter *//

	RaytracingPipeline* GetRaytracingPipeline() const { return raytracingPipeline_.get(); }

	DxrObject::BufferRecoreder* GetMissRecorder() const { return missRecorder_.get(); }

	//* setter *//

	void SetRaytracingEnable(bool enable) { isRaytracingEnabled_ = enable; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* window *//

	bool isDisplayRenderingConsole_ = true;

	//* rendering config *//

	bool isRaytracingEnabled_ = false;

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// behavior container using
	////////////////////////////////////////////////////////////////////////////////////////////
	using BehaviorContainer = std::list<BaseBehavior*>;

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* behavior *//
	//* container
	BehaviorContainer behaviors_;

	//* selected
	const BehaviorContainer*                         selectedBehaviorTable_ = nullptr;      //!< selectされてるbahevior container
	std::optional<BehaviorContainer::const_iterator> selectedBehavior_      = std::nullopt; //!< selectされているbehavior

	//* pipeline *//

	std::unique_ptr<RenderingPipeline> renderingPipeline_;

	//* raytracing *//

	std::unique_ptr<RaytracingPipeline> raytracingPipeline_;
	std::unique_ptr<RaytracingScene> raytracingScene_;

	std::unique_ptr<DxrObject::BufferRecoreder> raygenerationRecorder_;
	std::unique_ptr<DxrObject::BufferRecoreder> missRecorder_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitRaytracing();
	void TermRaytracing();

	//* display methods *//

	void DisplayOutliner();
	void DisplayAttribute();

	//* behavior methods *//

	bool IsSelectedBehavior(BaseBehavior* behavior);
	void SelectableBehavior(const BehaviorContainer::const_iterator& it, const BehaviorContainer& table);

	void DrawSystematicBehavior(BaseBehavior* behavior, SxavengerFrame* frame);
	void DrawAdaptiveBehavior(BaseBehavior* behavior, SxavengerFrame* frame);
	void DrawLateAdaptiveBehavior(BaseBehavior* behavior, SxavengerFrame* frame);
	void DrawRaytracingBehavior(BaseBehavior* behavior);

};