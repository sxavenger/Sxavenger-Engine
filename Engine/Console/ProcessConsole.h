#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Game/SxavengerPipeline/ProcessPipeline.h>
#include <Engine/Game/SxavengerPipeline/SxavengerFrame.h>

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

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* pipeline *//

	std::unique_ptr<ProcessPipeline> processPipeline_;

};