bool bDynStreamingMem;
float fDynamicStreamingMemPercentage;
DECL_HOOKv(GameProcess)
{
    GameProcess();

    if(bDynStreamingMem)
    {
        if(*ms_memoryAvailable < nMaxStreamingMemForDynamic)
        {
            float memUsedPercent = (float)*ms_memoryUsed / (float)*ms_memoryAvailable;
            if(memUsedPercent >= fDynamicStreamingMemPercentage)
            {
                BumpStreamingMemory(nDynamicStreamingMemBumpStep);
            }
        }
    }
}

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
                    (-0.1f * GetTimeStepMagic());
                #endif
            }
            break;
        }
        case SWIM_UNDERWATER_SPRINTING: {
            vecPedMoveSpeed   =  ped->m_vecAnimMovingShiftLocal.x * ped->GetRight();
            vecPedMoveSpeed   += cosf(task->m_fRotationX) * ped->m_vecAnimMovingShiftLocal.y * ped->GetForward();
            vecPedMoveSpeed.z += (sinf(task->m_fRotationX) * ped->m_vecAnimMovingShiftLocal.y + 0.01f)
            #ifdef SWIMSPEED_FIX
                / GetTimeStepMagic()
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
        * GetTimeStepInvMagic()
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
float *buoyancyTimescaleReplacement;
DECL_HOOKb(ProcessPedBuoyancy, void *self, CPed *pPed, float fBouyConst, CVector *pCentreOfBuoyancy, CVector *pBuoyancyForce)
{
    if (pPed->IsPlayer())
    {
        *buoyancyTimescaleReplacement = (1.0f + (GetTimeStepMagic() / 1.5f)) * GetTimeStepMagic();
    }
    else
    {
        *buoyancyTimescaleReplacement = *ms_fTimeStep;
    }
    bool ret = ProcessPedBuoyancy(self, pPed, fBouyConst, pCentreOfBuoyancy, pBuoyancyForce);
    *buoyancyTimescaleReplacement = *ms_fTimeStep;
    return ret;
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
        if(!task) return true;

        eTaskType type = task->GetTaskType();
        return type != TASK_SIMPLE_JETPACK && type != TASK_SIMPLE_GANG_DRIVEBY && type != TASK_COMPLEX_EVASIVE_DIVE_AND_GET_UP &&
               type != TASK_SIMPLE_NAMED_ANIM && task->MakeAbortable(player, ABORT_PRIORITY_URGENT, NULL);
    }
    return false;
}

// Fixes farclip glitch with wall (wardumb be like)
DECL_HOOKv(DistanceFogSetup_FogWall, float minDistance, float maxDistance, float red, float green, float blue)
{
    DistanceFogSetup_FogWall(0.8f * minDistance, 0.95f * maxDistance, red, green, blue);
}

// Wrong vehicle's parts colors!
DECL_HOOKv(ObjectRender_VehicleParts, CObject* self)
{
    ObjectRender_VehicleParts(self);
    if(self->m_nParentModelIndex != -1 && self->objectFlags.bChangesVehColor && self->ObjectCreatedBy == eObjectType::OBJECT_TEMPORARY)
    {
        if(self->m_pRwAtomic && self->m_pRwAtomic->object.object.type == 1)
        {
            auto ptr = gStoredMats;
            while(ptr->material != NULL)
            {
                ptr->material->texture = ptr->texture;
                ++ptr;
            }
            gStoredMats->material = NULL;
        }
    }
}

// Stunt smoke
DECL_HOOKb(Plane_ProcessControl_Horn, int a1)
{
    return WidgetIsTouched(7, NULL, 2);
}

// Falling Star
DECL_HOOKv(RenderState_Star, int a1, int a2)
{
    RenderState_Star(a1, a2);
    RenderState_Star(1, 0);
}

// Jetpack hover
DECL_HOOKb(Jetpack_IsHeldDown, int id, int enableWidget)
{
    static bool holdTheButton = false;
    if(Touch_IsDoubleTapped(WIDGETID_VEHICLEEXHAUST, true, 1))
    {
        memset(m_pWidgets[WIDGETID_VEHICLEEXHAUST]->tapTimes, 0, sizeof(float)*10); // CWidget::ClearTapHistory in a better way
        holdTheButton = !holdTheButton;
        return holdTheButton;
    }
    return WidgetIsTouched(WIDGETID_VEHICLEEXHAUST, NULL, 1) ^ holdTheButton;
}

