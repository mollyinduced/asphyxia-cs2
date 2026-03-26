//#include "stronghandle.h"
//#include "vector.h"
//
//class particle_effect
//{
//public:
//    const char* name{};
//    char pad[0x30]{};
//};
//
//class particle_information
//{
//public:
//    float time{};
//    float width{};
//    float unk2{};
//};
//
//class particle_data
//{
//public:
//    Vector_t* positions{};
//    char pad[0x74]{};
//    float* times{};
//    void* unk_ptr{};
//    char pad2[0x28];
//    float* times2{};
//    char pad3[0x98];
//    void* unk_ptr2{};
//};
//
//struct particle_color
//{
//    float r;
//    float g;
//    float b;
//};
//
//class c_particle_snapshot {
//public:
//    void draw(int count, void* data);
//};
//
//class c_game_particle_manager_system
//{
//public:
//    void create_effect_index(unsigned int* effect_index, particle_effect* effect_data);
//    void create_effect(unsigned int effect_index, int unk, void* clr, int unk2);
//    void unk_function(int effect_index, unsigned int unk, const CStrongHandle<c_particle_snapshot>* particle_snapshot);
//};
//
//class c_particle_manager
//{
//public:
//    void create_snapshot(CStrongHandle<c_particle_snapshot>* out_particle_snapshot);
//
//    void draw(CStrongHandle<c_particle_snapshot>* particle_snapshot, int count, void* data);
//};