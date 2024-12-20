#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"
#include "DxShaderCompiler.h"

//* engine
#include <Engine/System/UI/ISystemDebugGui.h>

//* lib
#include <Lib/Sxl/LowerPathMap.h>

//* c++
#include <memory>
#include <string>
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
_DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// CompileBlobCollection class
////////////////////////////////////////////////////////////////////////////////////////////
class CompileBlobCollection
	: public ISystemDebugGui {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	CompileBlobCollection()  = default;
	~CompileBlobCollection() { Term(); };

	void Init(ShaderCompiler* compiler);

	void Term();

	std::weak_ptr<ComPtr<IDxcBlob>> TryCreateBlob(const std::filesystem::path& filename, CompileProfile profile);

	void Reload(const std::filesystem::path& filename);

	std::weak_ptr<ComPtr<IDxcBlob>> GetBlob(const std::filesystem::path& filename);

	//* setter *//

	void SetCompiler(ShaderCompiler* compiler) { compiler_ = compiler; }

	//* getter *//

	ShaderCompiler* GetCompiler() const { return compiler_; }

	void SystemDebugGui() override;

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// CompileBlob structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct CompileBlob {
		std::shared_ptr<ComPtr<IDxcBlob>> blob;
		CompileProfile                    profile;
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	ShaderCompiler* compiler_ = nullptr;

	//* collection *//

	Sxl::LowerPathMap<CompileBlob> blobs_;
	//!< main, profile統一

};

_DXOBJECT_NAMESPACE_END