// Fixing a wrong value in carcols.dat
DECL_HOOK(int, CarColsDatLoad_sscanf, const char* str, const char* fmt, int *v1, int *v2, int *v3)
{
    if(CarColsDatLoad_sscanf(str, fmt, v1, v2, v3) != 3)
    {
        return CarColsDatLoad_sscanf(str, "%d.%d %d", v1, v2, v3);
    }
    return 3;
}

// Fix planes generation coordinates
DECL_HOOKb(FindPlaneCoors_CheckCol, int X, int Y, CColBox* box, CColSphere* sphere, CColSphere* A, CColSphere* B)
{
    return FindPlaneCoors_CheckCol(GetSectorForCoord(X), GetSectorForCoord(Y), box, sphere, A, B);
}

// Now CJ is able to exit a vehicle and start moving immediately, without being forced to close the door
DECL_HOOKb(DoorClosing_PadTarget, void* pad)
{
    return DoorClosing_PadTarget(pad) || GetPedWalkLR(pad) != 0x00 || GetPedWalkUD(pad) != 0x00;
}

// Peepo: Fix traffic lights
DECL_HOOKv(TrFix_RenderEffects)
{
    TrFix_RenderEffects();
    BrightLightsRender();
}
DECL_HOOKv(TrFix_InitGame2nd, const char* a1)
{
    TrFix_InitGame2nd(a1);
    BrightLightsInit();
}

// Money have 8 digits now? Exciting!
DECL_HOOKi(DrawMoney_sprintf, char* buf, const char* fmt, int arg1)
{
    static const char* positiveT = "$%08d";
    static const char* negativeT = "-$%07d";

    const char* newFmt = (arg1 >= 0) ? positiveT : negativeT;
    return DrawMoney_sprintf(buf, newFmt, arg1);
}

// ClimbDie
DECL_HOOK(bool, ClimbProcessPed, CTask* self, CPed* target)
{
    float save = *ms_fTimeStep; *ms_fTimeStep = fMagic;
    bool ret = ClimbProcessPed(self, target);
    *ms_fTimeStep = save;
    return ret;
}

// Colorpicker
DECL_HOOK(float, GetColorPickerValue, CWidgetRegionColorPicker* self)
{
    static float prevVal = 0.0f;
    if(self->IsTouched(NULL) != false) // IsTouched
    {
        CVector2D v = m_vecCachedPos[self->cachedPosNum];
        
        float left = self->screenRect.left * 0.7f;
        float right = self->screenRect.right * 0.7f;

        float bottom = self->screenRect.bottom * 0.8f;
        float top = self->screenRect.top * 0.8f;

        if(v.x < left || v.x > right || v.y < top || v.y > bottom) return prevVal;

        float ret = (int)(
            8.0f * ((v.x - left) / (right - left)) + 8 * (int)(
            8.0f * ((v.y - top) / (bottom - top))) );
        prevVal = ret;

        return ret;
    }
    return 0.0f;
}

// Fixes emergency vehicles
uintptr_t EmergencyVeh_BackTo;
__attribute__((optnone)) __attribute__((naked)) void EmergencyVeh_Inject(void)
{
    asm volatile(
        "fmov s0, %w0\n"
    :: "r" (fEmergencyVehiclesFix));
    asm volatile(
        "mov x8, %0\n"
        "br x8\n"
    :: "r" (EmergencyVeh_BackTo));
}
DECL_HOOKv(SetFOV_Emergency, float factor, bool unused)
{
    // Someone is using broken mods
    // So here is the workaround + a little value clamping
    if(factor < 1.0f)
    {
        fEmergencyVehiclesFix = 70.0f / 1.0f;
    }
    else if(factor > 170.0f)
    {
        fEmergencyVehiclesFix = 70.0f / 170.0f;
    }
    else
    {
        fEmergencyVehiclesFix = 70.0f / factor;
    }
    SetFOV_Emergency(factor, unused);
}

