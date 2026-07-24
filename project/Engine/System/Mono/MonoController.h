#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* mono
#include "MonoCommon.h"
#include "MonoDomain.h"
#include "MonoAssembly.h"
#include "MonoInstance.h"

//* engine
#include <Engine/Foundation.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// Mono namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Mono {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Controller class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief Monoランタイムの初期化/終了とドメイン管理を統括するコントローラ
	class Controller {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		Controller() { Init(); }
		~Controller() { Term(); }

		void Init();

		void Term();

		Mono::Instance CreateInstance(const std::string& _namespace, const std::string& _class) const;

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		static inline const std::string kDomainName = "Sxavenger Engine / Sxx Engine - Domain";

		Mono::Domain domain_;
		Mono::Assembly assembly_;

	};

}

SXAVENGER_ENGINE_NAMESPACE_END
