#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DxrObject/DxrAccelerationStructure.h>
#include <Engine/System/DxrObject/DxrRaytracingShaderBlob.h>
#include <Engine/System/DxrObject/DxrStateObject.h>
#include <Engine/System/DxrObject/DxrShaderTable.h>

////////////////////////////////////////////////////////////////////////////////////////////
// RaytracingScene class
////////////////////////////////////////////////////////////////////////////////////////////
class RaytracingScene {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	RaytracingScene()  = default;
	~RaytracingScene() { Term(); }

	void Term();

	//* tlas option *//

	void CreateTLAS();

	void BeginSetupTLAS();

	void EndSetupTLAS();

	void SetBLASInstance(const DxrObject::BottomLevelAS* blas, const Matrix4x4& worldMatrix, const DxrObject::BufferRecoreder* recorder, UINT instanceId = 0);

	//* blobGroup option *//

	void CreateBlobGroup();

	void SetBlob(const DxrObject::RaytracingShaderBlob* blob);

	//* state object option *//

	void CreateStateObject(const DxrObject::GlobalRootSignatureDesc& desc);

	void SetStateObject();

	//* table option *//

	void CreateTable();

	void SetupShaderTable(const DxrObject::BufferRecoreder* raygeneration, const DxrObject::BufferRecoreder* miss);

	void DispatchRays();

	//* getter *//

	DxrObject::TopLevelAS* GetTLAS() const { return tlas_.get(); }

private:

	//=========================================================================================
	// private varaibles
	//=========================================================================================

	std::unique_ptr<DxrObject::TopLevelAS>                tlas_;
	std::unique_ptr<DxrObject::RaytracingShaderBlobGroup> blobGroup_;
	std::unique_ptr<DxrObject::StateObject>               stateObject_;
	std::unique_ptr<DxrObject::ShaderTable>               table_;


};