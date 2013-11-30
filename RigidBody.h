#pragma once

//DirectX includes
#include <DirectXMath.h>
using namespace DirectX;

#include "Vec3.h"

class RigidBody
{
private:
	Vec3 position;
	float width;
	float height;
	float length;

	XMFLOAT4 orientation;

	float mass;
	XMFLOAT3X3 inertiaTensorInverse;

	XMFLOAT3 color;

	float rayTriangleIntersection(const Vec3 &rayStart, const Vec3 &rayDirection, const Vec3 &v0, const Vec3 &v1, const Vec3 &v2);
	float rayFaceIntersection(const Vec3 &rayStart, const Vec3 &rayDirection, const Vec3 &v0, const Vec3 &v1, const Vec3 &v2, const Vec3 & v3);
public:
	RigidBody(void);
	RigidBody(float x, float y, float z);

	void translate(float x, float y, float z);
	void setPosition(float x, float y, float z);
	Vec3 getPosition();

	void setWidth(float width);
	float getWidth();
	void setHeight(float height);
	float getHeight();
	void setLength(float length);
	float getLength();
	void setColor(float r, float g, float b);
	XMFLOAT3 getColor();

	Vec3 getEdgePoint(int index);

	XMFLOAT4 getOrientation();

	float getRayIntersection(const Vec3 &rayStart, const Vec3 &rayDirection);

	void step();
};

