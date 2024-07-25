#include "Vector3Math.h"
#include "math.h"

//加算
Vector3 Add(const Vector3& v1, const Vector3& v2) {

	Vector3 result{ 0.0f,0.0f,0.0f };

	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;
	result.z = v1.z + v2.z;

	return result;

}

//減算
Vector3 Subtract(const Vector3& v1, const Vector3& v2) {

	Vector3 result{ 0.0f,0.0f,0.0f };

	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;

	return result;

}

//スカラー倍
Vector3 Multiply(float scalar, const Vector3& v) {

	Vector3 result{ 0.0f,0.0f,0.0f };

	result.x = scalar * v.x;
	result.y = scalar * v.y;
	result.z = scalar * v.z;

	return result;

}

//内積
float Dot(const Vector3& v1, const Vector3& v2) {

	float result = 0.0f;

	result = (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);

	return result;

}

//長さ(ノルム)
float Length(const Vector3& v) {

	float result = 0.0f;

	result = sqrtf(Dot(v, v));

	return result;

}


//正規化
Vector3 Normalize(const Vector3& v) {

	Vector3 result{ 0.0f,0.0f,0.0f };

	result.x = v.x / Length(v);
	result.y = v.y / Length(v);
	result.z = v.z / Length(v);

	return result;

}

Vector3 Perpendicular(const Vector3& v) {

	if (v.x != 0.0f || v.y != 0.0f) {
		return { -v.y,v.x,0.0f };
	}

	return{ 0.0f,-v.z,v.y };
}

//クロス積
Vector3 Cross(const Vector3& v1, const Vector3& v2) {

	Vector3 result{
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x
	};

	return result;
}

//正射影ベクトルの生成
Vector3 Project(const Vector3& v1, const Vector3& v2) {

	float dot = Dot(v1, v2);

	float norm = powf(Length(v2), 2.0f);

	Vector3 result = {
		dot / norm * (v2.x),
		dot / norm * (v2.y),
		dot / norm * (v2.z),
	};

	return result;
}

//最近接点の生成
Vector3 ClosestPoint(const Vector3& point, const Segment& segment) {

	Vector3 project = Project(Subtract(point, segment.origin), segment.diff);

	Vector3 result;

	result.x = segment.origin.x + project.x;
	result.y = segment.origin.y + project.y;
	result.z = segment.origin.z + project.z;

	return result;
}

//ベクトル変換
Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m) {

	Vector3 result{
		v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0],
		v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1],
		v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2],
	};

	return result;
}

Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t) {

	Vector3 result;

	result.x = t * v1.x + (1.0f - t) * v2.x;
	result.y = t * v1.y + (1.0f - t) * v2.y;
	result.z = t * v1.z + (1.0f - t) * v2.z;

	return result;
}

Vector3 Bezier(const Vector3& p0, const Vector3& p1, const Vector3& p2, float t) {

	Vector3 p0p1 = Lerp(p0, p1, t);
	Vector3 p1p2 = Lerp(p1, p2, t);
	Vector3 p = Lerp(p0p1, p1p2, t);

	return p;
}