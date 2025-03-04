#include "ColliderPrimitiveDrawer.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Module/SxavengerModule.h>
#include <Engine/Render/Scene/Actor/Camera/ACameraActor.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ColliderPrimitiveDrawer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ColliderPrimitiveDrawer::Init() {
	CreatePipeline();
	CreateSphereIA();

	sphereBuffer_ = std::make_unique<DxObject::VectorDimensionBuffer<std::pair<Matrix4x4, Color4f>>>();
	sphereBuffer_->Reserve(SxavengerSystem::GetDxDevice(), 128);
}

void ColliderPrimitiveDrawer::DrawCollider(const std::optional<CollisionBoundings::Boundings>& boundings, const Vector3f& position, const Color4f& color) {
	if (!boundings.has_value()) {
		return;
	}

	ColliderPrimitiveDrawerVisitor visitor = {};
	visitor.drawer   = this;
	visitor.position = position;
	visitor.color    = color;

	std::visit(visitor, boundings.value());
}

void ColliderPrimitiveDrawer::CreateSphereIA() {

	sphereVB_ = std::make_unique<DxObject::VertexDimensionBuffer<Vector4f>>();
	sphereVB_->Create(SxavengerSystem::GetDxDevice(), (kSphereSubdivision + 1) * 3 - 1);

	uint32_t index = 0;

	// xz軸の円
	for (uint32_t lon = 0; lon <= kSphereSubdivision; ++lon) {
		float theta = kSphereRoundEvery * lon;

		// 単位円の生成
		Vector3f p = { //!< 始点
			std::sin(theta),
			0.0f,
			std::cos(theta),
		};

		sphereVB_->At(index++) = { p.x, p.y, p.z, 1.0f };
	}

	// xy軸の円
	for (uint32_t lat = 0; lat <= kSphereSubdivision; ++lat) {
		float phi = kSphereRoundEvery * lat;

		// 単位円の生成
		Vector3f p = { //!< 始点
			std::cos(phi),
			std::sin(phi),
			0.0f
		};

		sphereVB_->At(index++) = { p.x, p.y, p.z, 1.0f };
	}

	// yz軸の円
	for (uint32_t lat = 0; lat < kSphereSubdivision; ++lat) {
		float phi = kSphereRoundEvery * lat;

		// 単位円の生成
		Vector3f p = { //!< 始点
			0.0f,
			std::sin(phi),
			std::cos(phi)
		};

		sphereVB_->At(index++) = { p.x, p.y, p.z, 1.0f };
	}
}

void ColliderPrimitiveDrawer::CreatePipeline() {
	pipeline_ = std::make_unique<DxObject::ReflectionGraphicsPipelineState>();
	pipeline_->CreateBlob(kPackagesShaderDirectory / "primitive/collider/colliderPrimitive.vs.hlsl", GraphicsShaderType::vs);
	pipeline_->CreateBlob(kPackagesShaderDirectory / "primitive/collider/colliderPrimitive.ps.hlsl", GraphicsShaderType::ps);
	pipeline_->ReflectionRootSignature(SxavengerSystem::GetDxDevice());

	GraphicsPipelineDesc desc = {};
	desc.CreateDefaultDesc();
	desc.SetPrimitive(PrimitiveType::kLineStrip);
	desc.SetRTVFormat(0, DXGI_FORMAT_R32G32B32A32_FLOAT);

	pipeline_->CreatePipeline(SxavengerSystem::GetDxDevice(), desc);
}

void ColliderPrimitiveDrawer::DrawSphere(const Vector3f& position, const CollisionBoundings::Sphere& sphere, const Color4f& color) {
	uint32_t index = sphereBuffer_->GetSize();
	sphereBuffer_->Resize(SxavengerSystem::GetDxDevice(), index + 1);
	sphereBuffer_->At(index).first  = Matrix::MakeAffine({ sphere.radius, sphere.radius, sphere.radius }, Quaternion::Identity(), position);
	sphereBuffer_->At(index).second = color;
}

