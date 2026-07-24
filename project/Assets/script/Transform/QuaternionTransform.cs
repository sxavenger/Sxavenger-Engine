////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger - Transform namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Sxavenger.Transform {

	//-----------------------------------------------------------------------------------------
	// using
	//-----------------------------------------------------------------------------------------
	using System.Runtime.InteropServices;
	using Sxavenger.Mathematic;

	////////////////////////////////////////////////////////////////////////////////////////////
	// QuaternionTransform structure
	////////////////////////////////////////////////////////////////////////////////////////////
	[StructLayout(LayoutKind.Sequential)]
	public struct QuaternionTransform {

		//=========================================================================================
		// public variables
		//=========================================================================================

		public Vector3f scale;
		public Vector4f rotate; // TODO: Quaternionに変更
		public Vector3f translate;

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* constructor *//

		public QuaternionTransform(Vector3f scale, Vector4f rotate, Vector3f translate) {
			this.scale     = scale;
			this.rotate    = rotate;
			this.translate = translate;
		}

	}
}
