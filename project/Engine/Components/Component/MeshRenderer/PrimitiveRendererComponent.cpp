#include "PrimitiveRendererComponent.h"
SXAVENGER_ENGINE_USING

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

const TransformComponent* PrimitiveRendererComponent::RequireTransform() const {
	return BaseComponent::GetBehaviour()->RequireComponent<TransformComponent>();
}
