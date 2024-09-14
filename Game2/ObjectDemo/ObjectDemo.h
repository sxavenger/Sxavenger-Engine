#pragma once

//* Engine
#include <GraphicsRender.h>
#include <Transform.h>
#include <Material.h>
#include <InputAssembler.h>
#include <ObjectStructure.h>
#include <DrawMethod.h>
#include <Model.h>

#include <Attribute.h>
#include <Collider.h>

class ObjectDemo
	: public Attribute, public Collider {
public:

	ObjectDemo() = default;
	~ObjectDemo() = default;

	void Init();
	
	void Draw();

	//* attribute *//

	void SetAttributeImGui() override;

	//* collder *//

	const Vector3f& GetColliderPosition() const { return transform_[0].transform.translate; };

	void OnCollisionEnter(_MAYBE_UNUSED Collider* const other) override {}
	void OnCollisionExit(_MAYBE_UNUSED Collider* const other) override {}

private:

	EulerTransformBuffer transform_[2];
	ObjectMaterialBuffer material_;

	// InputAssembler<VertexData> inputAssembler_;

	std::unique_ptr<Model> model_;

};