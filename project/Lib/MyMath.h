//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Geometry/MathLib.h>

//* c++
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

Vector3f CatmullRomInterpolation(const Vector3f& p0, const Vector3f& p1, const Vector3f& p2, const Vector3f& p3, float t);
Vector3f CatmullRomPosition(const std::vector<Vector3f>& points, float t);

Vector3f CalculateEuler(const Vector3f& direction);