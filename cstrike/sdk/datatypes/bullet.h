#pragma once

#include <cstdint>
#include "vector.h"

struct BulletInfo_t
{

	BulletInfo_t()
	{
		position = Vector_t(0, 0, 0);
		time_stamp = 0;
		expire_time = -1;
	}

	BulletInfo_t(Vector_t position, float time_stamp, float expire_time)
	{
		position = position;
		time_stamp = time_stamp;
		expire_time = expire_time;
	}

	Vector_t	position{};
	float		time_stamp{};
	float		expire_time{};
};

class C_CSPlayerBulletServices
{
public:
	//SCHEMA(CCSPlayer_BulletServices, m_totalHitsOnServer, m_total_hits_on_server, int);
public:
	int m_BulletCount()
	{
		return *reinterpret_cast<int*>(std::uintptr_t(this) + 0x48);
	}
};