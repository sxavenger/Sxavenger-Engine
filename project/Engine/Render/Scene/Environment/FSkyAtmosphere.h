//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* environment
#include "FEnvironmentMap.h"

////////////////////////////////////////////////////////////////////////////////////////////
// FSkyAtmosphere class
////////////////////////////////////////////////////////////////////////////////////////////
class FSkyAtmosphere
	: public FEnvironmentMap {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Parameter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Parameter {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init();

		void SetImGuiCommand();

		//=========================================================================================
		// public variables
		//=========================================================================================

		Vector3f direction; //!< todo: transform componentから引っ張ってくる?
		float intensity;

	};

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

		void Commit(const DirectXThreadContext* context);

		//* resource option *//

		const DxObject::Descriptor& UseDescriptorSRV(const DirectXThreadContext* context);

		//=========================================================================================
		// public variables
		//=========================================================================================

		//* directX12 *//

		DxObject::ResourceStateTracker asyncResource;
		DxObject::Descriptor asyncDescriptorUAV;
		DxObject::Descriptor asyncDescriptorSRV;

		DxObject::ResourceStateTracker mainResource;
		DxObject::Descriptor mainDescriptorSRV;

		//* parameter *//

		Vector2ui size;

		//* pipeline *//

		std::unique_ptr<DxObject::ReflectionComputePipelineState> pipeline; //!< HACK

		//* buffer *//

		std::unique_ptr<DxObject::DimensionBuffer<Parameter>> parameter_;

	private:

		//=========================================================================================
		// private methods
		//=========================================================================================

		void CreateBuffer();

		void CreatePipeline();

		void CreateDimensionBuffer();

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Create(const Vector2ui& size);

	void Update(const DirectXThreadContext* context) override;

	void Task(const DirectXThreadContext* context) override;

	void SetImGuiCommand();

	//* atmosphere option *//

	const DxObject::Descriptor& UseAtmosphereDescriptor(const DirectXThreadContext* context);

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
