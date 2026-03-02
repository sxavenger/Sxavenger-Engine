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

	info_ = { _namespace, _class };

	class_ = mono_class_from_name(assembly->GetImage(), info_.namespace_.c_str(), info_.class_.c_str());
	StreamLogger::AssertA(class_ != nullptr, "failed to find class. name: " + info_.GetName());

	instance_ = mono_object_new(domain->GetDomain(), class_);
	StreamLogger::AssertA(instance_ != nullptr, "failed to create instance of class. name: " + info_.GetName());

	//!< GCHandleを作成して、Instanceが保持しているMonoObject*がGCによって移動されないようにする
	handle_ = mono_gchandle_new(instance_, true);
}

void Mono::Instance::Reset() {

	//!< Instanceが保持しているGCHandleを解放
	if (handle_.has_value()) {
		mono_gchandle_free(handle_.value());
	}

	info_ = {};

	class_    = nullptr;
	instance_ = nullptr;

	handle_ = std::nullopt;
}

void Mono::Instance::CallMethod(const std::string& name) {

	std::string function = info_.GetName() + "::" + name;

	MonoMethodDesc* desc = mono_method_desc_new(function.c_str(), true);
	StreamLogger::AssertA(desc != nullptr, "failed to find method. name: " + function);

	MonoMethod* method = mono_method_desc_search_in_class(desc, class_);
	StreamLogger::AssertA(method != nullptr, "failed to find method in class. name: " + function);

	MonoObject* error = nullptr;
	mono_runtime_invoke(method, instance_, nullptr, &error);

	if (error != nullptr) {
		MonoString* string = mono_object_to_string(error, nullptr);
		std::string_view message = mono_string_to_utf8(string);
		StreamLogger::Exception("mono call method error.", message.data());

		//!< RuntimeLogに変更するかも...
	}

}

Mono::Instance& Mono::Instance::operator=(std::nullptr_t) {
	Reset();
	return *this;
}
