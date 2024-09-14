#include "MeshBunny.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <Sxavenger.h>
#include <SxavengerGraphics.h>
#include <PrimitiveDrawer.h>

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using namespace DxObject;

////////////////////////////////////////////////////////////////////////////////////////////
// MeshBunny class methods
////////////////////////////////////////////////////////////////////////////////////////////

void MeshBunny::Init() {

	// mesh
	{
		// sphereのvertex, indexの取得
		model_ = std::make_unique<Model>("resources/model", "bunny.obj");

		// meshに書き込み
		mesh_ = std::make_unique<Mesh>(model_->GetMesh(0).GetVertexBuffer(), model_->GetMesh(0).GetIndexBuffer());
	}

	// Graphics
	blob_ = std::make_unique<GraphicsBlob>();
	blob_->Create(L"Default.AS.hlsl", GRAPHICS_AMPLIFICATION);
	blob_->Create(L"Default.MS.hlsl", GRAPHICS_MESH);
	blob_->Create(L"Default.PS.hlsl", GRAPHICS_PIXEL);

	GraphicsRootSignatureDesc desc;
	desc.SetVirtualSRV(0, VISIBILITY_ALL, 0); //!< vertices
	desc.SetVirtualSRV(1, VISIBILITY_ALL, 1); //!< uniqueVertexIndices
	desc.SetVirtualSRV(2, VISIBILITY_ALL, 2); //!< meshlets
	desc.SetVirtualSRV(3, VISIBILITY_ALL, 3); //!< primitiveIndices
	desc.SetVirtualSRV(4, VISIBILITY_ALL, 4); //!< cullData
	desc.SetCBV(5, VISIBILITY_ALL, 0); //!< meshInfo

	desc.SetCBV(6, VISIBILITY_ALL, 1); //!< camera
	desc.SetCBV(7, VISIBILITY_ALL, 2); //!< transfrom
	desc.SetCBV(8, VISIBILITY_ALL, 3); //!< cullCamera

	desc.SetCBV(9, VISIBILITY_PIXEL, 4); //!< Material
	desc.SetSRV(10, VISIBILITY_PIXEL, 5); //!< Texture
	desc.SetSampler(0, MODE_WRAP, VISIBILITY_PIXEL, 0);

	pipeline_ = std::make_unique<GraphicsPipeline>();
	pipeline_->CreateRootSignature(Sxavenger::GetDevicesObj(), desc);
	pipeline_->CreatePipeline(Sxavenger::GetDevicesObj(), blob_.get(), kBlendModeNormal);

	// materialResource
	material_ = std::make_unique<DxObject::BufferResource<Vector4f>>(Sxavenger::GetDevicesObj(), 1);
	material_->operator[](0) = { 1.0f, 1.0f, 1.0f, 1.0f };

	// matrixResources
	matrix_ = std::make_unique<DxObject::BufferResource<TransformationMatrix>>(Sxavenger::GetDevicesObj(), 1);
	matrix_->operator[](0).Init();

	cullingCamera_ = std::make_unique<Camera3D>();
	cullingCamera_->SetThisAttribute("cullingCamera");
	cullingCamera_->SetProjection(0.45f, static_cast<float>(kWindowWidth) / static_cast<float>(kWindowHeight), 0.1f, 10.0f);

	SetThisAttribute("bunny");
}

void MeshBunny::Term() {
}

void MeshBunny::Draw() {

	/*cullingCamera_->DrawFrustum({1.0f, 1.0f, 0.0f, 1.0f});*/

	auto commandList = Sxavenger::GetCommandList();

	pipeline_->SetPipeline(commandList);

	// ParamBuffers
	commandList->SetGraphicsRootConstantBufferView(6, SxavengerGraphics::camera3D->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(7, matrix_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(8, cullingCamera_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(9, material_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(10, Sxavenger::GetTextureHandleGPU("resources/uvChecker.png"));

	mesh_->Dispatch(0, 1, 2, 3, 4, 5);

}

void MeshBunny::SetAttributeImGui() {

	transform_.SetImGuiCommand();

	matrix_->operator[](0).worldMatrix = transform_.ToMatrix();
}