// PS2 coronas
DECL_HOOKv(RenderOneXLUSprite_Rotate_Aspect_PS2, CVector pos, CVector2D size, uint8_t r, uint8_t g, uint8_t b, int16_t intensity, float rz, float rotation, uint8_t alpha)
{
    RenderOneXLUSprite_Rotate_Aspect_PS2(pos, size, r, g, b, intensity, rz * 0.05f, rz, alpha);
}

// SilentPatch`s fix
DECL_HOOKv(SetLightsWithTimeOfDayColour_DirLight)
{
    *m_vecDirnLightToSun = m_VectorToSun[*m_CurrentStoredValue];
    SetLightsWithTimeOfDayColour_DirLight();
}

// FX particles distance multiplier!
float fxMultiplier;
DECL_HOOKv(LoadFX_sscanf, char* buf, char* fmt, char* randomAssBuffer, float* readVal)
{
    LoadFX_sscanf(buf, fmt, randomAssBuffer, readVal);
    *readVal *= fxMultiplier;
}

// Spread fix
DECL_HOOK(bool, FireInstantHit, CWeapon *self, CEntity *a2, CVector *a3, CVector *a4, CEntity *a5, CVector *a6, CVector *a7, int a8, int a9)
{
    *fPlayerAimRotRate = (rand() * 2.0f * M_PI) / (float)RAND_MAX;
    return FireInstantHit(self, a2, a3, a4, a5, a6, a7, a8, a9);
}

// SunGlare
DECL_HOOKv(RenderVehicle_SunGlare, CVehicle* self)
{
    RenderVehicle_SunGlare(self);
    DoSunGlare(self);
}

// Interior radar
DECL_HOOKv(DrawRadar, void* self)
{
    if(*currArea == 0 || IsOnAMission())
    {
        DrawRadar(self);
    }
}

// Green-ish detail texture
#define GREEN_TEXTURE_ID 14
inline void* GetDetailTexturePtr(int texId)
{
    return *(void**)((uintptr_t)(&(detailTextures->data[texId - 1]->raster->parent)) + *RasterExtOffset);
}
DECL_HOOKv(emu_TextureSetDetailTexture, RwTexture* texture, unsigned int tilingScale)
{
    if(texture == NULL)
    {
        emu_TextureSetDetailTexture(NULL, 0);
        return;
    }
    if(texture == GetDetailTexturePtr(GREEN_TEXTURE_ID))
    {
        *textureDetail = 0;
        emu_TextureSetDetailTexture(NULL, 0);
        return;
    }
    emu_TextureSetDetailTexture(texture, tilingScale);
    *textureDetail = 1;
}

// Random license plates
DECL_HOOK(char*, GetCustomCarPlateText_SetModelIdx, CVehicleModelInfo* mi)
{
    char *text = GetCustomCarPlateText(mi);
    if(!text)
    {
        text = (char*)&mi->m_szPlateText[0];
        GeneratePlateText(text, 8);
        text[8] = 0;
    }
    return text;
}

// Fix "You have worked out enough for today, come back tomorrow!"
uintptr_t Opcode0835_BackTo;
extern "C" uintptr_t Opcode0835_Patch(CRunningScript* script)
{
    char *scriptName = script->ScriptName;
    if(!strncasecmp(scriptName, "gymbike", 8) || !strncasecmp(scriptName, "gymbenc", 8) || !strncasecmp(scriptName, "gymtrea", 8) || !strncasecmp(scriptName, "gymdumb", 8))
    {
        ScriptParams[0].i = StatTypesInt[14]; // 134 = Days passed in game
        ScriptParams[1].i = -1;
    }
    else
    {
        ScriptParams[0].i = *ms_nGameClockDays;
        ScriptParams[1].i = *ms_nGameClockMonths;
    }
    return Opcode0835_BackTo;
}
__attribute__((optnone)) __attribute__((naked)) void Opcode0835_Inject(void)
{
    asm volatile(
        "MOV X0, X19\n"
        "BL Opcode0835_Patch\n"
        "MOV W1, #2\n"
        "BR X0\n");
}

