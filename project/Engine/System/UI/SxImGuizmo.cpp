#include "SxImGuizmo.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#include <imgui.h>
#include <imgui_internal.h>

//* c++
#include <numbers>
#include <string>

//* helper environment(配布時にこれは消す)

_SXIMGUIZMO_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// SxImGuizmo using
////////////////////////////////////////////////////////////////////////////////////////////

using OperationType = std::underlying_type_t<Operation>;

////////////////////////////////////////////////////////////////////////////////////////////
// SxImGuizmo intermediate variables
////////////////////////////////////////////////////////////////////////////////////////////

static const float kPi = std::numbers::pi_v<float>;

static const float kScreenRotateSize = 0.06f;
static const uint32_t kHalfCircleSegmentCount = 64;
const float kRotationDisplayFactor = 1.2f;

static const SxImGuizmo::vec_t kDirectionUnary[3] = { { 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 1.f } };

static const float kQuadMin = 0.5f;
static const float kQuadMax = 0.8f;
static const float kQuadUV[8] = { kQuadMin, kQuadMin, kQuadMin, kQuadMax, kQuadMax, kQuadMax, kQuadMax, kQuadMin };

static const char* kTranslationInfoMask[] = {
	"X : %5.3f", "Y : %5.3f", "Z : %5.3f",                               //!< axis
	"Y : %5.3f Z : %5.3f", "X : %5.3f Z : %5.3f", "X : %5.3f Y : %5.3f", //!< plane
	"X : %5.3f Y : %5.3f Z : %5.3f"                                      //!< circle
};

static const char* kRotationInfoMask[] = {
	"X : %5.2f deg %5.2f rad",
	"Y : %5.2f deg %5.2f rad",
	"Z : %5.2f deg %5.2f rad",
	"Screen : %5.2f deg %5.2f rad"
};

static const char* kScaleInfoMask[] = {
	"X : %5.2f",
	"Y : %5.2f",
	"Z : %5.2f",
	"XYZ : %5.2f"
};

static const int32_t kTranslationInfoIndex[] = { 0,0,0, 1,0,0, 2,0,0, 1,2,0, 0,2,0, 0,1,0, 0,1,2 };

static const OperationType kTranslatePlanes[3] = { TRANSLATE_Y | TRANSLATE_Z, TRANSLATE_X | TRANSLATE_Z, TRANSLATE_X | TRANSLATE_Y };

////////////////////////////////////////////////////////////////////////////////////////////
// utility methods
////////////////////////////////////////////////////////////////////////////////////////////

void FPU_MatrixF_x_MatrixF(const float* a, const float* b, float* r) {
	r[0] = a[0] * b[0] + a[1] * b[4] + a[2] * b[8] + a[3] * b[12];
	r[1] = a[0] * b[1] + a[1] * b[5] + a[2] * b[9] + a[3] * b[13];
	r[2] = a[0] * b[2] + a[1] * b[6] + a[2] * b[10] + a[3] * b[14];
	r[3] = a[0] * b[3] + a[1] * b[7] + a[2] * b[11] + a[3] * b[15];

	r[4] = a[4] * b[0] + a[5] * b[4] + a[6] * b[8] + a[7] * b[12];
	r[5] = a[4] * b[1] + a[5] * b[5] + a[6] * b[9] + a[7] * b[13];
	r[6] = a[4] * b[2] + a[5] * b[6] + a[6] * b[10] + a[7] * b[14];
	r[7] = a[4] * b[3] + a[5] * b[7] + a[6] * b[11] + a[7] * b[15];

	r[8] = a[8] * b[0] + a[9] * b[4] + a[10] * b[8] + a[11] * b[12];
	r[9] = a[8] * b[1] + a[9] * b[5] + a[10] * b[9] + a[11] * b[13];
	r[10] = a[8] * b[2] + a[9] * b[6] + a[10] * b[10] + a[11] * b[14];
	r[11] = a[8] * b[3] + a[9] * b[7] + a[10] * b[11] + a[11] * b[15];

	r[12] = a[12] * b[0] + a[13] * b[4] + a[14] * b[8] + a[15] * b[12];
	r[13] = a[12] * b[1] + a[13] * b[5] + a[14] * b[9] + a[15] * b[13];
	r[14] = a[12] * b[2] + a[13] * b[6] + a[14] * b[10] + a[15] * b[14];
	r[15] = a[12] * b[3] + a[13] * b[7] + a[14] * b[11] + a[15] * b[15];
}