void ColliderPrimitiveDrawer::DrawCapsule(const Vector3f& position, const CollisionBoundings::Capsule& capsule, const Color4f& color) {
	// todo:

	static const uint32_t kSubdivision = 24;

	// カプセルの両端の中心を計算
	Vector3f topCenter    = position + capsule.direction * (capsule.length * 0.5f);
	Vector3f bottomCenter = position - capsule.direction * (capsule.length * 0.5f);

	Vector3f arbitrary = (std::abs(capsule.direction.x) < std::abs(capsule.direction.y)) ? Vector3f(1, 0, 0) : Vector3f(0, 1, 0);
	Vector3f xAxis     = Normalize(Cross(capsule.direction, arbitrary)); // 半円の「横」方向
	Vector3f zAxis     = Normalize(Cross(capsule.direction, xAxis));     // 半円の「奥」方向

	const float kLonEvery = pi_v * 2.0f / kSubdivision;
	const float kLatEvery = pi_v / kSubdivision;

	// xz軸円
	for (uint32_t i = 0; i < kSubdivision; ++i) {

		float lon = kLonEvery * i;
		float x = std::cos(lon) * capsule.radius;
		float z = std::sin(lon) * capsule.radius;

		float nextX = std::cos(lon + kLonEvery) * capsule.radius;
		float nextZ = std::sin(lon + kLonEvery) * capsule.radius;

		// 現在の点を計算
		Vector3f currentPoint = xAxis * x + zAxis * z;
		Vector3f nextPoint    = xAxis * nextX + zAxis * nextZ;

		// 線を描画
		SxavengerModule::DrawLine(currentPoint + topCenter, nextPoint + topCenter, color);
		SxavengerModule::DrawLine(currentPoint + bottomCenter, nextPoint + bottomCenter, color);
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
		Vector3f nextPoint    = xAxis * nextX + capsule.direction * nextY;

		// 線を描画
		SxavengerModule::DrawLine((xAxis * x + capsule.direction * y) + topCenter, (xAxis * nextX + capsule.direction * nextY) + topCenter, color);
		SxavengerModule::DrawLine((xAxis * x - capsule.direction * y) + bottomCenter, (xAxis * nextX - capsule.direction * nextY) + bottomCenter, color);
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
		Vector3f nextPoint    = zAxis * nextZ + capsule.direction * nextY;

		// 線を描画
		SxavengerModule::DrawLine((zAxis * z + capsule.direction * y) + topCenter, (zAxis * nextZ + capsule.direction * nextY) + topCenter, color);
		SxavengerModule::DrawLine((zAxis * z - capsule.direction * y) + bottomCenter, (zAxis * nextZ - capsule.direction * nextY) + bottomCenter, color);
	}

	SxavengerModule::DrawLine(xAxis * capsule.radius + topCenter, xAxis * capsule.radius + bottomCenter, color);
	SxavengerModule::DrawLine(-xAxis * capsule.radius + topCenter, -xAxis * capsule.radius + bottomCenter, color);
	SxavengerModule::DrawLine(zAxis * capsule.radius + topCenter, zAxis * capsule.radius + bottomCenter, color);
	SxavengerModule::DrawLine(-zAxis * capsule.radius + topCenter, -zAxis * capsule.radius + bottomCenter, color);
}

void ColliderPrimitiveDrawer::DrawAABB(const Vector3f& position, const CollisionBoundings::AABB& aabb, const Color4f& color) {
	// todo:

	Vector3f pos[8] = {};

	// A
	for (int i = 0; i < 4; ++i) {
		pos[i] = aabb.min;
	}

	pos[1].z = aabb.max.z;
	pos[2].y = aabb.max.y;
	pos[2].z = aabb.max.z;
	pos[3].y = aabb.max.y;

	// B
	for (int i = 4; i < 8; ++i) {
		pos[i] = aabb.max;
	}

	pos[4].y = aabb.min.y;
	pos[4].z = aabb.min.z;
	pos[5].y = aabb.min.y;
	pos[7].z = aabb.min.z;

	for (int i = 0; i < 4; ++i) {
		int next = (i + 1) % 4;
		SxavengerModule::DrawLine(pos[i] + position, pos[next] + position, color);
		SxavengerModule::DrawLine(pos[i + 4] + position, pos[next + 4] + position, color);
		SxavengerModule::DrawLine(pos[i] + position, pos[i + 4] + position, color);
	}
}

