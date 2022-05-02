#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>
#include <dlfcn.h>
#include <vector>

#define GL_GLEXT_PROTOTYPES
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES2/gl2platform.h>

#include "GTASA_STRUCTS.h"

MYMODCFG(net.rusjj.jpatch, JPatch, 1.2.4, RusJJ)

union ScriptVariables
{
    int      i;
    float    f;
    uint32_t u;
    void*    p;
};

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////     Saves     ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////
uintptr_t pGTASA;
void* hGTASA;
static constexpr float fMagic = 50.0f / 30.0f;
static constexpr int nMaxScriptSprites = 384; // Changing it wont make it bigger.
float fEmergencyVehiclesFix;
CSprite2d** pNewScriptSprites = new CSprite2d*[nMaxScriptSprites] {NULL}; // 384*4=1536 0x600
void* pNewIntroRectangles = new void*[15*nMaxScriptSprites] {NULL}; // 384*60=23040 0x5A00
CRegisteredShadow* asShadowsStored_NEW;
CStaticShadow* aStaticShadows_NEW;

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////     Vars      ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CPlayerInfo* WorldPlayers;
CCamera* TheCamera;
RsGlobalType* RsGlobal;
MobileMenu *gMobileMenu;
CWidget** m_pWidgets;
ScriptVariables* ScriptParams;
CLinkList<AlphaObjectInfo>* m_alphaList;
CPool<CObject>** pObjectPool;
CZoneInfo** m_pCurrZoneInfo;

float *ms_fTimeStep, *ms_fFOV, *game_FPS, *CloudsRotation, *WeatherWind, *fSpriteBrightness, *m_f3rdPersonCHairMultX, *m_f3rdPersonCHairMultY, *ms_fAspectRatio;
void *g_surfaceInfos;
unsigned int *m_snTimeInMilliseconds;
int *lastDevice, *NumberOfSearchLights, *ms_numAnimBlocks, *RasterExtOffset, *detailTexturesStorage, *textureDetail;
bool *bDidWeProcessAnyCinemaCam, *bRunningCutscene;
uint32_t *CloudsIndividualRotation, *m_ZoneFadeTimer, *ms_memoryUsed, *ms_memoryAvailable;

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////     Funcs     ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Redirect(uintptr_t addr, uintptr_t to)
{
    if(!addr) return;
    uint32_t hook[2] = {0xE51FF004, to};
    if(addr & 1)
    {
        addr &= ~1;
        if (addr & 2)
        {
            aml->PlaceNOP(addr, 1); 
            addr += 2;
        }
        hook[0] = 0xF000F8DF;
    }
    aml->Write(addr, (uintptr_t)hook, sizeof(hook));
}
void (*RwRenderStateSet)(RwRenderState, void*);
void (*RwRenderStateGet)(RwRenderState, void*);
void (*ClearPedWeapons)(CPed*);
eBulletFxType (*GetBulletFx)(void* self, unsigned int surfaceId);
void (*LIB_PointerGetCoordinates)(int, int*, int*, float*);
bool (*Touch_IsDoubleTapped)(WidgetIDs, bool doTapEffect, int idkButBe1);
bool (*Touch_IsHeldDown)(WidgetIDs, int idkButBe1);
void (*SetCameraDirectlyBehindForFollowPed)(CCamera*);
void (*RestoreCamera)(CCamera*);
CVehicle* (*FindPlayerVehicle)(int playerId, bool unk);
void (*PhysicalApplyForce)(CPhysical* self, CVector force, CVector point, bool updateTurnSpeed);
char* (*GetFrameNodeName)(RwFrame*);
int (*SpriteCalcScreenCoors)(const RwV3d& posn, RwV3d* out, float* w, float* h, bool checkMaxVisible, bool checkMinVisible);
void (*WorldRemoveEntity)(CEntity*);
void (*SetFontColor)(CRGBA* clr);
bool (*ProcessVerticalLine)(const CVector& origin, float distance, CColPoint& outColPoint, CEntity*& outEntity, bool buildings, bool vehicles, bool peds, bool objects, bool dummies, bool doSeeThroughCheck, CStoredCollPoly* outCollPoly);
void (*CreateEntityRwObject)(CEntity*);
void (*RequestModel)(int id, int prio);
bool (*RemoveLeastUsedModel)(unsigned int streamingFlags);
void (*LoadAllRequestedModels)(bool bOnlyPriorityRequests);
void (*AddAnimBlockRef)(int animBlock);
void (*TimerStop)();
void (*TimerUpdate)();
void (*GetTouchPosition)(CVector2D*, int cachedPosNum);
bool (*StoreStaticShadow)(uint32_t id, uint8_t type, RwTexture* texture, CVector* posn, float frontX, float frontY, float sideX, float sideY, int16_t intensity, uint8_t red, uint8_t green, uint8_t blue, float zDistane, float scale, float drawDistance, bool temporaryShadow, float upDistance);
void (*TransformPoint)(RwV3d& point, const CSimpleTransform& placement, const RwV3d& vecPos);
void TransformFromObjectSpace(CEntity* self, CVector& outPos, const CVector& offset)
{
    if(self->m_matrix)
    {
        outPos = *self->m_matrix * offset;
        return;
    }
    TransformPoint((RwV3d&)outPos, self->m_placement, (RwV3d&)offset);
}
void BumpStreamingMemory(int megabytes)
{
    *ms_memoryAvailable += megabytes * 1024 * 1024;
}

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////     Hooks     ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////
extern "C" void adadad(void)
{
    //asm("VMOV.F32 S0, #0.5");
    asm("MOVT R1, #0x42F0");
}

// Moon phases
int moon_alphafunc, moon_vertexblend, moon_alphaval;
uintptr_t MoonVisual_1_BackTo;
extern "C" void MoonVisual_1(void)
{
    //glEnable(GL_ALPHA_TEST);

    RwRenderStateGet(rwRENDERSTATEALPHATESTFUNCTION, &moon_alphafunc);
    RwRenderStateGet(rwRENDERSTATEVERTEXALPHAENABLE, &moon_vertexblend);
    RwRenderStateGet(rwRENDERSTATEALPHATESTFUNCTIONREF, &moon_alphaval);
    
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTION, (void*)rwALPHATESTFUNCTIONALWAYS);
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)true);
    RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDZERO);

    //glAlphaFuncQCOM(GL_GREATER, 0.5f);
}
__attribute__((optnone)) __attribute__((naked)) void MoonVisual_1_inject(void)
{
    asm volatile(
        "push {r0-r11}\n"
        "bl MoonVisual_1\n");
    asm volatile(
        "mov r12, %0\n"
        "pop {r0-r11}\n"
        "bx r12\n"
    :: "r" (MoonVisual_1_BackTo));
}
uintptr_t MoonVisual_2_BackTo;
extern "C" void MoonVisual_2(void)
{
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTION, (void*)moon_alphafunc);
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)moon_vertexblend);
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)moon_alphaval);

    RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDDESTALPHA);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)false);

    //glDisable(GL_ALPHA_TEST);
}
__attribute__((optnone)) __attribute__((naked)) void MoonVisual_2_inject(void)
{
    asm volatile(
        "push {r0-r11}\n"
        "bl MoonVisual_2\n");
    asm volatile(
        "mov r12, %0\n"
        "pop {r0-r11}\n"
        "bx r12\n"
    :: "r" (MoonVisual_2_BackTo));
}

// FOV
DECL_HOOKv(SetFOV, float factor, bool unused)
{
    if(TheCamera->m_bIsInCutscene)
    {
        *ms_fFOV = factor;
    }
    else
    {
        SetFOV(factor, unused);
    }
}

