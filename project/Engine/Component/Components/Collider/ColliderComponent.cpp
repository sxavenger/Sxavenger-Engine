#include "ColliderComponent.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../Entity/MonoBehaviour.h"

//* collider
#include "CollisionCallbackCollection.h"

//* engine
#include <Engine/System/UI/SxImGui.h>
#include <Engine/Content/SxavengerContent.h>

//* external
#include <imgui.h>
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// BoundingPrimitiveLine structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void ColliderComponent::BoundingPrimitiveLine::operator()(const CollisionBoundings::Sphere& sphere) {
	SxavengerContent::PushSphere(
		position,
		sphere.radius,
		color
	);
}

void ColliderComponent::BoundingPrimitiveLine::operator()(const CollisionBoundings::Capsule& capsule) {
	static const uint32_t kSubdivision = 24;

	// カプセルの両端の中心を計算
	Vector3f topCenter = position + capsule.direction * (capsule.length * 0.5f);
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

void ColliderComponent::BoundingPrimitiveLine::operator()(const CollisionBoundings::AABB& aabb) {
	SxavengerContent::PushBox(
		position + aabb.min,
		position + aabb.max,
		color
	);
}

void ColliderComponent::BoundingPrimitiveLine::operator()(const CollisionBoundings::OBB& obb) {
	obb; // TODO:
}

////////////////////////////////////////////////////////////////////////////////////////////
// ColliderComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ColliderComponent::ShowComponentInspector() {

	ImGui::Checkbox("## enable", &isEnable_);

	ImGui::SameLine();

	SxImGui::InputText("tag", tag_);

	if (ImGui::BeginCombo("bounding", magic_enum::enum_name(static_cast<CollisionBoundings::BoundingType>(bounding_.index())).data())) {
		for (const auto& [value, name] : magic_enum::enum_entries<CollisionBoundings::BoundingType>()) {
			if (ImGui::Selectable(name.data(), bounding_.index() == static_cast<size_t>(value))) {
				switch (value) {
					case CollisionBoundings::BoundingType::Sphere:
						SetColliderBoundingSphere();
						break;
					case CollisionBoundings::BoundingType::Capsule:
						SetColliderBoundingCapsule();
						break;
					case CollisionBoundings::BoundingType::AABB:
						SetColliderBoundingAABB();
						break;
					case CollisionBoundings::BoundingType::OBB:
						SetColliderBoundingOBB();
						break;
				}
			}
		}

		ImGui::EndCombo();
	}

	switch (bounding_.index()) {
		case static_cast<size_t>(CollisionBoundings::BoundingType::Sphere):
			{
				auto& sphere = std::get<CollisionBoundings::Sphere>(bounding_);

				SxImGui::DragFloat("radius", &sphere.radius, 0.01f, 0.0f);
			}
			break;

		case static_cast<size_t>(CollisionBoundings::BoundingType::Capsule):
			{
				auto& capsule = std::get<CollisionBoundings::Capsule>(bounding_);

				if (SxImGui::DragVector3("direction", &capsule.direction.x, 0.01f)) {
					capsule.direction = capsule.direction.Normalize();
				}

				SxImGui::DragFloat("radius", &capsule.radius, 0.01f, 0.0f);
				SxImGui::DragFloat("length", &capsule.length, 0.01f, 0.0f);
			}
			break;

		case static_cast<size_t>(CollisionBoundings::BoundingType::AABB):
			{
				auto& aabb = std::get<CollisionBoundings::AABB>(bounding_);

				ImGui::DragFloat3("max", &aabb.max.x, 0.01f);
				ImGui::DragFloat3("min", &aabb.min.x, 0.01f);

				// minがmaxを上回らないようclamp
				aabb.min.x = (std::min)(aabb.min.x, aabb.max.x);
				aabb.max.x = (std::max)(aabb.min.x, aabb.max.x);

				aabb.min.y = (std::min)(aabb.min.y, aabb.max.y);
				aabb.max.y = (std::max)(aabb.min.y, aabb.max.y);

				aabb.min.z = (std::min)(aabb.min.z, aabb.max.z);
				aabb.max.z = (std::max)(aabb.min.z, aabb.max.z);
			}
			break;

		case static_cast<size_t>(CollisionBoundings::BoundingType::OBB):
			{
				auto& obb = std::get<CollisionBoundings::OBB>(bounding_);

				Vector3f e = Quaternion::ToEuler(obb.orientation);
				if (ImGui::DragFloat3("rotate", &e.x, 0.01f)) {
					obb.orientation = Quaternion::ToQuaternion(e);
				}

				ImGui::DragFloat3("size", &obb.size.x, 0.01f);
			}
			break;
	}

	PushBoundingLine();
}

void ColliderComponent::SetColliderBoundingSphere(const CollisionBoundings::Sphere& sphere) {
	bounding_ = sphere;
}

void ColliderComponent::SetColliderBoundingCapsule(const CollisionBoundings::Capsule& capsule) {
	bounding_ = capsule;
}

void ColliderComponent::SetColliderBoundingAABB(const CollisionBoundings::AABB& aabb) {
	bounding_ = aabb;
}

void ColliderComponent::SetColliderBoundingOBB(const CollisionBoundings::OBB& obb) {
	bounding_ = obb;
}

void ColliderComponent::UpdateColliderState() {
	//!< 次フレームの準備
	for (auto& state : states_ | std::views::values) {
		state <<= 1; //!< current情報をprevにシフト
	}
}

