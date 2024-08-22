#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// attribute
#include <Attribute.h>

// c++
#include <list>
#include <memory>
#include <vector>

// DxObject
#include <DxMesh.h>
#include <DxShaderBlob.h>
#include <DxGraphicsPipeline.h>

// IA
#include <Model.h>
#include <DrawMethod.h>

// engine
#include <MyEngine.h>
#include <Sprite.h>

// collision
#include <Collider.h>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class SubobjectManager;

////////////////////////////////////////////////////////////////////////////////////////////
// Subobject base class
////////////////////////////////////////////////////////////////////////////////////////////
class Subobject
	: public Attribute {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	virtual ~Subobject() {}

	virtual void Init(const SubobjectManager* manager) = 0;

	virtual void Update() = 0;

	virtual void Draw(Camera3D* mainCamera) = 0;

	bool IsDelete() const { return isDelete_; }

	void SetIsDelete(bool isDelete) { isDelete_ = isDelete; }

protected:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Material structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Material {
		Color4f color             = { 1.0f, 1.0f, 1.0f, 1.0f };
		Matrix4x4 uvTransform     = Matrix4x4::Identity();
		uint32_t isDisplayMeshlet = false;
		uint32_t lambertType; //!< LambertType参照
		uint32_t phongType;   //!< phongType参照
		float specPow;

		void Init() {
			color = { 1.0f, 1.0f, 1.0f, 1.0f };
			uvTransform = Matrix4x4::Identity();
			lambertType = LambertType::TYPE_HALF_LAMBERT;
			phongType   = PhongType::TYPE_PHONGNONE;
			specPow     = 100.0f;
		}

		void SetImGuiCommand() {

			ImGui::ColorEdit4("color", &color.r);
			
			ImGui::CheckboxFlags("isDisplayMeshlet", &isDisplayMeshlet, true);

			if (ImGui::BeginCombo("lambertType", lambertItems_[lambertType])) {
				for (uint32_t i = 0; i < LambertType::kLambertTypeCount; ++i) {
					bool isSelect = (lambertType == i);

					if (ImGui::Selectable(lambertItems_[i], isSelect)) {
						lambertType = i;
					}
				}
				ImGui::EndCombo();
			}

			if (ImGui::BeginCombo("phongType", phongItems_[phongType])) {
				for (uint32_t i = 0; i < PhongType::kPhongTypeCount; ++i) {
					bool isSelect = (phongType == i);

					if (ImGui::Selectable(phongItems_[i], isSelect)) {
						phongType = i;
					}
				}
				ImGui::EndCombo();
			}

			ImGui::DragFloat("specPow", &specPow, 0.1f);
		}
	};

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* externals *//

	const SubobjectManager* manager_ = nullptr;

	//* member *//

	bool isDelete_ = false;

};

