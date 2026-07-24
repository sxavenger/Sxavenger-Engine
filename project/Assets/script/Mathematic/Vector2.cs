////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger - Mathematic namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Sxavenger.Mathematic {

	//-----------------------------------------------------------------------------------------
	// using
	//-----------------------------------------------------------------------------------------
	using System.Runtime.InteropServices;

	////////////////////////////////////////////////////////////////////////////////////////////
	// Vector2 [floating-point] structure
	////////////////////////////////////////////////////////////////////////////////////////////
	[StructLayout(LayoutKind.Sequential)]
	public struct Vector2f {

		//=========================================================================================
		// public variables
		//=========================================================================================

		public float x, y;

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* constructor *//

		public Vector2f(float x, float y) {
			this.x = x;
			this.y = y;
		}

		//* operator [add] *//

		public static Vector2f operator+(Vector2f lhs, Vector2f rhs) {
			return new Vector2f(
				lhs.x + rhs.x,
				lhs.y + rhs.y
			);
		}

		//* operator [subtract] *//

		public static Vector2f operator-(Vector2f lhs, Vector2f rhs) {
			return new Vector2f(
				lhs.x - rhs.x,
				lhs.y - rhs.y
			);
		}

		//* operator [multiply] *//

		public static Vector2f operator *(Vector2f lhs, float rhs) {
			return new Vector2f(
				lhs.x * rhs,
				lhs.y * rhs
			);
		}

		public static Vector2f operator *(float lhs, Vector2f rhs) {
			return new Vector2f(
				lhs * rhs.x,
				lhs * rhs.y
			);
		}

		//* operator [divide] *//

		public static Vector2f operator /(Vector2f lhs, float rhs) {
			return new Vector2f(
				lhs.x / rhs,
				lhs.y / rhs
			);
		}

		//* operator [negate] *//

		public static Vector2f operator -(Vector2f v) {
			return new Vector2f(-v.x, -v.y);
		}

		//* operator [access] *//

		public float this[int index] {
			get {
				switch (index) {
					case 0: return x;
					case 1: return y;
					default: throw new System.IndexOutOfRangeException("[Vector2f] invalid index.");
				}
			}
			set {
				switch (index) {
					case 0: x = value; break;
					case 1: y = value; break;
					default: throw new System.IndexOutOfRangeException("[Vector2f] invalid index.");
				}
			}
		}

		//* string conversion *//

		public override string ToString() {
			return $"({x}, {y})";
		}

	}
}