// Limit particles
uintptr_t AddBulletImpactFx_BackTo;
unsigned int nextHeavyParticleTick = 0;
eBulletFxType nLimitWithSparkles = BULLETFX_NOTHING;
extern "C" eBulletFxType AddBulletImpactFx(unsigned int surfaceId)
{
    eBulletFxType nParticlesType = GetBulletFx(g_surfaceInfos, surfaceId);
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
__attribute__((optnone)) __attribute__((naked)) void AddBulletImpactFx_inject(void)
{
    asm volatile(
        "mov r12, r3\n"
        "push {r0-r7,r9-r11}\n"
        "mov r0, r12\n"
        "bl AddBulletImpactFx\n"
        "mov r8, r0\n");
    asm volatile(
        "mov r12, %0\n"
        "pop {r0-r7,r9-r11}\n"
        "mov r9, r1\n"
        "mov r4, r2\n"
        "bx r12\n"
    :: "r" (AddBulletImpactFx_BackTo));
}

// AimingRifleWalkFix
DECL_HOOKv(ControlGunMove, void* self, CVector2D* vec2D)
{
    float save = *ms_fTimeStep; *ms_fTimeStep = fMagic;
    ControlGunMove(self, vec2D);
    *ms_fTimeStep = save;
}

// SwimSpeedFix
uintptr_t SwimmingResistanceBack_BackTo;
float saveStep;
DECL_HOOKv(ProcessSwimmingResistance, void* self, CPed* ped)
{
    saveStep = *ms_fTimeStep;
    if(ped->m_nPedType == PED_TYPE_PLAYER1) *ms_fTimeStep *= 0.8954f/fMagic;
    else *ms_fTimeStep *= 1.14f/fMagic;
    ProcessSwimmingResistance(self, ped);
    *ms_fTimeStep = saveStep;
}
extern "C" void SwimmingResistanceBack(void)
{
    *ms_fTimeStep = saveStep;
}
__attribute__((optnone)) __attribute__((naked)) void SwimmingResistanceBack_inject(void)
{
    asm volatile(
        "push {r0-r11}\n"
        "bl SwimmingResistanceBack\n");
    asm volatile(
        "mov r12, %0\n"
        "pop {r0-r11}\n"
        "vldr s4, [r0]\n"
        "ldr r0, [r4]\n"
        "vmul.f32 s0, s4, s0\n"
        "bx r12\n"
    :: "r" (SwimmingResistanceBack_BackTo));
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

// Died penalty
uintptr_t DiedPenalty_BackTo;
extern "C" void DiedPenalty(void)
{
    if(WorldPlayers[0].m_nMoney > 0)
    {
        WorldPlayers[0].m_nMoney = WorldPlayers[0].m_nMoney - 100 < 0 ? 0 : WorldPlayers[0].m_nMoney - 100;
    }
    ClearPedWeapons(WorldPlayers[0].m_pPed);
}
__attribute__((optnone)) __attribute__((naked)) void DiedPenalty_inject(void)
{
    asm volatile(
        "push {r0-r11}\n"
        "bl DiedPenalty\n");
    asm volatile(
        "mov r12, %0\n"
        "pop {r0-r11}\n"
        "bx r12\n"
    :: "r" (DiedPenalty_BackTo));
}

// Emergency Vehicles
uintptr_t EmergencyVeh_BackTo;
__attribute__((optnone)) __attribute__((naked)) void EmergencyVeh_inject(void)
{
    asm volatile(
        "push {r0}\n");
    asm volatile(
        "vmov s0, %0\n"
    :: "r" (fEmergencyVehiclesFix));
    asm volatile(
        "mov r12, %0\n"
        "pop {r0}\n"
        "bx r12\n"
    :: "r" (EmergencyVeh_BackTo));
}
DECL_HOOKv(SetFOV_Emergency, float factor, bool unused)
{
    // Someone is using broken mods
    // So here is the workaround + a little value clamping
    if(factor < 5.0f)
    {
        fEmergencyVehiclesFix = 70.0f / 5.0f;
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

// Marker fix
DECL_HOOKv(PlaceRedMarker_MarkerFix, bool canPlace)
{
    if(canPlace)
    {
        int x, y;
        LIB_PointerGetCoordinates(*lastDevice, &x, &y, NULL);
        if(y > 0.85f * RsGlobal->maximumHeight &&
           x > ((float)RsGlobal->maximumWidth - 0.7f * RsGlobal->maximumHeight)) return;
    }
    PlaceRedMarker_MarkerFix(canPlace);
}

// SkimmerPlaneFix
// Changed the way it works, because ms_fTimeStep cannot be the same at the mod start (it is 0 at the mod start anyway)
uintptr_t SkimmerWaterResistance_BackTo;
extern "C" float SkimmerWaterResistance_patch(void)
{
    return 30.0f * (*ms_fTimeStep / fMagic);
}
__attribute__((optnone)) __attribute__((naked)) void SkimmerWaterResistance_inject(void)
{
    asm volatile(
        "vpush {s0-s2}\n"
        "bl SkimmerWaterResistance_patch\n"
        "vpop {s0-s2}\n"
        "vmov.f32 s4, r0\n");
    asm volatile(
        "mov r12, %0\n"
        "vadd.f32 s2, s2, s8\n"
        "bx r12\n"
    :: "r" (SkimmerWaterResistance_BackTo));
}

// Cinematic camera
bool toggledCinematic = false;
DECL_HOOKv(PlayerInfoProcess_Cinematic, CPlayerInfo* info, int playerNum)
{
    PlayerInfoProcess_Cinematic(info, playerNum);

    // Do it for local player only.
    if(info == &WorldPlayers[0])
    {
        if(!*bRunningCutscene &&
           info->m_pPed->m_pVehicle != NULL &&
           info->m_pPed->m_nPedState == PEDSTATE_DRIVING)
        {
            if(info->m_pPed->m_pVehicle->m_nVehicleType != VEHICLE_TYPE_TRAIN &&
               Touch_IsDoubleTapped(WIDGETID_CAMERAMODE, true, 1))
            {
                toggledCinematic = !TheCamera->m_bEnabledCinematicCamera;
                TheCamera->m_bEnabledCinematicCamera = toggledCinematic;

                memset(m_pWidgets[WIDGETID_CAMERAMODE]->tapTimes, 0, sizeof(float)*10); // CWidget::ClearTapHistory in a better way
            }
        }
        else
        {
            if(toggledCinematic)
            {
                TheCamera->m_bEnabledCinematicCamera = false;
                *bDidWeProcessAnyCinemaCam = false;
                if(!*bRunningCutscene &&
                   TheCamera->m_pTargetEntity == NULL)
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

// SWAT
uintptr_t GetCarGunFired_BackTo; // For our usage only
uintptr_t GetCarGunFired_BackTo1, GetCarGunFired_BackTo2; // For optimization?
extern "C" void GetCarGunFired_patch(void)
{
    CVehicle* veh = FindPlayerVehicle(-1, false);
    if(veh != NULL && (veh->m_nModelIndex == 407 || veh->m_nModelIndex == 601))
    {
        GetCarGunFired_BackTo = GetCarGunFired_BackTo1;
    }
    else
    {
        GetCarGunFired_BackTo = GetCarGunFired_BackTo2;
    }
}
__attribute__((optnone)) __attribute__((naked)) void GetCarGunFired_inject(void)
{
    asm volatile(
        "push {r0-r11}\n"
        "bl GetCarGunFired_patch\n");
    asm volatile(
        "mov r12, %0\n"
        "pop {r0-r11}\n"
        "bx r12\n"
    :: "r" (GetCarGunFired_BackTo));
}

// Fuzzy seek (im lazy to patch so lets just do this instead (because we need to inject the code))
DECL_HOOK(int, mpg123_param, void* mh, int key, long val, int ZERO, double fval)
{
    // 0x2000 = MPG123_SKIP_ID3V2
    // 0x200  = MPG123_FUZZY
    // 0x100  = MPG123_SEEKBUFFER
    // 0x40   = MPG123_GAPLESS
    return mpg123_param(mh, key, val | (0x2000 | 0x200 | 0x100 | 0x40), ZERO, fval);
}

// Fix water cannon
DECL_HOOKv(WaterCannonRender, void* self)
{
    float save = *ms_fTimeStep; *ms_fTimeStep = fMagic;
    WaterCannonRender(self);
    *ms_fTimeStep = save;
}
DECL_HOOKv(WaterCannonUpdate, void* self, int frames)
{
    float save = *ms_fTimeStep; *ms_fTimeStep = fMagic;
    WaterCannonUpdate(self, frames);
    *ms_fTimeStep = save;
}

// Moving objs (opcode 034E)
uintptr_t ProcessCommands800To899_BackTo;
extern "C" void ProcessCommands800To899_patch(void)
{
    float scale = 30.0f / *game_FPS;
    ScriptParams[4].f *= scale;
    ScriptParams[5].f *= scale;
    ScriptParams[6].f *= scale;
}
__attribute__((optnone)) __attribute__((naked)) void ProcessCommands800To899_inject(void)
{
    asm volatile(
        "mla r9, r1, r2, r0\n"
        "push {r0-r11}\n"
        "vpush {s0-s6}\n"
        "bl ProcessCommands800To899_patch\n");
    asm volatile(
        "mov r12, %0\n"
        "vpop {s0-s6}\n"
        "pop {r0-r11}\n"
        "bx r12\n"
    :: "r" (ProcessCommands800To899_BackTo));
}

// PhysicalApplyCollision
uintptr_t PhysicalApplyCollision_BackTo;
extern "C" void PhysicalApplyCollision_patch(CPhysical* self, CVector force, CVector point, bool updateTurnSpeed)
{
    force *= *ms_fTimeStep / fMagic;
    PhysicalApplyForce(self, force, point, updateTurnSpeed);
}
__attribute__((optnone)) __attribute__((naked)) void PhysicalApplyCollision_inject(void)
{
    asm volatile(
        "mov r0, r9\n"
        "ldr.w r11, [sp,#0xE0+0xAC]\n"
        "bl PhysicalApplyCollision_patch\n");
    asm volatile(
        "mov r0, %0\n"
        "bx r0\n"
    :: "r" (PhysicalApplyCollision_BackTo));
}

// Car Slowdown Fix
float *mod_HandlingManager_off4;
DECL_HOOKv(ProcessVehicleWheel, CVehicle* self, CVector& wheelFwd, CVector& wheelRight, CVector& wheelContactSpeed, CVector& wheelContactPoint,
        int32_t wheelsOnGround, float thrust, float brake, float adhesion, int8_t wheelId, float* wheelSpeed, void* wheelState, uint16_t wheelStatus)
{
    float save = *mod_HandlingManager_off4; *mod_HandlingManager_off4 = 0.9f * (*ms_fTimeStep / fMagic);
    ProcessVehicleWheel(self, wheelFwd, wheelRight, wheelContactSpeed, wheelContactPoint, wheelsOnGround, thrust, brake, adhesion, wheelId, wheelSpeed, wheelState, wheelStatus);
    *mod_HandlingManager_off4 = save;
}

// Heli rotor
float *fRotorFinalSpeed, *fRotor1Speed, *fRotor2Speed;
DECL_HOOKv(Heli_ProcessFlyingStuff, CHeli* self)
{
    *fRotor1Speed = 0.00454545454f * *fRotorFinalSpeed * (*ms_fTimeStep / fMagic);
    *fRotor2Speed = 3.0f * *fRotor1Speed;
    Heli_ProcessFlyingStuff(self);
}

DECL_HOOKv(PossiblyRemoveVehicle_Re3, CVehicle* veh)
{
    if(veh->vehicleFlags.bIsLocked) return;
    PossiblyRemoveVehicle_Re3(veh);
}

DECL_HOOKv(CloudsUpdate_Re3)
{
    float s = sinf(TheCamera->m_fOrientation - 0.85f);

    *CloudsRotation += *WeatherWind * s * 0.0025f * (*ms_fTimeStep / fMagic);
    *CloudsIndividualRotation += (*WeatherWind * *ms_fTimeStep + 0.3f * (*ms_fTimeStep / fMagic)) * 60.0f;
}

uint32_t* ThreadLaunch_GagNameSet;
DECL_HOOK(void*, OS_ThreadLaunch, void* threadFn, void* a1, uint32_t a2, char const* threadName, int a3, int threadPrio)
{
    bool noName = threadName == NULL || threadName[0] == 0;
    if(noName)
    {
        *ThreadLaunch_GagNameSet = 0xBF00BF00;
    }
    void* ret = OS_ThreadLaunch(threadFn, a1, a2, threadName, a3, threadPrio);
    if(noName)
    {
        *ThreadLaunch_GagNameSet = 0xED86F72A;
    }
    return ret;
}

// Free objects pool
DECL_HOOK(CObject*, Object_New, uint32_t size)
{
    CPool<CObject> *objPool = (*pObjectPool);
    CObject *obj = objPool->New();
    if (!obj)
    {
        int size = objPool->GetSize();
        for (int i = 0; i < size; ++i)
        {
            CObject *existing = objPool->GetAt(i);
            if (existing && existing->m_nObjectType == OBJECT_TEMPORARY)
            {
                int32_t handle = objPool->GetIndex(existing);
                WorldRemoveEntity(existing);
                delete existing;
                obj = objPool->New(handle);
                break;
            }
        }
	}
    return obj;
}

// Colored zone names
uintptr_t ColoredZoneNames_BackTo;
extern "C" void ColoredZoneNames_patch(void)
{
    CRGBA fontColor((*m_pCurrZoneInfo)->ZoneColor.red, (*m_pCurrZoneInfo)->ZoneColor.green, (*m_pCurrZoneInfo)->ZoneColor.blue, (unsigned char)(*m_ZoneFadeTimer * 0.255f));
    SetFontColor(&fontColor);
}
__attribute__((optnone)) __attribute__((naked)) void ColoredZoneNames_inject(void)
{
    asm volatile(
        "push {r0-r11}\n"
        "bl ColoredZoneNames_patch\n");
    asm volatile(
        "mov r12, %0\n"
        "pop {r0-r11}\n"
        "bx r12\n"
    :: "r" (ColoredZoneNames_BackTo));
}

// Find Ground Z detects objects
#define FINDGROUNDZ_DIST 1500
#define NOTFOUND_RETURN 20.0f
DECL_HOOK(float, FindGroundZ2D, float x, float y)
{
    CColPoint colp;
    CEntity* hitEnt;
    return ProcessVerticalLine(CVector(x, y, (float)(FINDGROUNDZ_DIST)), -(float)(FINDGROUNDZ_DIST), colp, hitEnt, true, false, false,
           (*bRunningCutscene || TheCamera->m_bIsInCutscene || TheCamera->m_pTargetEntity != NULL || !TheCamera->m_vecGameCamPosFixed.IsZero()) ? false : true, true, false, NULL) ? colp.m_vecPoint.z : NOTFOUND_RETURN;
}
DECL_HOOK(float, FindGroundZ3D, float x, float y, float z, bool* result, CEntity** ent)
{
    if(z <= 0)
    {
        if(result) *result = false;
        if(ent) *ent = NULL;
        return 0.0f;
    }
    CColPoint colp;
    CEntity* hitEnt;
    if(ProcessVerticalLine(CVector(x, y, z), z > FINDGROUNDZ_DIST ? -(float)(FINDGROUNDZ_DIST) : -z, colp, hitEnt, true, false, false, 
      (*bRunningCutscene || TheCamera->m_bIsInCutscene || TheCamera->m_pTargetEntity != NULL || !TheCamera->m_vecGameCamPosFixed.IsZero()) ? false : true, true, false, NULL))
    {
        if(result) *result = true;
        if(ent) *ent = hitEnt;
        return colp.m_vecPoint.z;
    }
    else
    {
        if(result) *result = false;
        if(ent) *ent = NULL;
        return 0.0f;
    }
}

// Road Reflections
uintptr_t RoadReflections_BackTo1, RoadReflections_BackTo2;
float spriteZ = 1.0f;
bool bRenderReflectionsMode = false;
DECL_HOOKv(RenderRefl)
{
    bRenderReflectionsMode = true;
    RenderRefl();
    bRenderReflectionsMode = false;
}
DECL_HOOK(int, CalcSpriteCoords, RwV3d *in, RwV3d *out, float *outw, float *outh, bool farclip, bool smth)
{
    if(!bRenderReflectionsMode) return CalcSpriteCoords(in, out, outw, outh, farclip, smth);

    int ret = CalcSpriteCoords(in, out, outw, outh, farclip, smth);
    spriteZ = out->z;
    return ret;
}
extern "C" int RoadReflections_patch1(const RwV3d& posn, RwV3d* out, float* w, float* h)
{
    int ret = SpriteCalcScreenCoors(posn, out, w, h, true, true);
    spriteZ = out->z;
    return ret;
}
__attribute__((optnone)) __attribute__((naked)) void RoadReflections_inject1(void)
{
    asm volatile(
        "VSTR S0, [SP,#0x128+0xC0]\n"
        "STRD R0, R0, [SP,#0x128+0x128]\n"
        "ADD R0, SP, #0x128+0xC8\n"
        "push {r4-r12}\n"
        "bl RoadReflections_patch1\n"
        "pop {r4-r12}\n");
    asm volatile(
        "push {r0}\n");
    asm volatile(
        "mov r12, %0\n"
        "pop {r0}\n"
        "bx r12\n"
    :: "r" (RoadReflections_BackTo1));
}
__attribute__((optnone)) __attribute__((naked)) void RoadReflections_inject2(void)
{
    asm volatile(
        "VDIV.F32 S2, S17, S16\n"
        "push {r0}\n");
    asm volatile(
        "mov r7, %0\n"
    :: "r" (spriteZ));
    asm volatile(
        "mov r12, %0\n"
        "pop {r0}\n"
        "bx r12\n"
    :: "r" (RoadReflections_BackTo2));
}
DECL_HOOK(void*, RenderBufferedOneXLUSprite, float x, float y, float z, float w, float h, uint8_t r, uint8_t g, uint8_t b, int16_t intensity, float recipNearZ, uint8_t a11)
{
    if(bRenderReflectionsMode) logger->Info("RenderBufferedOneXLUSprite(%f,%f,%f,%f,%f,%d,%d,%d,%d,%f,%d",x, y, z, w, h, r, g, b, intensity, recipNearZ, a11);
    return RenderBufferedOneXLUSprite(x, y, z, w, h, r, g, b, intensity, recipNearZ, a11);
}

// Heli rotor blur
// https://gtaforums.com/topic/703439-san-andreas-moon-and-rotor-blades-fix-for-pc/
// Doesnt work :)
DECL_HOOKv(RenderAlphaAtomics)
{
    char *nodeName;
    for(CLink<AlphaObjectInfo> *entry = m_alphaList->usedListTail.prev; entry != &m_alphaList->usedListHead; entry = entry->prev)
    {
        //entry->data.m_pCallback(entry->data.m_pAtomic, entry->data.m_fDepth);
        nodeName = GetFrameNodeName((RwFrame*)entry->data.m_pAtomic->object.object.parent);
        if(!strncmp(nodeName, "moving_rotor", 12) && (nodeName[12] == '\0' || (nodeName[12] == '2' && nodeName[13] == '\0')))
        {
            RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTION, (void*)rwALPHATESTFUNCTIONEQUAL);
            RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)255);
            entry->data.m_pCallback(entry->data.m_pAtomic, entry->data.m_fDepth);
            RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)false);
            RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)true);
            RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTION, (void*)rwALPHATESTFUNCTIONLESS);
            entry->data.m_pCallback(entry->data.m_pAtomic, entry->data.m_fDepth);
            RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)true);
            RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)false);
            RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTION, (void*)rwALPHATESTFUNCTIONGREATER);
            RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)100);
        }
        else
        {
            entry->data.m_pCallback(entry->data.m_pAtomic, entry->data.m_fDepth);
        }
    }
}

