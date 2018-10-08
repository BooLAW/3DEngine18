
#include "glmath.h"
#include "MathGeoLib\MathGeoLib.h"
#include "Color.h"
#include "Mesh.h"
#include <stdio.h>
#include "MathGeoLib/Geometry/LineSegment.h"
#include <math.h>

class Mesh;



void Init();
void CleanUp();

void DebugDraw(const Mesh* mesh, Color color = White, const float4x4& transform = float4x4::identity);
void BoxDD(const float3* points, Color color, bool debug = false);
void LineSegmentDraw(const float3* origin, const float3* direction, Color color, bool debug = false);


