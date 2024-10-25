#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Game/SxavengerPipeline/ProcessPipeline.h>
#include <Engine/Game/SxavengerPipeline/SxavengerFrame.h>

//* c++
#include <list>
#include <string>

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

	bool isDisplayProcessConsole_ = true;

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// layer container using
	////////////////////////////////////////////////////////////////////////////////////////////
	using LayerContainer = std::list<std::string>;

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* layers *//

	LayerContainer layers_;

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

	//* visual process methods *// //!< HACK

	void XclipseAtmoSphericScattering(SxavengerFrame* frame);

	//* visual process methods *// //!< HACK

	void VisualGlayscale(SxavengerFrame* frame);

};