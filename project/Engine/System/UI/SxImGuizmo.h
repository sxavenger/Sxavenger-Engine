#pragma once
/*
 # reference
 - https://github.com/CedricGuillemet/ImGuizmo //!< imguizmo
*/

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <imgui.h>

//* c++
#include <cstdint>
#include <cmath>
#include <optional>
#include <vector>
#include <array>
#include <bitset>

//-----------------------------------------------------------------------------------------
// environment
//-----------------------------------------------------------------------------------------
static_assert(_HAS_CXX20, "[SxImGuizmo] environment must c++20 over."); //!< c++20以上

#ifndef IMGUI_API
#define IMGUI_API
#endif

#define _SXIMGUIZMO_NAMESPACE_BEGIN namespace SxImGuizmo {
#define _SXIMGUIZMO_NAMESPACE_END }

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
struct ImGuiWindow;

////////////////////////////////////////////////////////////////////////////////////////////
// SxImGuizmo namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace SxImGuizmo {

	////////////////////////////////////////////////////////////////////////////////////////////
	// utility methods
	////////////////////////////////////////////////////////////////////////////////////////////

	void FPU_MatrixF_x_MatrixF(const float* a, const float* b, float* r);

	template <typename T> T Clamp(T x, T y, T z) { return ((x < y) ? y : ((x > z) ? z : x)); }
	template <typename T> T max(T x, T y) { return (x > y) ? x : y; }
	template <typename T> T min(T x, T y) { return (x < y) ? x : y; }
	template <typename T> bool IsWithin(T x, T y, T z) { return (x >= y) && (x <= z); }

	////////////////////////////////////////////////////////////////////////////////////////////
	// vec_t structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct matrix_t; //!< forward
	struct vec_t { //!< imguizmo
	public:
		float x, y, z, w;

		void Lerp(const vec_t& v, float t) {
			x += (v.x - x) * t;
			y += (v.y - y) * t;
			z += (v.z - z) * t;
			w += (v.w - w) * t;
		}

		void Set(float v) { x = y = z = w = v; }
		void Set(float _x, float _y, float _z = 0.f, float _w = 0.f) { x = _x; y = _y; z = _z; w = _w; }

		vec_t& operator -= (const vec_t& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
		vec_t& operator += (const vec_t& v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
		vec_t& operator *= (const vec_t& v) { x *= v.x; y *= v.y; z *= v.z; w *= v.w; return *this; }
		vec_t& operator *= (float v) { x *= v;    y *= v;    z *= v;    w *= v;    return *this; }

		vec_t operator * (float f) const { return { x * f, y * f, z * f, w * f }; }
		vec_t operator - () const { return { -x, -y, -z, -w }; }
		vec_t operator - (const vec_t& v) const { return { x - v.x, y - v.y, z - v.z, w - v.w }; }
		vec_t operator + (const vec_t& v) const { return { x + v.x, y + v.y, z + v.z, w + v.w }; }
		vec_t operator * (const vec_t& v) const { return { x * v.x, y * v.y, z * v.z, w * v.w }; }

		const vec_t& operator + () const { return (*this); }
		float Length() const { return std::sqrt(x * x + y * y + z * z); };
		float LengthSq() const { return (x * x + y * y + z * z); };
		vec_t Normalize() { (*this) *= (1.f / (Length() > FLT_EPSILON ? Length() : FLT_EPSILON)); return (*this); }
		vec_t Normalize(const vec_t& v) { this->Set(v.x, v.y, v.z, v.w); this->Normalize(); return (*this); }
		vec_t Abs() const { return { std::abs(x), std::abs(y), std::abs(z) }; }

		void Cross(const vec_t& v) {
			vec_t res;
			res.x = y * v.z - z * v.y;
			res.y = z * v.x - x * v.z;
			res.z = x * v.y - y * v.x;

			x = res.x;
			y = res.y;
			z = res.z;
			w = 0.f;
		}

		void Cross(const vec_t& v1, const vec_t& v2) {
			x = v1.y * v2.z - v1.z * v2.y;
			y = v1.z * v2.x - v1.x * v2.z;
			z = v1.x * v2.y - v1.y * v2.x;
			w = 0.f;
		}

		float Dot(const vec_t& v) const {
			return (x * v.x) + (y * v.y) + (z * v.z) + (w * v.w);
		}

		float Dot3(const vec_t& v) const {
			return (x * v.x) + (y * v.y) + (z * v.z);
		}

		void Transform(const matrix_t& matrix);
		void Transform(const vec_t& s, const matrix_t& matrix);

		void TransformVector(const matrix_t& matrix);
		void TransformPoint(const matrix_t& matrix);
		void TransformVector(const vec_t& v, const matrix_t& matrix) { (*this) = v; this->TransformVector(matrix); }
		void TransformPoint(const vec_t& v, const matrix_t& matrix) { (*this) = v; this->TransformPoint(matrix); }

		float& operator [] (size_t index) { return ((float*)&x)[index]; }
		const float& operator [] (size_t index) const { return ((float*)&x)[index]; }
		bool operator!=(const vec_t& other) const { return memcmp(this, &other, sizeof(vec_t)) != 0; }

		static vec_t makeVect(float _x, float _y, float _z = 0.f, float _w = 0.f) { vec_t res; res.x = _x; res.y = _y; res.z = _z; res.w = _w; return res; }
		static vec_t makeVect(ImVec2 v) { vec_t res; res.x = v.x; res.y = v.y; res.z = 0.f; res.w = 0.f; return res; }
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// matrix_t structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct matrix_t { //!< imguizmo
	public:

		union {
			float m[4][4];
			float m16[16];
			struct {
				vec_t right, up, dir, position;
			} v;
			vec_t component[4];
		};

		operator float* () { return m16; }
		operator const float* () const { return m16; }
		void Translation(float _x, float _y, float _z) { this->Translation({ _x, _y, _z }); }

		void Translation(const vec_t& vt) {
			v.right.Set(1.f, 0.f, 0.f, 0.f);
			v.up.Set(0.f, 1.f, 0.f, 0.f);
			v.dir.Set(0.f, 0.f, 1.f, 0.f);
			v.position.Set(vt.x, vt.y, vt.z, 1.f);
		}

		void Scale(float _x, float _y, float _z) {
			v.right.Set(_x, 0.f, 0.f, 0.f);
			v.up.Set(0.f, _y, 0.f, 0.f);
			v.dir.Set(0.f, 0.f, _z, 0.f);
			v.position.Set(0.f, 0.f, 0.f, 1.f);
		}
		void Scale(const vec_t& s) { Scale(s.x, s.y, s.z); }

		matrix_t& operator *= (const matrix_t& mat) {
			matrix_t tmpMat;
			tmpMat = *this;
			tmpMat.Multiply(mat);
			*this = tmpMat;
			return *this;
		}
		matrix_t operator * (const matrix_t& mat) const {
			matrix_t matT;
			matT.Multiply(*this, mat);
			return matT;
		}

		void Multiply(const matrix_t& matrix) {
			matrix_t tmp;
			tmp = *this;

			FPU_MatrixF_x_MatrixF((float*)&tmp, (float*)&matrix, (float*)this);
		}

		void Multiply(const matrix_t& m1, const matrix_t& m2) {
			FPU_MatrixF_x_MatrixF((float*)&m1, (float*)&m2, (float*)this);
		}

		float GetDeterminant() const {
			return m[0][0] * m[1][1] * m[2][2] + m[0][1] * m[1][2] * m[2][0] + m[0][2] * m[1][0] * m[2][1] -
				m[0][2] * m[1][1] * m[2][0] - m[0][1] * m[1][0] * m[2][2] - m[0][0] * m[1][2] * m[2][1];
		}

		float Inverse(const matrix_t& srcMatrix, bool affine = false);
		void SetToIdentity() {
			v.right.Set(1.f, 0.f, 0.f, 0.f);
			v.up.Set(0.f, 1.f, 0.f, 0.f);
			v.dir.Set(0.f, 0.f, 1.f, 0.f);
			v.position.Set(0.f, 0.f, 0.f, 1.f);
		}
		void Transpose() {
			matrix_t tmpm;
			for (int l = 0; l < 4; l++) {
				for (int c = 0; c < 4; c++) {
					tmpm.m[l][c] = m[c][l];
				}
			}
			(*this) = tmpm;
		}

		void RotationAxis(const vec_t& axis, float angle);

		void OrthoNormalize() {
			v.right.Normalize();
			v.up.Normalize();
			v.dir.Normalize();
		}
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Style structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Style {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// Color enum
		////////////////////////////////////////////////////////////////////////////////////////////
		enum Color : uint8_t {
			DIRECTION_X,      // directionColor[0]
			DIRECTION_Y,      // directionColor[1]
			DIRECTION_Z,      // directionColor[2]
			PLANE_X,          // planeColor[0]
			PLANE_Y,          // planeColor[1]
			PLANE_Z,          // planeColor[2]
			SELECTION,        // selectionColor
			INACTIVE,         // inactiveColor
			TRANSLATION_LINE, // translationLineColor
			SCALE_LINE,
			ROTATION_USING_BORDER,
			ROTATION_USING_FILL,
			HATCHED_AXIS_LINES,
			TEXT,
			TEXT_SHADOW,
			COUNT
		};

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		IMGUI_API Style();

		ImU32 GetColorU32(Color color) const;

		void ShowStyleEditor();

		//=========================================================================================
		// public variables
		//=========================================================================================

		float translationLineThickness;   // Thickness of lines for translation gizmo
		float translationLineArrowSize;   // Size of arrow at the end of lines for translation gizmo
		float rotationLineThickness;      // Thickness of lines for rotation gizmo
		float rotationOuterLineThickness; // Thickness of line surrounding the rotation gizmo
		float scaleLineThickness;         // Thickness of lines for scale gizmo
		float scaleLineCircleSize;        // Size of circle at the end of lines for scale gizmo
		float hatchedAxisLineThickness;   // Thickness of hatched axis lines
		float centerCircleSize;           // Size of circle at the center of the translate/scale gizmo

		ImVec4 colors[Color::COUNT];
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Operation enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum Operation : uint32_t {
		NONE             = 0,
		TRANSLATE_X      = (1u << 0),
		TRANSLATE_Y      = (1u << 1),
		TRANSLATE_Z      = (1u << 2),
		ROTATE_X         = (1u << 3),
		ROTATE_Y         = (1u << 4),
		ROTATE_Z         = (1u << 5),
		ROTATE_SCREEN    = (1u << 6),
		SCALE_X          = (1u << 7),
		SCALE_Y          = (1u << 8),
		SCALE_Z          = (1u << 9),
		BOUNDS           = (1u << 10),
		/*SCALE_XU         = (1u << 11),
		SCALE_YU         = (1u << 12),
		SCALE_ZU         = (1u << 13),*/
		
		TRANSLATE = TRANSLATE_X | TRANSLATE_Y | TRANSLATE_Z,
		ROTATE    = ROTATE_X | ROTATE_Y | ROTATE_Z | ROTATE_SCREEN,
		SCALE     = SCALE_X | SCALE_Y | SCALE_Z,
		//SCALEU    = SCALE_XU | SCALE_YU | SCALE_ZU, // universal
		//UNIVERSAL = TRANSLATE | ROTATE | SCALEU

		// univarsal削除するかも
	};

	bool Intersects(Operation lhs, Operation rhs);
	bool Contains(Operation lhs, Operation rhs);

	Operation operator|(Operation lhs, Operation rhs);

	////////////////////////////////////////////////////////////////////////////////////////////
	// MoveType enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum MoveType : uint8_t {
		MT_NONE,
		MT_MOVE_X,
		MT_MOVE_Y,
		MT_MOVE_Z,
		MT_MOVE_YZ,
		MT_MOVE_ZX,
		MT_MOVE_XY,
		MT_MOVE_SCREEN,
		MT_ROTATE_X,
		MT_ROTATE_Y,
		MT_ROTATE_Z,
		MT_ROTATE_SCREEN,
		MT_SCALE_X,
		MT_SCALE_Y,
		MT_SCALE_Z,
		MT_SCALE_XYZ
	};

	bool IsTranslateType(MoveType type);
	bool IsRotateType(MoveType type);
	bool IsScaleType(MoveType type);

	MoveType GetMoveType(Operation operation);

	////////////////////////////////////////////////////////////////////////////////////////////
	// Mode enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum Mode : uint32_t {
		WORLD,
		LOCAL
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Transform structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Transform {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//=========================================================================================
		// public variables
		//=========================================================================================

		vec_t translation;
		vec_t rotate;
		vec_t scale;

		// euler || quaterion

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Context structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Context {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		bool IsHoveringWindow();

		float GetSegmentLengthClipSpace(const vec_t& start, const vec_t& end, const bool localCoordinates = false);

		ImVec2 WorldToPos(const vec_t& worldPos, const matrix_t& mat) const;

		void ComputeCameraRay();

		void ComputeContext(const float* _view, const float* _proj, float* _matrix, Mode _mode);

		void ComputeColors(std::array<ImU32, 7>& colors, MoveType type, Operation operation);

		ImGuiID GetCurrentId();

		float GetParallelogram(const vec_t& ptO, const vec_t& ptA, const vec_t& ptB);

		void ComputeTripodAxisAndVisibility(const int32_t axisIndex, vec_t& dirAxis, vec_t& dirPlaneX, vec_t& dirPlaneY, bool& belowAxisLimit, bool& belowPlaneLimit, const bool localCoordinates = false);

		static float IntersectRayPlane(const vec_t& origin, const vec_t& dir, const vec_t& plan);

		static vec_t PointOnSegment(const vec_t& point, const vec_t& vertPos1, const vec_t& vertPos2);

		float ComputeAngleOnPlan();

		MoveType GetMoveType(Operation op);

		MoveType GetScaleType(Operation op);

		MoveType GetRotationType(Operation op);

		//* manipulate option *//

		// todo: deltaの追加, matrixではなく, vector || quaterion でやる

		bool HandleTranslation(const float* matrix, Operation op, MoveType& type);

		bool HandleScale(Operation op, MoveType& type);

		bool HandleRotation(Operation op, MoveType& type);

		//* draw option *//

		void DrawRotationGizmo(Operation op, MoveType type);

		void DrawHatchedAxis(const vec_t& axis);
		void DrawTranslationGizmo(Operation op, MoveType type);

		void DrawScaleGizmo(Operation op, MoveType type);

		//=========================================================================================
		// public variables
		//=========================================================================================

		ImDrawList* drawList;
		Style style;

		Mode mode;
		matrix_t view;
		matrix_t proj;

		//* 多分objectのworld matrix
		matrix_t model;
		matrix_t modelLocal; // orthonormalized model
		matrix_t modelInverse;
		matrix_t modelSource;
		matrix_t modelSourceInverse;
		matrix_t mvp;
		matrix_t mvpLocal; // MVP with full model matrix whereas mMVP's model matrix might only be translation in case of World space edition
		matrix_t viewProj;

		//Transform source;
		//Transform delta;

		// my variables
		ImVec2 prevMousePos;

		vec_t modelScaleOrigin;

		vec_t cameraEye;
		vec_t cameraRight;
		vec_t cameraDir;
		vec_t cameraUp;

		vec_t rayOrigin;
		vec_t rayDir;

		float screenFactor;
		vec_t relativeOrigin;

		float radiusSquareCenter;
		ImVec2 screenSquareCenter;
		ImVec2 screenSquareMin;
		ImVec2 screenSquareMax;

		bool isUsing;
		bool isUsingViewManipulate;
		bool isEnable;
		bool isMouseOver;
		bool isReversed; // reversed projection matrix
		bool isViewManipulatorHovered;

		// translation
		vec_t translationPlan;
		vec_t translationPlanOrigin;
		vec_t matrixOrigin;
		vec_t translationLastDelta; //!< deleted

		// rotation
		vec_t rotationVectorSource;
		float rotationAngle;
		float rotationAngleOrigin;

		// scale
		vec_t scale;
		vec_t scaleValueOrigin;
		vec_t scaleLast;
		ImVec2 saveMousePos;

		bool isUsingBounds;

		int32_t axisMask = 0;
		std::array<float, 3> axisFactor;
		std::bitset<3> belowAxisLimits;
		std::bitset<3> belowPlaneLimits;

		float axisLimit  = 0.0025f;
		float planeLimit = 0.02f;

		ImVec2 position; //!< x & y
		ImVec2 size;     //!< width & height
		ImVec2 max;      //!< maxX & maxY
		float displayRatio;

		bool isOrthographic = false;
		// check to not have multiple gizmo highlighted at the same time
		bool isOverGizmoHotspot = false;

		MoveType currentOperation;

		ImGuiWindow* alternativeWindow = nullptr;
		std::vector<ImGuiID> idStack;
		std::optional<ImGuiID> editingId   = std::nullopt;
		std::optional<Operation> operation = std::nullopt;

		bool isAllowAxisFlip     = true;
		float gizmoSizeClipSpace = 0.1f;

	private:

		//=========================================================================================
		// private methods
		//=========================================================================================

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// methods
	////////////////////////////////////////////////////////////////////////////////////////////

	IMGUI_API void SetDrawlist(ImDrawList* drawlist = nullptr);

	IMGUI_API void BeginFrame();

	IMGUI_API void SetRect(const ImVec2& position, const ImVec2& size);

	IMGUI_API void Enable(bool enable);

	IMGUI_API bool IsUsing();

	// todo: matrix -> objectのsrtに変更
	IMGUI_API bool Manipulate(const float* view, const float* proj, float* matrix, Operation operation, Mode mode);

	IMGUI_API Style& GetStyle();

	IMGUI_API void ShowStyleEditor();

}
