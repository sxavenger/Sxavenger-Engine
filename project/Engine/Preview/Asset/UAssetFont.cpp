#include "UAssetFont.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// UAssetFont class methods
////////////////////////////////////////////////////////////////////////////////////////////

void UAssetFont::Setup(const DirectXQueueContext* context, const stbtt_fontinfo& info, float size) {

	float scale = stbtt_ScaleForPixelHeight(&info, size);

	int32_t ascent, descent, lineGap;
	stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);
	// ascent:  文字の上端からベースラインまでの距離
	// descent: 文字の下端からベースラインまでの距離（負の値）
	// lineGap: 行間の追加スペース

	atlasData_.resize(static_cast<size_t>(kAtlasSize.x * kAtlasSize.y));

	CreateAtlasTexture();

	LoadGlyph(info, scale);

	UploadAtlasData(context);

	UBaseAsset::Complete();
	Logger::EngineThreadLog(std::format("[UAssetFont]: font setup complete. uuid: {}", UBaseAsset::GetId().Serialize()));
}

void UAssetFont::ShowInspector() {
	UBaseAsset::ShowInspector();

	if (!UBaseAsset::IsComplete()) {
		ImGui::Text("loading...");
		return;
	}

	ImGui::Text("Font Atlas Texture");
	ShowTexture();
}

void UAssetFont::CreateAtlasTexture() {

	auto device = SxavengerSystem::GetDxDevice()->GetDevice();

	{ //!< resourceの生成

		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Width            = kAtlasSize.x;
		desc.Height           = kAtlasSize.y;
		desc.DepthOrArraySize = 1;
		desc.MipLevels        = 1;
		desc.Format           = DXGI_FORMAT_R8_UNORM;
		desc.SampleDesc.Count = 1;

		auto hr = device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&resource_)
		);
		Exception::Assert(SUCCEEDED(hr), "font atlas texture resource create failed.");

		resource_->SetName(L"UAsset | Font Atlas Texture");
	}

	{ //!< SRVの生成

		descriptorSRV_ = SxavengerSystem::GetDescriptor(kDescriptor_SRV);

		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                  = DXGI_FORMAT_R8_UNORM;
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels     = 1;

		device->CreateShaderResourceView(
			resource_.Get(),
			&desc,
			descriptorSRV_.GetCPUHandle()
		);
	}
}

void UAssetFont::LoadGlyph(const stbtt_fontinfo& info, float scale) {
	for (wchar_t c = L' '; c <= L'~'; ++c) {
		glyphs_[c] = GenerateGlyphInfo(info, scale, c);
	}

	for (wchar_t c = L'あ'; c <= L'ん'; ++c) {
		glyphs_[c] = GenerateGlyphInfo(info, scale, c);
	}

	for (wchar_t c = L'ア'; c <= L'ン'; ++c) {
		glyphs_[c] = GenerateGlyphInfo(info, scale, c);
	}
}

UAssetFont::GlyphInfo UAssetFont::GenerateGlyphInfo(const stbtt_fontinfo& info, float scale, wchar_t c) {

	int32_t width, height, offsetX, offsetY;

	uint8_t* bitmap = stbtt_GetCodepointBitmap(
		&info, 0, scale, c,
		&width, &height, &offsetX, &offsetY
	);

	if (!bitmap) { //!< 文字が見つからない場合
		return GenerateGlyphInfo(info, scale, L'□');
	}

	// アトラスに配置
	if (static_cast<uint32_t>(current_.x + width) >= kAtlasSize.x) {
		current_.x = 0;
		current_.y += maxHeight_;
		maxHeight_ = 0;
	}

	// ビットマップをアトラスにコピー
	for (int32_t y = 0; y < height; ++y) {
		for (int32_t x = 0; x < width; ++x) {

			int32_t srcIndex = y * width + x;
			int32_t dstIndex = (current_.y + y) * kAtlasSize.x + (current_.x + x);

			if (dstIndex < atlasData_.size()) {
				atlasData_[dstIndex] = bitmap[srcIndex];
			}
		}
	}

	// グリフ情報を設定
	GlyphInfo glyph = {};
	glyph.uv[0]  = static_cast<Vector2f>(current_) / static_cast<Vector2f>(kAtlasSize);
	glyph.uv[1]  = Vector2f(current_ + Vector2i{ width, height }) / static_cast<Vector2f>(kAtlasSize);
	glyph.size   = Vector2f{ static_cast<float>(width), static_cast<float>(height) };
	glyph.offset = Vector2f{ static_cast<float>(offsetX), static_cast<float>(offsetY) };

	// アドバンス幅の取得
	int32_t advanceWidth, leftSlideBearing;
	stbtt_GetCodepointHMetrics(&info, c, &advanceWidth, &leftSlideBearing);
	glyph.advance = advanceWidth * scale;

	// アトラスの位置を調整
	current_.x += width + 1;
	maxHeight_ = std::max(maxHeight_, height + 1);

	stbtt_FreeBitmap(bitmap, nullptr);
	return glyph;
}

void UAssetFont::UploadAtlasData(const DirectXQueueContext* context) {

	auto device      = SxavengerSystem::GetDxDevice()->GetDevice();
	auto commandList = context->GetCommandList();

	size_t bufferSize = kAtlasSize.x * kAtlasSize.y;

	ComPtr<ID3D12Resource> intermediate = DxObject::CreateBufferResource(
		device, bufferSize
	);

	{ //!< dataの書き込み
		void* data;
		auto hr = intermediate->Map(0, nullptr, &data);
		Exception::Assert(SUCCEEDED(hr), "resource mapping failed.");

		std::memcpy(data, atlasData_.data(), bufferSize);

		intermediate->Unmap(0, nullptr);
	}
	
	// テクスチャコピー情報設定
	D3D12_TEXTURE_COPY_LOCATION src = {};
	src.pResource                          = intermediate.Get();
	src.Type                               = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	src.PlacedFootprint.Offset             = 0;
	src.PlacedFootprint.Footprint.Format   = DXGI_FORMAT_R8_UNORM;
	src.PlacedFootprint.Footprint.Width    = static_cast<UINT>(kAtlasSize.x);
	src.PlacedFootprint.Footprint.Height   = static_cast<UINT>(kAtlasSize.y);
	src.PlacedFootprint.Footprint.Depth    = 1;
	src.PlacedFootprint.Footprint.RowPitch = static_cast<UINT>(kAtlasSize.x);

	D3D12_TEXTURE_COPY_LOCATION dst = {};
	dst.pResource        = resource_.Get();
	dst.Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	dst.SubresourceIndex = 0;

	commandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

	// 使用可能状態に遷移
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource   = resource_.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_COMMON;

	commandList->ResourceBarrier(1, &barrier);

	// commandの実行
	context->ExecuteAllAllocators();
}

void UAssetFont::ShowTexture() {
	Vector2f region = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };

	// テクスチャの表示サイズを計算
	float scale = std::min(region.x / static_cast<float>(kAtlasSize.x), region.y / static_cast<float>(kAtlasSize.x));

	ImGui::Image(descriptorSRV_.GetGPUHandle().ptr, ImVec2(kAtlasSize.x * scale, kAtlasSize.y * scale));
}
