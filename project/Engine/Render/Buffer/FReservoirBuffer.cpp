#include "FReservoirBuffer.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/System.h>

//* lib
#include <Lib/Adapter/String/EncodedString.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FReservoirBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FReservoirBuffer::Create(const Vector2ui& resolution) {
	for (size_t i = 0; i < kLayoutCount; ++i) {
		reservoirs_[i].Create(System::GetDxDevice(), resolution.x * resolution.y);

		//!< nameの設定
		std::string name = "FReservoirBuffer | Reservoir | ";
		name += magic_enum::enum_name(static_cast<Layout>(i));
		reservoirs_[i].SetName(EncodedString::Convert(name).c_str());
	}

	moment_.Create(System::GetDxDevice(), resolution.x * resolution.y);
	moment_.SetName(L"FReservoirBuffer | Moment");
}
