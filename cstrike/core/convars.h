#pragma once

class CConVar;

namespace CONVAR
{
	// dump convars to file
	bool Dump(const wchar_t* wszFileName);
	// setup convars
	bool Setup();

	inline CConVar* m_pitch = nullptr;
	inline CConVar* m_yaw = nullptr;
	inline CConVar* sensitivity = nullptr;

	inline CConVar* game_type = nullptr;
	inline CConVar* game_mode = nullptr;

	inline CConVar* mp_teammates_are_enemies = nullptr;

	inline CConVar* sv_autobunnyhopping = nullptr;
	
	inline CConVar* sv_airaccelerate = nullptr;

	inline CConVar* cam_idealdist = nullptr;

	inline CConVar* cam_collision = nullptr;

	inline CConVar* cam_snapto = nullptr;

	inline CConVar* c_thirdpersonshoulder = nullptr;

	inline CConVar* c_thirdpersonshoulderaimdist = nullptr;

	inline CConVar* c_thirdpersonshoulderdist = nullptr;

	inline CConVar* c_thirdpersonshoulderheight = nullptr;

	inline CConVar* c_thirdpersonshoulderoffset = nullptr;

	inline CConVar* cl_interpolate = nullptr;

	inline CConVar* cl_interp_ratio = nullptr;

	inline CConVar* sv_friction = nullptr;

	inline CConVar* sv_accelerate = nullptr;

	inline CConVar* weapon_accuracy_shotgun_spread_patterns = nullptr;
	
	inline CConVar* weapon_accuracy_nospread = nullptr;
	
	inline CConVar* sv_maxunlag = nullptr;
}
