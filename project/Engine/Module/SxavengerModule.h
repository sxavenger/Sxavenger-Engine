#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* module
#include "DebugPrimitive/DebugPrimitive.h"
#include "Collider/ColliderCollection.h"
#include "Skeleton/SkinningPipeline.h"
#include "Audio/AudioController.h"

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerModule class
////////////////////////////////////////////////////////////////////////////////////////////
class SxavengerModule {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void Init();

	static void Term();

	//-----------------------------------------------------------------------------------------
	// primitive option
	//-----------------------------------------------------------------------------------------

	static void ResetPrimtive();

	static void DrawLine(const Vector3f& v1, const Vector3f& v2, const Color4f& color);

	static void DrawAxis(const Vector3f& center, float length);

	static void DrawGrid(const Vector3f& center, float size);

	static void DrawBox(const Vector3f& min, const Vector3f& max, const Color4f& color);

	static DebugPrimitive* GetDebugPrimitive();

	//-----------------------------------------------------------------------------------------
	// collider collection option
	//-----------------------------------------------------------------------------------------

	static void SetCollider(Collider* collider);

	static void EraseCollider(Collider* collider);

	static void CheckCollision();

	static void DrawCollider();

	static ColliderCollection* GetColliderCollection();

	//-----------------------------------------------------------------------------------------
	// skinning pipeline option
	//-----------------------------------------------------------------------------------------

	static void SetSkinningPipeline(const DirectXThreadContext* context);

	static void DispatchSkinningPipeline(const DirectXThreadContext* context, const DxObject::BindBufferDesc& desc, uint32_t vertexSize);

	//-----------------------------------------------------------------------------------------
	// audio controller option
	//-----------------------------------------------------------------------------------------

	static std::unique_ptr<Audio> CreateAudio(const AudioBuffer* buffer, bool isLoop = false);

	static void PlayOneShot(const AudioBuffer* buffer, float volume);

	static AudioController* GetAudioController();

private:
};
