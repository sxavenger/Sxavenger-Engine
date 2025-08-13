#include "ColliderPrimitiveRenderer.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../ComponentStorage.h"
#include "ColliderComponent.h"

//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Content/SxavengerContent.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ColliderPrimitiveSphere class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ColliderPrimitiveSphere::Init() {
	CreatePipeline();

	//* vertex buffer
	vb_ = std::make_unique<DxObject::VertexDimensionBuffer<Vector4f>>();
	vb_->Create(SxavengerSystem::GetDxDevice(), kSubdivision * 3 * 2);

	uint32_t index = 0;

	// xz軸の円
	for (uint32_t lon = 0; lon < kSubdivision; ++lon) {
		float theta = kRoundEvery * lon;

		// 単位円の生成
		Vector3f p1 = { //!< 始点
			std::sin(theta),
			0.0f,
			std::cos(theta),
		};

		Vector3 p2 = { //!< 終点
			std::sin(theta + kRoundEvery),
			0.0f,
			std::cos(theta + kRoundEvery),
		};

		vb_->At(index++) = { p1.x, p1.y, p1.z, 1.0f };
		vb_->At(index++) = { p2.x, p2.y, p2.z, 1.0f };
	}

	// xy軸の円
	for (uint32_t lat = 0; lat < kSubdivision; ++lat) {
		float phi = kRoundEvery * lat;

		// 単位円の生成
		Vector3f p1 = { //!< 始点
			std::cos(phi),
			std::sin(phi),
			0.0f
		};

		Vector3f p2 = { //!< 始点
			std::cos(phi + kRoundEvery),
			std::sin(phi + kRoundEvery),
			0.0f
		};

		vb_->At(index++) = { p1.x, p1.y, p1.z, 1.0f };
		vb_->At(index++) = { p2.x, p2.y, p2.z, 1.0f };
	}

	// yz軸の円
	for (uint32_t lat = 0; lat < kSubdivision; ++lat) {
		float phi = kRoundEvery * lat;

		// 単位円の生成
		Vector3f p1 = { //!< 始点
			0.0f,
			std::sin(phi),
			std::cos(phi)
		};

		Vector3f p2 = { //!< 終点
			0.0f,
			std::sin(phi + kRoundEvery),
			std::cos(phi + kRoundEvery)
		};

		vb_->At(index++) = { p1.x, p1.y, p1.z, 1.0f };
		vb_->At(index++) = { p2.x, p2.y, p2.z, 1.0f };
	}

	buffer_ = std::make_unique<DxObject::VectorDimensionBuffer<std::pair<Matrix4x4, Color4f>>>();
	buffer_->Resize(SxavengerSystem::GetDxDevice(), kMaxBuffer);
}

void ColliderPrimitiveSphere::Reset() {
	index_ = 0;
}

void ColliderPrimitiveSphere::StackBuffer(const Vector3f& position, const CollisionBoundings::Sphere& sphere, const Color4f& color) {
	if (index_ >= kMaxBuffer) {
		return; //!< buffer is full.
	}

	buffer_->At(index_).first  = Matrix4x4::MakeAffine({ sphere.radius, sphere.radius, sphere.radius }, Quaternion::Identity(), position);
	buffer_->At(index_).second = color;
	index_++;
}

void ColliderPrimitiveSphere::Render(const DirectXQueueContext* context, CameraComponent* component) {

	pipeline_->SetPipeline(context->GetDxCommand());

	D3D12_VERTEX_BUFFER_VIEW vbv = vb_->GetVertexBufferView();
	context->GetCommandList()->IASetVertexBuffers(0, 1, &vbv);

	BindBufferDesc desc = {};
	desc.SetAddress("gCamera", component->GetGPUVirtualAddress());
	desc.SetAddress("gParameter", buffer_->GetGPUVirtualAddress());

	pipeline_->BindGraphicsBuffer(context->GetDxCommand(), desc);

	context->GetCommandList()->DrawInstanced(vb_->GetSize(), index_, 0, 0);
}