static bool CanActivate() {
	if (ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive()) {
		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
// vec_t structure methods
////////////////////////////////////////////////////////////////////////////////////////////

vec_t Normalized(const vec_t& v) { vec_t res; res = v; res.Normalize(); return res; }
vec_t Cross(const vec_t& v1, const vec_t& v2) {
	vec_t res;
	res.x = v1.y * v2.z - v1.z * v2.y;
	res.y = v1.z * v2.x - v1.x * v2.z;
	res.z = v1.x * v2.y - v1.y * v2.x;
	res.w = 0.f;
	return res;
}

float Dot(const vec_t& v1, const vec_t& v2) {
	return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

vec_t BuildPlan(const vec_t& p_point1, const vec_t& p_normal) {
	vec_t normal, res;
	normal.Normalize(p_normal);
	res.w = normal.Dot(p_point1);
	res.x = normal.x;
	res.y = normal.y;
	res.z = normal.z;
	return res;
}

////////////////////////////////////////////////////////////////////////////////////////////
// matrix_t structure
////////////////////////////////////////////////////////////////////////////////////////////

void vec_t::Transform(const matrix_t& matrix) {
	vec_t out;

	out.x = x * matrix.m[0][0] + y * matrix.m[1][0] + z * matrix.m[2][0] + w * matrix.m[3][0];
	out.y = x * matrix.m[0][1] + y * matrix.m[1][1] + z * matrix.m[2][1] + w * matrix.m[3][1];
	out.z = x * matrix.m[0][2] + y * matrix.m[1][2] + z * matrix.m[2][2] + w * matrix.m[3][2];
	out.w = x * matrix.m[0][3] + y * matrix.m[1][3] + z * matrix.m[2][3] + w * matrix.m[3][3];

	x = out.x;
	y = out.y;
	z = out.z;
	w = out.w;
}

void vec_t::Transform(const vec_t& s, const matrix_t& matrix) {
	*this = s;
	Transform(matrix);
}

void vec_t::TransformPoint(const matrix_t& matrix) {
	vec_t out;

	out.x = x * matrix.m[0][0] + y * matrix.m[1][0] + z * matrix.m[2][0] + matrix.m[3][0];
	out.y = x * matrix.m[0][1] + y * matrix.m[1][1] + z * matrix.m[2][1] + matrix.m[3][1];
	out.z = x * matrix.m[0][2] + y * matrix.m[1][2] + z * matrix.m[2][2] + matrix.m[3][2];
	out.w = x * matrix.m[0][3] + y * matrix.m[1][3] + z * matrix.m[2][3] + matrix.m[3][3];

	x = out.x;
	y = out.y;
	z = out.z;
	w = out.w;
}

void vec_t::TransformVector(const matrix_t& matrix) {
	vec_t out;

	out.x = x * matrix.m[0][0] + y * matrix.m[1][0] + z * matrix.m[2][0];
	out.y = x * matrix.m[0][1] + y * matrix.m[1][1] + z * matrix.m[2][1];
	out.z = x * matrix.m[0][2] + y * matrix.m[1][2] + z * matrix.m[2][2];
	out.w = x * matrix.m[0][3] + y * matrix.m[1][3] + z * matrix.m[2][3];

	x = out.x;
	y = out.y;
	z = out.z;
	w = out.w;
}

vec_t AxisAngleQuat(const vec_t& axis, float angle) {
	vec_t i = axis * std::sin(angle * 0.5f);
	return { i.x, i.y, i.z, std::cos(angle * 0.5f) };
}

vec_t InverseQuat(const vec_t& quat) {
	vec_t conj = { -quat.x, -quat.y, -quat.z, quat.w };
	float norm2 = quat.Dot(quat);

	return { conj.x / norm2, conj.y / norm2, conj.z / norm2, conj.w / norm2 };
}

float matrix_t::Inverse(const matrix_t& srcMatrix, bool affine) {
	float det = 0;

	if (affine) {
		det = GetDeterminant();
		float s = 1 / det;
		m[0][0] = (srcMatrix.m[1][1] * srcMatrix.m[2][2] - srcMatrix.m[1][2] * srcMatrix.m[2][1]) * s;
		m[0][1] = (srcMatrix.m[2][1] * srcMatrix.m[0][2] - srcMatrix.m[2][2] * srcMatrix.m[0][1]) * s;
		m[0][2] = (srcMatrix.m[0][1] * srcMatrix.m[1][2] - srcMatrix.m[0][2] * srcMatrix.m[1][1]) * s;
		m[1][0] = (srcMatrix.m[1][2] * srcMatrix.m[2][0] - srcMatrix.m[1][0] * srcMatrix.m[2][2]) * s;
		m[1][1] = (srcMatrix.m[2][2] * srcMatrix.m[0][0] - srcMatrix.m[2][0] * srcMatrix.m[0][2]) * s;
		m[1][2] = (srcMatrix.m[0][2] * srcMatrix.m[1][0] - srcMatrix.m[0][0] * srcMatrix.m[1][2]) * s;
		m[2][0] = (srcMatrix.m[1][0] * srcMatrix.m[2][1] - srcMatrix.m[1][1] * srcMatrix.m[2][0]) * s;
		m[2][1] = (srcMatrix.m[2][0] * srcMatrix.m[0][1] - srcMatrix.m[2][1] * srcMatrix.m[0][0]) * s;
		m[2][2] = (srcMatrix.m[0][0] * srcMatrix.m[1][1] - srcMatrix.m[0][1] * srcMatrix.m[1][0]) * s;
		m[3][0] = -(m[0][0] * srcMatrix.m[3][0] + m[1][0] * srcMatrix.m[3][1] + m[2][0] * srcMatrix.m[3][2]);
		m[3][1] = -(m[0][1] * srcMatrix.m[3][0] + m[1][1] * srcMatrix.m[3][1] + m[2][1] * srcMatrix.m[3][2]);
		m[3][2] = -(m[0][2] * srcMatrix.m[3][0] + m[1][2] * srcMatrix.m[3][1] + m[2][2] * srcMatrix.m[3][2]);
	} else {
		// transpose matrix
		float src[16];
		for (int i = 0; i < 4; ++i) {
			src[i] = srcMatrix.m16[i * 4];
			src[i + 4] = srcMatrix.m16[i * 4 + 1];
			src[i + 8] = srcMatrix.m16[i * 4 + 2];
			src[i + 12] = srcMatrix.m16[i * 4 + 3];
		}

		// calculate pairs for first 8 elements (cofactors)
		float tmp[12]; // temp array for pairs
		tmp[0] = src[10] * src[15];
		tmp[1] = src[11] * src[14];
		tmp[2] = src[9] * src[15];
		tmp[3] = src[11] * src[13];
		tmp[4] = src[9] * src[14];
		tmp[5] = src[10] * src[13];
		tmp[6] = src[8] * src[15];
		tmp[7] = src[11] * src[12];
		tmp[8] = src[8] * src[14];
		tmp[9] = src[10] * src[12];
		tmp[10] = src[8] * src[13];
		tmp[11] = src[9] * src[12];

		// calculate first 8 elements (cofactors)
		m16[0] = (tmp[0] * src[5] + tmp[3] * src[6] + tmp[4] * src[7]) - (tmp[1] * src[5] + tmp[2] * src[6] + tmp[5] * src[7]);
		m16[1] = (tmp[1] * src[4] + tmp[6] * src[6] + tmp[9] * src[7]) - (tmp[0] * src[4] + tmp[7] * src[6] + tmp[8] * src[7]);
		m16[2] = (tmp[2] * src[4] + tmp[7] * src[5] + tmp[10] * src[7]) - (tmp[3] * src[4] + tmp[6] * src[5] + tmp[11] * src[7]);
		m16[3] = (tmp[5] * src[4] + tmp[8] * src[5] + tmp[11] * src[6]) - (tmp[4] * src[4] + tmp[9] * src[5] + tmp[10] * src[6]);
		m16[4] = (tmp[1] * src[1] + tmp[2] * src[2] + tmp[5] * src[3]) - (tmp[0] * src[1] + tmp[3] * src[2] + tmp[4] * src[3]);
		m16[5] = (tmp[0] * src[0] + tmp[7] * src[2] + tmp[8] * src[3]) - (tmp[1] * src[0] + tmp[6] * src[2] + tmp[9] * src[3]);
		m16[6] = (tmp[3] * src[0] + tmp[6] * src[1] + tmp[11] * src[3]) - (tmp[2] * src[0] + tmp[7] * src[1] + tmp[10] * src[3]);
		m16[7] = (tmp[4] * src[0] + tmp[9] * src[1] + tmp[10] * src[2]) - (tmp[5] * src[0] + tmp[8] * src[1] + tmp[11] * src[2]);

		// calculate pairs for second 8 elements (cofactors)
		tmp[0] = src[2] * src[7];
		tmp[1] = src[3] * src[6];
		tmp[2] = src[1] * src[7];
		tmp[3] = src[3] * src[5];
		tmp[4] = src[1] * src[6];
		tmp[5] = src[2] * src[5];
		tmp[6] = src[0] * src[7];
		tmp[7] = src[3] * src[4];
		tmp[8] = src[0] * src[6];
		tmp[9] = src[2] * src[4];
		tmp[10] = src[0] * src[5];
		tmp[11] = src[1] * src[4];

		// calculate second 8 elements (cofactors)
		m16[8] = (tmp[0] * src[13] + tmp[3] * src[14] + tmp[4] * src[15]) - (tmp[1] * src[13] + tmp[2] * src[14] + tmp[5] * src[15]);
		m16[9] = (tmp[1] * src[12] + tmp[6] * src[14] + tmp[9] * src[15]) - (tmp[0] * src[12] + tmp[7] * src[14] + tmp[8] * src[15]);
		m16[10] = (tmp[2] * src[12] + tmp[7] * src[13] + tmp[10] * src[15]) - (tmp[3] * src[12] + tmp[6] * src[13] + tmp[11] * src[15]);
		m16[11] = (tmp[5] * src[12] + tmp[8] * src[13] + tmp[11] * src[14]) - (tmp[4] * src[12] + tmp[9] * src[13] + tmp[10] * src[14]);
		m16[12] = (tmp[2] * src[10] + tmp[5] * src[11] + tmp[1] * src[9]) - (tmp[4] * src[11] + tmp[0] * src[9] + tmp[3] * src[10]);
		m16[13] = (tmp[8] * src[11] + tmp[0] * src[8] + tmp[7] * src[10]) - (tmp[6] * src[10] + tmp[9] * src[11] + tmp[1] * src[8]);
		m16[14] = (tmp[6] * src[9] + tmp[11] * src[11] + tmp[3] * src[8]) - (tmp[10] * src[11] + tmp[2] * src[8] + tmp[7] * src[9]);
		m16[15] = (tmp[10] * src[10] + tmp[4] * src[8] + tmp[9] * src[9]) - (tmp[8] * src[9] + tmp[11] * src[10] + tmp[5] * src[8]);

		// calculate determinant
		det = src[0] * m16[0] + src[1] * m16[1] + src[2] * m16[2] + src[3] * m16[3];

		// calculate matrix inverse
		float invdet = 1 / det;
		for (int j = 0; j < 16; ++j) {
			m16[j] *= invdet;
		}
	}

	return det;
}

void matrix_t::RotationAxis(const vec_t& axis, float angle) {
	float length2 = axis.LengthSq();
	if (length2 < FLT_EPSILON) {
		SetToIdentity();
		return;
	}

	vec_t n = axis * (1.f / std::sqrt(length2));
	float s = std::sin(angle);
	float c = std::cos(angle);
	float k = 1.f - c;

	float xx = n.x * n.x * k + c;
	float yy = n.y * n.y * k + c;
	float zz = n.z * n.z * k + c;
	float xy = n.x * n.y * k;
	float yz = n.y * n.z * k;
	float zx = n.z * n.x * k;
	float xs = n.x * s;
	float ys = n.y * s;
	float zs = n.z * s;

	m[0][0] = xx;
	m[0][1] = xy + zs;
	m[0][2] = zx - ys;
	m[0][3] = 0.f;
	m[1][0] = xy - zs;
	m[1][1] = yy;
	m[1][2] = yz + xs;
	m[1][3] = 0.f;
	m[2][0] = zx + ys;
	m[2][1] = yz - xs;
	m[2][2] = zz;
	m[2][3] = 0.f;
	m[3][0] = 0.f;
	m[3][1] = 0.f;
	m[3][2] = 0.f;
	m[3][3] = 1.f;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Style structure methods
////////////////////////////////////////////////////////////////////////////////////////////

Style::Style() {
	// default values
	translationLineThickness   = 3.0f;
	translationLineArrowSize   = 6.0f;
	rotationLineThickness      = 2.0f;
	rotationOuterLineThickness = 3.0f;
	scaleLineThickness         = 3.0f;
	scaleLineCircleSize        = 6.0f;
	hatchedAxisLineThickness   = 6.0f;
	centerCircleSize           = 6.0f;

	// initialize default colors
	colors[DIRECTION_X]           = ImVec4(0.666f, 0.000f, 0.000f, 1.000f);
	colors[DIRECTION_Y]           = ImVec4(0.000f, 0.666f, 0.000f, 1.000f);
	colors[DIRECTION_Z]           = ImVec4(0.000f, 0.000f, 0.666f, 1.000f);
	colors[PLANE_X]               = ImVec4(0.666f, 0.000f, 0.000f, 0.380f);
	colors[PLANE_Y]               = ImVec4(0.000f, 0.666f, 0.000f, 0.380f);
	colors[PLANE_Z]               = ImVec4(0.000f, 0.000f, 0.666f, 0.380f);
	colors[SELECTION]             = ImVec4(1.000f, 0.500f, 0.062f, 0.541f);
	colors[INACTIVE]              = ImVec4(0.600f, 0.600f, 0.600f, 0.600f);
	colors[TRANSLATION_LINE]      = ImVec4(0.666f, 0.666f, 0.666f, 0.666f);
	colors[SCALE_LINE]            = ImVec4(0.250f, 0.250f, 0.250f, 1.000f);
	colors[ROTATION_USING_BORDER] = ImVec4(1.000f, 0.500f, 0.062f, 1.000f);
	colors[ROTATION_USING_FILL]   = ImVec4(1.000f, 0.500f, 0.062f, 0.500f);
	colors[HATCHED_AXIS_LINES]    = ImVec4(0.000f, 0.000f, 0.000f, 0.500f);
	colors[TEXT]                  = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
	colors[TEXT_SHADOW]           = ImVec4(0.000f, 0.000f, 0.000f, 1.000f);
}

ImU32 Style::GetColorU32(Color color) const {
	IM_ASSERT(color < Color::COUNT);
	return ImGui::ColorConvertFloat4ToU32(colors[color]);
}

void SxImGuizmo::Style::ShowStyleEditor() {
	ImGui::SeparatorText("SxImGuizmo Style Editor");

	ImGuiTabBarFlags flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("SxImGuizmo Style Tab", flags)) {

		//* sizes *//
		if (ImGui::BeginTabItem("sizes")) {

			ImGui::SeparatorText("translation");
			ImGui::DragFloat("translation line thickness",  &translationLineThickness, 0.01f);
			ImGui::DragFloat("translation line arrow size", &translationLineArrowSize, 0.01f);

			ImGui::SeparatorText("rotation");
			ImGui::DragFloat("rotation line thickness",       &rotationLineThickness, 0.01f);
			ImGui::DragFloat("rotation outer line thickness", &rotationOuterLineThickness, 0.01f);

			ImGui::SeparatorText("scale");
			ImGui::DragFloat("scale line thickness",   &scaleLineThickness, 0.01f);
			ImGui::DragFloat("scale line circle size", &scaleLineCircleSize, 0.01f);

			ImGui::SeparatorText("other");
			ImGui::DragFloat("hatched axis line thickness", &hatchedAxisLineThickness, 0.01f);
			ImGui::DragFloat("center circle size",          &centerCircleSize, 0.01f);

			ImGui::EndTabItem();
		}

		//* colors *//
		if (ImGui::BeginTabItem("colors")) {

			// TODO: 
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}


}

////////////////////////////////////////////////////////////////////////////////////////////
// Operation enum methods
////////////////////////////////////////////////////////////////////////////////////////////

bool Intersects(Operation lhs, Operation rhs) { return (lhs & rhs) != 0; }
bool Contains(Operation lhs, Operation rhs) { return (lhs & rhs) == rhs; }

Operation operator|(Operation lhs, Operation rhs) { return static_cast<Operation>(static_cast<OperationType>(lhs) | static_cast<OperationType>(rhs)); }

////////////////////////////////////////////////////////////////////////////////////////////
// MoveType enum methods
////////////////////////////////////////////////////////////////////////////////////////////

bool IsTranslateType(MoveType type) { return type >= MT_MOVE_X && type <= MT_MOVE_SCREEN; }
bool IsRotateType(MoveType type) { return type >= MT_ROTATE_X && type <= MT_ROTATE_SCREEN; }
bool IsScaleType(MoveType type) { return type >= MT_SCALE_X && type <= MT_SCALE_XYZ; }

////////////////////////////////////////////////////////////////////////////////////////////
// Context structure methods
////////////////////////////////////////////////////////////////////////////////////////////

bool Context::IsHoveringWindow() {
	ImGuiContext& g     = *ImGui::GetCurrentContext();
	ImGuiWindow* window = ImGui::FindWindowByName(drawList->_OwnerName);
	if (g.HoveredWindow == window)   // Mouse hovering drawlist window
		return true;
	if (alternativeWindow != nullptr && g.HoveredWindow == alternativeWindow)
		return true;
	if (g.HoveredWindow != NULL)     // Any other window is hovered
		return false;
	if (ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max, false))   // Hovering drawlist window rect, while no other window is hovered (for _NoInputs windows)
		return true;
	return false;
}

float Context::GetSegmentLengthClipSpace(const vec_t& start, const vec_t& end, const bool localCoordinates) {
	vec_t startOfSegment = start;
	const matrix_t& mvpMat = localCoordinates ? mvpLocal : mvp;
	startOfSegment.TransformPoint(mvpMat);
	if (fabsf(startOfSegment.w) > FLT_EPSILON) // check for axis aligned with camera direction
	{
		startOfSegment *= 1.f / startOfSegment.w;
	}

	vec_t endOfSegment = end;
	endOfSegment.TransformPoint(mvpMat);
	if (fabsf(endOfSegment.w) > FLT_EPSILON) // check for axis aligned with camera direction
	{
		endOfSegment *= 1.f / endOfSegment.w;
	}

	vec_t clipSpaceAxis = endOfSegment - startOfSegment;
	if (displayRatio < 1.0)
		clipSpaceAxis.x *= displayRatio;
	else
		clipSpaceAxis.y /= displayRatio;
	float segmentLengthInClipSpace = sqrtf(clipSpaceAxis.x * clipSpaceAxis.x + clipSpaceAxis.y * clipSpaceAxis.y);
	return segmentLengthInClipSpace;
}

ImVec2 Context::WorldToPos(const vec_t& worldPos, const matrix_t& mat) const {
	vec_t trans;
	trans.TransformPoint(worldPos, mat);
	trans *= 0.5f / trans.w;
	trans += vec_t::makeVect(0.5f, 0.5f);
	trans.y = 1.f - trans.y;
	trans.x *= size.x;
	trans.y *= size.y;
	trans.x += position.x;
	trans.y += position.y;
	return ImVec2(trans.x, trans.y);
}

void Context::ComputeCameraRay() {
	ImGuiIO& io = ImGui::GetIO();

	matrix_t mViewProjInverse;
	mViewProjInverse.Inverse(viewProj);

	const float mox = ((io.MousePos.x - position.x) / size.x) * 2.f - 1.f;
	const float moy = (1.f - ((io.MousePos.y - position.y) / size.y)) * 2.f - 1.f;

	const float zNear = isReversed ? (1.f - FLT_EPSILON) : 0.f;
	const float zFar = isReversed ? 0.f : (1.f - FLT_EPSILON);

	rayOrigin.Transform(vec_t::makeVect(mox, moy, zNear, 1.f), mViewProjInverse);
	rayOrigin *= 1.f / rayOrigin.w;
	vec_t rayEnd;
	rayEnd.Transform(vec_t::makeVect(mox, moy, zFar, 1.f), mViewProjInverse);
	rayEnd *= 1.f / rayEnd.w;
	rayDir = Normalized(rayEnd - rayOrigin);
}

void Context::ComputeContext(const float* _view, const float* _proj, const float* _matrix, Mode _mode) {
	mode = _mode;
	view = *(matrix_t*)_view;
	proj = *(matrix_t*)_proj;
	isMouseOver = IsHoveringWindow();

	modelLocal = *(matrix_t*)_matrix;
	modelLocal.OrthoNormalize();

	if (mode == Mode::Local) {
		model = modelLocal;

	} else {
		model.Translation(((matrix_t*)_matrix)->v.position);
	}

	modelSource = *(matrix_t*)_matrix;
	modelScaleOrigin.Set(modelSource.v.right.Length(), modelSource.v.up.Length(), modelSource.v.dir.Length());

	modelInverse.Inverse(model);
	modelSourceInverse.Inverse(modelSource);
	viewProj = view * proj;
	mvp      = model * viewProj;
	mvpLocal = modelLocal * viewProj;

	matrix_t viewInverse;
	viewInverse.Inverse(view);
	cameraDir   = viewInverse.v.dir;
	cameraEye   = viewInverse.v.position;
	cameraRight = viewInverse.v.right;
	cameraUp    = viewInverse.v.up;

	// projection reverse
	vec_t nearPos, farPos;
	nearPos.Transform(vec_t::makeVect(0, 0, 1.f, 1.f), proj);
	farPos.Transform(vec_t::makeVect(0, 0, 2.f, 1.f), proj);
	isReversed = (nearPos.z / nearPos.w) > (farPos.z / farPos.w);

	// compute scale from the size of camera right vector projected on screen at the matrix position
	vec_t pointRight = viewInverse.v.right;
	pointRight.TransformPoint(viewProj);

	vec_t rightViewInverse = viewInverse.v.right;
	rightViewInverse.TransformVector(modelInverse);
	float rightLength = GetSegmentLengthClipSpace(vec_t::makeVect(0.f, 0.f), rightViewInverse);
	screenFactor = gizmoSizeClipSpace / rightLength;

	ImVec2 centerSSpace = WorldToPos(vec_t::makeVect(0.f, 0.f), mvp);
	screenSquareCenter = centerSSpace;
	screenSquareMin = ImVec2(centerSSpace.x - 10.f, centerSSpace.y - 10.f);
	screenSquareMax = ImVec2(centerSSpace.x + 10.f, centerSSpace.y + 10.f);

	ComputeCameraRay();
}

void Context::ComputeColors(std::array<ImU32, 7>& colors, MoveType type, Operation _operation) {
	if (isEnable) {
		ImU32 selectionColor = style.GetColorU32(Style::SELECTION);

		switch (_operation) {
			case TRANSLATE:
				colors[0] = (type == MT_MOVE_SCREEN) ? selectionColor : IM_COL32_WHITE;
				for (int32_t i = 0; i < 3; i++) {
					colors[i + 1] = (type == (int32_t)(MT_MOVE_X + i)) ? selectionColor : style.GetColorU32(static_cast<Style::Color>(Style::DIRECTION_X + i));
					colors[i + 4] = (type == (int32_t)(MT_MOVE_YZ + i)) ? selectionColor : style.GetColorU32(static_cast<Style::Color>(Style::PLANE_X + i));
					colors[i + 4] = (type == MT_MOVE_SCREEN) ? selectionColor : colors[i + 4];
				}
				break;

			case ROTATE:
				colors[0] = (type == MT_ROTATE_SCREEN) ? selectionColor : IM_COL32_WHITE;
				for (int32_t i = 0; i < 3; i++) {
					colors[i + 1] = (type == (int32_t)(MT_ROTATE_X + i)) ? selectionColor : style.GetColorU32(static_cast<Style::Color>(Style::DIRECTION_X + i));
				}
				break;

			case SCALE:
				colors[0] = (type == MT_SCALE_XYZ) ? selectionColor : IM_COL32_WHITE;
				for (int32_t i = 0; i < 3; i++) {
					colors[i + 1] = (type == (int32_t)(MT_SCALE_X + i)) ? selectionColor : style.GetColorU32(static_cast<Style::Color>(Style::DIRECTION_X + i));
				}
				break;
				// note: this internal function is only called with three possible values for operation
		}

	} else {
		ImU32 inactiveColor = style.GetColorU32(Style::INACTIVE);
		for (int32_t i = 0; i < 7; i++) {
			colors[i] = inactiveColor;
		}
	}
}

ImGuiID Context::GetCurrentId() {
	if (idStack.empty()) {
		idStack.emplace_back(std::numeric_limits<uint32_t>::max());
	}

	return idStack.back();
}

float Context::GetParallelogram(const vec_t& ptO, const vec_t& ptA, const vec_t& ptB) {
	vec_t pts[] = { ptO, ptA, ptB };

	for (uint32_t i = 0; i < 3; ++i) {
		pts[i].TransformPoint(mvp);
		if (std::abs(pts[i].w) > FLT_EPSILON) { // check for axis aligned with camera direction
			pts[i] *= 1.f / pts[i].w;
		}
	}

	vec_t segA = pts[1] - pts[0];
	vec_t segB = pts[2] - pts[0];
	segA.y /= displayRatio;
	segB.y /= displayRatio;

	vec_t segAOrtho = { -segA.y, segA.x };
	segAOrtho.Normalize();

	float dt = segAOrtho.Dot3(segB);
	float surface = std::sqrt(segA.x * segA.x + segA.y * segA.y) * std::abs(dt);

	return surface;
}

void Context::ComputeTripodAxisAndVisibility(const int32_t axisIndex, vec_t& dirAxis, vec_t& dirPlaneX, vec_t& dirPlaneY, bool& belowAxisLimit, bool& belowPlaneLimit, const bool localCoordinates) {
	dirAxis   = kDirectionUnary[axisIndex];
	dirPlaneX = kDirectionUnary[(axisIndex + 1) % 3];
	dirPlaneY = kDirectionUnary[(axisIndex + 2) % 3];

	if (isUsing && (GetCurrentId() == editingId.value())) {
		// when using, use stored factors so the gizmo doesn't flip when we translate

		// Apply axis mask to axes and planes
		belowAxisLimit  = belowAxisLimits[axisIndex] && ((1 << axisIndex) & axisMask);
		belowPlaneLimit = belowPlaneLimits[axisIndex] && (((1 << axisIndex) == axisMask) || !axisMask);

		dirAxis   *= axisFactor[axisIndex];
		dirPlaneX *= axisFactor[(axisIndex + 1) % 3];
		dirPlaneY *= axisFactor[(axisIndex + 2) % 3];

	} else {
		// new method
		float lenDir      = GetSegmentLengthClipSpace({ 0.f, 0.f, 0.f }, dirAxis, localCoordinates);
		float lenDirMinus = GetSegmentLengthClipSpace({ 0.f, 0.f, 0.f }, -dirAxis, localCoordinates);

		float lenDirPlaneX      = GetSegmentLengthClipSpace({ 0.f, 0.f, 0.f }, dirPlaneX, localCoordinates);
		float lenDirMinusPlaneX = GetSegmentLengthClipSpace({ 0.f, 0.f, 0.f }, -dirPlaneX, localCoordinates);

		float lenDirPlaneY      = GetSegmentLengthClipSpace({ 0.f, 0.f, 0.f }, dirPlaneY, localCoordinates);
		float lenDirMinusPlaneY = GetSegmentLengthClipSpace({ 0.f, 0.f, 0.f }, -dirPlaneY, localCoordinates);

		// For readability
		bool& allowFlip = isAllowAxisFlip;
		float mulAxis   = (allowFlip && lenDir < lenDirMinus && std::abs(lenDir - lenDirMinus) > FLT_EPSILON) ? -1.f : 1.f;
		float mulAxisX  = (allowFlip && lenDirPlaneX < lenDirMinusPlaneX && std::abs(lenDirPlaneX - lenDirMinusPlaneX) > FLT_EPSILON) ? -1.f : 1.f;
		float mulAxisY  = (allowFlip && lenDirPlaneY < lenDirMinusPlaneY && std::abs(lenDirPlaneY - lenDirMinusPlaneY) > FLT_EPSILON) ? -1.f : 1.f;

		dirAxis   *= mulAxis;
		dirPlaneX *= mulAxisX;
		dirPlaneY *= mulAxisY;

		// for axis
		float axisLengthInClipSpace = GetSegmentLengthClipSpace({ 0.f, 0.f, 0.f }, dirAxis * screenFactor, localCoordinates);

		float paraSurf = GetParallelogram({ 0.f, 0.f, 0.f }, dirPlaneX * screenFactor, dirPlaneY * screenFactor);
		// Apply axis mask to axes and planes
		belowPlaneLimit = (paraSurf > axisLimit) && (((1 << axisIndex) == axisMask) || !axisMask);
		belowAxisLimit  = (axisLengthInClipSpace > planeLimit) && !((1 << axisIndex) & axisMask);

		// and store values
		axisFactor[axisIndex]           = mulAxis;
		axisFactor[(axisIndex + 1) % 3] = mulAxisX;
		axisFactor[(axisIndex + 2) % 3] = mulAxisY;
		belowAxisLimits[axisIndex]  = belowAxisLimit;
		belowPlaneLimits[axisIndex] = belowPlaneLimit;
	}
}

float SxImGuizmo::Context::IntersectRayPlane(const vec_t& origin, const vec_t& dir, const vec_t& plan) {
	const float numer = plan.Dot3(origin) - plan.w;
	const float denom = plan.Dot3(dir);

	if (std::abs(denom) < FLT_EPSILON) { // normal is orthogonal to vector, cant intersect
		return -1.0f;
	}

	return -(numer / denom);
}

vec_t Context::PointOnSegment(const vec_t& point, const vec_t& vertPos1, const vec_t& vertPos2) {
	vec_t c = point - vertPos1;
	vec_t V;

	V.Normalize(vertPos2 - vertPos1);
	float d = (vertPos2 - vertPos1).Length();
	float t = V.Dot3(c);

	if (t < 0.f) {
		return vertPos1;
	}

	if (t > d) {
		return vertPos2;
	}

	return vertPos1 + V * t;
}

float Context::ComputeAngleOnPlan() {
	const float len = IntersectRayPlane(rayOrigin, rayDir, translationPlan);
	vec_t localPos = Normalized(rayOrigin + rayDir * len - model.v.position);

	vec_t perpendicularVector;
	perpendicularVector.Cross(rotationVectorSource, translationPlan);
	perpendicularVector.Normalize();
	float acosAngle = Clamp(Dot(localPos, rotationVectorSource), -1.f, 1.f);
	float angle = acosf(acosAngle);
	angle *= (Dot(localPos, perpendicularVector) < 0.f) ? 1.f : -1.f;
	return angle;
}

MoveType Context::GetMoveType(Operation op) {
	if (!Intersects(op, TRANSLATE) || isUsing || !isMouseOver) {
		return MT_NONE;
	}

	bool isNoAxesMasked       = !axisMask;
	bool isMultipleAxesMasked = axisMask & (axisMask - 1);

	ImGuiIO& io  = ImGui::GetIO();
	MoveType type = MT_NONE;

	// screen
	if (io.MousePos.x >= screenSquareMin.x && io.MousePos.x <= screenSquareMax.x &&
		io.MousePos.y >= screenSquareMin.y && io.MousePos.y <= screenSquareMax.y &&
		Contains(op, TRANSLATE)) {
		type = MT_MOVE_SCREEN;
	}

	ImVec2 coord            = io.MousePos - position;
	const vec_t screenCoord = { coord.x, coord.y };

	// compute
	for (int32_t i = 0; i < 3 && type == MT_NONE; i++) {
		bool isAxisMasked = (1 << i) & axisMask;
		vec_t dirPlaneX, dirPlaneY, dirAxis;
		bool belowAxisLimit, belowPlaneLimit;
		ComputeTripodAxisAndVisibility(i, dirAxis, dirPlaneX, dirPlaneY, belowAxisLimit, belowPlaneLimit);
		dirAxis.TransformVector(model);
		dirPlaneX.TransformVector(model);
		dirPlaneY.TransformVector(model);

		const float len = IntersectRayPlane(rayOrigin, rayDir, BuildPlan(model.v.position, dirAxis));
		vec_t posOnPlan = rayOrigin + rayDir * len;

		const ImVec2 axisStartOnScreen = WorldToPos(model.v.position + dirAxis * screenFactor * 0.1f, viewProj) - position;
		const ImVec2 axisEndOnScreen   = WorldToPos(model.v.position + dirAxis * screenFactor, viewProj) - position;

		vec_t closestPointOnAxis = PointOnSegment(screenCoord, { axisStartOnScreen.x, axisStartOnScreen.y }, { axisEndOnScreen.x, axisEndOnScreen.y });

		if ((closestPointOnAxis - screenCoord).Length() < 12.f && Intersects(op, static_cast<Operation>(TRANSLATE_X << i))) { // pixel size
			if (isAxisMasked) {
				break;
			}
				
			type = static_cast<MoveType>(MT_MOVE_X + i);
		}

		const float dx = dirPlaneX.Dot3((posOnPlan - model.v.position) * (1.f / screenFactor));
		const float dy = dirPlaneY.Dot3((posOnPlan - model.v.position) * (1.f / screenFactor));

		if (belowPlaneLimit && dx >= kQuadUV[0] && dx <= kQuadUV[4] && dy >= kQuadUV[1] && dy <= kQuadUV[3] && Contains(op, static_cast<Operation>(kTranslatePlanes[i]))) {
			if ((!isAxisMasked || isMultipleAxesMasked) && !isNoAxesMasked) {
				break;
			}
				
			type = static_cast<MoveType>(MT_MOVE_YZ + i); //!< FIXME: ここが反応しない
		}
	}

	return type;
}

MoveType Context::GetScaleType(Operation op) {
	if (isUsing) {
		return MT_NONE;
	}

	ImGuiIO& io   = ImGui::GetIO();
	MoveType type = MT_NONE;

	// screen
	if (io.MousePos.x >= screenSquareMin.x && io.MousePos.x <= screenSquareMax.x &&
		io.MousePos.y >= screenSquareMin.y && io.MousePos.y <= screenSquareMax.y &&
		Contains(op, SCALE)) {
		type = MT_SCALE_XYZ;
	}

	// compute
	for (int32_t i = 0; i < 3 && type == MT_NONE; ++i) {
		if (!Intersects(op, static_cast<Operation>(SCALE_X << i))) {
			continue;
		}

		bool isAxisMasked = (1 << i) & axisMask;

		vec_t dirPlaneX, dirPlaneY, dirAxis;
		bool belowAxisLimit, belowPlaneLimit;
		ComputeTripodAxisAndVisibility(i, dirAxis, dirPlaneX, dirPlaneY, belowAxisLimit, belowPlaneLimit, true);

		dirAxis.TransformVector(modelLocal);
		dirPlaneX.TransformVector(modelLocal);
		dirPlaneY.TransformVector(modelLocal);

		const float len = IntersectRayPlane(rayOrigin, rayDir, BuildPlan(modelLocal.v.position, dirAxis));
		vec_t posOnPlan = rayOrigin + rayDir * len;

		const float startOffset = Contains(op, static_cast<Operation>(TRANSLATE_X << i)) ? 1.0f : 0.1f;
		const float endOffset   = Contains(op, static_cast<Operation>(TRANSLATE_X << i)) ? 1.4f : 1.0f;
		const ImVec2 posOnPlanScreen   = WorldToPos(posOnPlan, viewProj);
		const ImVec2 axisStartOnScreen = WorldToPos(modelLocal.v.position + dirAxis * screenFactor * startOffset, viewProj);
		const ImVec2 axisEndOnScreen   = WorldToPos(modelLocal.v.position + dirAxis * screenFactor * endOffset, viewProj);

		vec_t closestPointOnAxis = PointOnSegment({ posOnPlanScreen.x,  posOnPlanScreen.y }, { axisStartOnScreen.x, axisStartOnScreen.y }, { axisEndOnScreen.x, axisEndOnScreen.y });

		if ((closestPointOnAxis - vec_t{ posOnPlanScreen.x, posOnPlanScreen.y }).Length() < 12.f) { // pixel size
			if (!isAxisMasked) {
				type = static_cast<MoveType>(MT_SCALE_X + i);
			}
		}

		// skip universal
	}

	return type;
}

MoveType Context::GetRotationType(Operation op) {
	if (isUsing) {
		return MT_NONE;
	}

	bool isNoAxesMasked       = !axisMask;
	bool isMultipleAxesMasked = axisMask & (axisMask - 1);

	ImGuiIO& io   = ImGui::GetIO();
	MoveType type = MT_NONE;

	vec_t deltaScreen = { io.MousePos.x - screenSquareCenter.x, io.MousePos.y - screenSquareCenter.y, 0.f, 0.f };
	float dist        = deltaScreen.Length();

	if (Intersects(op, ROTATE_SCREEN) && dist >= (radiusSquareCenter - 4.0f) && dist < (radiusSquareCenter + 4.0f)) {
		if (!isNoAxesMasked) {
			return MT_NONE;
		}
		
		type = MT_ROTATE_SCREEN;
	}

	const vec_t planNormals[] = { model.v.right, model.v.up, model.v.dir };

	vec_t modelViewPos;
	modelViewPos.TransformPoint(model.v.position, view);

	for (int32_t i = 0; i < 3 && type == MT_NONE; ++i) {
		if (!Intersects(op, static_cast<Operation>(ROTATE_X << i))) {
			continue;
		}

		bool isAxisMasked = (1 << i) & axisMask;
		// pickup plan
		vec_t pickupPlan = BuildPlan(model.v.position, planNormals[i]);

		const float len = IntersectRayPlane(rayOrigin, rayDir, pickupPlan);
		const vec_t intersectWorldPos = rayOrigin + rayDir * len;
		vec_t intersectViewPos;
		intersectViewPos.TransformPoint(intersectWorldPos, view);

		if (std::abs(modelViewPos.z) - std::abs(intersectViewPos.z) < -FLT_EPSILON) {
			continue;
		}

		const vec_t localPos = intersectWorldPos - model.v.position;
		vec_t idealPosOnCircle = Normalized(localPos);
		idealPosOnCircle.TransformVector(modelInverse);
		const ImVec2 idealPosOnCircleScreen = WorldToPos(idealPosOnCircle * kRotationDisplayFactor * screenFactor, mvp);

		const ImVec2 distanceOnScreen = idealPosOnCircleScreen - io.MousePos;

		const float distance = vec_t{ distanceOnScreen.x, distanceOnScreen.y }.Length();
		if (distance < 8.f) { // pixel size
			if ((!isAxisMasked || isMultipleAxesMasked) && !isNoAxesMasked) {
				break;
			}

			type = static_cast<MoveType>(MT_ROTATE_X + i);
		}
	}

	return type;
}

bool Context::HandleTranslation(GizmoOutput& output, Operation op, MoveType& type) {
	if (!Intersects(op, TRANSLATE) || type != MT_NONE) {
		return false;
	}

	const ImGuiIO& io = ImGui::GetIO();
	bool modified     = false;

	// move
	if (isUsing && (GetCurrentId() == editingId) && IsTranslateType(currentOperation)) {
		ImGui::SetNextFrameWantCaptureMouse(true);

		const float signedLength = IntersectRayPlane(rayOrigin, rayDir, translationPlan);
		const float len          = std::abs(signedLength); // near plan
		const vec_t newPos       = rayOrigin + rayDir * len;
		
		// compute delta
		const vec_t newOrigin = newPos - relativeOrigin * screenFactor;
		vec_t delta = newOrigin - model.v.position;

		// 1 axis constraint
		if (currentOperation >= MT_MOVE_X && currentOperation <= MT_MOVE_Z) {
			const int axisIndex = currentOperation - MT_MOVE_X;
			const vec_t& axisValue = *(vec_t*)&model.m[axisIndex];
			const float lengthOnAxis = Dot(axisValue, delta);
			delta = axisValue * lengthOnAxis;
		}

		// skip snap.

		if (delta != translationLastDelta) {
			modified = true;
		}

		translationLastDelta = delta;

		output.type  = GizmoOutput::OutputType::Translation;
		output.value = delta;

		if (!io.MouseDown[0]) {
			isUsing = false;
		}

		type = currentOperation;

	} else {

		// find new possible way to move
		type = isOverGizmoHotspot ? MT_NONE : GetMoveType(op);
		isOverGizmoHotspot |= (type != MT_NONE);

		if (type != MT_NONE) {
			ImGui::SetNextFrameWantCaptureMouse(true);
		}

		if (CanActivate() && type != MT_NONE) {
			isUsing = true;
			editingId = GetCurrentId();
			currentOperation = type;

			vec_t movePlanNormal[] = {
				model.v.right, model.v.up, model.v.dir,
				model.v.right, model.v.up, model.v.dir,
				-cameraDir
			};

			vec_t cameraToModelNormalized = Normalized(model.v.position - cameraEye);

			for (uint32_t i = 0; i < 3; i++) {
				vec_t orthoVector = Cross(movePlanNormal[i], cameraToModelNormalized);
				movePlanNormal[i].Cross(orthoVector);
				movePlanNormal[i].Normalize();
			}

			// pickup plan
			translationPlan = BuildPlan(model.v.position, movePlanNormal[type - MT_MOVE_X]);
			const float len = IntersectRayPlane(rayOrigin, rayDir, translationPlan);
			translationPlanOrigin = rayOrigin + rayDir * len;
			matrixOrigin          = model.v.position;

			relativeOrigin = (translationPlanOrigin - model.v.position) * (1.f / screenFactor);
		}
	}

	return modified;
}

bool Context::HandleScale(GizmoOutput& output, Operation op, MoveType& type) {

	if (!Intersects(op, SCALE) || type != MT_NONE || !isMouseOver) {
		return false;
	}

	ImGuiIO& io   = ImGui::GetIO();
	bool modified = false;

	if (!isUsing) {
		type = isOverGizmoHotspot ? MT_NONE : GetScaleType(op);
		isOverGizmoHotspot |= (type != MT_NONE);

		if (type != MT_NONE) {
			ImGui::SetNextFrameWantCaptureMouse(true);
		}

		if (CanActivate() && type != MT_NONE) {
			isUsing          = true;
			editingId        = GetCurrentId();
			currentOperation = type;

			const vec_t movePlanNormal[] = { modelLocal.v.up, modelLocal.v.dir, modelLocal.v.right, modelLocal.v.dir, modelLocal.v.up, modelLocal.v.right, -cameraDir };
			// pickup plan

			translationPlan = BuildPlan(modelLocal.v.position, movePlanNormal[type - MT_SCALE_X]);
			const float len = IntersectRayPlane(rayOrigin, rayDir, translationPlan);
			translationPlanOrigin = rayOrigin + rayDir * len;
			matrixOrigin = modelLocal.v.position;
			scale.Set(1.f, 1.f, 1.f);
			relativeOrigin = (translationPlanOrigin - modelLocal.v.position) * (1.f / screenFactor);
			scaleValueOrigin = { modelSource.v.right.Length(), modelSource.v.up.Length(), modelSource.v.dir.Length() };
			saveMousePos = io.MousePos;
		}
	}

	// scale
	if (isUsing && (GetCurrentId() == editingId) && IsScaleType(currentOperation)) {
		ImGui::SetNextFrameWantCaptureMouse(true);

		const float len = IntersectRayPlane(rayOrigin, rayDir, translationPlan);
		vec_t newPos    = rayOrigin + rayDir * len;
		vec_t newOrigin = newPos - relativeOrigin * screenFactor;
		vec_t delta     = newOrigin - modelLocal.v.position;

		// 1 axis constraint
		if (currentOperation >= MT_SCALE_X && currentOperation <= MT_SCALE_Z) {
			int axisIndex = currentOperation - MT_SCALE_X;
			const vec_t& axisValue = *(vec_t*)&modelLocal.m[axisIndex];
			float lengthOnAxis = Dot(axisValue, delta);
			delta = axisValue * lengthOnAxis;

			vec_t baseVector = translationPlanOrigin - modelLocal.v.position;
			float ratio = Dot(axisValue, baseVector + delta) / Dot(axisValue, baseVector);

			scale[axisIndex] = std::max(ratio, 0.001f);

		} else {
			float scaleDelta = (io.MousePos.x - saveMousePos.x) * 0.01f;
			scale.Set(std::max(1.f + scaleDelta, 0.001f));
		}

		// skip snap

		// no 0 allowed
		for (int32_t i = 0; i < 3; i++) {
			scale[i] = std::max(scale[i], 0.001f);
		}

		if (scaleLast != scale) {
			modified = true;
		}

		scaleLast = scale;

		output.type  = GizmoOutput::OutputType::Scale;
		output.value = scale * scaleValueOrigin;

		if (!io.MouseDown[0]) {
			isUsing = false;
			scale.Set(1.f, 1.f, 1.f);
		}

		type = currentOperation;
	}

	return modified;
}

bool Context::HandleRotation(GizmoOutput& output, Operation op, MoveType& type) {
	if (!Intersects(op, ROTATE) || type != MT_NONE || !isMouseOver) {
		return false;
	}

	ImGuiIO& io = ImGui::GetIO();
	bool applyRotationLocaly = mode == Mode::Local;
	bool modified = false;

	if (!isUsing) {
		type = isOverGizmoHotspot ? MT_NONE : GetRotationType(op);
		isOverGizmoHotspot |= type != MT_NONE;

		if (type != MT_NONE) {
			ImGui::SetNextFrameWantCaptureMouse(true);
		}

		if (type == MT_ROTATE_SCREEN) {
			applyRotationLocaly = true;
		}

		if (CanActivate() && type != MT_NONE) {
			isUsing = true;
			editingId = GetCurrentId();
			currentOperation = type;
			const vec_t rotatePlanNormal[] = { model.v.right, model.v.up, model.v.dir, -cameraDir };

			// pickup plan
			if (applyRotationLocaly) {
				translationPlan = BuildPlan(model.v.position, rotatePlanNormal[type - MT_ROTATE_X]);

			} else {
				translationPlan = BuildPlan(modelSource.v.position, kDirectionUnary[type - MT_ROTATE_X]);
			}

			const float len = IntersectRayPlane(rayOrigin, rayDir, translationPlan);
			vec_t localPos = rayOrigin + rayDir * len - model.v.position;
			rotationVectorSource = Normalized(localPos);
			rotationAngleOrigin = ComputeAngleOnPlan();
		}
	}

	// rotation
	if (isUsing && (GetCurrentId() == editingId) && IsRotateType(currentOperation)) {
		ImGui::SetNextFrameWantCaptureMouse(true);

		rotationAngle = ComputeAngleOnPlan();

		// skip snap.

		vec_t rotationAxisLocalSpace;
		rotationAxisLocalSpace.TransformVector({ translationPlan.x, translationPlan.y, translationPlan.z, 0.f }, modelInverse);
		rotationAxisLocalSpace.Normalize();

		output.type  = GizmoOutput::OutputType::Rotation;
		

		if (mode == Mode::Local) {
			output.value = AxisAngleQuat({ rotationAxisLocalSpace.x, rotationAxisLocalSpace.y, rotationAxisLocalSpace.z }, rotationAngle - rotationAngleOrigin);

		} else if (mode == Mode::World) {



			// FIXME: worldの回転方向が2軸以上だとおかしい
			output.value = AxisAngleQuat({ rotationAxisLocalSpace.x, rotationAxisLocalSpace.y, rotationAxisLocalSpace.z }, rotationAngleOrigin - rotationAngle);
			output.value = InverseQuat(output.value);
		}

		if (rotationAngle != rotationAngleOrigin) {
			modified = true;
		}

		rotationAngleOrigin = rotationAngle;

		if (!io.MouseDown[0]) {
			isUsing   = false;
			editingId = -1;
		}

		type = currentOperation;
	}

	return modified;
}

void Context::DrawRotationGizmo(Operation _operation, MoveType type) {
	if (!Intersects(_operation, ROTATE)) {
		return;
	}

	if (drawList == nullptr) {
		return;
	}

	bool isMultipleAxesMasked = axisMask & (axisMask - 1);
	bool isNoAxesMasked       = !axisMask;

	// colors
	std::array<ImU32, 7> colors = {};
	ComputeColors(colors, type, ROTATE);

	vec_t cameraToModelNormalized;
	if (isOrthographic) {
		matrix_t viewInverse;
		viewInverse.Inverse(view);
		cameraToModelNormalized = -viewInverse.v.dir;

	} else {
		cameraToModelNormalized = Normalized(model.v.position - cameraEye);
	}

	cameraToModelNormalized.TransformVector(modelInverse);

	radiusSquareCenter = kScreenRotateSize * size.y;

	bool hasRSC = Intersects(_operation, ROTATE_SCREEN);
	for (uint8_t axis = 0; axis < 3; ++axis) {
		if (!Intersects(_operation, static_cast<Operation>(ROTATE_Z >> axis))) {
			continue;
		}

		bool isAxisMasked = (1 << (2 - axis)) & axisMask;

		if ((!isAxisMasked || isMultipleAxesMasked) && !isNoAxesMasked) {
			continue;
		}

		const bool usingAxis = (isUsing && type == MT_ROTATE_Z - axis);
		const int32_t circleMul = (hasRSC && !usingAxis) ? 1 : 2;

		std::vector<ImVec2> circlePos(circleMul * kHalfCircleSegmentCount + 1);

		float angleStart = std::atan2(cameraToModelNormalized[(4 - axis) % 3], cameraToModelNormalized[(3 - axis) % 3]) + kPi * 0.5f;

		for (size_t i = 0; i < circlePos.size(); ++i) {
			float ng = angleStart + (float)circleMul * kPi * ((float)i / (float)(circleMul * kHalfCircleSegmentCount));
			vec_t axisPos = vec_t::makeVect(std::cos(ng), std::sinf(ng), 0.f);
			vec_t pos = vec_t::makeVect(axisPos[axis], axisPos[(axis + 1) % 3], axisPos[(axis + 2) % 3]) * screenFactor * kRotationDisplayFactor;
			circlePos[i] = WorldToPos(pos, mvp);
		}

		if (!isUsing || usingAxis) {
			drawList->AddPolyline(circlePos.data(), static_cast<int32_t>(circlePos.size()), colors[3 - axis], false, style.rotationLineThickness);
		}

		float radiusAxis = std::sqrt((ImLengthSqr(WorldToPos(model.v.position, viewProj) - circlePos[0])));
		if (radiusAxis > radiusSquareCenter) {
			radiusSquareCenter = radiusAxis;
		}
	}

	if (hasRSC && (!isUsing || type == MT_ROTATE_SCREEN) && (!isMultipleAxesMasked && isNoAxesMasked)) {
		drawList->AddCircle(WorldToPos(model.v.position, viewProj), radiusSquareCenter, colors[0], kHalfCircleSegmentCount, style.rotationOuterLineThickness);
	}

	if (isUsing && (GetCurrentId() == editingId) && IsRotateType(type)) {
		ImVec2 circlePos[kHalfCircleSegmentCount + 1];

		circlePos[0] = WorldToPos(model.v.position, viewProj);

		for (uint32_t i = 1; i < kHalfCircleSegmentCount + 1; i++) {
			float ng = rotationAngle * ((float)(i - 1) / (float)(kHalfCircleSegmentCount - 1));
			matrix_t rotateVectorMatrix;
			rotateVectorMatrix.RotationAxis(translationPlan, ng);
			vec_t pos;
			pos.TransformPoint(rotationVectorSource, rotateVectorMatrix);
			pos *= screenFactor * kRotationDisplayFactor;
			circlePos[i] = WorldToPos(pos + model.v.position, viewProj);
		}

		drawList->AddConvexPolyFilled(circlePos, kHalfCircleSegmentCount + 1, style.GetColorU32(Style::ROTATION_USING_FILL));
		drawList->AddPolyline(circlePos, kHalfCircleSegmentCount + 1, style.GetColorU32(Style::ROTATION_USING_FILL), true, style.rotationLineThickness);

		ImVec2 destinationPosOnScreen = circlePos[1];

		std::string tmps;
		tmps.resize(512);

		float deg = (rotationAngle / kPi) * 180.0f;
		float rad = rotationAngle;

		ImFormatString(tmps.data(), tmps.size(), kRotationInfoMask[type - MT_ROTATE_X], deg, rad);

		drawList->AddText(ImVec2(destinationPosOnScreen.x + 15, destinationPosOnScreen.y + 15), style.GetColorU32(Style::TEXT_SHADOW), tmps.c_str());
		drawList->AddText(ImVec2(destinationPosOnScreen.x + 14, destinationPosOnScreen.y + 14), style.GetColorU32(Style::TEXT), tmps.c_str());
	}
}

void Context::DrawHatchedAxis(const vec_t& axis) {
	if (style.hatchedAxisLineThickness <= 0.0f) {
		return;
	}

	for (int32_t j = 1; j < 10; j++) {
		ImVec2 baseSSpace2 = WorldToPos(axis * 0.05f * (float)(j * 2) * screenFactor, mvp);
		ImVec2 worldDirSSpace2 = WorldToPos(axis * 0.05f * (float)(j * 2 + 1) * screenFactor, mvp);
		drawList->AddLine(baseSSpace2, worldDirSSpace2, style.GetColorU32(Style::HATCHED_AXIS_LINES), style.hatchedAxisLineThickness);
	}
}

void Context::DrawTranslationGizmo(Operation op, MoveType type) {
	if (!Intersects(op, TRANSLATE)) {
		return;
	}

	if (drawList == nullptr) {
		return;
	}

	std::array<ImU32, 7> colors;
	ComputeColors(colors, type, TRANSLATE);

	const ImVec2 origin = WorldToPos(model.v.position, viewProj);

	// draw
	bool belowAxisLimit = false;
	bool belowPlaneLimit = false;

	for (int32_t i = 0; i < 3; ++i) {
		vec_t dirPlaneX, dirPlaneY, dirAxis;
		ComputeTripodAxisAndVisibility(i, dirAxis, dirPlaneX, dirPlaneY, belowAxisLimit, belowPlaneLimit);

		// draw axis
		if (!isUsing || (isUsing && type == MT_MOVE_X + i)) {
			if (belowAxisLimit && Intersects(op, static_cast<Operation>(TRANSLATE_X << i))) {
				ImVec2 baseSSpace     = WorldToPos(dirAxis * 0.1f * screenFactor, mvp);
				ImVec2 worldDirSSpace = WorldToPos(dirAxis * screenFactor, mvp);

				drawList->AddLine(baseSSpace, worldDirSSpace, colors[i + 1], style.translationLineThickness);

				// Arrow head begin
				ImVec2 dir(origin - worldDirSSpace);

				float d = sqrtf(ImLengthSqr(dir));
				dir /= d; // Normalize
				dir *= style.translationLineArrowSize;

				ImVec2 ortogonalDir(dir.y, -dir.x); // Perpendicular vector
				ImVec2 a(worldDirSSpace + dir);
				drawList->AddTriangleFilled(worldDirSSpace - dir, a + ortogonalDir, a - ortogonalDir, colors[i + 1]);
				// Arrow head end

				if (axisFactor[i] < 0.f) {
					DrawHatchedAxis(dirAxis);
				}
			}
		}

		// draw plane
		if (!isUsing || (isUsing && type == MT_MOVE_YZ + i)) {
			if (belowPlaneLimit && Contains(op, static_cast<Operation>(kTranslatePlanes[i]))) {

				ImVec2 screenQuadPts[4];
				for (int32_t j = 0; j < 4; ++j) {
					vec_t cornerWorldPos = (dirPlaneX * kQuadUV[j * 2] + dirPlaneY * kQuadUV[j * 2 + 1]) * screenFactor;
					screenQuadPts[j] = WorldToPos(cornerWorldPos, mvp);
				}

				drawList->AddPolyline(screenQuadPts, 4, style.GetColorU32(static_cast<Style::Color>(Style::DIRECTION_X + i)), true, 1.0f);
				drawList->AddConvexPolyFilled(screenQuadPts, 4, colors[i + 4]);
			}
		}
	}

	drawList->AddCircleFilled(screenSquareCenter, style.centerCircleSize, colors[0], 32);

	if (isUsing && (GetCurrentId() == editingId) && IsTranslateType(type)) {
		ImU32 translationLineColor = style.GetColorU32(Style::TRANSLATION_LINE);

		ImVec2 sourcePosOnScreen      = WorldToPos(matrixOrigin, viewProj);
		ImVec2 destinationPosOnScreen = WorldToPos(model.v.position, viewProj);
		vec_t dif = { destinationPosOnScreen.x - sourcePosOnScreen.x, destinationPosOnScreen.y - sourcePosOnScreen.y, 0.f, 0.f };
		dif.Normalize();
		dif *= 5.f;
		drawList->AddCircle(sourcePosOnScreen, 6.f, translationLineColor);
		drawList->AddCircle(destinationPosOnScreen, 6.f, translationLineColor);
		drawList->AddLine(ImVec2(sourcePosOnScreen.x + dif.x, sourcePosOnScreen.y + dif.y), ImVec2(destinationPosOnScreen.x - dif.x, destinationPosOnScreen.y - dif.y), translationLineColor, 2.f);

		char tmps[512];
		vec_t deltaInfo = model.v.position - matrixOrigin;
		int componentInfoIndex = (type - MT_MOVE_X) * 3;
		ImFormatString(tmps, sizeof(tmps), kTranslationInfoMask[type - MT_MOVE_X], deltaInfo[kTranslationInfoIndex[componentInfoIndex]], deltaInfo[kTranslationInfoIndex[componentInfoIndex + 1]], deltaInfo[kTranslationInfoIndex[componentInfoIndex + 2]]);
		drawList->AddText(ImVec2(destinationPosOnScreen.x + 15, destinationPosOnScreen.y + 15), style.GetColorU32(Style::TEXT_SHADOW), tmps);
		drawList->AddText(ImVec2(destinationPosOnScreen.x + 14, destinationPosOnScreen.y + 14), style.GetColorU32(Style::TEXT), tmps);
	}
}

void Context::DrawScaleGizmo(Operation op, MoveType type) {
	if (!Intersects(op, SCALE)) {
		return;
	}

	std::array<ImU32, 7> colors = {};
	ComputeColors(colors, type, SCALE);

	// draw
	vec_t scaleDisplay = { 1.f, 1.f, 1.f, 1.f };

	if (isUsing && (GetCurrentId() == editingId)) {
		scaleDisplay = scale;
	}

	for (int32_t i = 0; i < 3; i++) {
		if (!Intersects(op, static_cast<Operation>(SCALE_X << i))) {
			continue;
		}

		const bool usingAxis = (isUsing && type == MT_SCALE_X + i);

		if (!isUsing || usingAxis) {
			vec_t dirPlaneX, dirPlaneY, dirAxis;
			bool belowAxisLimit, belowPlaneLimit;
			ComputeTripodAxisAndVisibility(i, dirAxis, dirPlaneX, dirPlaneY, belowAxisLimit, belowPlaneLimit, true);

			// draw axis
			if (belowAxisLimit) {
				bool hasTranslateOnAxis = Contains(op, static_cast<Operation>(TRANSLATE_X << i));
				float markerScale = hasTranslateOnAxis ? 1.4f : 1.0f;
				ImVec2 baseSSpace = WorldToPos(dirAxis * 0.1f * screenFactor, mvp);
				ImVec2 worldDirSSpaceNoScale = WorldToPos(dirAxis * markerScale * screenFactor, mvp);
				ImVec2 worldDirSSpace = WorldToPos((dirAxis * markerScale * scaleDisplay[i]) * screenFactor, mvp);

				if (isUsing && (GetCurrentId() == editingId)) {
					ImU32 scaleLineColor = style.GetColorU32(Style::SCALE_LINE);
					drawList->AddLine(baseSSpace, worldDirSSpaceNoScale, scaleLineColor, style.scaleLineThickness);
					drawList->AddCircleFilled(worldDirSSpaceNoScale, style.scaleLineCircleSize, scaleLineColor);
				}

				if (!hasTranslateOnAxis || isUsing) {
					drawList->AddLine(baseSSpace, worldDirSSpace, colors[i + 1], style.scaleLineThickness);
				}

				drawList->AddCircleFilled(worldDirSSpace, style.scaleLineCircleSize, colors[i + 1]);

				if (axisFactor[i] < 0.f) {
					DrawHatchedAxis(dirAxis * scaleDisplay[i]);
				}
			}
		}
	}

	// draw screen cirle
	drawList->AddCircleFilled(screenSquareCenter, style.centerCircleSize, colors[0], 32);

	if (isUsing && (GetCurrentId() == editingId) && IsScaleType(type)) {
		ImVec2 destinationPosOnScreen = WorldToPos(model.v.position, viewProj);

		char tmps[512];
		int componentInfoIndex = (type - MT_SCALE_X) * 3;
		ImFormatString(tmps, sizeof(tmps), kScaleInfoMask[type - MT_SCALE_X], scaleDisplay[kTranslationInfoIndex[componentInfoIndex]]);

		drawList->AddText(ImVec2(destinationPosOnScreen.x + 15, destinationPosOnScreen.y + 15), style.GetColorU32(Style::TEXT_SHADOW), tmps);
		drawList->AddText(ImVec2(destinationPosOnScreen.x + 14, destinationPosOnScreen.y + 14), style.GetColorU32(Style::TEXT), tmps);
	}

}

static Context sContext = {}; //!< global variables

////////////////////////////////////////////////////////////////////////////////////////////
// SxImGuizmo namespace methods
////////////////////////////////////////////////////////////////////////////////////////////

void SetDrawlist(ImDrawList* drawlist) {
	sContext.drawList = drawlist ? drawlist : ImGui::GetWindowDrawList();
}

void BeginFrame() {
	sContext.isOverGizmoHotspot = false;
}

void SetRect(const ImVec2& position, const ImVec2& size) {
	sContext.position     = position;
	sContext.size         = size;
	sContext.max          = { position.x + size.x, position.y + size.y };
	sContext.displayRatio = size.x / size.y;
}

void Enable(bool enable) {
	sContext.isEnable = enable;

	if (!enable) {
		sContext.isUsing       = false;
		sContext.isUsingBounds = false;
	}
}

bool IsUsing() {
	return (sContext.isUsing && (sContext.GetCurrentId() == sContext.editingId)) || sContext.isUsingBounds;
}

bool Manipulate(const float* view, const float* proj, const float* matrix, GizmoOutput& output, Operation operation, Mode mode) {
	output = {};

	sContext.drawList->PushClipRect(sContext.position, sContext.max, false);
	sContext.ComputeContext(view, proj, matrix, (operation & SCALE) ? Mode::Local : mode);

	// behind camera
	vec_t camSpacePosition;
	camSpacePosition.TransformPoint(vec_t::makeVect(0.f, 0.f, 0.f), sContext.mvp);
	if (!sContext.isOrthographic && camSpacePosition.z < 0.001f && !sContext.isUsing) {
		return false;
	}

	MoveType type = MT_NONE;
	bool manipulated = false;

	if (sContext.isEnable && !sContext.isUsingBounds) {
		manipulated |= sContext.HandleTranslation(output, operation, type);
		manipulated |= sContext.HandleScale(output, operation, type);
		manipulated |= sContext.HandleRotation(output, operation, type);
	}

	// skiped local bounds.

	sContext.operation = operation;

	if (!sContext.isUsingBounds) {
		sContext.DrawRotationGizmo(operation, type);
		sContext.DrawTranslationGizmo(operation, type);
		sContext.DrawScaleGizmo(operation, type);
	}

	sContext.drawList->PopClipRect();
	return manipulated;
}

Style& GetStyle() {
	return sContext.style;
}

void ShowStyleEditor() {
	sContext.style.ShowStyleEditor();
}

_SXIMGUIZMO_NAMESPACE_END
