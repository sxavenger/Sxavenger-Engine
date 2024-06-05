#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// directX
#include <d3d12.h>
#include <dxgi1_6.h>

// c++
#include <cstdint>
#include <vector>
#include <string>

// ComPtr
#include <ComPtr.h>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
namespace DxObject {
	class Devices;
}

////////////////////////////////////////////////////////////////////////////////////////////
// DxrObject namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace DxrObject {

	////////////////////////////////////////////////////////////////////////////////////////////
	// TextureMode enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum TextureMode {
		MODE_WRAP        = 1,
		MODE_MIRROR      = 2,
		MODE_CLAMP       = 3,
		MODE_BORDER      = 4,
		MODE_MIRROR_ONCE = 5
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// ExportType enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum ExportType {
		RAYGENERATION,
		HITGROUP,

		kNotSelectExport
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// RootSignatureDesc class
	////////////////////////////////////////////////////////////////////////////////////////////
	class RootSignatureDesc {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		~RootSignatureDesc() { Clear(); }

		void Resize(uint32_t paramSize, uint32_t samplerSize);

		void Clear();

		void SetCBV(uint32_t index, UINT shaderRegister);

		void SetSRV(uint32_t index, UINT shaderRegister);

		void SetUAV(uint32_t index, UINT shaderRegister);

		void SetSampler(uint32_t index, TextureMode mode, UINT shaderRegiste);

		void SetLocalRootSignature(ExportType type, const std::wstring& exportStr);

		//=========================================================================================
		// public variables
		//=========================================================================================

		std::vector<D3D12_ROOT_PARAMETER>      params;
		std::vector<D3D12_STATIC_SAMPLER_DESC> samplers;


		// ローカルルートシグネイチャ用の設定

		ExportType exportType   = kNotSelectExport;
		std::wstring exportName = L"";

		/*
		 ローカルルートシグネイチャで使用
		 raygenerationのローカルルートシグネイチャだった場合, main関数名
		 closest, anyhit, intersection の場合, hitgroup名
		*/

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		std::vector<D3D12_DESCRIPTOR_RANGE> ranges;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// RootSignature class
	////////////////////////////////////////////////////////////////////////////////////////////
	class RootSignature {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		~RootSignature() { Term(); }

		virtual void Init(DxObject::Devices* devices, const RootSignatureDesc& desc);

		void Term();

		ID3D12RootSignature* GetRootSignature() const { return rootSignature_.Get(); }

	protected:

		//=========================================================================================
		// protected variables
		//=========================================================================================

		ComPtr<ID3D12RootSignature> rootSignature_;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// LocalRootSignature class
	////////////////////////////////////////////////////////////////////////////////////////////
	class LocalRootSignature
		: public RootSignature {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init(DxObject::Devices* devices, const RootSignatureDesc& desc) override;

		ExportType GetExportType() const { return exportType_; }

		const std::wstring& GetExportName() const { return exportName_; }

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		ExportType exportType_;
		std::wstring exportName_;

	};

}