void ColliderPrimitiveSphere::CreatePipeline() {
	pipeline_ = std::make_unique<DxObject::ReflectionGraphicsPipelineState>();
	pipeline_->CreateBlob(kPackagesShaderDirectory / "render/debug/collider/colliderPrimitive.vs.hlsl", GraphicsShaderType::vs);
	pipeline_->CreateBlob(kPackagesShaderDirectory / "render/debug/collider/colliderPrimitive.ps.hlsl", GraphicsShaderType::ps);
	pipeline_->ReflectionRootSignature(SxavengerSystem::GetDxDevice());

	GraphicsPipelineDesc desc = {};
	desc.CreateDefaultDesc();
	desc.SetBlendMode(0, BlendMode::kBlendModeNormal_AlphaMax);
	desc.SetPrimitive(PrimitiveType::LineList);
	desc.SetRTVFormat(0, DXGI_FORMAT_R16G16B16A16_FLOAT);

	pipeline_->CreatePipeline(SxavengerSystem::GetDxDevice(), desc);
}

////////////////////////////////////////////////////////////////////////////////////////////
// ColliderPrimitiveLine class methods
////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////
// ColliderPrimitiveVisitor structure
////////////////////////////////////////////////////////////////////////////////////////////
struct ColliderPrimitiveVisitor {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void operator()(const CollisionBoundings::Sphere& sphere) {
		drawer->StackSphere(position, sphere, color);
	}

	void operator()(const CollisionBoundings::Capsule& capsule) {
		// todo: 専用のRendererを用意する

		static const uint32_t kSubdivision = 24;

		// カプセルの両端の中心を計算
		Vector3f topCenter    = position + capsule.direction * (capsule.length * 0.5f);
		Vector3f bottomCenter = position - capsule.direction * (capsule.length * 0.5f);

		Vector3f arbitrary = (std::abs(capsule.direction.x) < std::abs(capsule.direction.y)) ? Vector3f(1, 0, 0) : Vector3f(0, 1, 0);
		Vector3f xAxis = Vector3f::Cross(capsule.direction, arbitrary).Normalize(); // 半円の「横」方向
		Vector3f zAxis = Vector3f::Cross(capsule.direction, xAxis).Normalize();     // 半円の「奥」方向

		const float kLonEvery = kTau / kSubdivision;
		const float kLatEvery = kPi / kSubdivision;

		// xz軸円
		for (uint32_t i = 0; i < kSubdivision; ++i) {

			float lon = kLonEvery * i;
			float x = std::cos(lon) * capsule.radius;
			float z = std::sin(lon) * capsule.radius;

			float nextX = std::cos(lon + kLonEvery) * capsule.radius;
			float nextZ = std::sin(lon + kLonEvery) * capsule.radius;

			// 現在の点を計算
			Vector3f currentPoint = xAxis * x + zAxis * z;
			Vector3f nextPoint = xAxis * nextX + zAxis * nextZ;

			// 線を描画
			SxavengerContent::PushLine(currentPoint + topCenter, nextPoint + topCenter, color);
			SxavengerContent::PushLine(currentPoint + bottomCenter, nextPoint + bottomCenter, color);
		}

		// xy軸円
		for (uint32_t i = 0; i < kSubdivision; ++i) {
			float lat = kLatEvery * i;
			float x = std::cos(lat) * capsule.radius;
			float y = std::sin(lat) * capsule.radius;

			float nextX = std::cos(lat + kLatEvery) * capsule.radius;
			float nextY = std::sin(lat + kLatEvery) * capsule.radius;

			// 現在の点を計算
			Vector3f currentPoint = xAxis * x + capsule.direction * y;
			Vector3f nextPoint = xAxis * nextX + capsule.direction * nextY;

			// 線を描画
			SxavengerContent::PushLine((xAxis * x + capsule.direction * y) + topCenter, (xAxis * nextX + capsule.direction * nextY) + topCenter, color);
			SxavengerContent::PushLine((xAxis * x - capsule.direction * y) + bottomCenter, (xAxis * nextX - capsule.direction * nextY) + bottomCenter, color);
		}

		// xy軸円
		for (uint32_t i = 0; i < kSubdivision; ++i) {
			float lat = kLatEvery * i;
			float y = std::sin(lat) * capsule.radius;
			float z = std::cos(lat) * capsule.radius;

			float nextY = std::sin(lat + kLatEvery) * capsule.radius;
			float nextZ = std::cos(lat + kLatEvery) * capsule.radius;

			// 現在の点を計算
			Vector3f currentPoint = zAxis * z + capsule.direction * y;
			Vector3f nextPoint = zAxis * nextZ + capsule.direction * nextY;

			// 線を描画
			SxavengerContent::PushLine((zAxis * z + capsule.direction * y) + topCenter, (zAxis * nextZ + capsule.direction * nextY) + topCenter, color);
			SxavengerContent::PushLine((zAxis * z - capsule.direction * y) + bottomCenter, (zAxis * nextZ - capsule.direction * nextY) + bottomCenter, color);
		}

		SxavengerContent::PushLine(xAxis * capsule.radius + topCenter, xAxis * capsule.radius + bottomCenter, color);
		SxavengerContent::PushLine(-xAxis * capsule.radius + topCenter, -xAxis * capsule.radius + bottomCenter, color);
		SxavengerContent::PushLine(zAxis * capsule.radius + topCenter, zAxis * capsule.radius + bottomCenter, color);
		SxavengerContent::PushLine(-zAxis * capsule.radius + topCenter, -zAxis * capsule.radius + bottomCenter, color);
	}

