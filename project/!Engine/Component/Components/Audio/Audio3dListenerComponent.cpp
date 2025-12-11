#include "Audio3dListenerComponent.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* entity
#include "../../Entity/MonoBehaviour.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Audio3dListenerComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

const X3DAUDIO_LISTENER Audio3dListenerComponent::GetListener() const {

	const TransformComponent* transform = RequireTransform();

	Vector3f position = transform->GetPosition();

	X3DAUDIO_LISTENER listener = {};
	listener.Position    = { position.x, position.y, position.z };
	listener.OrientFront = { 0.0f, 0.0f, 1.0f };
	listener.OrientTop   = { 0.0f, 1.0f, 0.0f };
	listener.Velocity    = { 0.0f, 0.0f, 0.0f };

	return listener;
}

const TransformComponent* Audio3dListenerComponent::RequireTransform() const {
	return BaseComponent::GetBehaviour()->RequireComponent<TransformComponent>();
}
