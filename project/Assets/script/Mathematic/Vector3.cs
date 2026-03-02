////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger - Mathematic namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Sxavenger.Mathematic {

	//-----------------------------------------------------------------------------------------
	// using
	//-----------------------------------------------------------------------------------------
	using System.Runtime.InteropServices;

	////////////////////////////////////////////////////////////////////////////////////////////
	// Vector3 [floating-point] structure
	////////////////////////////////////////////////////////////////////////////////////////////
	[StructLayout(LayoutKind.Sequential)]
	public struct Vector3f {

		//=========================================================================================
		// public variables
		//=========================================================================================

		public float x, y, z;

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* constructor *//

		public Vector3f(float x, float y, float z) {
			this.x = x;
			this.y = y;
			this.z = z;
		}

		//* operator [add] *//

		public static Vector3f operator+(Vector3f lhs, Vector3f rhs) {
			return new Vector3f(
				lhs.x + rhs.x,
				lhs.y + rhs.y,
				lhs.z + rhs.z
			);
		}

		//* operator [subtract] *//

		public static Vector3f operator-(Vector3f lhs, Vector3f rhs) {
			return new Vector3f(
				lhs.x - rhs.x,
				lhs.y - rhs.y,
				lhs.z - rhs.z
			);
		}

		//* operator [multiply] *//

		public static Vector3f operator*(Vector3f lhs, float rhs) {
			return new Vector3f(
				lhs.x * rhs,
				lhs.y * rhs,
				lhs.z * rhs
			);
		}

		public static Vector3f operator*(float lhs, Vector3f rhs) {
			return new Vector3f(
				lhs * rhs.x,
				lhs * rhs.y,
				lhs * rhs.z
			);
		}

		//* operator [divide] *//

		public static Vector3f operator/(Vector3f lhs, float rhs) {
			return new Vector3f(
				lhs.x / rhs,
				lhs.y / rhs,
				lhs.z / rhs
			);
		}

		//* operator [negate] *//

		public static Vector3f operator-(Vector3f v) {
			return new Vector3f(-v.x, -v.y, -v.z);
		}

		//* operator [access] *//

		public float this[int index] {
			get {
				switch (index) {
					case 0: return x;
					case 1: return y;
					case 2: return z;
					default: throw new System.IndexOutOfRangeException("[Vector3f] invalid index.");
				}
			}
			set {
				switch (index) {
					case 0: x = value; break;
					case 1: y = value; break;
					case 2: z = value; break;
					default: throw new System.IndexOutOfRangeException("[Vector3f] invalid index.");
				}
			}
		}

		//* string conversion *//

		public override string ToString() {
			return $"({x}, {y}, {z})";
		}

	}

}