// LODs
#define SKIP_START_FRAMES 3
#define ANIMBLOCK_OFFSET  25575

uintptr_t LoadScene_BackTo;
std::vector<CEntity*> g_aLODs;
std::vector<int> g_aPeds;
bool bPreloadLOD, bPreloadAnim, bPreloadPed = false;
bool bUnloadUnusedModels, bDynStreamingMem, bDontUnloadInCutscenes;
float fRemoveUnusedStreamMemPercentage, fDynamicStreamingMemPercentage; int nRemoveUnusedInterval; unsigned int lastTimeRemoveUnused = 0;
DECL_HOOKv(GameProcess)
{
    GameProcess();

    static char nLoadChecks = SKIP_START_FRAMES+1;
    if(nLoadChecks != 0) // Ignore first SKIP_START_FRAMES frames
    {
        --nLoadChecks;
        if(nLoadChecks == 1)
        {
            // Dont break some engine calculations because of that
            TimerStop();

            if(bPreloadLOD)
            {
                int size = g_aLODs.size();
                CEntity* ent = NULL;
                RwObject* obj = NULL;
                for(int i = 0; i < size; ++i)
                {
                    ent = g_aLODs[i];
                    obj = ent->m_pRwObject;

                    RequestModel(ent->m_nModelIndex, STREAMING_MISSION_REQUIRED);
                    if(obj == NULL) CreateEntityRwObject(g_aLODs[i]);
                }
                LoadAllRequestedModels(false);
            }
            if(bPreloadAnim)
            {
                for(int i = 1; i < *ms_numAnimBlocks; ++i)
                {
                    RequestModel(i + ANIMBLOCK_OFFSET, STREAMING_MISSION_REQUIRED);
                    AddAnimBlockRef(i);
                }
                LoadAllRequestedModels(false);
            }
            if(bPreloadPed) // Crashing, maybe in the future sometime
            {
                int size = g_aPeds.size();
                for(int i = 0; i < size; ++i)
                {
                    RequestModel(g_aPeds[i], STREAMING_MISSION_REQUIRED);
                }
                LoadAllRequestedModels(false);
            }

            // Resume all game timers
            TimerUpdate();
            nLoadChecks = 0;
        }
        return;
    }
    else if(bUnloadUnusedModels || bDynStreamingMem)
    {
        float memUsedPercent = (float)*ms_memoryUsed / (float)*ms_memoryAvailable;
        if(!bDontUnloadInCutscenes || !*bRunningCutscene)
        {
            if(bUnloadUnusedModels && memUsedPercent >= fRemoveUnusedStreamMemPercentage)
            {
                int removeUnusedIntervalMsTweaked;
                if (memUsedPercent >= 0.95f) removeUnusedIntervalMsTweaked = (int)(nRemoveUnusedInterval * 0.5f);
                else removeUnusedIntervalMsTweaked = nRemoveUnusedInterval;

                if ((*m_snTimeInMilliseconds - lastTimeRemoveUnused) > removeUnusedIntervalMsTweaked)
                {
                    RemoveLeastUsedModel(STREAMING_NONE);
                    lastTimeRemoveUnused = *m_snTimeInMilliseconds;
                }
            }
        }
        if(bDynStreamingMem && memUsedPercent >= fDynamicStreamingMemPercentage)
        {
            BumpStreamingMemory(32);
        }
    }
}
DECL_HOOK(CBaseModelInfo*, AddPedModel, int id)
{
    g_aPeds.push_back(id);
    return AddPedModel(id);
}
extern "C" void LoadScene_patch(CEntity* ent)
{
    g_aLODs.push_back(ent);
}
__attribute__((optnone)) __attribute__((naked)) void LoadScene_inject(void)
{
    asm volatile(
        "LDRSH.W R2, [R6,#0x26]\n"
        "LDRB.W R1, [R0,#0x38]\n"
        "MOV R12, R6\n"
        "push {r0-r10}\n"
        "MOV R0, R12\n"
        "bl LoadScene_patch\n");
    asm volatile(
        "mov r12, %0\n"
        "pop {r0-r10}\n"
        "LDR.W R11, [R9,R2,LSL#2]\n"
        "bx r12\n"
    :: "r" (LoadScene_BackTo));
}

