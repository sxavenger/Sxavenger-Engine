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
	~ProcessConsole() { Term(); }

	void Init();

	void Term();

	void UpdateConsole();

	void ProcessXclipse(SxavengerFrame* frame);
	void ProcessVisual(SxavengerFrame* frame);

	//* pipeline option *//

	void SetPipeline(ProcessPipelineType type);

	void Dispatch(const Vector2ui& size);


private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* pipeline *//

	ProcessPipeline pipeline_;

};