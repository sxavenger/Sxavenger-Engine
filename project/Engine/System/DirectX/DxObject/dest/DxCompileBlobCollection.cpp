#include "DxCompileBlobCollection.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Config/SxavengerDirectory.h>

//* external
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// CompileBlobCollection class methods
////////////////////////////////////////////////////////////////////////////////////////////

void CompileBlobCollection::Init(ShaderCompiler* compiler) {
	SetCompiler(compiler);
}

void CompileBlobCollection::Term() {
}

std::weak_ptr<ComPtr<IDxcBlob>> CompileBlobCollection::TryCreateBlob(const std::filesystem::path& filename, CompileProfile profile) {

	if (!blobs_.Contains(filename)) {
		std::shared_ptr<ComPtr<IDxcBlob>> blob
			= std::make_shared<ComPtr<IDxcBlob>>(compiler_->Compile(filename, profile, L"main"));

		blobs_.Emplace(filename, { std::move(blob), profile});
	}

	return blobs_.At(filename).blob;
}

void CompileBlobCollection::Reload(const std::filesystem::path& filename) {
	if (!blobs_.Contains(filename)) {
		return; //!< filenameのblobが見つからなかったため
	}

	blobs_[filename].blob = std::make_shared<ComPtr<IDxcBlob>>(compiler_->Compile(filename, blobs_.At(filename).profile, L"main"));
}

std::weak_ptr<ComPtr<IDxcBlob>> CompileBlobCollection::GetBlob(const std::filesystem::path& filename) {
	Assert(blobs_.Contains(filename), "Compile Blob not found. filename: " + filename.string());
	return blobs_.At(filename).blob;
}

void CompileBlobCollection::SystemDebugGui() {

	ImGui::SeparatorText("reload shaders");

	for (auto& blob : blobs_.GetMap()) {

		std::string filepath = blob.first.generic_string();

		if (ImGui::Selectable(filepath.c_str(), false)) {
			Reload(blob.first);
		}
	}
}
