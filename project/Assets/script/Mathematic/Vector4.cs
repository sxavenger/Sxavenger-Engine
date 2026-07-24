////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger - Mathematic namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Sxavenger.Mathematic {

	//-----------------------------------------------------------------------------------------
	// using
	//-----------------------------------------------------------------------------------------
	using System.Runtime.InteropServices;

	////////////////////////////////////////////////////////////////////////////////////////////
	// Vector4 [floating-point] structure
	////////////////////////////////////////////////////////////////////////////////////////////
	[StructLayout(LayoutKind.Sequential)]
	public struct Vector4f {

		//=========================================================================================
		// public variables
		//=========================================================================================

		public float x, y, z, w;

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* constructor *//

		public Vector4f(float x, float y, float z, float w) {
			this.x = x;
			this.y = y;
			this.z = z;
			this.w = w;
		}

		//* operator [add] *//

		public static Vector4f operator+(Vector4f lhs, Vector4f rhs) {
			return new Vector4f(
				lhs.x + rhs.x,
				lhs.y + rhs.y,
				lhs.z + rhs.z,
				lhs.w + rhs.w
			);
		}

		//* operator [subtract] *//

		public static Vector4f operator-(Vector4f lhs, Vector4f rhs) {
			return new Vector4f(
				lhs.x - rhs.x,
				lhs.y - rhs.y,
				lhs.z - rhs.z,
				lhs.w - rhs.w
			);
		}

		//* operator [multiply] *//

		public static Vector4f operator*(Vector4f lhs, float rhs) {
			return new Vector4f(
				lhs.x * rhs,
				lhs.y * rhs,
				lhs.z * rhs,
				lhs.w * rhs
			);
		}

		public static Vector4f operator*(float lhs, Vector4f rhs) {
			return new Vector4f(
				lhs * rhs.x,
				lhs * rhs.y,
				lhs * rhs.z,
				lhs * rhs.w
			);
		}

		//* operator [divide] *//

		public static Vector4f operator/(Vector4f lhs, float rhs) {
			return new Vector4f(
				lhs.x / rhs,
				lhs.y / rhs,
				lhs.z / rhs,
				lhs.w / rhs
			);
		}

		//* operator [negate] *//

		public static Vector4f operator-(Vector4f v) {
			return new Vector4f(-v.x, -v.y, -v.z, -v.w);
		}

		//* operator [access] *//

		public float this[int index] {
			get {
				switch (index) {
					case 0: return x;
					case 1: return y;
					case 2: return z;
					case 3: return w;
					default: throw new System.IndexOutOfRangeException("[Vector4f] invalid index.");
				}
			}
			set {
				switch (index) {
					case 0: x = value; break;
					case 1: y = value; break;
					case 2: z = value; break;
					case 3: w = value; break;
					default: throw new System.IndexOutOfRangeException("[Vector4f] invalid index.");
				}
			}
		}

		//* string conversion *//

		public override string ToString() {
			return $"({x}, {y}, {z}, {w})";
		}

	}

}