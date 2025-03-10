#include "ColliderPrimitiveRenderer.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../ComponentStorage.h"
#include "ColliderComponent.h"

//* engine
#include <Engine/System/SxavengerSystem.h>

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

		vb_->At(index++) = { p1.x, p1.y, p1.z };
		vb_->At(index++) = { p2.x, p2.y, p2.z };
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

		vb_->At(index++) = { p1.x, p1.y, p1.z };
		vb_->At(index++) = { p2.x, p2.y, p2.z };
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

		vb_->At(index++) = { p1.x, p1.y, p1.z };
		vb_->At(index++) = { p2.x, p2.y, p2.z };
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

	buffer_->At(index_).first  = Matrix::MakeAffine({ sphere.radius, sphere.radius, sphere.radius }, Quaternion::Identity(), position);
	buffer_->At(index_).second = color;
	index_++;
}

void ColliderPrimitiveSphere::Render(const DirectXThreadContext* context, CameraComponent* component) {
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
	pipeline_->CreateBlob(kPackagesShaderDirectory / "primitive/collider/colliderPrimitive.vs.hlsl", GraphicsShaderType::vs);
	pipeline_->CreateBlob(kPackagesShaderDirectory / "primitive/collider/colliderPrimitive.ps.hlsl", GraphicsShaderType::ps);
	pipeline_->ReflectionRootSignature(SxavengerSystem::GetDxDevice());

	GraphicsPipelineDesc desc = {};
	desc.CreateDefaultDesc();
	desc.SetPrimitive(PrimitiveType::kLineList);
	desc.SetRTVFormat(0, DXGI_FORMAT_R32G32B32A32_FLOAT);

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
		capsule;
	}

	void operator()(const CollisionBoundings::AABB& aabb) {
		aabb;
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

void ColliderPrimitiveRenderer::Render(const DirectXThreadContext* context, CameraComponent* component) {
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
