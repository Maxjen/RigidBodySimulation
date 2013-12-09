#pragma once

#include "Vec3.h"

class Contact
{
public:
	Vec3 position;
	Vec3 normal;
	float depth;
	int bodyI, bodyJ;
	float vRel;

	/*Contact(void);
	~Contact(void);*/
};

