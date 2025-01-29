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

	virtual void SetupToplevelAS(const SetupContext& context) override;

	//* option *//

	//SkeletonMesh* GetSkeleton() { return skeleton_.get(); }

	void UpdateAnimation(const Animation& animation, TimePointf<TimeUnit::second> time, bool isLoop = true);

	void UpdateTransitionAnimation(
		const Animation& before, TimePointf<TimeUnit::second> beforeTime, bool isLoopBefore,
		const Animation& after, TimePointf<TimeUnit::second> afterTime, bool isLoopAfter,
		float t
	);

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::unique_ptr<SkeletonMesh> skeleton_;

	//=========================================================================================
	// protected methods
	//=========================================================================================

	void UpdateBottomLevelAS();

};
