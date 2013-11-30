#include "RigidBody.h"


RigidBody::RigidBody(void)
{
}

RigidBody::RigidBody(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;

	color.x = 0.5f;
	color.y = 0.5f;
	color.z = 0.5f;

	width = height = length = 1.0f;

	//XMVECTOR r = XMQuaternionRotationMatrix(XMMatrixIdentity());
	XMVECTOR r = XMQuaternionIdentity();
	orientation.x = XMVectorGetX(r);
	orientation.y = XMVectorGetY(r);
	orientation.z = XMVectorGetZ(r);
	orientation.w = XMVectorGetW(r);

	mass = 1.0f;
	XMStoreFloat3x3(&inertiaTensorInverse, XMMatrixIdentity());
	inertiaTensorInverse._11 = 1.0f/12 * mass * (height*height + length*length);
	inertiaTensorInverse._22 = 1.0f/12 * mass * (width*width + length*length);
	inertiaTensorInverse._33 = 1.0f/12 * mass * (width*width + height*height);
	XMStoreFloat3x3(&inertiaTensorInverse, XMMatrixInverse(nullptr, XMLoadFloat3x3(&inertiaTensorInverse)));
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
	resultVector = XMVector3Transform(resultVector, trans * rot);
	XMStoreFloat3(&result, resultVector);
	
	return Vec3(result.x, result.y, result.z);
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
		vertices[i] = position;

	vertices[0].x -= width / 2;
	vertices[0].y -= height / 2;
	vertices[0].z += length / 2;

	vertices[1].x += width / 2;
	vertices[1].y -= height / 2;
	vertices[1].z += length / 2;

	vertices[2].x += width / 2;
	vertices[2].y += height / 2;
	vertices[2].z += length / 2;

	vertices[3].x -= width / 2;
	vertices[3].y += height / 2;
	vertices[3].z += length / 2;

	vertices[4].x -= width / 2;
	vertices[4].y -= height / 2;
	vertices[4].z -= length / 2;

	vertices[5].x += width / 2;
	vertices[5].y -= height / 2;
	vertices[5].z -= length / 2;

	vertices[6].x += width / 2;
	vertices[6].y += height / 2;
	vertices[6].z -= length / 2;

	vertices[7].x -= width / 2;
	vertices[7].y += height / 2;
	vertices[7].z -= length / 2;

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

	/*Vec3 v0, v1, v2;
	v0 = v1 = v2 = position;

	v0.x -= width / 2;
	v0.y -= height / 2;
	v0.z += length / 2;

	v1.x += width / 2;
	v1.y -= height / 2;
	v1.z += length / 2;

	v2.x += width / 2;
	v2.y += height / 2;
	v2.z += length / 2;

	return rayTriangleIntersection(rayStart, rayDirection, v0, v1, v2);*/
}

void RigidBody::step()
{
	/*rotX += 2 * 3.14159265f / 60;

	while (rotX >= 2 * 3.14159265f)
		rotX -= 2 * 3.14159265f;*/
}
