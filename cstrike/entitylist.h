#pragma once

#include "utilities/fnv1a.h"
#include "sdk/entity_handle.h"
#include <vector>
#include <iostream>

class C_BaseEntity;
class CEntityInstance;
enum EEntityType : unsigned int
{
    ENTITY_INVALID = 0,
    ENTITY_PLAYER,
    ENTITY_GRENADE,
    ENTITY_INFERNO,
    ENTITY_WEAPON,
    ENTITY_PLANTED_C4,
    ENTITY_CHICKEN,
    ENTITY_FISH,
    ENTITY_POSTPROCESSING,
    ENTITY_FOG,
    ENTITY_GRADIENT_FOG
};

struct EntityObject_t
{
    EntityObject_t(C_BaseEntity* pEntity, int nIndex, EEntityType eEntityType, FNV1A_t uHashedName, float flSpawnTime = 0.0f) :
        m_pEntity(pEntity), m_nIndex(nIndex), m_eType(eEntityType), m_uHashedName(uHashedName), m_flSpawnTime(flSpawnTime) { }

    C_BaseEntity* m_pEntity = nullptr;
    int			  m_nIndex = 0;
    EEntityType	  m_eType = EEntityType::ENTITY_INVALID;
    FNV1A_t       m_uHashedName = FNV1A::Hash("");
    float         m_flSpawnTime = 0.0f;
};

namespace EntityList
{
    // handle new entities
    void OnAddEntity(CEntityInstance* pInstance, CBaseHandle uHandle);
    // handle removed entities
    void OnRemoveEntity(CEntityInstance* pInstance, CBaseHandle uHandle);
    // force update entity list 
    void ForceUpdateEntityList();

    // entity list container
    inline std::vector<EntityObject_t> vecEntities;
}