	void operator()(const CollisionBoundings::AABB& aabb) {
		SxavengerContent::PushBox(
			position + aabb.min,
			position + aabb.max,
			color
		);
	}

	void operator()(const CollisionBoundings::OBB& obb) {
		obb;
	}

	//=========================================================================================
	// public variables
	//=========================================================================================

	ColliderPrimitiveRenderer* drawer = nullptr;

	Vector3f position = {};
	Color4f color     = {};

};

////////////////////////////////////////////////////////////////////////////////////////////
// ColliderPrimitiveRenderer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ColliderPrimitiveRenderer::Init() {
	sphere_ = std::make_unique<ColliderPrimitiveSphere>();
	sphere_->Init();
}

void ColliderPrimitiveRenderer::Term() {
	sphere_.reset();
}

void ColliderPrimitiveRenderer::Render(const DirectXQueueContext* context, CameraComponent* component) {
	sphere_->Reset();

	sComponentStorage->ForEach<ColliderComponent>([this](ColliderComponent* collider) {
		if (!isDisableRender_ && !collider->IsActive()) {
			return;
		}

		const std::optional<CollisionBoundings::Boundings>& boundings = collider->GetBoundings();

		if (boundings.has_value()) {
			ColliderPrimitiveVisitor visitor = {};
			visitor.drawer   = this;
			visitor.position = collider->GetTransform()->GetPosition();
			visitor.color = collider->IsActive() ? enableColor_ : disableColor_;

			std::visit(visitor, boundings.value());
		}
	});

	context->RequestQueue(DirectXQueueContext::RenderQueue::Direct);
	sphere_->Render(context, component);
}

void ColliderPrimitiveRenderer::StackSphere(const Vector3f& position, const CollisionBoundings::Sphere& sphere, const Color4f& color) {
	sphere_->StackBuffer(position, sphere, color);
}

void ColliderPrimitiveRenderer::SetImGuiCommand() {
	ImGui::ColorEdit3("enable color", &enableColor_.x);

	ImGui::Checkbox("disable render", &isDisableRender_);
	ImGui::BeginDisabled(!isDisableRender_);
	ImGui::ColorEdit3("disable color", &disableColor_.x);
	ImGui::EndDisabled();
}
