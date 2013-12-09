#include "RigidBody.h"


RigidBody::RigidBody(void)
{
}

RigidBody::RigidBody(float x, float y, float z)
{
	// Initialize values for simulation algorithm 3D

	position.x = x;
	position.y = y;
	position.z = z;

	isFixed = false;

	color.x = 0.5f;
	color.y = 0.5f;
	color.z = 0.5f;

	width = height = length = 1.0f;

	XMVECTOR r = XMQuaternionIdentity();
	orientation.x = XMVectorGetX(r);
	orientation.y = XMVectorGetY(r);
	orientation.z = XMVectorGetZ(r);
	orientation.w = XMVectorGetW(r);

	massInverse = 1.0f;
	XMStoreFloat3x3(&inertiaTensorInverse, XMMatrixIdentity());
	inertiaTensorInverse._11 = 1.0f/(12 * massInverse) * (height*height + length*length);
	inertiaTensorInverse._22 = 1.0f/(12 * massInverse) * (width*width + length*length);
	inertiaTensorInverse._33 = 1.0f/(12 * massInverse) * (width*width + height*height);
	XMStoreFloat3x3(&inertiaTensorInverse, XMMatrixInverse(nullptr, XMLoadFloat3x3(&inertiaTensorInverse)));

	velocity.zero();
	angularMomentum.zero();
	angularVelocity.zero();

	forceAccumulator.zero();
	torqueAccumulator.zero();

	timeStep = 1.0f/60;

	//XMFLOAT3X3 currentInertiaTensorInverse = calculateInertiaTensorInverse();
}

XMFLOAT3X3 RigidBody::computeInertiaTensorInverse()
{
	XMFLOAT3X3 result;
	if (!isFixed)
	{
		XMVECTOR orientationQuaternion = XMLoadFloat4(&orientation);
		XMMATRIX rot = XMMatrixRotationQuaternion(orientationQuaternion);

		orientationQuaternion = XMQuaternionInverse(orientationQuaternion);
		XMMATRIX rotInverse = XMMatrixRotationQuaternion(orientationQuaternion);

		XMMATRIX inertiaTensorInverseMatrix = XMLoadFloat3x3(&inertiaTensorInverse);
		inertiaTensorInverseMatrix = rot * inertiaTensorInverseMatrix * rotInverse;

		
		XMStoreFloat3x3(&result, inertiaTensorInverseMatrix);
	}
	else
	{
		XMStoreFloat3x3(&result, XMMatrixIdentity() * 0);
	}
	return result;
}

void RigidBody::translate(float x, float y, float z)
{
	position.x += x;
	position.y += y;
	position.z += z;
}

void RigidBody::setPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}

Vec3 RigidBody::getPosition()
{
	return position;
}

void RigidBody::setIsFixed(bool isFixed)
{
	this->isFixed = isFixed;
}

void RigidBody::setWidth(float width)
{
	this->width = width;
}

float RigidBody::getWidth()
{
	return width;
}

void RigidBody::setHeight(float height)
{
	this->height = height;
}

float RigidBody::getHeight()
{
	return height;
}

void RigidBody::setLength(float length)
{
	this->length = length;
}

float RigidBody::getLength()
{
	return length;
}

void RigidBody::setColor(float r, float g, float b)
{
	color.x = r;
	color.y = g;
	color.z = b;
}

XMFLOAT3 RigidBody::getColor()
{
	return color;
}

float RigidBody::getMassInverse()
{
	if (!isFixed)
		return massInverse;
	else
		return 0;
}

Vec3 RigidBody::getEdgePoint(int i)
{
	XMFLOAT3 result;
	result.x = result.y = result.z = 0;
	if (i == 0)
	{
		result.x -= width / 2;
		result.y -= height / 2;
		result.z += length / 2;
	}
	else if (i == 1)
	{
		result.x += width / 2;
		result.y -= height / 2;
		result.z += length / 2;
	}
	else if (i == 2)
	{
		result.x += width / 2;
		result.y += height / 2;
		result.z += length / 2;
	}
	else if (i == 3)
	{
		result.x -= width / 2;
		result.y += height / 2;
		result.z += length / 2;
	}
	else if (i == 4)
	{
		result.x -= width / 2;
		result.y -= height / 2;
		result.z -= length / 2;
	}
	else if (i == 5)
	{
		result.x += width / 2;
		result.y -= height / 2;
		result.z -= length / 2;
	}
	else if (i == 6)
	{
		result.x += width / 2;
		result.y += height / 2;
		result.z -= length / 2;
	}
	else if (i == 7)
	{
		result.x -= width / 2;
		result.y += height / 2;
		result.z -= length / 2;
	}
	XMMATRIX trans = XMMatrixTranslation(position.x, position.y, position.z);
	XMVECTOR orientationQuaternion = XMLoadFloat4(&orientation);
	XMMATRIX rot = XMMatrixRotationQuaternion(orientationQuaternion);
	XMVECTOR resultVector = XMLoadFloat3(&result);
	resultVector = XMVector3Transform(resultVector, rot * trans);
	XMStoreFloat3(&result, resultVector);
	
	return Vec3(result.x, result.y, result.z);
}

void RigidBody::setOrientation(XMFLOAT4 orientation)
{
	this->orientation = orientation;
}

XMFLOAT4 RigidBody::getOrientation()
{
	return orientation;
}

