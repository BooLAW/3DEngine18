#include "glmath.h"
#include "MathGeoLib\MathGeoLib.h"
#include "Color.h"

void Init();
void CleanUp();

void DebugDraw(const AABB& bb, Color color = White, const float4x4& transform = float4x4::identity);
void BoxDD(const float3* points, Color color, bool debug = false);


