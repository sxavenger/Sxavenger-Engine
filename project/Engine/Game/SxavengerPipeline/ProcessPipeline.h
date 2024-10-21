#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DxObject/DxShaderBlob.h>
#include <Engine/System/DxObject/DxCSPipeline.h>

//* lib
#include <Lib/Geometry/Vector2.h>

//* c++
#include <memory>
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// ProcessPipelineType enum
////////////////////////////////////////////////////////////////////////////////////////////
enum ProcessPipelineType : uint32_t {
	kTransition_SampleLighting,

	kCountOfProcessPipelineType
};

////////////////////////////////////////////////////////////////////////////////////////////
// ProcessPipeline class
////////////////////////////////////////////////////////////////////////////////////////////
class ProcessPipeline {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////
	template <class T>
	using ProcessArray = std::array<T, ProcessPipelineType::kCountOfProcessPipelineType>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ProcessPipeline()  = default;
	~ProcessPipeline() = default;

	void Init();

	void Term();

	//* option *//

	void SetPipeline(ProcessPipelineType type);

	void Dispatch(const Vector2ui& size);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	ProcessArray<std::unique_ptr<DxObject::CSBlob>>     blobs_;
	ProcessArray<std::unique_ptr<DxObject::CSPipeline>> pipelines_;

	//* config *//

	const Vector2ui threadNum_ = { 32, 32 };

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateTransition();

};