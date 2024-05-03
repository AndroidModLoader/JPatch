// Limit particles
uintptr_t AddBulletImpactFx_BackTo;
eBulletFxType nLimitWithSparkles = BULLETFX_NOTHING;
DECL_HOOK(eBulletFxType, GetBulletFx_Limited, void* surfInfos, uint32_t surfId)
{
    static uint32_t nextHeavyParticleTick = 0;
    eBulletFxType nParticlesType = GetBulletFx_Limited(surfInfos, surfId);
    if(nParticlesType == BULLETFX_SAND || nParticlesType == BULLETFX_DUST)
    {
        if(nextHeavyParticleTick < *m_snTimeInMilliseconds)
        {
            nextHeavyParticleTick = *m_snTimeInMilliseconds + 100;
        }
        else
        {
            return nLimitWithSparkles;
        }
    }
    return nParticlesType;
}

// Madd Dogg's Mansion Basketball glitch
DECL_HOOK(ScriptHandle, GenerateNewPickup_MaddDogg, float x, float y, float z, int16_t modelId, ePickupType pickupType, int ammo, int16_t moneyPerDay, bool isEmpty, const char* msg)
{
    if(modelId == 1277 && x == 1263.05f && y == -773.67f && z == 1091.39f)
    {
        return GenerateNewPickup_MaddDogg(1291.2f, -798.0f, 1089.39f, modelId, pickupType, ammo, moneyPerDay, isEmpty, msg);
    }
    return GenerateNewPickup_MaddDogg(x, y, z, modelId, pickupType, ammo, moneyPerDay, isEmpty, msg);
}

// Fix Star bribe which is missing minus sign in X coordinate and spawns inside the building
DECL_HOOK(ScriptHandle, GenerateNewPickup_SFBribe, float x, float y, float z, int16_t modelId, ePickupType pickupType, int ammo, int16_t moneyPerDay, bool isEmpty, const char* msg)
{
    if(modelId == 1247 && (int)x == -2120 && (int)y == 96)
    {
        return GenerateNewPickup_SFBribe(2120.0f, y, z, modelId, pickupType, ammo, moneyPerDay, isEmpty, msg);
    }
    return GenerateNewPickup_SFBribe(x, y, z, modelId, pickupType, ammo, moneyPerDay, isEmpty, msg);
}

// Fix a Rifle weapon pickup that is located inside the stadium wall since beta
DECL_HOOK(ScriptHandle, GenerateNewPickup_SFRiflePickup, float x, float y, float z, int16_t modelId, ePickupType pickupType, int ammo, int16_t moneyPerDay, bool isEmpty, const char* msg)
{
    if(modelId == 357 && x == -2094.0f && y == -488.0f)
    {
        return GenerateNewPickup_SFRiflePickup(x, -490.2f, z, modelId, pickupType, ammo, moneyPerDay, isEmpty, msg);
    }
    return GenerateNewPickup_SFRiflePickup(x, y, z, modelId, pickupType, ammo, moneyPerDay, isEmpty, msg);
}

// Do not drop-off jetpack in air
DECL_HOOKv(DropJetPackTask, void* task, CPed* ped)
{
    if(!ped->m_PedFlags.bIsStanding) return;
    DropJetPackTask(task, ped);
}

// Immediately leave the car
DECL_HOOK(bool, CanPedStepOutCar, CVehicle* self, bool bIgnoreSpeedUpright)
{
    float lookupAngle = self->m_matrix->at.z;
    if((lookupAngle <= 0.1f && lookupAngle >= -0.1f) ||
       (self->m_nVehicleType != eVehicleType::VEHICLE_TYPE_BOAT && !bIgnoreSpeedUpright))
    {
        if(fabs(self->m_vecMoveSpeed.z) > 0.05f || self->m_vecMoveSpeed.Magnitude2D() >= 0.01f) return false;
    }
    return true;
}

