#include "Material.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <Sxavenger.h>

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
_DXOBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// ObjectMaterial class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ObjectMaterialBuffer::Init() {
	buffer_ = std::make_unique<BufferResource<ObjectMaterial>>(Sxavenger::GetDevicesObj(), 1);
	(*buffer_)[0].Init();
}

void ObjectMaterialBuffer::Transfer() {
	(*buffer_)[0].color       = color;
	(*buffer_)[0].uvTransform = uvTransform.ToMatrix();
}
