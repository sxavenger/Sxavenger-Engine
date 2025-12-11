#include "FPresenter.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FRenderCore.h"

////////////////////////////////////////////////////////////////////////////////////////////
// FPresenter class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FPresenter::Present(const DirectXQueueContext* context, const Vector2ui& size, const D3D12_GPU_DESCRIPTOR_HANDLE& handle) {
	FRenderCore::GetInstance()->GetTransition()->Present(context, size, handle);
}