////////////////////////////////////////////////////////////////////////////////////////////
// Subobjects namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Subobjects {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Sprite class
	////////////////////////////////////////////////////////////////////////////////////////////
	class SpriteSubobject
		: public Subobject {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init(const SubobjectManager* manager) override;

		void Update() override;

		void Draw(Camera3D* mainCamera) override;

		void SetAttributeImGui() override;

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		std::unique_ptr<Sprite> sprite_;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Plane class
	////////////////////////////////////////////////////////////////////////////////////////////
	class Plane
		: public Subobject {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init(const SubobjectManager* manager) override;

		void Update() override;

		void Draw(Camera3D* mainCamera) override;

		void SetAttributeImGui() override;

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		//* member *//
		//* IA
		std::unique_ptr<Model>          model_;
		std::unique_ptr<DxObject::Mesh> mesh_;

		//* buffer
		std::unique_ptr<DxObject::BufferResource<TransformationMatrix>> matBuffer_;
		EulerTransform                                                  transform_;

		std::unique_ptr<DxObject::BufferResource<Material>> material_;
		UVTransform                                         uvTransform_;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Sphere class
	////////////////////////////////////////////////////////////////////////////////////////////
	class Sphere
		: public Subobject
		, public Collider {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init(const SubobjectManager* manager) override;

		void Update() override;

		void Draw(Camera3D* mainCamera) override;

		void SetAttributeImGui() override;

		const Vector3f& GetColliderPosition() const override { return transform_.translate; }

		void OnCollisionEnter(_MAYBE_UNUSED Collider* const other) override;

		void OnCollisionExit(_MAYBE_UNUSED Collider* const other) override;

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		//* member *//
		//* IA
		std::unique_ptr<Model>          model_;
		std::unique_ptr<DxObject::Mesh> mesh_;

		//* buffer
		std::unique_ptr<DxObject::BufferResource<TransformationMatrix>> matBuffer_;
		EulerTransform                                                  transform_;

		std::unique_ptr<DxObject::BufferResource<Material>> material_;
		UVTransform                                         uvTransform_;


	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Teapot class
	////////////////////////////////////////////////////////////////////////////////////////////
	class Teapot
		: public Subobject {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init(const SubobjectManager* manager) override;

		void Update() override;

		void Draw(Camera3D* mainCamera) override;

		void SetAttributeImGui() override;

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		//* member *//
		//* IA
		std::unique_ptr<Model>          model_;
		std::unique_ptr<DxObject::Mesh> mesh_;

		//* buffer
		std::unique_ptr<DxObject::BufferResource<TransformationMatrix>> matBuffer_;
		EulerTransform                                                  transform_;

		std::unique_ptr<DxObject::BufferResource<Material>> material_;
		UVTransform                                         uvTransform_;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Bunny class
	////////////////////////////////////////////////////////////////////////////////////////////
	class Bunny
		: public Subobject {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init(const SubobjectManager* manager) override;

		void Update() override;

		void Draw(Camera3D* mainCamera) override;

		void SetAttributeImGui() override;

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		//* member *//
		//* IA
		std::unique_ptr<Model>          model_;
		std::unique_ptr<DxObject::Mesh> mesh_;

		//* buffer
		std::unique_ptr<DxObject::BufferResource<TransformationMatrix>> matBuffer_;
		EulerTransform                                                  transform_;

		std::unique_ptr<DxObject::BufferResource<Material>> material_;
		UVTransform                                         uvTransform_;


	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// MultiMesh class
	////////////////////////////////////////////////////////////////////////////////////////////
	class MultiMesh
		: public Subobject {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init(const SubobjectManager* manager) override;

		void Update() override;

		void Draw(Camera3D* mainCamera) override;

		void SetAttributeImGui() override;

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		//* member *//
		//* IA
		std::unique_ptr<Model>                       model_;
		std::vector<std::unique_ptr<DxObject::Mesh>> meshes_;

		//* buffer
		std::unique_ptr<DxObject::BufferResource<TransformationMatrix>> matBuffer_;
		EulerTransform                                                  transform_;

		std::unique_ptr<DxObject::BufferResource<Material>> material_;
		UVTransform                                         uvTransform_;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// MultiMaterial class
	////////////////////////////////////////////////////////////////////////////////////////////
	class MultiMaterial
		: public Subobject {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init(const SubobjectManager* manager) override;

		void Update() override;

		void Draw(Camera3D* mainCamera) override;

		void SetAttributeImGui() override;

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		//* member *//
		//* IA
		std::unique_ptr<Model>                       model_;
		std::vector<std::unique_ptr<DxObject::Mesh>> meshes_;

		//* buffer
		std::unique_ptr<DxObject::BufferResource<TransformationMatrix>> matBuffer_;
		EulerTransform                                                  transform_;

		std::vector<std::unique_ptr<DxObject::BufferResource<Material>>> materials_;
		std::vector<UVTransform>                                         uvTransforms_;

		//* data *//

		uint32_t meshNum_ = {};

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Suzanne class
	////////////////////////////////////////////////////////////////////////////////////////////
	class Suzanne
		: public Subobject {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init(const SubobjectManager* manager) override;

		void Update() override;

		void Draw(Camera3D* mainCamera) override;

		void SetAttributeImGui() override;

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		//* member *//
		//* IA
		std::unique_ptr<Model>          model_;
		std::unique_ptr<DxObject::Mesh> mesh_;

		//* buffer
		std::unique_ptr<DxObject::BufferResource<TransformationMatrix>> matBuffer_;
		EulerTransform                                                  transform_;

		std::unique_ptr<DxObject::BufferResource<Material>> material_;

	};

}