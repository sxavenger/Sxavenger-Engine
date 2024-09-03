#include "SxavengerGraphics.h"

////////////////////////////////////////////////////////////////////////////////////////////
// namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace {

	DefaultGraphics* sDefaultGraphics = nullptr;

}

//=========================================================================================
// static variables
//=========================================================================================

Camera3D* SxavengerGraphics::camera3D = nullptr;

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerRendering class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SxavengerGraphics::Init() {
	sDefaultGraphics = DefaultGraphics::GetInstance();
	sDefaultGraphics->Init();
}

void SxavengerGraphics::Term() {
	sDefaultGraphics->Term();
	sDefaultGraphics = nullptr;
}

DefaultGraphics* SxavengerGraphics::GetDefaultGraphics() {
	return sDefaultGraphics;
}