// Michelle dating fix
uintptr_t Opcode039E_BackTo;
extern "C" uintptr_t Opcode039E_Patch(CRunningScript* script, CPed* ped)
{
    char *scriptName = script->ScriptName;
    if(strncasecmp(scriptName, "gfdate", 7) != 0)
    {
        ped->m_PedFlags.bDontDragMeOutCar = ScriptParams[1].i;
    }
    return Opcode039E_BackTo;
}
__attribute__((optnone)) __attribute__((naked)) void Opcode039E_Inject(void)
{
    asm volatile(
        "MOV X0, X19\n"
        "MOV X1, X8\n"
        "BL Opcode039E_Patch\n"
        "BR X0\n");
}

// Inverse swimming controls to dive/go up (to match PC version)
eSwimState curSwimState = SWIM_TREAD;
DECL_HOOKv(TaskSwim_ProcessInput, CTaskSimpleSwim *self, CPlayerPed *ped)
{
    curSwimState = self->m_nSwimState;
    TaskSwim_ProcessInput(self, ped);
}
DECL_HOOK(int, GetPedWalkUpDown_Swimming, void* pad)
{
    switch(curSwimState)
    {
        case SWIM_UNDERWATER_SPRINTING:
        case SWIM_DIVE_UNDERWATER:
            return -GetPedWalkUpDown_Swimming(pad);

        default:
            return GetPedWalkUpDown_Swimming(pad);
    }
}

// ParaLand Anim fix
DECL_HOOKv(PlayerInfoProcess_ParachuteAnim, CPlayerInfo* self, int playerNum)
{
    CPed* ped = self->pPed;
    if(ped->m_WeaponSlots[ped->m_nCurrentWeapon].m_eWeaponType == eWeaponType::WEAPON_PARACHUTE)
    {
        auto anim = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, "FALL_front");
        if(anim != NULL)
        {
            anim->m_bitsFlag |= ANIMATION_FREEZE_LAST_FRAME;
            anim->m_fBlendDelta = -1000.0f;

            CTask* task = TaskConstructor();
            TaskStartNamedAnim(task, "PARA_Land", "PARACHUTE", ANIMATION_UNLOCK_LAST_FRAME | ANIMATION_PARTIAL | ANIMATION_TRANSLATE_Y | ANIMATION_TRANSLATE_X, 10.0f, -1, true, *ms_iActiveSequence > -1, false, false);
            SetTask(&ped->m_pPedIntelligence->m_taskManager, task, TASK_PRIMARY_PRIMARY, false);
        }
    }
    PlayerInfoProcess_ParachuteAnim(self, playerNum);
}

// Unused detonator animation is in the ped.ifp, lol
DECL_HOOKv(UseDetonator, CEntity* ent)
{
    UseDetonator(ent);

    if(ent->m_nType == eEntityType::ENTITY_TYPE_PED)
    {
        CTask* task = TaskConstructor();
        //TaskStartNamedAnim(task, "BOMBER", "DETONATOR", ANIMATION_UNLOCK_LAST_FRAME | ANIMATION_PARTIAL, 4.0f, -1, true, *ms_iActiveSequence > -1, false, false);
        TaskStartNamedAnim(task, "bomber", "ped", ANIMATION_UNLOCK_LAST_FRAME | ANIMATION_PARTIAL, 4.0f, -1, true, *ms_iActiveSequence > -1, false, false);
        SetTask(&((CPed*)ent)->m_pPedIntelligence->m_taskManager, task, TASK_PRIMARY_PRIMARY, false);
    }
}

// Taxi lights
DECL_HOOKv(AutomobileRender, CAutomobile* self)
{
    AutomobileRender(self);
    
    if(self->m_nModelIndex == 420 ||
       self->m_nModelIndex == 438)
    {
        if(self->vehicleFlags.bEngineOn &&
           self->m_pDriver &&
           self->m_nNumPassengers == 0 &&
           self->m_fHealth > 0)
        {
            SetTaxiLight(self, true);
        }
        else
        {
            SetTaxiLight(self, false);
        }
    }
}

// Car Slowdown Fix
DECL_HOOKv(ProcessVehicleWheel, CVehicle* self, CVector& wheelFwd, CVector& wheelRight, CVector& wheelContactSpeed, CVector& wheelContactPoint,
        int32_t wheelsOnGround, float thrust, float brake, float adhesion, int8_t wheelId, float* wheelSpeed, void* wheelState, uint16_t wheelStatus)
{
    float save = *mod_HandlingManager_off4; *mod_HandlingManager_off4 *= GetTimeStepMagic();
    ProcessVehicleWheel(self, wheelFwd, wheelRight, wheelContactSpeed, wheelContactPoint, wheelsOnGround, thrust, brake, adhesion, wheelId, wheelSpeed, wheelState, wheelStatus);
    *mod_HandlingManager_off4 = save;
}

