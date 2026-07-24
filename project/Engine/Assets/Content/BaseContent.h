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
// BaseContent class
////////////////////////////////////////////////////////////////////////////////////////////
class BaseContent
	: public BaseInspector {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Status enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Status : uint8_t {
		None,    //!< 初期状態
		Complete //!< 読み込み完了
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	BaseContent(Async::Execution execution) : execution_(execution) {}

	virtual ~BaseContent() = default;

	//* content option *//

	virtual void Attach(const std::filesystem::path& filepath, const std::any& parameter);

	virtual void Load(MAYBE_UNUSED const DirectXQueueContext* context) = 0;

	void ShowInspector() override {}

	//* parameter option *//

	const std::filesystem::path& GetFilepath() const { return filepath_; }

	const std::any& GetParameter() const { return parameter_; }

	static const std::filesystem::path& GetMetaExtension() { return kMetaExtension; }

	//* execution option *//

	const Async::Execution GetExecution() const { return execution_; }

	void WaitComplete() const;

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

	//* execution methods *//

	void SetComplete() { status_ = Status::Complete; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	const Async::Execution execution_;

	static const inline std::filesystem::path kMetaExtension = ".content";

	Status status_ = Status::None;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* meta helper methods *//

	static std::filesystem::path GetMetaFilepath(const std::filesystem::path& filepath);

};

////////////////////////////////////////////////////////////////////////////////////////////
// concept
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
concept Content = std::derived_from<T, BaseContent>;

SXAVENGER_ENGINE_NAMESPACE_END