// Died penalty
uintptr_t DiedPenalty_BackTo;
extern "C" void DiedPenalty(void)
{
    if(WorldPlayers[0].Score > 0)
    {
        WorldPlayers[0].Score = (WorldPlayers[0].Score - 100) < 0 ? 0 : (WorldPlayers[0].Score - 100);
    }
    ClearPedWeapons(WorldPlayers[0].pPed);
}
__attribute__((optnone)) __attribute__((naked)) void DiedPenalty_Inject(void)
{
    asm("STR X1, [SP, #-16]!");
    asm volatile("BL DiedPenalty");
    asm volatile("MOV X16, %0" :: "r"(DiedPenalty_BackTo));
    asm("LDR X1, [SP], #16\nBR X16");
}

// AimingRifleWalkFix
DECL_HOOKv(ControlGunMove, void* self, CVector2D* vec2D)
{
    float save = *ms_fTimeStep; *ms_fTimeStep = fMagic;
    ControlGunMove(self, vec2D);
    *ms_fTimeStep = save;
}

// Water physics fix
// https://github.com/gta-reversed/gta-reversed-modern/blob/163ddc6ab22181004afd57e017618d9e3953a734/source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp#L440
#define SWIMSPEED_FIX
DECL_HOOKv(ProcessSwimmingResistance, CTaskSimpleSwim* task, CPed* ped)
{
    float fSubmergeZ = -1.0f;
    CVector vecPedMoveSpeed{};

    switch (task->m_nSwimState)
    {
        case SWIM_TREAD:
        case SWIM_SPRINT:
        case SWIM_SPRINTING: {
            float fAnimBlendSum = 0.0f;
            float fAnimBlendDifference = 1.0f;

            CAnimBlendAssociation* animSwimBreast = RpAnimBlendClumpGetAssociationU(ped->m_pRwClump, ANIM_ID_SWIM_BREAST);
            if (animSwimBreast) {
                fAnimBlendSum = 0.4f * animSwimBreast->m_fBlendAmount;
                fAnimBlendDifference = 1.0f - animSwimBreast->m_fBlendAmount;
            }

            CAnimBlendAssociation* animSwimCrawl = RpAnimBlendClumpGetAssociationU(ped->m_pRwClump, ANIM_ID_SWIM_CRAWL);
            if (animSwimCrawl) {
                fAnimBlendSum += 0.2f * animSwimCrawl->m_fBlendAmount;
                fAnimBlendDifference -= animSwimCrawl->m_fBlendAmount;
            }
            if (fAnimBlendDifference < 0.0f) {
                fAnimBlendDifference = 0.0f;
            }

            fSubmergeZ = fAnimBlendDifference * 0.55f + fAnimBlendSum;

            vecPedMoveSpeed =  ped->m_vecAnimMovingShiftLocal.x * ped->GetRight();
            vecPedMoveSpeed += ped->m_vecAnimMovingShiftLocal.y * ped->GetForward();
            break;
        }
        case SWIM_DIVE_UNDERWATER: {
            vecPedMoveSpeed =  ped->m_vecAnimMovingShiftLocal.x * ped->GetRight();
            vecPedMoveSpeed += ped->m_vecAnimMovingShiftLocal.y * ped->GetForward();

            auto animSwimDiveUnder = RpAnimBlendClumpGetAssociationU(ped->m_pRwClump, ANIM_ID_SWIM_DIVE_UNDER);
            if (animSwimDiveUnder) {
                vecPedMoveSpeed.z = animSwimDiveUnder->m_fCurrentTime / animSwimDiveUnder->m_pAnimBlendHierarchy->m_fTotalTime * 
                #ifndef SWIMSPEED_FIX
                    -0.1f;
                #else
                    (-0.1f * (*ms_fTimeStep / fMagic));
                #endif
            }
            break;
        }
        case SWIM_UNDERWATER_SPRINTING: {
            vecPedMoveSpeed   =  ped->m_vecAnimMovingShiftLocal.x * ped->GetRight();
            vecPedMoveSpeed   += cosf(task->m_fRotationX) * ped->m_vecAnimMovingShiftLocal.y * ped->GetForward();
            vecPedMoveSpeed.z += (sinf(task->m_fRotationX) * ped->m_vecAnimMovingShiftLocal.y + 0.01f)
            #ifdef SWIMSPEED_FIX
                / (*ms_fTimeStep / fMagic)
            #endif
            ;
            break;
        }
        case SWIM_BACK_TO_SURFACE: {
            auto animClimb = RpAnimBlendClumpGetAssociationU(ped->m_pRwClump, ANIM_ID_CLIMB_JUMP);
            if (!animClimb)
                animClimb = RpAnimBlendClumpGetAssociationU(ped->m_pRwClump, ANIM_ID_SWIM_JUMPOUT);

            if (animClimb) {
                if (animClimb->m_pAnimBlendHierarchy->m_fTotalTime > animClimb->m_fCurrentTime &&
                    (animClimb->m_fBlendAmount >= 1.0f || animClimb->m_fBlendDelta > 0.0f)
                ) {
                    float fMoveForceZ = GetTimeStep() * ped->m_fMass * 0.3f * 0.008f;
                    ApplyMoveForce(ped, 0.0f, 0.0f, fMoveForceZ);
                }
            }
            return;
        }
        default: {
            return;
        }
    }

    float fTheTimeStep = powf(0.9f, GetTimeStep());
    vecPedMoveSpeed *= (1.0f - fTheTimeStep)
    #ifdef SWIMSPEED_FIX
        * (fMagic / *ms_fTimeStep)
    #endif
    ;
    ped->m_vecMoveSpeed *= fTheTimeStep;
    #ifdef SWIMSPEED_FIX
        if(ped->IsPlayer()) vecPedMoveSpeed *= 1.25f;
    #endif
    ped->m_vecMoveSpeed += vecPedMoveSpeed;

    auto& pedPos = ped->GetPosition();
    bool bUpdateRotationX = true;
    CVector vecCheckWaterLevelPos = GetTimeStep() * ped->m_vecMoveSpeed + pedPos;
    float fWaterLevel = 0.0f;
    if (!GetWaterLevel(vecCheckWaterLevelPos, fWaterLevel, true, NULL)) {
        fSubmergeZ = -1.0f;
        bUpdateRotationX = false;
    } else {
        if (task->m_nSwimState != SWIM_UNDERWATER_SPRINTING || task->m_fStateChanger < 0.0f) {
            bUpdateRotationX = false;
        } else {
            if (pedPos.z + 0.65f > fWaterLevel && task->m_fRotationX > 0.7854f) {
                task->m_nSwimState = SWIM_TREAD;
                task->m_fStateChanger = 0.0f;
                bUpdateRotationX = false;
            }
        }
    }

    if (bUpdateRotationX) {
        if (task->m_fRotationX >= 0.0f) {
            if (pedPos.z + 0.65f <= fWaterLevel) {
                if (task->m_fStateChanger <= 0.001f)
                    task->m_fStateChanger = 0.0f;
                else
                    task->m_fStateChanger *= 0.95f;
            } else {
                float fMinimumSpeed = 0.05f * 0.5f;
                if (task->m_fStateChanger > fMinimumSpeed) {
                    task->m_fStateChanger *= 0.95f;
                }
                if (task->m_fStateChanger < fMinimumSpeed) {
                    task->m_fStateChanger += GetTimeStepInSeconds() / 10.0f;
                    task->m_fStateChanger = std::min(fMinimumSpeed, task->m_fStateChanger);
                }
                task->m_fRotationX += GetTimeStep() * task->m_fStateChanger;
                fSubmergeZ = (0.55f - 0.2f) * (task->m_fRotationX * 4.0f / PI) * 0.75f + 0.2f;
            }
        } else {
            if (pedPos.z - sin(task->m_fRotationX) + 0.65f <= fWaterLevel) {
                if (task->m_fStateChanger > 0.001f)
                    task->m_fStateChanger *= 0.95f;
                else
                    task->m_fStateChanger = 0.0f;
            } else {
                task->m_fStateChanger += GetTimeStepInSeconds() / 10.0f;
                task->m_fStateChanger = std::min(task->m_fStateChanger, 0.05f);
            }
            task->m_fRotationX += GetTimeStep() * task->m_fStateChanger;
        }
    }

    if (fSubmergeZ > 0.0f) {
        fWaterLevel -= fSubmergeZ + pedPos.z;
        float fTimeStepMoveSpeedZ = fWaterLevel / GetTimeStep();
        float fTimeStep = GetTimeStep() * 0.1f;
        fTimeStepMoveSpeedZ = std::clamp(fTimeStepMoveSpeedZ, -fTimeStep, fTimeStep);
        fTimeStepMoveSpeedZ -= ped->m_vecMoveSpeed.z;
        fTimeStep = GetTimeStepInSeconds();
        fTimeStepMoveSpeedZ = std::clamp(fTimeStepMoveSpeedZ, -fTimeStep, fTimeStep);
        ped->m_vecMoveSpeed.z += fTimeStepMoveSpeedZ;
    }

    if (pedPos.z < -69.0f) {
        pedPos.z = -69.0f;
        ped->m_vecMoveSpeed.z = std::max(ped->m_vecMoveSpeed.z, 0.0f);
    }
}
uintptr_t ProcessBuoyancy_BackTo;
extern "C" float ProcessBuoyancy_Patch(CPhysical* physical)
{
    if (physical->m_nType == eEntityType::ENTITY_TYPE_PED)
    {
        CPed* ped = (CPed*)physical;
        if (ped->IsPlayer()) // we only need this for player, due to swim bug
        {
            return (1.0f + ((*ms_fTimeStep / fMagic) / 1.5f)) * (*ms_fTimeStep / fMagic);
        }
    }
    return *ms_fTimeStep;
}
__attribute__((optnone)) __attribute__((naked)) void ProcessBuoyancy_Inject(void)
{
    asm volatile(
        "MOV X0, X19\n"
        "BL ProcessBuoyancy_Patch");
    asm volatile("MOV X16, %0" :: "r"(ProcessBuoyancy_BackTo));
    asm volatile(
        "FMOV S2, W0\n"
        "LDR S0, [X19 ,#0xCC]\n"
        "LDR S1, [X19 ,#0x7C]\n"
        "BR X16");
}

