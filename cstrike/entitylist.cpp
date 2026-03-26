#include "entitylist.h"
#include "sdk/entity.h"

#include "core/interfaces.h"
#include "sdk/interfaces/igameresourceservice.h"
#include "sdk/interfaces/cgameentitysystem.h"

void EntityList::OnAddEntity(CEntityInstance* pInstance, CBaseHandle uHandle)
{
    const int nIndex = uHandle.GetEntryIndex();
    if (nIndex < 0)
        return;

    // isn't valid or networked
    C_BaseEntity* pEntity = reinterpret_cast<C_BaseEntity*>(pInstance);
    if (!pEntity || uHandle.GetEntryIndex() > 0x3FFF || pEntity->GetRefEHandle() != uHandle)
        return;

    SchemaClassInfoData_t* pClassInfo = nullptr;
    pEntity->GetSchemaClassInfo(&pClassInfo);
    if (!pClassInfo)
        return;

    const FNV1A_t uHashedName = FNV1A::Hash(pClassInfo->szName);
    switch (uHashedName)
    {
    case FNV1A::HashConst("CCSPlayerController"):
        vecEntities.emplace_back(EntityObject_t(pEntity, nIndex, EEntityType::ENTITY_PLAYER, uHashedName));
        break;

    //case FNV1A::HashConst("C_HEGrenadeProjectile"):
    //case FNV1A::HashConst("C_FlashbangProjectile"):
    //case FNV1A::HashConst("C_SmokeGrenadeProjectile"):
    //case FNV1A::HashConst("C_DecoyProjectile"):
    //case FNV1A::HashConst("C_MolotovProjectile"):
    //    vecEntities.emplace_back(EntityObject_t(pEntity, uHandle, nIndex, EEntityType::ENTITY_GRENADE, uHashedName, Interfaces::m_pGlobalVariables->m_flCurrentTime));
    //    break;

    //case FNV1A::HashConst("C_Inferno"):
    //    vecEntities.emplace_back(EntityObject_t(pEntity, uHandle, nIndex, EEntityType::ENTITY_INFERNO, uHashedName, Interfaces::m_pGlobalVariables->m_flCurrentTime));
    //    break;

    //case FNV1A::HashConst("C_PlantedC4"):
    //    vecEntities.emplace_back(EntityObject_t(pEntity, uHandle, nIndex, EEntityType::ENTITY_PLANTED_C4, uHashedName, Interfaces::m_pGlobalVariables->m_flCurrentTime));
    //    break;

    //case FNV1A::HashConst("C_Chicken"):
    //    vecEntities.emplace_back(EntityObject_t(pEntity, uHandle, nIndex, EEntityType::ENTITY_CHICKEN, uHashedName));
    //    break;

    //case FNV1A::HashConst("C_Fish"):
    //    vecEntities.emplace_back(EntityObject_t(pEntity, uHandle, nIndex, EEntityType::ENTITY_FISH, uHashedName));
    //    break;

    //case FNV1A::HashConst("C_PostProcessingVolume"):
    //    vecEntities.emplace_back(EntityObject_t(pEntity, uHandle, nIndex, EEntityType::ENTITY_POSTPROCESSING, uHashedName));
    //    break;

    //case FNV1A::HashConst("C_EnvCubemapFog"):
    //    vecEntities.emplace_back(EntityObject_t(pEntity, uHandle, nIndex, EEntityType::ENTITY_FOG, uHashedName));
    //    break;

    //case FNV1A::HashConst("C_GradientFog"):
    //    vecEntities.emplace_back(EntityObject_t(pEntity, uHandle, nIndex, EEntityType::ENTITY_GRADIENT_FOG, uHashedName));
    //    break;

    default:
        //std::cout << pClassInfo->szName << std::endl;
        break; // could perhaps return here, will give same result as breaking since there is no more code after this switch statement
    }
}

void EntityList::OnRemoveEntity(CEntityInstance* pInstance, CBaseHandle uHandle)
{
    const int nIndex = uHandle.GetEntryIndex();
    if (nIndex < 0)
        return;

    // isn't valid or networked
    C_BaseEntity* pEntity = reinterpret_cast<C_BaseEntity*>(pInstance);
    if (pEntity == nullptr || uHandle.GetEntryIndex() > 0x3FFF)
        return;

    // get current entity entry
    const auto current = std::ranges::find_if(vecEntities, [&](const EntityObject_t& entry) { return entry.m_nIndex == nIndex; });

    if (current == vecEntities.end())
        return;

    vecEntities.erase(current);
}

void EntityList::ForceUpdateEntityList()
{
    // clear
    vecEntities.clear();

    for (int i = 0; i < I::GameResourceService->pGameEntitySystem->GetHighestEntityIndex(); i++)
    {
        C_BaseEntity* pEntity = I::GameResourceService->pGameEntitySystem->Get(i);
        if (!pEntity || !pEntity->GetIdentity() || !pEntity->GetIdentity()->pInstance || !pEntity->GetRefEHandle().IsValid())
            continue;

        OnAddEntity(pEntity->GetIdentity()->pInstance, pEntity->GetRefEHandle());
    }
}