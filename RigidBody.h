#pragma once

//DirectX includes
#include <DirectXMath.h>
using namespace DirectX;

#include "Vec3.h"

class RigidBody
{
private:
	float width;
	float height;
	float length;

	bool isFixed;

	XMFLOAT3 color;

	Vec3 position;
	Vec3 velocity;
	float massInverse;
	Vec3 forceAccumulator;

	XMFLOAT4 orientation;
	Vec3 angularMomentum;
	Vec3 angularVelocity;
	XMFLOAT3X3 inertiaTensorInverse;
	Vec3 torqueAccumulator;

	float timeStep;

	float rayTriangleIntersection(const Vec3 &rayStart, const Vec3 &rayDirection, const Vec3 &v0, const Vec3 &v1, const Vec3 &v2);
	float rayFaceIntersection(const Vec3 &rayStart, const Vec3 &rayDirection, const Vec3 &v0, const Vec3 &v1, const Vec3 &v2, const Vec3 & v3);
public:
	RigidBody(void);
	RigidBody(float x, float y, float z);

	XMFLOAT3X3 computeInertiaTensorInverse();

	void translate(float x, float y, float z);
	void setPosition(float x, float y, float z);
	Vec3 getPosition();

	void setIsFixed(bool isFixed);
	void setWidth(float width);
	float getWidth();
	void setHeight(float height);
	float getHeight();
	void setLength(float length);
	float getLength();
	void setColor(float r, float g, float b);
	XMFLOAT3 getColor();
	float getMassInverse();

	Vec3 getEdgePoint(int index);

	void setOrientation(XMFLOAT4 orientation);
	XMFLOAT4 getOrientation();

	float getRayIntersection(const Vec3 &rayStart, const Vec3 &rayDirection);
	Vec3 getVelocityAtPosition(Vec3 position);

	void clearForce();
	void clearTorque();
	
	void addForce(Vec3 force);
	void addTorque(Vec3 torque);

	void eulerStep();
	void updateVnL(float impulse, Vec3 normal, Vec3 collisionPosition);
};

