#include "cgameparticlemanagersystem.h"
#include "../../utilities/memory.h"

//void c_particle_snapshot::draw(int count, void* data) {
//    MEM::CallVFunc<void, 1U>(this, count, data);
//}
//
//void c_game_particle_manager_system::create_effect_index(unsigned int* effect_index, particle_effect* effect_data) {
//    using fn_t = void(__fastcall*)(c_game_particle_manager_system*, unsigned int*, particle_effect*);
//    static fn_t fn = (fn_t)MEM::FindPattern(CLIENT_DLL, "40 57 48 83 EC 20 49 8B 00");
//    fn(this, effect_index, effect_data);
//}
//
//void c_game_particle_manager_system::create_effect(unsigned int effect_index, int unk, void* clr, int unk2) {
//    using fn_t = void(__fastcall*)(c_game_particle_manager_system*, unsigned int, int, void*, int);
//    static fn_t fn = (fn_t)MEM::FindPattern(CLIENT_DLL, "48 83 EC 58 F3 41 0F 10 51");
//    fn(this, effect_index, unk, clr, unk2);
//}
//
//void c_game_particle_manager_system::unk_function(int effect_index, unsigned int unk, const CStrongHandle<c_particle_snapshot>* particle_snapshot) {
//    using fn_t = void(__fastcall*)(c_game_particle_manager_system*, int, unsigned int, const CStrongHandle<c_particle_snapshot>*);
//    static fn_t fn = (fn_t)MEM::FindPattern(CLIENT_DLL, "48 89 74 24 10 57 48 83 EC 30 4C 8B D9 49 8B F9 33 C9 41 8B F0 83 FA FF 0F");
//    fn(this, effect_index, unk, particle_snapshot);
//}
//
//void c_particle_manager::create_snapshot(CStrongHandle<c_particle_snapshot>* out_particle_snapshot)
//{
//    __int64 nUnknown = 0;
//    MEM::CallVFunc<void, 42>(this, out_particle_snapshot, &nUnknown);
//}
//
//void c_particle_manager::draw(CStrongHandle<c_particle_snapshot>* particle_snapshot, int count, void* data)
//{
//    MEM::CallVFunc<void, 43>(this, particle_snapshot, count, data);
//}