// Colorpicker
typedef bool (*IsWTouched)(CWidget*);
DECL_HOOK(float, GetColorPickerValue, CWidgetRegionColorPicker* self)
{
    static float prevVal = 0.0f;
    if((*(IsWTouched*)(self->vtable + 80))(self) != false) // IsTouched
    {
        CVector2D v; GetTouchPosition(&v, self->cachedPosNum);
        
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

// Light shadows from poles
uintptr_t ProcessLightsForEntity_BackTo;
float fLightDist = 40.0f;
extern "C" void ProcessLightsForEntity_patch(CEntity* ent, C2dEffect* eff, int effectNum)
{
    if(eff->light.m_fShadowSize != 0.0f)
    {
        CVector a;
        TransformFromObjectSpace(ent, a, eff->m_vecPosn);
        float intensity = ((float)eff->light.m_nShadowColorMultiplier / 255.0f) * 0.1f * *fSpriteBrightness;
        float zDist = eff->light.m_nShadowZDistance ? eff->light.m_nShadowZDistance : 15.0f;
        StoreStaticShadow((uint32_t)ent + effectNum, 2, eff->light.m_pShadowTex, &a, eff->light.m_fShadowSize, 0.0f, 0.0f, -eff->light.m_fShadowSize,
                           128, intensity * eff->light.m_color.red, intensity * eff->light.m_color.green, intensity * eff->light.m_color.blue, zDist, 1.0f, fLightDist, false, 0.0f);
    }
}
__attribute__((optnone)) __attribute__((naked)) void ProcessLightsForEntity_inject(void)
{
    asm volatile(
        "MOV R0, R9\n"
        "MOV R10, R6\n"
        "PUSH {R1-R7,R9-R11}\n"
        "MOV R1, R8\n"
        "MOV R2, R10\n"
        "BL ProcessLightsForEntity_patch\n");
    asm volatile(
        "MOV R12, %0\n"
        "POP {R1-R7,R9-R11}\n"
        "LDR.W R10, [SP,#0x150+0x98]\n"
        "BX R12\n"
    :: "r" (ProcessLightsForEntity_BackTo));
}

// Green-ish detail texture
#define GREEN_TEXTURE_ID 14
inline void* GetDetailTexturePtr(int texId)
{
    return *(void**)(**(int**)(*detailTexturesStorage + 4 * (texId-1)) + *RasterExtOffset);
}

DECL_HOOKv(emu_TextureSetDetailTexture, void* texture, unsigned int tilingScale)
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

// ClimbDie
DECL_HOOK(bool, ClimbProcessPed, CTask* self, CPed* target)
{
    float save = *ms_fTimeStep; *ms_fTimeStep = fMagic;
    bool ret = ClimbProcessPed(self, target);
    *ms_fTimeStep = save;
    return ret;
}

// Driving school bikes
DECL_HOOK(int, CreateCarGenerator, float x, float y, float z, float angle, int32_t modelId, int16_t color1, int16_t color2, uint8_t forceSpawn, uint8_t alarmChances,
                                   uint8_t doorLockChances, uint16_t minDelay, uint16_t maxDelay, uint8_t iplId, bool ignorePopulationLimit)
{
    if((modelId == 463 || modelId == 521 || modelId == 522) && z == 10.1203f && (int)x == 1174)
    {
        if(y == 1364.832f)
            return CreateCarGenerator(1175.76, y, z, angle, modelId, color1, color2, forceSpawn, alarmChances, doorLockChances, minDelay, maxDelay, iplId, ignorePopulationLimit);
        if(y == 1366.479f)
            return CreateCarGenerator(1175.999, 1368.4, z, angle, modelId, color1, color2, forceSpawn, alarmChances, doorLockChances, minDelay, maxDelay, iplId, ignorePopulationLimit);
        if(y == 1368.359f)
            return CreateCarGenerator(1175.967, 1372.0, z, angle, modelId, color1, color2, forceSpawn, alarmChances, doorLockChances, minDelay, maxDelay, iplId, ignorePopulationLimit);
    }
    return CreateCarGenerator(x, y, z, angle, modelId, color1, color2, forceSpawn, alarmChances, doorLockChances, minDelay, maxDelay, iplId, ignorePopulationLimit);
}

// Fixing a crosshair by very stupid math
static constexpr float ar43 = 4.0f/3.0f;
DECL_HOOKv(DrawCrosshair)
{
    float save1 = *m_f3rdPersonCHairMultX; *m_f3rdPersonCHairMultX = 0.530f - (*ms_fAspectRatio - ar43) * 0.01125f;
    float save2 = *m_f3rdPersonCHairMultY; *m_f3rdPersonCHairMultY = 0.400f + (*ms_fAspectRatio - ar43) * 0.03600f;
    DrawCrosshair();
    *m_f3rdPersonCHairMultX = save1; *m_f3rdPersonCHairMultY = save2;
}

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////     Funcs     ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////
extern "C" void OnModLoad()
{
    cfg->Bind("Author", "", "About")->SetString("[-=KILL MAN=-]");
    cfg->Bind("IdeasFrom", "", "About")->SetString("MTA:SA Team, re3 contributors, JuniorDjjr, ThirteenAG, Blackbird88, 0x416c69, Whitetigerswt, XMDS");
    cfg->Bind("Discord", "", "About")->SetString("https://discord.gg/2MY7W39kBg");
    cfg->Bind("GitHub", "", "About")->SetString("https://github.com/AndroidModLoader/JPatch");
    cfg->Save();

    logger->SetTag("JPatch");
    pGTASA = aml->GetLib("libGTASA.so");
    hGTASA = dlopen("libGTASA.so", RTLD_LAZY);

    // Functions Start //
    SET_TO(RwRenderStateSet,        aml->GetSym(hGTASA, "_Z16RwRenderStateSet13RwRenderStatePv"));
    SET_TO(RwRenderStateGet,        aml->GetSym(hGTASA, "_Z16RwRenderStateGet13RwRenderStatePv"));
    SET_TO(ClearPedWeapons,         aml->GetSym(hGTASA, "_ZN4CPed12ClearWeaponsEv"));
    SET_TO(GetBulletFx,             aml->GetSym(hGTASA, "_ZN14SurfaceInfos_c11GetBulletFxEj"));
    SET_TO(LIB_PointerGetCoordinates, aml->GetSym(hGTASA, "_Z25LIB_PointerGetCoordinatesiPiS_Pf"));
    SET_TO(Touch_IsDoubleTapped,    aml->GetSym(hGTASA, "_ZN15CTouchInterface14IsDoubleTappedENS_9WidgetIDsEbi"));
    SET_TO(Touch_IsHeldDown,        aml->GetSym(hGTASA, "_ZN15CTouchInterface10IsHeldDownENS_9WidgetIDsEi"));
    SET_TO(SetCameraDirectlyBehindForFollowPed, aml->GetSym(hGTASA, "_ZN7CCamera48SetCameraDirectlyBehindForFollowPed_CamOnAStringEv"));
    SET_TO(RestoreCamera,           aml->GetSym(hGTASA, "_ZN7CCamera7RestoreEv"));
    SET_TO(FindPlayerVehicle,       aml->GetSym(hGTASA, "_Z17FindPlayerVehicleib"));
    SET_TO(PhysicalApplyForce,      aml->GetSym(hGTASA, "_ZN9CPhysical10ApplyForceE7CVectorS0_b"));
    SET_TO(GetFrameNodeName,        aml->GetSym(hGTASA, "_Z16GetFrameNodeNameP7RwFrame"));
    SET_TO(SpriteCalcScreenCoors,   aml->GetSym(hGTASA, "_ZN7CSprite15CalcScreenCoorsERK5RwV3dPS0_PfS4_bb"));
    SET_TO(WorldRemoveEntity,       aml->GetSym(hGTASA, "_ZN6CWorld6RemoveEP7CEntity"));
    SET_TO(SetFontColor,            aml->GetSym(hGTASA, "_ZN5CFont8SetColorE5CRGBA"));
    SET_TO(ProcessVerticalLine,     aml->GetSym(hGTASA, "_ZN6CWorld19ProcessVerticalLineERK7CVectorfR9CColPointRP7CEntitybbbbbbP15CStoredCollPoly"));
    SET_TO(CreateEntityRwObject,    aml->GetSym(hGTASA, "_ZN7CEntity14CreateRwObjectEv"));
    SET_TO(RequestModel,            aml->GetSym(hGTASA, "_ZN10CStreaming12RequestModelEii"));
    SET_TO(LoadAllRequestedModels,  aml->GetSym(hGTASA, "_ZN10CStreaming22LoadAllRequestedModelsEb"));
    SET_TO(AddAnimBlockRef,         aml->GetSym(hGTASA, "_ZN12CAnimManager15AddAnimBlockRefEi"));
    SET_TO(TimerStop,               aml->GetSym(hGTASA, "_ZN6CTimer4StopEv"));
    SET_TO(TimerUpdate,             aml->GetSym(hGTASA, "_ZN6CTimer6UpdateEv"));
    SET_TO(RemoveLeastUsedModel,    aml->GetSym(hGTASA, "_ZN10CStreaming20RemoveLeastUsedModelEj"));
    SET_TO(GetTouchPosition,        aml->GetSym(hGTASA, "_ZN15CTouchInterface16GetTouchPositionEi"));
    SET_TO(StoreStaticShadow,       aml->GetSym(hGTASA, "_ZN8CShadows17StoreStaticShadowEjhP9RwTextureP7CVectorffffshhhfffbf"));
    SET_TO(TransformPoint,          aml->GetSym(hGTASA, "_Z14TransformPointR5RwV3dRK16CSimpleTransformRKS_"));
    // Functions End   //
    
    // Variables Start //
    SET_TO(ms_fTimeStep,            aml->GetSym(hGTASA, "_ZN6CTimer12ms_fTimeStepE"));
    SET_TO(WorldPlayers,            *(void**)(pGTASA + 0x6783C8)); // Patched CWorld::Players will work now!
    SET_TO(ms_fFOV,                 aml->GetSym(hGTASA, "_ZN5CDraw7ms_fFOVE"));
    SET_TO(game_FPS,                aml->GetSym(hGTASA, "_ZN6CTimer8game_FPSE"));
    SET_TO(TheCamera,               aml->GetSym(hGTASA, "TheCamera"));
    SET_TO(RsGlobal,                aml->GetSym(hGTASA, "RsGlobal"));
    SET_TO(g_surfaceInfos,          aml->GetSym(hGTASA, "g_surfaceInfos"));
    SET_TO(m_snTimeInMilliseconds,  aml->GetSym(hGTASA, "_ZN6CTimer22m_snTimeInMillisecondsE"));
    SET_TO(gMobileMenu,             aml->GetSym(hGTASA, "gMobileMenu"));
    SET_TO(NumberOfSearchLights,    aml->GetSym(hGTASA, "_ZN5CHeli20NumberOfSearchLightsE"));
    SET_TO(lastDevice,              aml->GetSym(hGTASA, "lastDevice"));
    SET_TO(m_pWidgets,              *(void**)(pGTASA + 0x67947C)); // Patched CTouchInterface::m_pWidgets will work now!
    SET_TO(bDidWeProcessAnyCinemaCam, aml->GetSym(hGTASA, "bDidWeProcessAnyCinemaCam"));
    SET_TO(bRunningCutscene,        aml->GetSym(hGTASA, "_ZN12CCutsceneMgr10ms_runningE"));
    SET_TO(ScriptParams,            *(void**)(pGTASA + 0x676F7C)); // Patched ScriptParams will work now!
    SET_TO(m_alphaList,             aml->GetSym(hGTASA, "_ZN18CVisibilityPlugins11m_alphaListE"));
    SET_TO(CloudsRotation,          aml->GetSym(hGTASA, "_ZN7CClouds13CloudRotationE"));
    SET_TO(CloudsIndividualRotation, aml->GetSym(hGTASA, "_ZN7CClouds18IndividualRotationE"));
    SET_TO(WeatherWind,             aml->GetSym(hGTASA, "_ZN8CWeather4WindE"));
    SET_TO(pObjectPool,             *(void**)(pGTASA + 0x676BBC));
    SET_TO(m_pCurrZoneInfo,         aml->GetSym(hGTASA, "_ZN9CPopCycle15m_pCurrZoneInfoE"));
    SET_TO(m_ZoneFadeTimer,         aml->GetSym(hGTASA, "_ZN4CHud15m_ZoneFadeTimerE"));
    SET_TO(ms_numAnimBlocks,        aml->GetSym(hGTASA, "_ZN12CAnimManager16ms_numAnimBlocksE"));
    SET_TO(ms_memoryUsed,           aml->GetSym(hGTASA, "_ZN10CStreaming13ms_memoryUsedE"));
    SET_TO(ms_memoryAvailable,      aml->GetSym(hGTASA, "_ZN10CStreaming18ms_memoryAvailableE"));
    SET_TO(fSpriteBrightness,       pGTASA + 0x966590);
    SET_TO(detailTexturesStorage,   aml->GetSym(hGTASA, "_ZN22TextureDatabaseRuntime14detailTexturesE") + 8); // pGTASA + 0x6BD1D8
    SET_TO(textureDetail,           aml->GetSym(hGTASA, "textureDetail"));
    SET_TO(RasterExtOffset,         aml->GetSym(hGTASA, "RasterExtOffset"));
    SET_TO(m_f3rdPersonCHairMultX,  aml->GetSym(hGTASA, "_ZN7CCamera22m_f3rdPersonCHairMultXE"));
    SET_TO(m_f3rdPersonCHairMultY,  aml->GetSym(hGTASA, "_ZN7CCamera22m_f3rdPersonCHairMultYE"));
    SET_TO(ms_fAspectRatio,         aml->GetSym(hGTASA, "_ZN5CDraw15ms_fAspectRatioE"));
    // Variables End   //

    // Animated textures
    if(cfg->Bind("EnableAnimatedTextures", true, "Visual")->GetBool())
    {
        aml->Write(aml->GetSym(hGTASA, "RunUVAnim"), (uintptr_t)"\x01", 1);
    }

    // Vertex weight
    if(cfg->Bind("FixVertexWeight", true, "Visual")->GetBool())
    {
        aml->Write(pGTASA + 0x1C8064, (uintptr_t)"\x01", 1);
        aml->Write(pGTASA + 0x1C8082, (uintptr_t)"\x01", 1);
    }

    // Fix moon!
    // War Drum moment: cannot get Alpha testing to work
    //if(cfg->Bind("MoonPhases", true, "Visual")->GetBool())
    //{
    //    //aml->Write(pGTASA + 0x1AF5C2, (uintptr_t)"\x4F\xF0\x00\x03", 4);
    //    MoonVisual_1_BackTo = pGTASA + 0x59ED90 + 0x1;
    //    MoonVisual_2_BackTo = pGTASA + 0x59EE4E + 0x1;
    //    Redirect(pGTASA + 0x59ED80 + 0x1, (uintptr_t)MoonVisual_1_inject);
    //    Redirect(pGTASA + 0x59EE36 + 0x1, (uintptr_t)MoonVisual_2_inject);
    //}

    // Fix sky multitude
    if(cfg->Bind("FixSkyMultitude", true, "Visual")->GetBool())
    {
        aml->Unprot(pGTASA + 0x59FB8C, 2*sizeof(float));
        *(float*)(pGTASA + 0x59FB8C) = -10.0f;
        *(float*)(pGTASA + 0x59FB90) =  10.0f;
    }

    // Fix vehicles backlights light state
    if(cfg->Bind("FixCarsBacklightLightState", true, "Visual")->GetBool())
    {
        aml->Write(pGTASA + 0x591272, (uintptr_t)"\x02", 1);
        aml->Write(pGTASA + 0x59128E, (uintptr_t)"\x02", 1);
    }

    // Limit sand/dust particles on bullet impact (they are EXTREMELY dropping FPS)
    if(cfg->Bind("LimitSandDustBulletParticles", true, "Visual")->GetBool())
    {
        AddBulletImpactFx_BackTo = pGTASA + 0x36478E + 0x1;
        Redirect(pGTASA + 0x36477C + 0x1, (uintptr_t)AddBulletImpactFx_inject);
        if(cfg->Bind("LimitSandDustBulletParticlesWithSparkles", false, "Visual")->GetBool())
        {
            nLimitWithSparkles = BULLETFX_SPARK;
        }
    }

    // Do not set primary color to the white on vehicles paintjob
    if(cfg->Bind("PaintJobDontSetPrimaryToWhite", true, "Visual")->GetBool())
    {
        aml->PlaceNOP(pGTASA + 0x582328, 2);
    }

    // Fix walking while rifle-aiming
    if(cfg->Bind("FixAimingWalkRifle", true, "Gameplay")->GetBool())
    {
        HOOKPLT(ControlGunMove, pGTASA + 0x66F9D0);
    }

    // Fix slow swimming speed
    if(cfg->Bind("SwimmingSpeedFix", true, "Gameplay")->GetBool())
    {
        SwimmingResistanceBack_BackTo = pGTASA + 0x53BD3A + 0x1;
        HOOKPLT(ProcessSwimmingResistance, pGTASA + 0x66E584);
        Redirect(pGTASA + 0x53BD30 + 0x1, (uintptr_t)SwimmingResistanceBack_inject);
    }

    // Fix stealable items sucking
    if(cfg->Bind("ClampObjectToStealDist", true, "Gameplay")->GetBool())
    {
        aml->Write(pGTASA + 0x40B162, (uintptr_t)"\xB7\xEE\x00\x0A", 4);
    }

    // Fix broken basketball minigame by placing the save icon away from it
    if(cfg->Bind("MaddDoggMansionSaveFix", true, "SCMFixes")->GetBool())
    {
        HOOKPLT(GenerateNewPickup_MaddDogg, pGTASA + 0x674DE4);
    }

    // Fix broken basketball minigame by placing the save icon away from it
    if(cfg->Bind("FixStarBribeInSFBuilding", true, "SCMFixes")->GetBool())
    {
        HOOKPLT(GenerateNewPickup_SFBribe, pGTASA + 0x674DE4);
    }

    // Fix rifle pickup that stuck inside the stadium
    if(cfg->Bind("FixSFStadiumRiflePickup", true, "SCMFixes")->GetBool())
    {
        HOOKPLT(GenerateNewPickup_SFRiflePickup, pGTASA + 0x674DE4);
    }

    // Remove jetpack leaving on widget press while in air?
    if(cfg->Bind("DisableDropJetPackInAir", true, "Gameplay")->GetBool())
    {
        HOOKPLT(DropJetPackTask, pGTASA + 0x675AA8);
    }

    // Dont stop the car before leaving it
    if(cfg->Bind("ImmediatelyLeaveTheCar", true, "Gameplay")->GetBool())
    {
        aml->PlaceNOP(pGTASA + 0x409A18, 3);
    }

    // Bring back penalty when CJ dies!
    if(cfg->Bind("WeaponPenaltyIfDied", true, "Gameplay")->GetBool())
    {
        DiedPenalty_BackTo = pGTASA + 0x3088E0 + 0x1;
        Redirect(pGTASA + 0x3088BE + 0x1, (uintptr_t)DiedPenalty_inject);
    }

    // Fix emergency vehicles
    if(cfg->Bind("FixEmergencyVehicles", true, "Gameplay")->GetBool())
    {
        EmergencyVeh_BackTo = pGTASA + 0x3DD88C + 0x1;
        Redirect(pGTASA + 0x3DD87A + 0x1, (uintptr_t)EmergencyVeh_inject);
        HOOKPLT(SetFOV_Emergency, pGTASA + 0x673DDC);
        aml->Write(pGTASA + 0x3DD8A0, (uintptr_t)"\xB0\xEE\x42\x1A", 4); // WarDumbs are multiplying it by 0.8? Reasonable for 2013 but why didnt they remove that in 2.00?
        aml->Write(pGTASA + 0x3DD8A4, (uintptr_t)"\xB0\xEE\x40\x2A", 4); // Same thing but by 0.875... Cringe.
    }

    // Fix cutscene FOV (disabled by default right now, causes the camera being too close on ultrawide screens)
    if(cfg->Bind("FixCutsceneFOV", false, "Visual")->GetBool())
    {
        HOOKPLT(SetFOV, pGTASA + 0x673DDC);
    }

    // Fix red marker that cannot be placed in a menu on ultrawide screens
    // Kinda trashy fix...
    if(cfg->Bind("FixRedMarkerUnplaceable", true, "Gameplay")->GetBool())
    {
        aml->Unprot(pGTASA + 0x2A9E60, sizeof(float));
        *(float*)(pGTASA + 0x2A9E60) /= 1.2f;
        aml->Write(pGTASA + 0x2A9D42, (uintptr_t)"\x83\xEE\x0C\x3A", 4);
        HOOKPLT(PlaceRedMarker_MarkerFix, pGTASA + 0x6702C8);
    }

    // Dont set player on fire when he's on burning BMX (MTA:SA)
    if(cfg->Bind("DontBurnPlayerOnBurningBMX", true, "Gameplay")->GetBool())
    {
        Redirect(pGTASA + 0x3F1ECC + 0x1, pGTASA + 0x3F1F24 + 0x1);
    }

    // Increase the number of vehicles types (not actual vehicles) that can be loaded at once (MTA:SA)
    // Causes crash and completely useless
    //if(cfg->Bind("DesiredNumOfCarsLoadedBuff", true, "Gameplay")->GetBool())
    //{
    //    *(unsigned char*)(aml->GetSym(hGTASA, "_ZN10CStreaming24desiredNumVehiclesLoadedE")) = 50; // Game hardcoded to 50 max (lazy to fix crashes for patches below)
    //    aml->PlaceNOP(pGTASA + 0x46BE1E, 1);
    //    aml->PlaceNOP(pGTASA + 0x47269C, 2);
    //}

    // THROWN projectiles throw more accurately (MTA:SA)
    if(cfg->Bind("ThrownProjectilesAccuracy", true, "Gameplay")->GetBool())
    {
        Redirect(pGTASA + 0x5DBBC8 + 0x1, pGTASA + 0x5DBD0C + 0x1);
    }

    // Disable call to FxSystem_c::GetCompositeMatrix in CAEFireAudioEntity::UpdateParameters 
    // that was causing a crash - spent ages debugging, the crash happens if you create 40 or 
    // so vehicles that catch fire (upside down) then delete them, repeating a few times.
    // (MTA:SA)
    if(cfg->Bind("GetCompositeMatrixFixPossibleCrash", true, "Gameplay")->GetBool())
    {
        aml->PlaceNOP(pGTASA + 0x395E6A, 7);
    }

    // Disable setting the occupied's vehicles health to 75.0f when a burning ped enters it (MTA:SA)
    if(cfg->Bind("DontGiveCarHealthFromBurningPed", true, "Gameplay")->GetBool())
    {
        aml->PlaceNOP(pGTASA + 0x3F1CAC, 0xD);
    }

    // Increase intensity of vehicle tail light corona (MTA:SA)
    // Is this even working on Android?
    if(cfg->Bind("IncreaseTailLightIntensity", true, "Gameplay")->GetBool())
    {
        aml->Write(pGTASA + 0x591016, (uintptr_t)"\xF0", 1);
    }

    // Cinematic vehicle camera on double tap
    if(cfg->Bind("CinematicCameraOnDoubleTap", true, "Gameplay")->GetBool())
    {
        HOOKPLT(PlayerInfoProcess_Cinematic, pGTASA + 0x673E84);
    }
    
    // Fix Skimmer plane ( https://github.com/XMDS )
    if (cfg->Bind("SkimmerPlaneFix", true, "Gameplay")->GetBool())
    {
        SkimmerWaterResistance_BackTo = pGTASA + 0x589ADC + 0x1;
        Redirect(pGTASA + 0x589AD4 + 0x1, (uintptr_t)SkimmerWaterResistance_inject);
    }

    // Buff streaming
    if(cfg->Bind("BuffStreamingMem", true, "Gameplay")->GetBool())
    {
        int wantsMB = cfg->Bind("BuffStreamingMem_CountMB", 512, "Gameplay")->GetInt();
        if(wantsMB >= 20)
        {
            aml->PlaceNOP(pGTASA + 0x46BE18, 1);
            aml->PlaceNOP(pGTASA + 0x47272A, 2);
            aml->PlaceNOP(pGTASA + 0x472690, 2);
            if(*ms_memoryAvailable < wantsMB*1024*1024)
            {
                *ms_memoryAvailable = wantsMB * 1024 * 1024;
            }
        }
    }

    // Buff streaming (dynamic)
    bDynStreamingMem = cfg->Bind("DynamicStreamingMem", true, "Gameplay")->GetBool();
    fDynamicStreamingMemPercentage = 0.001f * cfg->Bind("DynamicStreamingMem_Percentage", 80, "Gameplay")->GetInt();

    // Buff planes max height
    if(cfg->Bind("BuffPlanesMaxHeight", true, "Gameplay")->GetBool())
    {
        float* heights;
        aml->Unprot(pGTASA + 0x585674, sizeof(float)*7);
        SET_TO(heights, pGTASA + 0x585674);
        for(int i = 0; i < 7; ++i)
        {
            heights[i] *= 1.25f;
        }
    }

    // Buff jetpack max height
    if(cfg->Bind("BuffJetpackMaxHeight", true, "Gameplay")->GetBool())
    {
        aml->Unprot(pGTASA + 0x5319D0, sizeof(float));
        *(float*)(pGTASA + 0x5319D0) *= 2.0f;
    }

    // 44100 Hz Audio support (without a mod OpenAL Update) (is this working?)
    if(cfg->Bind("Allow44100HzAudio", true, "Gameplay")->GetBool())
    {
        aml->Unprot(pGTASA + 0x613E0A, sizeof(int));
        *(int*)(pGTASA + 0x613E0A) = 44100;
    }

    // Disable GTA vehicle detachment at rotation awkwardness
    if(cfg->Bind("FixVehicleDetachmentAtRot", true, "Visual")->GetBool())
    {
        Redirect(pGTASA + 0x407344 + 0x1, pGTASA + 0x407016 + 0x1);
    }

    // Bring back missing "Shoot" button for S.W.A.T. when we dont have a weapon. WarDrum forgot about it.
    if(cfg->Bind("FixMissingShootBtnForSWAT", true, "Gameplay")->GetBool())
    {
        GetCarGunFired_BackTo1 = pGTASA + 0x3F99E8 + 0x1;
        GetCarGunFired_BackTo2 = pGTASA + 0x3F9908 + 0x1;
        Redirect(pGTASA + 0x3F99C4 + 0x1, (uintptr_t)GetCarGunFired_inject);
    }

    // Just a fuzzy seek. Tell MPG123 to not load useless data.
    if(cfg->Bind("FuzzySeek", true, "Gameplay")->GetBool())
    {
        HOOKPLT(mpg123_param, pGTASA + 0x66F3D4);
    }

    // Fix water cannon on a high fps
    if(cfg->Bind("FixHighFPSWaterCannons", true, "Gameplay")->GetBool())
    {
        HOOKPLT(WaterCannonRender, pGTASA + 0x67432C);
        HOOKPLT(WaterCannonUpdate, pGTASA + 0x6702EC);
    }

    // Fix moving objects on a high fps (through the scripts)
    if(cfg->Bind("FixHighFPSOpcode034E", true, "SCMFixes")->GetBool())
    {
        ProcessCommands800To899_BackTo = pGTASA + 0x347866 + 0x1;
        Redirect(pGTASA + 0x346E84 + 0x1, (uintptr_t)ProcessCommands800To899_inject);
    }

    // Fix pushing force
    if(cfg->Bind("FixPhysicalPushForce", true, "Gameplay")->GetBool())
    {
        PhysicalApplyCollision_BackTo = pGTASA + 0x402B72 + 0x1;
        Redirect(pGTASA + 0x402B68 + 0x1, (uintptr_t)PhysicalApplyCollision_inject);
    }

    // Can now rotate the camera inside the heli/plane?
    // https://github.com/TheOfficialFloW/gtasa_vita/blob/6417775e182b0c8b789cc9a0c1161e6f1b43814f/loader/main.c#L736
    if(cfg->Bind("UnstuckHeliCamera", true, "Gameplay")->GetBool())
    {
        aml->Write(pGTASA + 0x3C0866, (uintptr_t)"\x00\x20\x00\xBF", 4);
        aml->Write(pGTASA + 0x3C1518, (uintptr_t)"\x00\x20\x00\xBF", 4);
        aml->Write(pGTASA + 0x3C198A, (uintptr_t)"\x00\x20\x00\xBF", 4);
        aml->Write(pGTASA + 0x3FC462, (uintptr_t)"\x00\x20\x00\xBF", 4);
        aml->Write(pGTASA + 0x3FC754, (uintptr_t)"\x00\x20\x00\xBF", 4);
    }

    // Classic CJ shadow
    if(cfg->Bind("FixClassicCJShadow", true, "Gameplay")->GetBool())
    {
        aml->PlaceNOP(pGTASA + 0x5B86C4, 7);
    }

    // Car Slowdown Fix
    if(cfg->Bind("FixCarSlowdownHighFPS", true, "Gameplay")->GetBool())
    {
        SET_TO(mod_HandlingManager_off4, (*(uintptr_t*)(pGTASA + 0x6777C8)) + 4); // FLA
        HOOKPLT(ProcessVehicleWheel, pGTASA + 0x66FC7C);
    }

    // Heli rotor speed fix
    if(cfg->Bind("FixHeliRotorSpeedHighFPS", true, "Visual")->GetBool())
    {
        aml->Unprot(pGTASA + 0x572604, sizeof(float)*5);
        SET_TO(fRotorFinalSpeed, pGTASA + 0x572604);
        SET_TO(fRotor1Speed, pGTASA + 0x572610);
        SET_TO(fRotor2Speed, pGTASA + 0x572608);
        HOOK(Heli_ProcessFlyingStuff, aml->GetSym(hGTASA, "_ZN5CHeli21ProcessFlyingCarStuffEv"));
    }

    // Give more space for opcodes 038D+038F
    if(cfg->Bind("FixOpcodes038D/F", true, "SCMFixes")->GetBool())
    {
        aml->Unprot(pGTASA + 0x678EAC, sizeof(void*));
        *(uintptr_t*)(pGTASA + 0x678EAC) = (uintptr_t)pNewScriptSprites;
        aml->Unprot(pGTASA + 0x67915C, sizeof(void*));
        *(uintptr_t*)(pGTASA + 0x67915C) = (uintptr_t)pNewIntroRectangles;

        aml->Write(pGTASA + 0x327E6E, (uintptr_t)"\xB6\xF5\xC0\x6F", 4); // CMissionCleanup::Process
        aml->Write(pGTASA + 0x328298, (uintptr_t)"\xB4\xF5\xC0\x6F", 4); // CTheScripts::RemoveScriptTextureDictionary
        aml->Write(pGTASA + 0x32A638, (uintptr_t)"\xB4\xF5\xC0\x6F", 4); // CTheScripts::Init
        aml->Write(pGTASA + 0x1A3736, (uintptr_t)"\x40\xF2\xFC\x54", 4); // sub_1A3730
        aml->Write(pGTASA + 0x1A37F4, (uintptr_t)"\xB5\xF5\xC0\x6F", 4); // sub_1A3750
        
        aml->Write(pGTASA + 0x329E6C, (uintptr_t)"\xB8\xF5\xB4\x4F", 4); // CTheScripts::DrawScriptSpritesAndRectangles
        aml->Write(pGTASA + 0x32A5FC, (uintptr_t)"\xB6\xF5\xB4\x4F", 4); // CTheScripts::Init
        aml->Write(pGTASA + 0x32B040, (uintptr_t)"\xB2\xF5\xB4\x4F", 4); // CTheScripts::Process
    }

    // RE3: Fix R* optimization that prevents peds to spawn
    if(cfg->Bind("Re3_PedSpawnDeoptimize", true, "Gameplay")->GetBool())
    {
        aml->Write(pGTASA + 0x3F40E8, (uintptr_t)"\x03", 1);
    }

    // RE3: Make cars and peds to not despawn when you look away
    if(cfg->Bind("Re3_ExtOffscreenDespRange", true, "Gameplay")->GetBool())
    {
        Redirect(pGTASA + 0x2EC660 + 0x1, pGTASA + 0x2EC6D6 + 0x1); // Vehicles
        Redirect(pGTASA + 0x4CE4EA + 0x1, pGTASA + 0x4CE55C + 0x1); // Peds
    }

    // RE3: Do not remove locked cars
    if(cfg->Bind("Re3_DontRemoveLockedCars", true, "Gameplay")->GetBool())
    {
        HOOKPLT(PossiblyRemoveVehicle_Re3, pGTASA + 0x6736E4);
    }

    // RE3: Correct clouds rotating speed
    if(cfg->Bind("Re3_CloudsRotationHighFPS", true, "Visual")->GetBool())
    {
        HOOKPLT(CloudsUpdate_Re3, pGTASA + 0x670358);
    }

    // RE3: multiple instances of semaphore fix
    if(cfg->Bind("Re3_CdStreamMultipleInst", true, "Gameplay")->GetBool())
    {
        aml->Unprot(pGTASA + 0x26C0DA, 4);
        aml->Write(pGTASA + 0x2C97DC, (uintptr_t)"\x00\x23", 2);
        SET_TO(ThreadLaunch_GagNameSet, pGTASA + 0x26C0DA);
        HOOKPLT(OS_ThreadLaunch, pGTASA + 0x66E8A0);
    }

    // RE3: Fix a lil mistake in AskForObjectToBeRenderedInGlass
    if(cfg->Bind("Re3_InGlassRenderedPlus1", true, "Visual")->GetBool())
    {
        aml->Write(pGTASA + 0x5AC61C, (uintptr_t)"\x1F", 1);
    }

    // RE3: Free the space for an object in a pool by deleting temp objects if there is no space
    if(cfg->Bind("Re3_FreePlaceInObjectPool", true, "Gameplay")->GetBool())
    {
        HOOKPLT(Object_New, pGTASA + 0x6726EC);
    }

    // Lower threads sleeping timer
    if(cfg->Bind("LowerThreadsSleeping", true, "Gameplay")->GetBool())
    {
        aml->Write(pGTASA + 0x1D248E, (uintptr_t)"\x08", 1);
        aml->Write(pGTASA + 0x266D3A, (uintptr_t)"\x08", 1);
        aml->Write(pGTASA + 0x26706E, (uintptr_t)"\x08", 1);
        aml->Write(pGTASA + 0x26FDCC, (uintptr_t)"\x08", 1);
    }

    // Dont kill peds when jacking their car, monster!
    if(cfg->Bind("DontKillPedsOnCarJacking", true, "Gameplay")->GetBool())
    {
        Redirect(pGTASA + 0x4F5FC4 + 0x1, pGTASA + 0x4F5FD6 + 0x1);
    }

    // Colored zone names are back
    if(cfg->Bind("ColoredZoneNames", true, "Visual")->GetBool())
    {
        ColoredZoneNames_BackTo = pGTASA + 0x438404 + 0x1;
        Redirect(pGTASA + 0x4383D6 + 0x1, (uintptr_t)ColoredZoneNames_inject);
    }

    // Bigger max count of peds
    if(cfg->Bind("BuffMaxPedsCount", true, "Gameplay")->GetBool())
    {
        *(int*)aml->GetSym(hGTASA, "_ZN11CPopulation20MaxNumberOfPedsInUseE") = 0x23;
        aml->Write(pGTASA + 0x3F4DE0, (uintptr_t)"\x23", 1);
        aml->Write(pGTASA + 0x4CC284, (uintptr_t)"\x23", 1);
        aml->Write(pGTASA + 0x4CCBE0, (uintptr_t)"\x23", 1);
        aml->Write(pGTASA + 0x4CCBEA, (uintptr_t)"\x1C", 1);
    }

    // Bigger max count of cars
    if(cfg->Bind("BuffMaxCarsCount", true, "Gameplay")->GetBool())
    {
        *(int*)aml->GetSym(hGTASA, "_ZN8CCarCtrl20MaxNumberOfCarsInUseE") = 0x14;
        aml->Write(pGTASA + 0x3F4DD2, (uintptr_t)"\x14", 1);
    }

    // Frick your "improved characters models", War Dumb
    if(cfg->Bind("FixPedSpecInShaders", true, "Visual")->GetBool())
    {
        aml->Write(pGTASA + 0x1CE2F0, (uintptr_t)"\x40\x46\x00\xBF", 4);
        aml->Write(pGTASA + 0x1CEDC4, (uintptr_t)"\x40\xF2\x40\x60", 4);
        aml->Write(pGTASA + 0x1CEF1A, (uintptr_t)"\x58\x46\x00\xBF", 4);
        Redirect(pGTASA + 0x1CF5C8 + 0x1, pGTASA + 0x1CF658 + 0x1);
    }

    // Tells "FindGroundZ" functions that we need can teleport on objects too
    if(cfg->Bind("IncludeObjectsForFindZ", true, "Gameplay")->GetBool())
    {
        HOOKPLT(FindGroundZ2D, pGTASA + 0x66EDB8);
        HOOKPLT(FindGroundZ3D, pGTASA + 0x67022C);
    }

    // RE3: 
    //if(cfg->Bind("Re3_WetRoadsReflections", true, "Visual")->GetBool())
    //{
    //    //RoadReflections_BackTo1 = pGTASA + 0x5A2E30 + 0x1;
    //    //RoadReflections_BackTo2 = pGTASA + 0x5A2EA6 + 0x1;
    //    //Redirect(pGTASA + 0x5A2E22 + 0x1, (uintptr_t)RoadReflections_inject1);
    //    //Redirect(pGTASA + 0x5A2E9C + 0x1, (uintptr_t)RoadReflections_inject2);
    //    //aml->Write(pGTASA + 0x5A2F18, (uintptr_t)"\x3A\x46", 2);
    //    //aml->Write(pGTASA + 0x5A2EF6, (uintptr_t)"\xFF", 1);
    //    //HOOKPLT(RenderRefl, pGTASA + 0x6738D4);
    //    //HOOKPLT(CalcSpriteCoords, pGTASA + 0x674B3C);
    //    //HOOKPLT(RenderBufferedOneXLUSprite, pGTASA + 0x673890);
    // None of those are working.
    //}

    // Helicopter's rotor blur
    //if(cfg->Bind("HeliRotorBlur", true, "Visual")->GetBool())
    //{
    //    HOOKPLT(RenderAlphaAtomics, pGTASA + 0x670E90);
    //}

    /* ImprovedStreaming by ThirteenAG & Junior_Djjr */
    /* ImprovedStreaming by ThirteenAG & Junior_Djjr */
    /* ImprovedStreaming by ThirteenAG & Junior_Djjr */
    
    // Preload LOD models
    bPreloadLOD = cfg->Bind("IS_PreloadLODs", false, "Gameplay")->GetBool();
    bPreloadAnim = cfg->Bind("IS_PreloadAnims", false, "Gameplay")->GetBool();
    //bPreloadPed = cfg->Bind("PreloadPeds", true, "Gameplay")->GetBool();
    if(bPreloadLOD || bPreloadAnim || bPreloadPed || bDynStreamingMem)
    {
        HOOKPLT(GameProcess, pGTASA + 0x66FE58);
        if(bPreloadLOD)
        {
            LoadScene_BackTo = pGTASA + 0x4691E2 + 0x1;
            Redirect(pGTASA + 0x4691D6 + 0x1, (uintptr_t)LoadScene_inject);
        }
        if(bPreloadPed)
        {
            HOOKPLT(AddPedModel, pGTASA + 0x675D98);
        }
    }
    bUnloadUnusedModels = cfg->Bind("IS_UnloadUnusedModels", true, "Gameplay")->GetBool();
    if(bUnloadUnusedModels)
    {
        bDontUnloadInCutscenes = !cfg->Bind("IS_UnloadUnusedModels_InCutscene", false, "Gameplay")->GetBool();
        fRemoveUnusedStreamMemPercentage = 0.001f * cfg->Bind("IS_UnloadUnusedModels_Percentage", 80, "Gameplay")->GetInt();
        nRemoveUnusedInterval = cfg->Bind("IS_UnloadUnusedModels_Interval", 60, "Gameplay")->GetInt();

        if(fRemoveUnusedStreamMemPercentage < 0.001f ||
           fRemoveUnusedStreamMemPercentage > 0.99f  ||
           nRemoveUnusedInterval < 0)
        {
            bUnloadUnusedModels = false;
        }
    }

    // Fix color picker widget
    if(cfg->Bind("FixColorPicker", true, "Visual")->GetBool())
    {
        HOOKPLT(GetColorPickerValue, pGTASA + 0x6645C4);
    }

    // Bigger distance for light coronas
    if(cfg->Bind("BuffDistForLightCoronas", true, "Visual")->GetBool())
    {
        aml->Write(pGTASA + 0x5A4960, (uintptr_t)"\x00\x22\xC4\xF2\xC8\x32", 6); // CEntity::ProcessLightsForEntity
        aml->Write(pGTASA + 0x5A4960, (uintptr_t)"\x00\x20\xC4\xF2\xC8\x32", 6); // CTrafficLights::DisplayActualLight
        aml->Write(pGTASA + 0x56585E, (uintptr_t)"\x00\x21\xC4\xF2\xF0\x21", 6); // CBike::PreRender
        aml->Write(pGTASA + 0x5658FC, (uintptr_t)"\x00\x20\xC4\xF2\xF0\x21", 6); // CBike::PreRender
        aml->Write(pGTASA + 0x573826, (uintptr_t)"\x00\x20\xC4\xF2\x96\x30", 6); // CHeli::SearchLightCone
        aml->Unprot(pGTASA + 0x55BAD0, sizeof(float)); *(float*)(pGTASA + 0x55BAD0) = 300.0f; // CAutomobile::PreRender
    }

    // Bigger distance for light shadows
    if(cfg->Bind("BuffDistForLightShadows", true, "Visual")->GetBool())
    {
        aml->Write(pGTASA + 0x36311C, (uintptr_t)"\xC4\xF2\xF0\x21", 4); // CTrafficLights::DisplayActualLight, 40 -> 120
        aml->Write(pGTASA + 0x3F1996, (uintptr_t)"\xC4\xF2\xF0\x21", 4); // CFireManager::Update, 40 -> 120
        fLightDist = 120.0f; // For thingies below \/
    }

    // Bring back light shadows from poles!
    if(cfg->Bind("BackPolesLightShadow", true, "Visual")->GetBool())
    {
        // CShadows::CastShadowEntityXYZ maybe the reason of broken lightshadows?
        ProcessLightsForEntity_BackTo = pGTASA + 0x5A4DA8 + 0x1;
        Redirect(pGTASA + 0x5A4578 + 0x1, (uintptr_t)ProcessLightsForEntity_inject);
    }

    // Fix greenish detail tex
    if(cfg->Bind("FixGreenTextures", true, "Visual")->GetBool())
    {
        aml->PlaceNOP(pGTASA + 0x1B00B0, 5); // Dont set textureDetail variable! We'll handle it by ourselves!
        HOOK(emu_TextureSetDetailTexture, aml->GetSym(hGTASA, "_Z27emu_TextureSetDetailTexturePvj"));
    }

    // Bring back light shadows from poles!
    if(cfg->Bind("BuffStaticShadowsCount", true, "Gameplay")->GetBool())
    {
        // Static shadows?
        asShadowsStored_NEW = new CRegisteredShadow[0xFF];
        aStaticShadows_NEW = new CStaticShadow[0xFF];
        aml->Write(pGTASA + 0x677BEC, (uintptr_t)&asShadowsStored_NEW, sizeof(void*));
        aml->Write(pGTASA + 0x6798EC, (uintptr_t)&aStaticShadows_NEW, sizeof(void*));

        // Registered Shadows:
        // CShadows::StoreShadowToBeRendered
        aml->Write(pGTASA + 0x5B929A, (uintptr_t)"\xFE", 1);
        aml->Write(pGTASA + 0x5B92C0, (uintptr_t)"\xFE", 1);
        aml->Write(pGTASA + 0x5B92E6, (uintptr_t)"\xFE", 1);
        aml->Write(pGTASA + 0x5B930A, (uintptr_t)"\xFE", 1);
        aml->Write(pGTASA + 0x5B932E, (uintptr_t)"\xFE", 1);
        aml->Write(pGTASA + 0x5B9358, (uintptr_t)"\xFE", 1);
        // CShadows::StoreShadowToBeRendered (2nd arg is RwTexture*)
        aml->Write(pGTASA + 0x5B9444, (uintptr_t)"\xFE", 1);
        // CShadows::StoreShadowForVehicle
        aml->Write(pGTASA + 0x5B9BD4, (uintptr_t)"\xFE", 1);
        aml->Write(pGTASA + 0x5B9B2A, (uintptr_t)"\xFE", 1);
        // CShadows::StoreShadowForPedObject
        aml->Write(pGTASA + 0x5B9F62, (uintptr_t)"\xFE", 1);
        // CShadows::StoreRealTimeShadow
        aml->Write(pGTASA + 0x5BA29E, (uintptr_t)"\xFE", 1);
        // CShadows::RenderExtraPlayerShadows
        aml->Write(pGTASA + 0x5BDDBA, (uintptr_t)"\xFE", 1);
        aml->Write(pGTASA + 0x5BDD5A, (uintptr_t)"\xFE", 1);

        // Static Shadows:
        // CShadows::StoreStaticShadow
        aml->Write(pGTASA + 0x5B8E28, (uintptr_t)"\xFF", 1);
        aml->Write(pGTASA + 0x5B88C6, (uintptr_t)"\xB1\xF5\x7F\x5F", 4); // CMP.W R1, #16320
        // CShadows::RenderStaticShadows
        aml->Write(pGTASA + 0x5BB898, (uintptr_t)"\xFF", 1);
        aml->Write(pGTASA + 0x5BB8AA, (uintptr_t)"\xFF", 1);
        // CShadows::UpdateStaticShadows
        aml->Write(pGTASA + 0x5BD2EC, (uintptr_t)"\xFF", 1);
    }

    // Move shadows closer to the ground
    if(cfg->Bind("MoveShadowsToTheGround", true, "Visual")->GetBool())
    {
        aml->Unprot(pGTASA + 0x5A224C, sizeof(float)); *(float*)(pGTASA + 0x5A224C) = -0.013f;
        aml->Unprot(pGTASA + 0x5B3ED4, sizeof(float)); *(float*)(pGTASA + 0x5B3ED4) = 0.013f;
        aml->Unprot(pGTASA + 0x5BB80C, sizeof(float)); *(float*)(pGTASA + 0x5BB80C) = 0.013f;
        aml->Unprot(pGTASA + 0x5BC188, sizeof(float)); *(float*)(pGTASA + 0x5BC188) = 0.013f;
        aml->Unprot(pGTASA + 0x5E03D0, sizeof(float)); *(float*)(pGTASA + 0x5E03D0) = 0.013f;
    }

    // Radar
    if(cfg->Bind("FixRadarStreaming", true, "Visual")->GetBool())
    {
        Redirect(pGTASA + 0x44313A + 0x1, pGTASA + 0x443146 + 0x1);
    }

    // texture2D bias? In theory, this thing is giving better FPS + better textures
    if(cfg->Bind("NoShaderTextureBias", true, "Visual")->GetBool())
    {
        aml->Write(pGTASA + 0x5EAB20 + 52, (uintptr_t)"      ", 6);
        aml->Write(pGTASA + 0x5EAB94 + 52, (uintptr_t)"      ", 6);
        aml->Write(pGTASA + 0x5EAC96 + 75, (uintptr_t)"      ", 6);
        aml->Write(pGTASA + 0x5EABE8 + 53, (uintptr_t)"      ", 6);
        aml->Write(pGTASA + 0x5EABE8 + 99, (uintptr_t)"      ", 6);
    }

    // Sweet's roof is not that tasty anymore
    if(cfg->Bind("FixClimbDying", true, "Gameplay")->GetBool())
    {
        HOOKPLT(ClimbProcessPed, pGTASA + 0x66CC28);
    }

    // Sweet's roof is not that tasty anymore
    if(cfg->Bind("FixDrivingSchoolBikesSpawn", true, "SCMFixes")->GetBool())
    {
        HOOKPLT(CreateCarGenerator, pGTASA + 0x672808);
    }

    // Fixing a crosshair position by very stupid math
    if(cfg->Bind("FixCrosshair", true, "Visual")->GetBool())
    {
        HOOKPLT(DrawCrosshair, pGTASA + 0x672880);
    }

    // Fix ped conversations are gone
    //if(cfg->Bind("FixPedConversation", true, "Gameplay")->GetBool())
    //{
    //    Redirect(pGTASA + 0x301BFE + 0x1, pGTASA + 0x301C0E + 0x1);
    //}
}