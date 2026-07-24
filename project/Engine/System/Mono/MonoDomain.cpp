#include "MonoDomain.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>

///////////////////////////////////////////////////////////////////////////////////////////
// Domain class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Mono::Domain::Create(const std::string& name) {

	domain_ = mono_jit_init(name.c_str());
	StreamLogger::AssertA(domain_ != nullptr, "failed to initialize Mono Domain. name: " + name);

#ifdef _DEVELOPMENT
	mono_debug_domain_create(domain_);
#endif

}

void Mono::Domain::Reset() {
	if (domain_ != nullptr) {
		mono_jit_cleanup(domain_);
		domain_ = nullptr;
	}
}

MonoDomain* Mono::Domain::GetDomain() const {
	StreamLogger::AssertA(domain_ != nullptr, "Mono Domain is not initialized.");
	return domain_;
}
