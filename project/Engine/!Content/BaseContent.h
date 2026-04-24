#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>
#include <Engine/System/Runtime/Async/AsyncExecutionTask.h>
#include <Engine/Editors/Editor/InspectorEditor.h>

//* lib
#include <Lib/Adapter/Uuid/Uuid.h>
#include <Lib/Adapter/Json/JsonHandler.h>

//* c++
#include <filesystem>
#include <any>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// Content namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Content {

	////////////////////////////////////////////////////////////////////////////////////////////
	// BaseContent class
	////////////////////////////////////////////////////////////////////////////////////////////
	class BaseContent
		: public BaseInspector {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* constructor / destructor *//

		BaseContent(Async::Execution execution);
		virtual ~BaseContent() = default;

		//* content option *//

		virtual void Attach(const std::filesystem::path& filepath, const std::any& parameter) = 0;

		virtual void Load(MAYBE_UNUSED const DirectXQueueContext* context) = 0;

		//* parameter option *//

		void SetFilepath(const std::filesystem::path& filepath);

		const std::filesystem::path& GetFilepath() const;

		void SetParameter(const std::any& parameter) { parameter_ = parameter; }

		const std::any& GetParameter() const { return parameter_; }

		//* execution option *//

		const Async::Execution GetExecution() const { return execution_; }

	protected:

		//=========================================================================================
		// protected variables
		//=========================================================================================

		std::filesystem::path filepath_;
		std::any parameter_;
		// HACK: Anyの廃止

		//=========================================================================================
		// protected methods
		//=========================================================================================

		//* meta helper methods *//

		static json LoadMetaData(const std::filesystem::path& filepath);

		static void SaveMetaData(const json& data, const std::filesystem::path& filepath);

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		const Async::Execution execution_;

		static const inline std::filesystem::path kMetaExtension = ".content";

		//=========================================================================================
		// private methods
		//=========================================================================================

		//* meta helper methods *//

		static std::filesystem::path GetMetaFilepath(const std::filesystem::path& filepath);

	};

}

SXAVENGER_ENGINE_NAMESPACE_END