// SkimmerPlaneFix
// Changed the way it works, because ms_fTimeStep cannot be the same at the mod start (it is 0 at the mod start anyway)
// UPD: Changed the way again!
DECL_HOOKv(ApplyBoatWaterResistance, CVehicle* self, tBoatHandlingData *boatHandling, float fImmersionDepth)
{
    float fSpeedMult = sq(fImmersionDepth) * self->m_pHandling->fSuspensionForce * self->m_fMass / 1000.0F;
    if (self->m_nModelIndex == 460) // MODEL_SKIMMER
    {
        fSpeedMult *= 30.0F;
    }
    fSpeedMult *= GetTimeStepMagic();

    auto fMoveDotProduct = DotProduct(self->m_vecMoveSpeed, self->GetForward());
    fSpeedMult *= sq(fMoveDotProduct) + 0.05F;
    fSpeedMult += 1.0F;
    fSpeedMult = fabsf(fSpeedMult);
    fSpeedMult = 1.0F / fSpeedMult;

    float fUsedTimeStep = GetTimeStep() * 0.5F;
    auto vecSpeedMult = (boatHandling->vecMoveResistance * fSpeedMult).Pow(fUsedTimeStep);

    CVector vecMoveSpeedMatrixDotProduct = self->GetMatrix()->InverseTransformVector(self->m_vecMoveSpeed);
    self->m_vecMoveSpeed = vecMoveSpeedMatrixDotProduct * vecSpeedMult;

    auto fMassMult = (vecSpeedMult.y - 1.0F) * self->m_vecMoveSpeed.y * self->m_fMass;
    CVector vecTransformedMoveSpeed = self->GetMatrix()->TransformVector(self->m_vecMoveSpeed);
    self->m_vecMoveSpeed = vecTransformedMoveSpeed;

    auto vecDown = self->GetUp() * -1.0F;
    auto vecTurnForce = self->GetForward() * fMassMult;
    ApplyTurnForce(self, vecTurnForce, vecDown);

    if (self->m_vecMoveSpeed.z <= 0.0F)
        self->m_vecMoveSpeed.z *= ((1.0F - vecSpeedMult.z) * 0.5F + vecSpeedMult.z);
    else
        self->m_vecMoveSpeed.z *= vecSpeedMult.z;
}

// Cinematic camera
bool toggledCinematic = false;
DECL_HOOKv(PlayerInfoProcess_Cinematic, CPlayerInfo* info, int playerNum)
{
    PlayerInfoProcess_Cinematic(info, playerNum);

    // Do it for the local player only.
    if(info == &WorldPlayers[0])
    {
        if(!*bRunningCutscene &&
           info->pRemoteVehicle == NULL &&
           info->pPed->m_pVehicle != NULL &&
           info->pPed->m_nPedState == PEDSTATE_DRIVING)
        {
            if(info->pPed->m_pVehicle->m_nVehicleType != VEHICLE_TYPE_TRAIN &&
               Touch_IsDoubleTapped(WIDGETID_CAMERAMODE, true, 1))
            {
                toggledCinematic = !TheCamera->m_bForceCinemaCam;
                TheCamera->m_bForceCinemaCam = toggledCinematic;

                memset(m_pWidgets[WIDGETID_CAMERAMODE]->tapTimes, 0, sizeof(float)*10); // CWidget::ClearTapHistory in a better way
            }
        }
        else
        {
            if(toggledCinematic)
            {
                TheCamera->m_bForceCinemaCam = false;
                *bDidWeProcessAnyCinemaCam = false;
                if(!*bRunningCutscene &&
                   TheCamera->pTargetEntity == NULL)
                {
                    RestoreCamera(TheCamera);
                    SetCameraDirectlyBehindForFollowPed(TheCamera);
                }
                m_pWidgets[WIDGETID_CAMERAMODE]->enabled = false;
                toggledCinematic = false;
            }
        }
    }
}