void ColliderComponent::CallbackOnCollision(const CollisionCallbackCollection* collection) {
	for (auto it = states_.begin(); it != states_.end();) {
		switch (it->second.to_ulong()) {
			case 0b00: //!< prev 0, current 0
				it = states_.erase(it);
				continue;

			case 0b01: //!< prev 0, current 1
				collection->CallbackOnCollisionEnter(this, it->first);
				break;

			case 0b10: //!< prev 1, current 0
				collection->CallbackOnCollisionExit(this, it->first);
				break;

			case 0b11: //!< prev 1, current 1
				collection->CallbackOnCollisionStay(this, it->first);
				break;
		};

		++it;
	}
}

void ColliderComponent::OnCollision(ColliderComponent* other) {
	states_[other].set(static_cast<bool>(CollisionState::kCurrent)); //!< 現在frameで当たった
}

void ColliderComponent::SetCollisionState(
	ColliderComponent* const other,
	const std::optional<bool>& isHitCurrent, const std::optional<bool>& isHitPrev) {

	auto& state = states_[other];

	if (isHitCurrent.has_value()) {
		state.set(static_cast<bool>(CollisionState::kCurrent), isHitCurrent.value());
	}

	if (isHitPrev.has_value()) {
		state.set(static_cast<bool>(CollisionState::kPrev), isHitPrev.value());
	}

}

TransformComponent* ColliderComponent::RequireTransform() const {
	return BaseComponent::GetBehaviour()->RequireComponent<TransformComponent>();
}

json ColliderComponent::PerseToJson() const {
	json data = json::object();

	data["tag"]      = tag_;
	data["isEnable"] = isEnable_;

	json& bounding = data["bounding"] = json::object();

	bounding["type"] = magic_enum::enum_name(static_cast<CollisionBoundings::BoundingType>(bounding_.index()));

	switch (bounding_.index()) {
		case static_cast<size_t>(CollisionBoundings::BoundingType::Sphere):
			{
				const auto& sphere = std::get<CollisionBoundings::Sphere>(bounding_);

				bounding["radius"] = sphere.radius;
			}
			break;

		case static_cast<size_t>(CollisionBoundings::BoundingType::Capsule):
			{
				const auto& capsule = std::get<CollisionBoundings::Capsule>(bounding_);

				bounding["direction"] = GeometryJsonSerializer::ToJson(capsule.direction);
				bounding["radius"]    = capsule.radius;
				bounding["length"]    = capsule.length;
			}
			break;

		case static_cast<size_t>(CollisionBoundings::BoundingType::AABB):
			{
				const auto& aabb = std::get<CollisionBoundings::AABB>(bounding_);

				bounding["min"] = GeometryJsonSerializer::ToJson(aabb.min);
				bounding["max"] = GeometryJsonSerializer::ToJson(aabb.max);
			}
			break;

		case static_cast<size_t>(CollisionBoundings::BoundingType::OBB):
			{
				const auto& obb = std::get<CollisionBoundings::OBB>(bounding_);

				bounding["orientation"] = GeometryJsonSerializer::ToJson(obb.orientation);
				bounding["size"]        = GeometryJsonSerializer::ToJson(obb.size);
			}
			break;
	}

	return data;
}

void ColliderComponent::InputJson(const json& data) {
	tag_      = data.value("tag", tag_);
	isEnable_ = data.value("isEnable", isEnable_);

	const auto& bounding = data["bounding"];
	const auto type      = magic_enum::enum_cast<CollisionBoundings::BoundingType>(bounding["type"].get<std::string>()).value();

	switch (type) {
		case CollisionBoundings::BoundingType::Sphere:
			{
				CollisionBoundings::Sphere sphere = {};
				sphere.radius = bounding["radius"].get<float>();

				SetColliderBounding(sphere);
			}
			break;

		case CollisionBoundings::BoundingType::Capsule:
			{
				CollisionBoundings::Capsule capsule = {};
				capsule.direction = GeometryJsonSerializer::JsonToVector3f(bounding["direction"]);
				capsule.radius    = bounding["radius"].get<float>();
				capsule.length    = bounding["length"].get<float>();

				SetColliderBounding(capsule);
			}
			break;

		case CollisionBoundings::BoundingType::AABB:
			{
				CollisionBoundings::AABB aabb = {};
				aabb.min = GeometryJsonSerializer::JsonToVector3f(bounding["min"]);
				aabb.max = GeometryJsonSerializer::JsonToVector3f(bounding["max"]);

				SetColliderBounding(aabb);
			}
			break;

		case CollisionBoundings::BoundingType::OBB:
			{
				CollisionBoundings::OBB obb = {};
				obb.orientation = GeometryJsonSerializer::JsonToQuaternion(bounding["orientation"]);
				obb.size        = GeometryJsonSerializer::JsonToVector3f(bounding["size"]);

				SetColliderBounding(obb);
			}
			break;
	}
}

void ColliderComponent::PushBoundingLine() const {
	BoundingPrimitiveLine line = {};
	line.position = RequireTransform()->GetPosition();
	line.color    = isEnable_ ? Color4f{ 1.0f, 1.0f, 0.0f, 1.0f } : Color4f{ 0.4f, 0.4f, 0.4f, 1.0f };

	std::visit(line, bounding_);
}
