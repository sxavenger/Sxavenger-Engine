#include "ColliderComponent.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../Entity/EntityBehaviour.h"

//* collider
#include "CollisionCallbackCollection.h"

//* engine
#include <Engine/System/UI/SxImGui.h>
#include <Engine/Graphics/Graphics.h>

//* external
#include <imgui.h>
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// State structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void ColliderComponent::State::Update() {
	//!< 現在の衝突検出結果を1frame前の衝突検出結果にコピーして、現在の衝突検出結果をリセットする
	At(History::Previous) = At(History::Current);
	At(History::Current)  = std::nullopt;
}

Sxl::Flag<ColliderComponent::History> ColliderComponent::State::GetBit() const {

	Sxl::Flag<History> bit = History::None;

	if (At(History::Current).HasPenetration()) {
		bit |= History::Current;
	}

	if (At(History::Previous).HasPenetration()) {
		bit |= History::Previous;
	}

	return bit;
}

void ColliderComponent::State::Set(History history, const CollisionDetection::Detection& detection) {
	StreamLogger::AssertA(history == History::Current || history == History::Previous, "History is not a valid index.");
	At(history) = detection;
}

CollisionDetection::Detection& ColliderComponent::State::operator[](History history) {
	StreamLogger::AssertA(history == History::Current || history == History::Previous, "History is not a valid index.");
	return detections[static_cast<uint8_t>(history) - 1];
}

const CollisionDetection::Detection& ColliderComponent::State::operator[](History history) const {
	StreamLogger::AssertA(history == History::Current || history == History::Previous, "History is not a valid index.");
	return detections[static_cast<uint8_t>(history) - 1];
}

CollisionDetection::Detection& ColliderComponent::State::At(History history) {
	StreamLogger::AssertA(history == History::Current || history == History::Previous, "History is not a valid index.");
	return detections[static_cast<uint8_t>(history) - 1];
}

const CollisionDetection::Detection& ColliderComponent::State::At(History history) const {
	StreamLogger::AssertA(history == History::Current || history == History::Previous, "History is not a valid index.");
	return detections[static_cast<uint8_t>(history) - 1];
}

////////////////////////////////////////////////////////////////////////////////////////////
// BoundingPrimitiveLine structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void ColliderComponent::BoundingPrimitiveLine::operator()(const CollisionBoundings::Sphere& sphere) {
	Graphics::PushSphere(
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
		Graphics::PushLine(currentPoint + topCenter, nextPoint + topCenter, color);
		Graphics::PushLine(currentPoint + bottomCenter, nextPoint + bottomCenter, color);
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
		Graphics::PushLine((xAxis * x + capsule.direction * y) + topCenter, (xAxis * nextX + capsule.direction * nextY) + topCenter, color);
		Graphics::PushLine((xAxis * x - capsule.direction * y) + bottomCenter, (xAxis * nextX - capsule.direction * nextY) + bottomCenter, color);
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
		Graphics::PushLine((zAxis * z + capsule.direction * y) + topCenter, (zAxis * nextZ + capsule.direction * nextY) + topCenter, color);
		Graphics::PushLine((zAxis * z - capsule.direction * y) + bottomCenter, (zAxis * nextZ - capsule.direction * nextY) + bottomCenter, color);
	}

	Graphics::PushLine(xAxis * capsule.radius + topCenter, xAxis * capsule.radius + bottomCenter, color);
	Graphics::PushLine(-xAxis * capsule.radius + topCenter, -xAxis * capsule.radius + bottomCenter, color);
	Graphics::PushLine(zAxis * capsule.radius + topCenter, zAxis * capsule.radius + bottomCenter, color);
	Graphics::PushLine(-zAxis * capsule.radius + topCenter, -zAxis * capsule.radius + bottomCenter, color);
}

