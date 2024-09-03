#include "DefaultGraphics.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <SxavengerGraphics.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DefaultGraphics class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DefaultGraphics::Init() {
	// 各pipelineの設定
	{
		auto& render = renders_[kDefault_Texture];
		render.Init();
		render.CreateBlob(L"SxavengerShader/Default.VS.hlsl", GRAPHICS_VERTEX);
		render.CreateBlob(L"SxavengerShader/DefaultTexture.PS.hlsl", GRAPHICS_PIXEL);
		render.CreateTable();
		render.BindBuffer("gCamera",    D3D12_GPU_VIRTUAL_ADDRESS{});
		render.BindBuffer("gTransform", D3D12_GPU_VIRTUAL_ADDRESS{});
		render.BindBuffer("gMaterial",  D3D12_GPU_VIRTUAL_ADDRESS{});
		render.BindBuffer("gAlbed",     D3D12_GPU_DESCRIPTOR_HANDLE{});
		render.CreatePipeline();
	}
}

void DefaultGraphics::Term() {
}

void DefaultGraphics::DrawDefaultTexture(
	const InputAssembler<VertexData>& ia,
	const BaseTransformBuffer& transform, const ObjectMaterialBuffer& material,
	const D3D12_GPU_DESCRIPTOR_HANDLE& texture) {

	auto commandList = Sxavenger::GetCommandList();

	auto& render = renders_[kDefault_Texture];
	render.SetPipeline(commandList);

	ia.SetBuffer();

	render.BindBuffer("gCamera", SxavengerGraphics::camera3D->GetGPUVirtualAddress());
	render.BindBuffer("gTransform", transform.GetGPUVirtualAddress());
	render.BindBuffer("gMaterial", material.GetGPUVirtualAddress());
	render.BindBuffer("gAlbed", texture);

	render.BindGraphicsParameter(commandList);

	ia.DrawCall();

}

DefaultGraphics* DefaultGraphics::GetInstance() {
	static DefaultGraphics instance;
	return &instance;
}
