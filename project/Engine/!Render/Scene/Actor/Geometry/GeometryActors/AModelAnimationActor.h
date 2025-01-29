#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* actor
#include "AModelActor.h"

//* engine
#include <Engine/Module/Skeleton/SkeletonMesh.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AModelAnimationActor class
////////////////////////////////////////////////////////////////////////////////////////////
class AModelAnimationActor
	: public AModelActor {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AModelAnimationActor()          = default;
	virtual ~AModelAnimationActor() = default;

	void Init();

	//* render *//

	virtual void RenderOpaque(const RendererContext& context) override;

	virtual void RenderTransparent(const RendererContext& context) override;

	//* raytracing option *//

	virtual void SetupToplevelAS(const SetupContext& context) override;

	//* option *//

	SkeletonMesh* GetSkeleton() { return skeleton_.get(); }

protected:

	//=========================================================================================
	// protected methods
	//=========================================================================================

	std::unique_ptr<SkeletonMesh> skeleton_;

};