void ColliderComponent::BoundingPrimitiveLine::operator()(const CollisionBoundings::AABB& aabb) {
	Graphics::PushBox(
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

	if (ImGui::BeginCombo("bounding", magic_enum::enum_name(GetBoundingType()).data())) {
		for (const auto& [value, name] : magic_enum::enum_entries<CollisionBoundings::BoundingType>()) {
			if (ImGui::Selectable(name.data(), GetBoundingType() == value)) {
				switch (value) {
					case CollisionBoundings::BoundingType::Sphere:
						SetBoundingSphere();
						break;
					case CollisionBoundings::BoundingType::Capsule:
						SetBoundingCapsule();
						break;
					case CollisionBoundings::BoundingType::AABB:
						SetBoundingAABB();
						break;
					case CollisionBoundings::BoundingType::OBB:
						SetBoundingOBB();
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

void ColliderComponent::Update() {
	for (auto& state : states_ | std::views::values) {
		state.Update(); //!< 衝突検出結果の履歴を更新
	}
}

void ColliderComponent::SetBoundingSphere(const CollisionBoundings::Sphere& sphere) {
	bounding_ = sphere;
}

void ColliderComponent::SetBoundingCapsule(const CollisionBoundings::Capsule& capsule) {
	bounding_ = capsule;
}

void ColliderComponent::SetBoundingAABB(const CollisionBoundings::AABB& aabb) {
	bounding_ = aabb;
}

void ColliderComponent::SetBoundingOBB(const CollisionBoundings::OBB& obb) {
	bounding_ = obb;
}

void ColliderComponent::OnCollision(ColliderComponent* other, const CollisionDetection::Detection& detection) {
	states_[other].Set(History::Current, detection); //!< 現在frameの衝突検出結果を保存
}

void ColliderComponent::CallbackOnCollision(const CollisionCallbackCollection* collection) {

	for (auto it = states_.begin(); it != states_.end();) {

		switch (static_cast<History>(it->second.GetBit())) {
			case History::None: //!< prev 0, current 0
				it = states_.erase(it);
				//!< 衝突履歴がないものは削除.
				continue;

			case History::Current:
				collection->CallbackOnCollisionEnter(this, it->first, it->second[History::Current].GetPenetration());
				break;

			case History::Previous:
				collection->CallbackOnCollisionExit(this, it->first, it->second[History::Previous].GetPenetration());
				break;

			case History::All:
				collection->CallbackOnCollisionStay(this, it->first, it->second[History::Current].GetPenetration());
				//!< penetrationは最新情報を渡す.
				break;
		};

		it++;
	}
}

void ColliderComponent::SetCollisionState(
	ColliderComponent* const other,
	History history, const CollisionDetection::Detection& detection) {

	//!< historyはCurrentかPreviousのどちらかでなければならない
	StreamLogger::AssertA(history == History::Current || history == History::Previous, "History is not a valid index.");

	states_[other].Set(history, detection);
}

TransformComponent* ColliderComponent::RequireTransform() const {
	return BaseComponent::GetBehaviour()->RequireComponent<TransformComponent>();
}

json ColliderComponent::ParseToJson() const {
	json data = json::object();

	data["isEnable"] = JsonSerializeFormatter<bool>::Serialize(isEnable_);
	data["tag"]      = JsonSerializeFormatter<std::string>::Serialize(tag_);

	json& bounding = data["bounding"] = json::object();

	bounding["type"] = magic_enum::enum_name(GetBoundingType());

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

				bounding["direction"] = JsonSerializeFormatter<Vector3f>::Serialize(capsule.direction);
				bounding["radius"]    = JsonSerializeFormatter<float>::Serialize(capsule.radius);
				bounding["length"]    = JsonSerializeFormatter<float>::Serialize(capsule.length);
			}
			break;

		case static_cast<size_t>(CollisionBoundings::BoundingType::AABB):
			{
				const auto& aabb = std::get<CollisionBoundings::AABB>(bounding_);

				bounding["min"] = JsonSerializeFormatter<Vector3f>::Serialize(aabb.min);
				bounding["max"] = JsonSerializeFormatter<Vector3f>::Serialize(aabb.max);
			}
			break;

		case static_cast<size_t>(CollisionBoundings::BoundingType::OBB):
			{
				const auto& obb = std::get<CollisionBoundings::OBB>(bounding_);

				bounding["orientation"] = JsonSerializeFormatter<Quaternion>::Serialize(obb.orientation);
				bounding["size"]        = JsonSerializeFormatter<Vector3f>::Serialize(obb.size);
			}
			break;
	}

	return data;
}

void ColliderComponent::InputJson(const json& data) {
	tag_      = JsonSerializeFormatter<std::string>::Deserialize(data["tag"]);
	isEnable_ = JsonSerializeFormatter<bool>::Deserialize(data["isEnable"]);

	const auto& bounding = data["bounding"];
	const auto type      = magic_enum::enum_cast<CollisionBoundings::BoundingType>(JsonSerializeFormatter<std::string>::Deserialize(bounding.at("type"))).value();

	switch (type) {
		case CollisionBoundings::BoundingType::Sphere:
			{
				CollisionBoundings::Sphere sphere = {};
				sphere.radius = JsonSerializeFormatter<float>::Deserialize(bounding["radius"]);

				SetBounding(sphere);
			}
			break;

		case CollisionBoundings::BoundingType::Capsule:
			{
				CollisionBoundings::Capsule capsule = {};
				capsule.direction = JsonSerializeFormatter<Vector3f>::Deserialize(bounding["direction"]);
				capsule.radius    = JsonSerializeFormatter<float>::Deserialize(bounding["radius"]);
				capsule.length    = JsonSerializeFormatter<float>::Deserialize(bounding["length"]);

				SetBounding(capsule);
			}
			break;

		case CollisionBoundings::BoundingType::AABB:
			{
				CollisionBoundings::AABB aabb = {};
				aabb.min = JsonSerializeFormatter<Vector3f>::Deserialize(bounding["min"]);
				aabb.max = JsonSerializeFormatter<Vector3f>::Deserialize(bounding["max"]);

				SetBounding(aabb);
			}
			break;

		case CollisionBoundings::BoundingType::OBB:
			{
				CollisionBoundings::OBB obb = {};
				obb.orientation = JsonSerializeFormatter<Quaternion>::Deserialize(bounding["orientation"]);
				obb.size        = JsonSerializeFormatter<Vector3f>::Deserialize(bounding["size"]);

				SetBounding(obb);
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