// Para dmg anim fix
DECL_HOOKv(ComputeDamageAnim, uintptr_t self, CPed* victim, bool a2)
{
    bool bNeedFix = *(eWeaponType*)(self + 36) == WEAPON_PARACHUTE;
    
    if(bNeedFix) *(eWeaponType*)(self + 36) = WEAPON_UNARMED;
    ComputeDamageAnim(self, victim, a2);
    if(bNeedFix) *(eWeaponType*)(self + 36) = WEAPON_PARACHUTE;
}

// Force DXT
DECL_HOOKv(LoadTexDBThumbs, const char* dbName, int unk, TextureDatabaseFormat format)
{
    LoadTexDBThumbs(dbName, unk, (format == DF_Default) ? DF_DXT : format);
}

// Fix wheels rotation speed on high FPS
DECL_HOOK(float, ProcessWheelRotation_FPS, CVehicle *self, tWheelState WheelState, const CVector *vecForward, const CVector *WheelSpeed, float fRadius)
{
    return ProcessWheelRotation_FPS(self, WheelState, vecForward, WheelSpeed, fRadius) * GetTimeStep();
}

// Fix FX memory leak
DECL_HOOKv(FxInfoMan_FXLeak, uintptr_t self_x58)
{
    RwTexture** texturePtr4 = (RwTexture**)(self_x58 - 0x58 + 0x30);
    if(*texturePtr4)
    {
        RwTextureDestroy(*texturePtr4);
        *texturePtr4 = NULL;
    }
    FxInfoMan_FXLeak(self_x58);
}

// Bring back light shadows from poles!
float fLightDist = 40.0f;
uintptr_t ProcessLightsForEntity_BackTo;
CEntity* lastEntityEffect;
C2dEffect* lastEffect;
int lastEffectNum;
DECL_HOOK(C2dEffect*, GetBaseEffect_AddLight, CBaseModelInfo* mi, int i)
{
    lastEffect = GetBaseEffect_AddLight(mi, i);
    lastEffectNum = i;
    return lastEffect;
}
DECL_HOOKv(ProcessLightsForEntity_AddLight, UInt8 Type, CVector Coors, CVector Dir, float Range, float Red, float Green, float Blue, UInt8 FogEffect, bool bCastsShadowFromPlayerCarAndPed, CEntity *pArgCastingEntity)
{
    ProcessLightsForEntity_AddLight(Type, Coors, Dir, Range, Red, Green, Blue, FogEffect, bCastsShadowFromPlayerCarAndPed, NULL);
    if(lastEffect->light.m_fShadowSize != 0)
    {
        float intensity = ((lastEffect->light.m_nShadowColorMultiplier * 0.125f * *fSpriteBrightness) / 256.0f);
        float zDist = lastEffect->light.m_nShadowZDistance ? lastEffect->light.m_nShadowZDistance : 15.0f;

        bool bCurrentlyActive = *(bool*)(&pArgCastingEntity);
        if(bCurrentlyActive)
        {
            StoreStaticShadow((uintptr_t)lastEntityEffect + lastEffectNum, 2, lastEffect->light.m_pShadowTex, &Coors, lastEffect->light.m_fShadowSize, 0.0f, 0.0f, -lastEffect->light.m_fShadowSize,
                            128, intensity * lastEffect->light.m_color.red, intensity * lastEffect->light.m_color.green, intensity * lastEffect->light.m_color.blue, zDist, 1.0f, fLightDist, false, 0.0f);
        }
        else
        {
            StoreStaticShadow((uintptr_t)lastEntityEffect + lastEffectNum, 2, lastEffect->light.m_pShadowTex, &Coors, lastEffect->light.m_fShadowSize, 0.0f, 0.0f, -lastEffect->light.m_fShadowSize,
                            0, 0, 0, 0, zDist, 1.0f, fLightDist, false, 0.0f);
        }
    }
}
DECL_HOOKv(ProcessLightsForEntity_ItSelf, CEntity* self)
{
    lastEntityEffect = self;
    ProcessLightsForEntity_ItSelf(self);
}

