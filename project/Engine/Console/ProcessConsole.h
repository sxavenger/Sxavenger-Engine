#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Game/SxavengerPipeline/ProcessPipeline.h>
#include <Engine/Game/SxavengerPipeline/SxavengerFrame.h>
#include <Engine/Game/SxavengerPipeline/VisualProcessLayer.h>

#include <Engine/Game/SxavengerPipeline/ProcessLayer.h>

//* c++
#include <list>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class BaseProcessLayer;

////////////////////////////////////////////////////////////////////////////////////////////
// ProcessConsole class
////////////////////////////////////////////////////////////////////////////////////////////
class ProcessConsole {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ProcessConsole()  = default;
	~ProcessConsole() = default;

	void Init();

	void Term();

	void UpdateConsole();

	void ProcessXclipse(SxavengerFrame* frame);
	void ProcessVisual(SxavengerFrame* frame);

	//* pipeline option *//

	void SetProcessPipeline(ProcessPipelineType type);

	void Dispatch(const Vector2ui& size);

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* config *//

	bool isDisplayProcessConsole_ = false;

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// layer container using
	////////////////////////////////////////////////////////////////////////////////////////////
	using VisualLayerContainer = std::list<std::unique_ptr<BaseVisualProcessLayer>>;

	//=========================================================================================
	// private variables
	//=========================================================================================

	VisualLayerContainer                                visualProcessLayer_;
	std::optional<VisualLayerContainer::const_iterator> selectedVisualProcess_ = std::nullopt;

	//* pipeline *//

	std::unique_ptr<ProcessPipeline> processPipeline_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* display console methods *//

	void DisplayCanvas();
	void DisplayLayer();

	//* layer methods *//

	void SelectableVisualLayer(const VisualLayerContainer::iterator& it);

	//* xclipse process methods *// //!< HACK

	void XclipseAtmoSphericScattering(SxavengerFrame* frame);

	//* visual process methods *// //!< HACK

	void VisualGlayscale(SxavengerFrame* frame);
	void VisualLUT(SxavengerFrame* frame);

};