float RigidBody::rayTriangleIntersection(const Vec3 &rayStart, const Vec3 &rayDirection, const Vec3 &v0, const Vec3 &v1, const Vec3 &v2)
{
	Vec3 e1, e2, h, s, q;
	float a, f, u, v, t;
	e1 = v1 - v0;
	e2 = v2 - v0;

	h = cross(rayDirection, e2);
	a = e1 * h;

	if (a > -0.00001f && a < 0.00001)
		return -1.0f;

	f = 1 / a;
	s = rayStart - v0;
	u = f * (s * h);

	if (u < 0.0f || u > 1.0f)
		return -1.0f;

	q = cross(s, e1);
	v = f * (rayDirection * q);

	if (v < 0.0f || u + v > 1.0f)
		return -1.0f;

	t = f * (e2 * q);

	if (t > 0.00001f)
		return t;
	else
		return -1.0f;
}

float RigidBody::rayFaceIntersection(const Vec3 &rayStart, const Vec3 &rayDirection, const Vec3 &v0, const Vec3 &v1, const Vec3 &v2, const Vec3 & v3)
{
	float r1 = rayTriangleIntersection(rayStart, rayDirection, v0, v1, v2);
	float r2 = rayTriangleIntersection(rayStart, rayDirection, v2, v3, v0);

	float result;
	if (r1 != -1.0f && r2 == -1.0f)
		result = r1;
	else if (r1 == -1.0f && r2 != -1.0f)
		result = r2;
	else if (r1 == -1.0f && r2 == -1.0f)
		result = -1.0f;
	else
		result = (r1 > r2) ? r1 : r2;
	return result;
}

float RigidBody::getRayIntersection(const Vec3 &rayStart, const Vec3 &rayDirection)
{
	Vec3 vertices[8];
	for (int i = 0; i < 8; i++)
		vertices[i] = getEdgePoint(i);

	float results[6];
	results[0] = rayFaceIntersection(rayStart, rayDirection, vertices[0], vertices[1], vertices[2], vertices[3]);
	results[1] = rayFaceIntersection(rayStart, rayDirection, vertices[1], vertices[5], vertices[6], vertices[2]);
	results[2] = rayFaceIntersection(rayStart, rayDirection, vertices[5], vertices[4], vertices[7], vertices[6]);
	results[3] = rayFaceIntersection(rayStart, rayDirection, vertices[4], vertices[0], vertices[3], vertices[7]);
	results[4] = rayFaceIntersection(rayStart, rayDirection, vertices[3], vertices[2], vertices[6], vertices[7]);
	results[5] = rayFaceIntersection(rayStart, rayDirection, vertices[1], vertices[0], vertices[4], vertices[5]);

	float result = 100.0f;
	for (int i = 0; i < 6; i++)
	{
		if (results[i] != -1 && results[i] < result)
			result = results[i];
	}

	return (result < 100.0f) ? result : 100.0f;
}

Vec3 RigidBody::getVelocityAtPosition(Vec3 position)
{
	if (!isFixed)
		return velocity + cross(angularVelocity, position);
	else
		return Vec3(0, 0, 0);
}

void RigidBody::clearForce()
{
	forceAccumulator.zero();
}

void RigidBody::clearTorque()
{
	torqueAccumulator.zero();
}

void RigidBody::addForce(Vec3 force)
{
	forceAccumulator += force;
}

void RigidBody::addTorque(Vec3 torque)
{
	torqueAccumulator += torque;
}

void RigidBody::eulerStep()
{
	if (!isFixed)
	{
		// add gravity force
		forceAccumulator += 1.0f/massInverse * Vec3(0, -10, 0);

		// integrate position and velocity
		position += timeStep * velocity;
		velocity += timeStep * forceAccumulator * massInverse;

		// integrate orientation
		XMVECTOR deltaOrientationQ = XMVectorSet(angularVelocity.x, angularVelocity.y, angularVelocity.z, 0);
		XMVECTOR orientationQ = XMLoadFloat4(&orientation);
		deltaOrientationQ = XMQuaternionMultiply(deltaOrientationQ, orientationQ);
		XMVectorScale(deltaOrientationQ, timeStep/2);
		orientationQ = XMVectorAdd(orientationQ, deltaOrientationQ);
		XMQuaternionNormalize(orientationQ);
		XMStoreFloat4(&orientation, orientationQ);

		// integrate angular momentum
		angularMomentum += timeStep * torqueAccumulator;

		// compute angular velocity
		XMFLOAT3X3 currentInertiaTensorInverse = computeInertiaTensorInverse();
		XMMATRIX currentInertiaTensorInverseM = XMLoadFloat3x3(&currentInertiaTensorInverse);
		XMFLOAT3 angularMomentumF;
		angularMomentumF.x = angularMomentum.x;
		angularMomentumF.y = angularMomentum.y;
		angularMomentumF.z = angularMomentum.z;
		XMVECTOR angularMomentumV = XMLoadFloat3(&angularMomentumF);
		XMVECTOR angularVelocityV = XMVector3TransformNormal(angularMomentumV, currentInertiaTensorInverseM);
		angularVelocity.x = XMVectorGetX(angularVelocityV);
		angularVelocity.y = XMVectorGetY(angularVelocityV);
		angularVelocity.z = XMVectorGetZ(angularVelocityV);
	}
}

void RigidBody::updateVnL(float impulse, Vec3 normal, Vec3 collisionPosition)
{
	if (!isFixed)
	{
		Vec3 deltaVelocity = impulse * normal * massInverse;
		velocity += deltaVelocity;
		Vec3 deltaAngularMomentum = cross(collisionPosition, impulse * normal);
		angularMomentum += deltaAngularMomentum;
	}
}
