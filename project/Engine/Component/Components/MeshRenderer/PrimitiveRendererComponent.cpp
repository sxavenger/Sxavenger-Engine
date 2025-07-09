#include "PrimitiveRendererComponent.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../Entity/MonoBehaviour.h"

////////////////////////////////////////////////////////////////////////////////////////////
// PrimitiveRendererComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PrimitiveRendererComponent::ShowComponentInspector() {
}

const TransformComponent* PrimitiveRendererComponent::GetTransform() const {
	if (transform_ != nullptr) {
		return transform_;
	}

	return BaseComponent::GetBehaviour()->RequireComponent<TransformComponent>(); //!< TransformComponentが必要
}
