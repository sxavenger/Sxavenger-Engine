#include "VisualLayer.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Console/Console.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseVisualLayer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BaseVisualLayer::Term() {
	ResetIterator();
}

void BaseVisualLayer::SetToConsole() {
	Assert(iterator_ == std::nullopt, "iterator is not null.");
	iterator_ = sConsole->SetLayer(this);
}

void BaseVisualLayer::SetToConsole(const std::string& name) {
	SetName(name);
	SetToConsole();
}

void BaseVisualLayer::ResetIterator() {
	if (!iterator_.has_value()) {
		return;
	}

	sConsole->EraseLayer(iterator_.value());
	iterator_ = std::nullopt;
}