void ColliderPrimitiveDrawer::DrawOBB(const Vector3f& position, const CollisionBoundings::OBB& obb, const Color4f& color) {
	// todo:

	Vector3f pos[8] = {};

	// sizeのみの座標に設定
	pos[0] = { -obb.size.x, obb.size.y, -obb.size.z };
	pos[4] = { -obb.size.x, obb.size.y, obb.size.z };

	pos[1] = { -obb.size.x, -obb.size.y, -obb.size.z };
	pos[5] = { -obb.size.x, -obb.size.y, obb.size.z };

	pos[2] = { obb.size.x, -obb.size.y, -obb.size.z };
	pos[6] = { obb.size.x, -obb.size.y, obb.size.z };

	pos[3] = { obb.size.x, obb.size.y, -obb.size.z };
	pos[7] = { obb.size.x, obb.size.y, obb.size.z };

	Matrix4x4 mat = Matrix::MakeAffine(kUnit3<float>, obb.orientation, position);

	for (int i = 0; i < 8; ++i) {
		pos[i] = Matrix::Transform(pos[i], mat); //!< 姿勢行列 + centerの適用
	}

	for (int i = 0; i < 4; ++i) {
		int next = (i + 1) % 4;
		SxavengerModule::DrawLine(pos[i], pos[next], color);
		SxavengerModule::DrawLine(pos[i + 4], pos[next + 4], color);
		SxavengerModule::DrawLine(pos[i], pos[i + 4], color);
	}
}

void ColliderPrimitiveDrawer::Render(const DirectXThreadContext* context, ACameraActor* actor) {
	if (sphereBuffer_->GetSize() != 0) {
		pipeline_->SetPipeline(context->GetDxCommand());

		D3D12_VERTEX_BUFFER_VIEW vbv = sphereVB_->GetVertexBufferView();
		context->GetCommandList()->IASetVertexBuffers(0, 1, &vbv);

		BindBufferDesc desc = {};
		desc.SetAddress("gCamera", actor->GetGPUVirtualAddress());
		desc.SetAddress("gParameter", sphereBuffer_->GetGPUVirtualAddress());

		pipeline_->BindGraphicsBuffer(context->GetDxCommand(), desc);

		context->GetCommandList()->DrawInstanced(sphereVB_->GetSize(), sphereBuffer_->GetSize(), 0, 0);
	}
}

void ColliderPrimitiveDrawer::Clear() {
	sphereBuffer_->Resize(SxavengerSystem::GetDxDevice(), 0);
}

////////////////////////////////////////////////////////////////////////////////////////////
// ColliderPrimitiveDrawerVisitor class
////////////////////////////////////////////////////////////////////////////////////////////

void ColliderPrimitiveDrawerVisitor::operator()(const CollisionBoundings::Sphere& sphere) {
	drawer->DrawSphere(position, sphere, color);
}

void ColliderPrimitiveDrawerVisitor::operator()(const CollisionBoundings::Capsule& capsule) {
	drawer->DrawCapsule(position, capsule, color);
}

void ColliderPrimitiveDrawerVisitor::operator()(const CollisionBoundings::AABB& aabb) {
	drawer->DrawAABB(position, aabb, color);
}

void ColliderPrimitiveDrawerVisitor::operator()(const CollisionBoundings::OBB& obb) {
	drawer->DrawOBB(position, obb, color);
}
