#include "MonoInstance.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Instance class
////////////////////////////////////////////////////////////////////////////////////////////

void Mono::Instance::Create(
	const Mono::Domain* domain, const Mono::Assembly* assembly,
	const std::string& _namespace, const std::string& _class) {

	//!< namespaceとclassを結合して、クラスの完全修飾名を作成
	name_ = _namespace + "." + _class;

	class_ = mono_class_from_name(assembly->GetImage(), _namespace.c_str(), _class.c_str());
	StreamLogger::AssertA(class_ != nullptr, "failed to find class. name: " + name_);

	instance_ = mono_object_new(domain->GetDomain(), class_);
	StreamLogger::AssertA(instance_ != nullptr, "failed to create instance of class. name: " + name_);

}

void Mono::Instance::CallMethod(const std::string& name) {

	std::string function = name_ + "::" + name;

	MonoMethodDesc* desc = mono_method_desc_new(function.c_str(), true);
	StreamLogger::AssertA(desc != nullptr, "failed to find method. name: " + function);

	MonoMethod* method = mono_method_desc_search_in_class(desc, class_);
	StreamLogger::AssertA(method != nullptr, "failed to find method in class. name: " + function);


	MonoObject* error = nullptr;
	mono_runtime_invoke(method, instance_, nullptr, &error);

	if (error != nullptr) {
		
	}

}
