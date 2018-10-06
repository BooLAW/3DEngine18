
#include "glmath.h"
#include "MathGeoLib\MathGeoLib.h"
#include "Color.h"
#include "Mesh.h"
#include <stdio.h>
#include "MathGeoLib/Geometry/LineSegment.h"
#include <math.h>

class Mesh;
class LineSegment;


void Init();
void CleanUp();

void DebugDraw(const Mesh* mesh, Color color = White, const float4x4& transform = float4x4::identity);
void BoxDD(const float3* points, Color color, bool debug = false);
void LineSegmentDraw(const vec origin, const vec direction, Color color, bool debug = false);


