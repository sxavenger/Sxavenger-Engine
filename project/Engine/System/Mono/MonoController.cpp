#include "MonoController.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Configuration/Configuration.h>
#include <Engine/System/Utility/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Controller class
////////////////////////////////////////////////////////////////////////////////////////////

void Mono::Controller::Init() {
	Mono::Common::Init();
	
	domain_.Create(kDomainName.c_str());
	StreamLogger::EngineLog("[Mono::Controller]: domain create. name: " + kDomainName);

	std::filesystem::path assembly
		= SxxEngine::kAssetsDirectory / L"Script" / L"bin" / _PROFILE / L"net4.8" / L"Script.dll";

	assembly_.Load(&domain_, assembly);

	if (assembly_ != nullptr) {
		StreamLogger::EngineLog("[Mono::Controller]: assembly load. filepath: " + assembly.generic_string());
	}
	

	StreamLogger::EngineLog("[Mono::Controller]: initialize complete.");
}

void Mono::Controller::Term() {
	Mono::Common::Term();
}

Mono::Instance Mono::Controller::CreateInstance(const std::string& _namespace, const std::string& _class) const {
	Mono::Instance instance;
	instance.Create(&domain_, &assembly_, _namespace, _class);

	return instance;
}
