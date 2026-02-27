#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* mono
#include "MonoCommon.h"

//* c++
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// Mono namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Mono {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Domain class
	////////////////////////////////////////////////////////////////////////////////////////////
	class Domain {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Create(const std::string& name);

		void Reset();

		//* operator [equal] *//

		bool operator==(const Domain& other) const { return domain_ == other.domain_; }
		bool operator!=(const Domain& other) const { return domain_ != other.domain_; }

		bool operator==(std::nullptr_t) const { return domain_ == nullptr; }
		bool operator!=(std::nullptr_t) const { return domain_ != nullptr; }

		//* getter *//

		MonoDomain* GetDomain() const;

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		MonoDomain* domain_ = nullptr;

	};

}

SXAVENGER_ENGINE_NAMESPACE_END