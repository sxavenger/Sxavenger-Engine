#include "PrimitiveDrawer.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <MyEngine.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PrimitiveDrawer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PrimitiveDrawer::Init() {
	
}

void PrimitiveDrawer::Term() {
	
}

void PrimitiveDrawer::DrawAll() {
}

//void PrimitiveDrawer::DrawTriangle(const Vector3f& w1, const Vector3f& w2, const Vector3f& w3, uint32_t color) {
//	// drawDetaに登録
//
//}

PrimitiveDrawer* PrimitiveDrawer::GetInstance() {
	static PrimitiveDrawer instance;
	return &instance;
}