// Static shadows
DECL_HOOKv(RenderStaticShadows, bool a1)
{
    for(int i = 48; i < 256; ++i)
    {
        aStaticShadows_NEW[i].m_bRendered = false;
    }
    RenderStaticShadows(a1);
}
DECL_HOOKv(InitShadows)
{
    static CPolyBunch bunchezTail[BUNCHTAILS_EX];
    
    InitShadows();
    for(int i = 0; i < BUNCHTAILS_EX-1; ++i)
    {
        bunchezTail[i].pNext = &bunchezTail[i+1];
    }
    bunchezTail[BUNCHTAILS_EX-1].pNext = NULL;
    aPolyBunches[360-1].pNext = &bunchezTail[0];
}

// Max loading splashes
int mobilescCount = 7;
int DoRand(int max)
{
    srand(time(NULL));
    return (int)(((double)rand() / (double)RAND_MAX) * max);
}
DECL_HOOKv(LoadSplash_sscanf, char* buf, const char* fmt, int randArg)
{
    LoadSplash_sscanf(buf, fmt, DoRand(mobilescCount));
}

// Moving objs (opcode 034E)
DECL_HOOKv(CollectParams_034E, CRunningScript* script, int count)
{
    CollectParams_034E(script, count);

    float scale = 30.0f / *game_FPS;
    ScriptParams[4].f *= scale;
    ScriptParams[5].f *= scale;
    ScriptParams[6].f *= scale;
}

// Fix pushing force
DECL_HOOKv(ApplyForce_Collision, CPhysical *self, CVector vecForce, CVector vecOffset, bool bValidTorque)
{
    ApplyForce_Collision(self, vecForce * GetTimeStepMagic(), vecOffset, bValidTorque);
}

// Marker fix
DECL_HOOKv(PlaceRedMarker_MarkerFix, bool canPlace)
{
    if(canPlace)
    {
        int x, y;
        LIB_PointerGetCoordinates(*lastDevice, &x, &y, NULL);
        if(y > 0.88f * RsGlobal->maximumHeight &&
           x > ((float)RsGlobal->maximumWidth - 0.87f * RsGlobal->maximumHeight)) return;
    }
    PlaceRedMarker_MarkerFix(canPlace);
}














// Camera is saving screenshots?
DECL_HOOK(RwImage*, Patch_psGrabScreen, RwCamera* camera)
{
    if(!camera || !camera->framebuf) return NULL;

    RwImage* newImage = RwImageCreate(camera->framebuf->width, camera->framebuf->height, 32);
    if(newImage)
    {
        RwImageAllocatePixels(newImage);
		RwImageSetFromRaster(newImage, camera->framebuf);
    }
    return newImage;
}
DECL_HOOKb(TakePhoto, CWeapon* self, CEntity* unused, CVector* cameraPos)
{
    static uint32_t nextAllowedPic = 0;
    if(nextAllowedPic < *m_snTimeInMilliseconds)
    {
        nextAllowedPic = *m_snTimeInMilliseconds + 200;

        TimerStop();
        SetDirMyDocuments();
        JPegCompressScreenToFile(TheCamera->m_pRwCamera, "screenshot_test.jpg");
        FileMgrSetDir("");
        TimerUpdate();
    }

    return TakePhoto(self, unused, cameraPos);
}
DECL_HOOKv(ShowRasterIdle, RwCamera* camera)
{
    static uint32_t nextAllowedPic = 0;
    if(nextAllowedPic < *m_snTimeInMilliseconds)
    {
        nextAllowedPic = *m_snTimeInMilliseconds + 5000;

        TimerStop();
        SetDirMyDocuments();
        //JPegCompressScreenToFile(camera, "screenshot_test.jpg");
        RwGrabScreen(camera, "screenshot_test.bmp");
        FileMgrSetDir("");
        TimerUpdate();
    }
    ShowRasterIdle(camera);
}
DECL_HOOKv(jpeg_samplecopy, uint8_t **input_array, int source_row, uint8_t **output_array, int dest_row, int num_rows, uint num_cols)
{
    jpeg_samplecopy(input_array, source_row, output_array, dest_row, num_rows, num_cols);
    logger->Info("jpeg_samplecopy: dest_row (i) = %d, num_rows (height) = %d, num_cols (width) = %d", dest_row, num_rows, num_cols);
}