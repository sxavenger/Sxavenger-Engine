#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

float SolveQuadraticForPositiveReals(float a, float b, float c) {
	float delta = b * b - 4.0 * a * c;
	
	if (delta < 0.0 || a == 0.0) {
		return -1.0;
	}
	
	float solution0 = (-b - sqrt(delta)) / (2.0 * a);
	float solution1 = (-b + sqrt(delta)) / (2.0 * a);
	
	if (solution0 < 0.0 && solution1 < 0.0) {
		return -1.0;
	}
	
	if (solution0 < 0.0) {
		return max(0.0, solution1);
		
	} else if (solution1 < 0.0) {
		return max(0.0, solution0);
	}
	
	return max(0.0, min(solution0, solution1));
}

float FindClosestRayCircleIntersection(float2 o, float2 d, float r) {
	return SolveQuadraticForPositiveReals(dot(d, d), 2.0 * dot(d, o), dot(o, o) - (r * r));
}

bool FindIntersectionTMax(out float t_max, float2 o, float2 d, float bottom_radius, float top_radius) {
	
	float t_bottom = FindClosestRayCircleIntersection(o, d, bottom_radius);
	float t_top    = FindClosestRayCircleIntersection(o, d, top_radius);

	if (t_bottom < 0.0) {
		if (t_top < 0.0) {
			t_max = 0.0f;
			return false;
			
		} else {
			t_max = t_top;
		}
		
	} else {
		if (t_top > 0.0) {
			t_max = min(t_bottom, t_top);
			
		} else {
			t_max = 0.0f;
		}
	}
	
	return true;
}

bool QuadraticHasPositiveRealSolutions(float a, float b, float c) {
	float delta = b * b - 4.0 * a * c;
	return (delta >= 0.0 && a != 0.0) && (((-b - sqrt(delta)) / (2.0 * a)) >= 0.0 || ((-b + sqrt(delta)) / (2.0 * a)) >= 0.0);
}

float FindClosestRaySphereIntersection(float3 o, float3 d, float3 c, float r) {
	float3 dist = o - c;
	return SolveQuadraticForPositiveReals(dot(d, d), 2.0 * dot(d, dist), dot(dist, dist) - (r * r));
}

bool RayIntersectionSphere(float3 o, float3 d, float3 c, float r) {
	float3 dist = o - c;
	return QuadraticHasPositiveRealSolutions(dot(d, d), 2.0 * dot(d, dist), dot(dist, dist) - (r * r));
}

bool FindAtmosphereTMaxTBottom(out float t_max, out float t_bottom, float3 o, float3 d, float3 c, float bottom_radius, float top_radius) {
	t_bottom = FindClosestRaySphereIntersection(o, d, c, bottom_radius);
	float t_top = FindClosestRaySphereIntersection(o, d, c, top_radius);

	if (t_bottom < 0.0) {
		if (t_top < 0.0) {
			t_max = 0.0;
			return false;
			
		} else {
			t_max = t_top;
		}
		
	} else {
		if (t_top > 0.0) {
			t_max = min(t_top, t_bottom);
		} else {
			t_max = t_bottom;
		}
	}
	
	return true;
}

bool FindAtmosphereTMax(out float t_max, float3 o, float3 d, float3 c, float bottom_radius, float top_radius) {
	float t_bottom = FindClosestRaySphereIntersection(o, d, c, bottom_radius);
	float t_top    = FindClosestRaySphereIntersection(o, d, c, top_radius);

	if (t_bottom < 0.0) {
		if (t_top < 0.0) {
			t_max = 0.0;
			return false;
			
		} else {
			t_max = t_top;
		}
		
	} else {
		if (t_top > 0.0) {
			t_max = min(t_top, t_bottom);
			
		} else {
			t_max = t_bottom;
		}
	}
	return true;

}

float ComputePlanetShadow(float3 o, float3 d, float3 c, float r) {
	return float(!RayIntersectionSphere(o, d, c, r));
}
