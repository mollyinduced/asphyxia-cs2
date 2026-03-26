//#include "../../core/sdk.h"
//#include "../../core/interfaces.h"
//#include "../../sdk/datatypes/cgameparticlemanagersystem.h"
//
//void* relative_address(void* addr)
//{
//	std::uintptr_t rva = *reinterpret_cast<std::uint32_t*>(addr);
//	std::uintptr_t rip = (uintptr_t)addr + sizeof(std::uint32_t);
//
//	return (void*)(rva + rip);
//}
//
//class particle_effect
//{
//public:
//	const char* name{};
//	char pad[0x30]{};
//};
//
//class particle_information
//{
//public:
//	float time{};
//	float width{};
//	float unk2{};
//};
//
//class particle_data
//{
//public:
//	Vector_t* positions{};
//	char pad[0x74]{};
//	float* times{};
//	void* unk_ptr{};
//	char pad2[0x28];
//	float* times2{};
//	char pad3[0x98];
//	void* unk_ptr2{};
//};
//
//struct particle_color
//{
//	float r;
//	float g;
//	float b;
//};
//
//class c_particle_snapshot {
//public:
//	void draw(int count, void* data);
//};
//
//class c_game_particle_manager_system
//{
//public:
//	void create_effect_index(unsigned int* effect_index, particle_effect* effect_data);
//	void create_effect(unsigned int effect_index, int unk, void* clr, int unk2);
//	void unk_function(int effect_index, unsigned int unk, const CStrongHandle<c_particle_snapshot>* particle_snapshot);
//};
//
//class c_particle_manager
//{
//public:
//	void create_snapshot(CStrongHandle<c_particle_snapshot>* out_particle_snapshot);
//
//	void draw(CStrongHandle<c_particle_snapshot>* particle_snapshot, int count, void* data);
//};
//
//std::vector<tracer_info> bullets{};
//
//void add_bullet_trace(Vector_t start, Vector_t end, Color_t clr_)
//{
//	auto& bullet = bullets.emplace_back();
//
//	if (!I::GameParticleManagerSystem) {
//		using fn_t = c_game_particle_manager_system * (__fastcall*)();
//		fn_t fn = (fn_t)MEM::GetAbsoluteAddress(MEM::FindPattern(CLIENT_DLL, "E8 ? ? ? ? 41 B0 ? 8B D3"), 0x1);
//		I::GameParticleManagerSystem = fn();
//	}
//
//	if (!I::ParticleManager) {
//		void* penis = MEM::FindPattern(CLIENT_DLL, "48 8B 0D ? ? ? ? 48 89 44 24 28 4C 89 4C 24 20");
//		I::ParticleManager = (c_particle_manager*)MEM::ResolveRelativeAddress((uint8_t*)penis, 0x3, 0x7);
//	}
//
//	particle_effect particle_effect_{};
//	particle_effect_.name = "particles/entity/spectator_utility_trail.vpcf";
//	particle_effect_.pad[0] = 8;
//	I::GameParticleManagerSystem->create_effect_index(&bullet.effect_index, &particle_effect_);
//	particle_color clr = { float(clr_.r), float(clr_.g), float(clr_.b) };
//	I::GameParticleManagerSystem->create_effect(bullet.effect_index, 16, &clr, 0);
//
//	bullet.particle_data_ = {};
//
//	auto dir = (end - start);
//	auto stage_2 = start + (dir * 0.3f);
//	auto stage_3 = start + (dir * 0.5f);
//
//	Vector_t positions_[] = { start,stage_2, stage_3,end };
//
//	for (int i{}; i < sizeof(positions_) / sizeof(Vector_t); i++) {
//
//		particle_information particle_info{};
//		particle_info.time = 4.f;
//		particle_info.width = 1.5f;
//		particle_info.unk2 = 1.f;
//		I::GameParticleManagerSystem->create_effect(bullet.effect_index, 3, &particle_info, 0);
//
//		bullet.positions = new Vector_t[i + 1];
//		bullet.times = new float[i + 1];
//
//		for (int j{}; j < i + 1; j++) {
//			bullet.positions[j] = positions_[j];
//			bullet.times[j] = TICK_INTERVAL * float(j);
//		}
//
//		bullet.particle_data_.positions = bullet.positions;
//		bullet.particle_data_.times2 = bullet.times;
//
//		I::ParticleManager->create_snapshot(&bullet.handle_snapshot_particle);
//
//		I::GameParticleManagerSystem->unk_function(bullet.effect_index, 0, &bullet.handle_snapshot_particle);
//		I::ParticleManager->draw(&bullet.handle_snapshot_particle, i + 1, &bullet.particle_data_);
//
//		delete[] bullet.positions;
//		delete[] bullet.times;
//	}
//}