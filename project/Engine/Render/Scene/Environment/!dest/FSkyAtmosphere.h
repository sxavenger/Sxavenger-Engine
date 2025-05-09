//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* environment
#include "../FEnvironmentMap.h"

////////////////////////////////////////////////////////////////////////////////////////////
// FSkyAtmosphere class
////////////////////////////////////////////////////////////////////////////////////////////
class FSkyAtmosphere
	: public FEnvironmentMap {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// AtmosphereMap structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct AtmosphereMap {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Create(const Vector2ui& _size);

		void Dispatch(const DirectXThreadContext* context);

		//=========================================================================================
		// public variables
		//=========================================================================================

		//* directX12 *//

		ComPtr<ID3D12Resource> resource; //!< cubemap.
		DxObject::Descriptor descriptorSRV;
		DxObject::Descriptor descriptorUAV;

		//* parameter *//

		Vector2ui size;

		//* pipeline *//

		std::unique_ptr<DxObject::ReflectionComputePipelineState> pipeline;

	private:

		//=========================================================================================
		// private methods
		//=========================================================================================

		void CreateBuffer(const Vector2ui& _size);

		void CreatePipeline();

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Create(const Vector2ui& size);

	void Update(const DirectXThreadContext* context);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* map *//

	AtmosphereMap atmosphere_;

	//* parameter *//

	static inline constexpr UINT16 kCubemap_       = 6;
	static inline constexpr Vector2ui kNumThreads_ = { 16, 16 };

};
