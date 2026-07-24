#include "MonoAssembly.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Assembly class
////////////////////////////////////////////////////////////////////////////////////////////

void Mono::Assembly::Load(const Mono::Domain* domain, const std::filesystem::path& filepath) {

	if (!std::filesystem::exists(filepath)) {
		StreamLogger::EngineLog("[Mono::Assembly] warning | file not found. filepath: " + filepath.generic_string());
		return;
	}

	assembly_ = mono_domain_assembly_open(domain->GetDomain(), filepath.generic_string().c_str());
	StreamLogger::AssertA(assembly_ != nullptr, "failed to load assembly: " + filepath.generic_string());

	image_ = mono_assembly_get_image(assembly_);
	StreamLogger::AssertA(image_ != nullptr, "failed to get image from assembly: " + filepath.generic_string());
}

MonoImage* Mono::Assembly::GetImage() const {
	StreamLogger::AssertA(assembly_ != nullptr, "Mono Assembly is not loaded.");
	return image_;
}
