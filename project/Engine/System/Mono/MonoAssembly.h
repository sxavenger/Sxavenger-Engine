#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* mono
#include "MonoCommon.h"
#include "MonoDomain.h"

//* c++
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// Mono namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Mono {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Assembly class
	////////////////////////////////////////////////////////////////////////////////////////////
	class Assembly {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Load(const Mono::Domain* domain, const std::filesystem::path& filepath);

		//* operator [equal] *//

		bool operator==(const Assembly& other) const { return assembly_ == other.assembly_; }
		bool operator!=(const Assembly& other) const { return assembly_ != other.assembly_; }

		bool operator==(std::nullptr_t) const { return assembly_ == nullptr; }
		bool operator!=(std::nullptr_t) const { return assembly_ != nullptr; }

		//* getter *//

		MonoImage* GetImage() const;

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		MonoAssembly* assembly_ = nullptr;
		MonoImage* image_       = nullptr;

	};

}

SXAVENGER_ENGINE_NAMESPACE_END