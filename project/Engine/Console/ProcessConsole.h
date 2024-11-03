#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Game/SxavengerPipeline/ProcessPipeline.h>
#include <Engine/Game/SxavengerPipeline/SxavengerFrame.h>
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

	//* layer option *//

	void SetLayer(BaseProcessLayer* layer);

	//* pipeline option *//

	void SetProcessPipeline(ProcessPipelineType type);

	void Dispatch(const Vector2ui& size);

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* config *//

	bool isDisplayProcessConsole_ = true;

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// layer container using
	////////////////////////////////////////////////////////////////////////////////////////////
	using LayerContainer = std::list<BaseProcessLayer*>;

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* layers *//

	LayerContainer                                layers_;
	std::optional<LayerContainer::const_iterator> selectedLayer_ = std::nullopt;

	//* test layer *//

	std::unique_ptr<BaseProcessLayer> layer1_;
	std::unique_ptr<BaseProcessLayer> layer2_;
	std::unique_ptr<BaseProcessLayer> layer3_;

	//* pipeline *//

	std::unique_ptr<ProcessPipeline> processPipeline_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* display console methods *//

	void DisplayCanvas();
	void DisplayLayer();

	//* layer methods *//

	void SelectableLayer(const LayerContainer::const_iterator& it);

	//* xclipse process methods *// //!< HACK

	void XclipseAtmoSphericScattering(SxavengerFrame* frame);

	//* visual process methods *// //!< HACK

	void VisualGlayscale(SxavengerFrame* frame);

};