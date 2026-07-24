#include "FPresenter.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "Core/FRenderCore.h"
#include "Core/FRenderCoreTransition.h"

////////////////////////////////////////////////////////////////////////////////////////////
// FPresenter class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FPresenter::Present(const DirectXQueueContext* context, const Vector2ui& resolution, const D3D12_GPU_DESCRIPTOR_HANDLE& handle) {
	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreTransition>();
	core->Present(context, resolution, handle);
}
