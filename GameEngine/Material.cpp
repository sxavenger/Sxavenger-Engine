#include "Material.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <MyEngine.h>

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
_DXOBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// ObjectMaterial class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ObjectMaterialBuffer::Init() {
	buffer_ = std::make_unique<BufferResource<ObjectMaterial>>(MyEngine::GetDevicesObj(), 1);
	(*buffer_)[0].Init();
}

void ObjectMaterialBuffer::Transfer() {
	(*buffer_)[0].color       = color;
	(*buffer_)[0].uvTransform = uvTransform.ToMatrix();
}