// Fixing a crosshair by very stupid math
float fWideScreenWidthScale, fWideScreenHeightScale;
DECL_HOOKv(DrawCrosshair)
{
    static constexpr float XSVal = 1024.0f / 1920.0f; // prev. 0.530, now it's 0.533333..3
    static constexpr float YSVal = 768.0f / 1920.0f; // unchanged :p

    CPlayerPed* player = WorldPlayers[0].pPed;
    if(player->m_WeaponSlots[player->m_nCurrentWeapon].m_eWeaponType == WEAPON_COUNTRYRIFLE)
    {
        // Weirdo logic but ok
        float save1 = *m_f3rdPersonCHairMultX; *m_f3rdPersonCHairMultX = 0.530f - 0.84f * ar43 * 0.01115f; // 0.01125f;
        float save2 = *m_f3rdPersonCHairMultY; *m_f3rdPersonCHairMultY = 0.400f + 0.84f * ar43 * 0.038f; // 0.03600f;
        DrawCrosshair();
        *m_f3rdPersonCHairMultX = save1; *m_f3rdPersonCHairMultY = save2;
        return;
    }

    float save1 = *m_f3rdPersonCHairMultX; *m_f3rdPersonCHairMultX = 0.530f - fAspectCorrection * 0.01115f; // 0.01125f;
    float save2 = *m_f3rdPersonCHairMultY; *m_f3rdPersonCHairMultY = 0.400f + fAspectCorrection * 0.038f; // 0.03600f;
    DrawCrosshair();
    *m_f3rdPersonCHairMultX = save1; *m_f3rdPersonCHairMultY = save2;
}

// Enter-Vehicle tasks
DECL_HOOKb(Patch_ExitVehicleJustDown, void* pad, bool bCheckTouch, CVehicle *pVehicle, bool bEntering, CVector *vecVehicle)
{
    if(Patch_ExitVehicleJustDown(pad, bCheckTouch, pVehicle, bEntering, vecVehicle))
    {
        CPlayerPed* player = FindPlayerPed(-1);
        if(!player) return false;

        CTask* task = GetActiveTask(&player->m_pPedIntelligence->m_taskManager);
        return (task || ( task && task->MakeAbortable(player, ABORT_PRIORITY_URGENT, NULL) ));
    }
    return false;
}