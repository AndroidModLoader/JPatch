#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>
#include <dlfcn.h>
#include <time.h>

#include "GTASA_STRUCTS.h"

MYMODCFG(net.rusjj.jpatch, JPatch, 1.1.1, RusJJ)

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////     Saves     ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////
uintptr_t pGTASA;
void* hGTASA;
static constexpr float fMagic = 50.0f / 30.0f;
float fEmergencyVehiclesFix;

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////     Vars      ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CPlayerInfo* WorldPlayers;
CCamera* TheCamera;
RsGlobalType* RsGlobal;
MobileMenu *gMobileMenu;
CWidget** m_pWidgets;

float *ms_fTimeStep, *ms_fFOV;
void *g_surfaceInfos;
unsigned int *m_snTimeInMilliseconds;
int *lastDevice;
bool *bDidWeProcessAnyCinemaCam;

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
void (*_rwOpenGLSetRenderState)(RwRenderState, int);
void (*_rwOpenGLGetRenderState)(RwRenderState, void*);
void (*ClearPedWeapons)(CPed*);
eBulletFxType (*GetBulletFx)(void* self, unsigned int surfaceId);
void (*LIB_PointerGetCoordinates)(int, int*, int*, float*);
bool (*Touch_IsDoubleTapped)(WidgetIDs, bool doTapEffect, int idkButBe1);
bool (*Touch_IsHeldDown)(WidgetIDs, int idkButBe1);
void (*SetCameraDirectlyBehindForFollowPed)(CCamera*);
void (*RestoreCamera)(CCamera*);

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////     Hooks     ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////
extern "C" void adadad(void)
{
    asm("VMOV.F32 S0, #0.5");
}

// Moon phases
int moon_alphafunc, moon_vertexblend;
uintptr_t MoonVisual_1_BackTo;
extern "C" void MoonVisual_1(void)
{
    _rwOpenGLGetRenderState(rwRENDERSTATEALPHATESTFUNCTION, &moon_alphafunc);
    _rwOpenGLGetRenderState(rwRENDERSTATEVERTEXALPHAENABLE, &moon_vertexblend);
    _rwOpenGLSetRenderState(rwRENDERSTATEALPHATESTFUNCTION, rwALPHATESTFUNCTIONALWAYS);
    _rwOpenGLSetRenderState(rwRENDERSTATEVERTEXALPHAENABLE, true);

    _rwOpenGLSetRenderState(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
    _rwOpenGLSetRenderState(rwRENDERSTATEDESTBLEND, rwBLENDZERO);
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
    _rwOpenGLSetRenderState(rwRENDERSTATEALPHATESTFUNCTION, moon_alphafunc);
    _rwOpenGLSetRenderState(rwRENDERSTATEVERTEXALPHAENABLE, moon_vertexblend);
    _rwOpenGLSetRenderState(rwRENDERSTATESRCBLEND, rwBLENDONE);
    _rwOpenGLSetRenderState(rwRENDERSTATESRCBLEND, rwBLENDDESTALPHA);
    _rwOpenGLSetRenderState(rwRENDERSTATEZWRITEENABLE, false);
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
    fEmergencyVehiclesFix = 70.0f / factor;
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
uintptr_t jump_addr_00589ADC;
float FixValue;
__attribute__((optnone)) __attribute__((naked)) void SkimmerPlaneFix_00589AD4(void)
{
    asm volatile(
        ".thumb\n"
        ".hidden jump_addr_00589ADC\n"
        ".hidden FixValue\n"
        "PUSH {R0,R1}\n"
        "LDR R0, =(FixValue - 100001f - 2*(100002f-100001f))\n"
        "100001:\n"
        "ADD R0, PC\n"
        "100002:\n"
        "VLDR S4, [R0]\n"
        "POP {R0,R1}\n"
        "VADD.F32 S2, S2, S8\n"
        "PUSH {R0,R1}\n"
        "LDR R0, =(jump_addr_00589ADC - 100001f - 2*(100002f-100001f))\n"
        "100001:\n"
        "ADD R0, PC\n"
        "100002:\n"
        "LDR R0, [R0]\n"
        "STR R0, [SP, #4]\n"
        "POP {R0,PC}\n"
        );
}


// Cinematic camera
int nextTickAllowCinematic = 0;
bool toggledCinematic = false;
DECL_HOOKv(PlayerInfoProcess, CPlayerInfo* info, int playerNum)
{
    PlayerInfoProcess(info, playerNum);

    // Do it for local player only.
    if(info == &WorldPlayers[0])
    {
        if(info->m_pPed->m_pVehicle != NULL && info->m_pPed->m_nPedState == PEDSTATE_DRIVING)
        {
            if(info->m_pPed->m_pVehicle->m_nVehicleType != VEHICLE_TYPE_TRAIN &&
               nextTickAllowCinematic < *m_snTimeInMilliseconds               &&
               Touch_IsDoubleTapped(WIDGETID_CAMERAMODE, true, 1))
            {
                nextTickAllowCinematic = *m_snTimeInMilliseconds + 500;
                toggledCinematic = !TheCamera->m_bEnabledCinematicCamera;
                TheCamera->m_bEnabledCinematicCamera = toggledCinematic;
            }
        }
        else
        {
            if(toggledCinematic)
            {
                TheCamera->m_bEnabledCinematicCamera = false;
                RestoreCamera(TheCamera);
                SetCameraDirectlyBehindForFollowPed(TheCamera);
                //m_pWidgets[WIDGETID_CAMERAMODE]->enabled = false;
                *bDidWeProcessAnyCinemaCam = false;
                toggledCinematic = false;
            }
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////     Funcs     ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////
extern "C" void OnModLoad()
{
    logger->SetTag("JPatch");
    pGTASA = aml->GetLib("libGTASA.so");
    hGTASA = dlopen("libGTASA.so", RTLD_LAZY);

    // Functions Start //
    SET_TO(_rwOpenGLSetRenderState, aml->GetSym(hGTASA, "_Z23_rwOpenGLSetRenderState13RwRenderStatePv"));
    SET_TO(_rwOpenGLGetRenderState, aml->GetSym(hGTASA, "_Z23_rwOpenGLGetRenderState13RwRenderStatePv"));
    SET_TO(ClearPedWeapons,         aml->GetSym(hGTASA, "_ZN4CPed12ClearWeaponsEv"));
    SET_TO(GetBulletFx,             aml->GetSym(hGTASA, "_ZN14SurfaceInfos_c11GetBulletFxEj"));
    SET_TO(LIB_PointerGetCoordinates, aml->GetSym(hGTASA, "_Z25LIB_PointerGetCoordinatesiPiS_Pf"));
    SET_TO(Touch_IsDoubleTapped,    aml->GetSym(hGTASA, "_ZN15CTouchInterface14IsDoubleTappedENS_9WidgetIDsEbi"));
    SET_TO(Touch_IsHeldDown,        aml->GetSym(hGTASA, "_ZN15CTouchInterface10IsHeldDownENS_9WidgetIDsEi"));
    SET_TO(SetCameraDirectlyBehindForFollowPed, aml->GetSym(hGTASA, "_ZN7CCamera48SetCameraDirectlyBehindForFollowPed_CamOnAStringEv"));
    SET_TO(RestoreCamera,           aml->GetSym(hGTASA, "_ZN7CCamera7RestoreEv"));
    // Functions End   //
    
    // Variables Start //
    SET_TO(ms_fTimeStep,            aml->GetSym(hGTASA, "_ZN6CTimer12ms_fTimeStepE"));
    SET_TO(WorldPlayers,            *(void**)(pGTASA + 0x6783C8)); // Patched CWorld::Players will work now!
    SET_TO(ms_fFOV,                 aml->GetSym(hGTASA, "_ZN5CDraw7ms_fFOVE"));
    SET_TO(TheCamera,               aml->GetSym(hGTASA, "TheCamera"));
    SET_TO(RsGlobal,                aml->GetSym(hGTASA, "RsGlobal"));
    SET_TO(g_surfaceInfos,          aml->GetSym(hGTASA, "g_surfaceInfos"));
    SET_TO(m_snTimeInMilliseconds,  aml->GetSym(hGTASA, "_ZN6CTimer22m_snTimeInMillisecondsE"));
    SET_TO(gMobileMenu,             aml->GetSym(hGTASA, "gMobileMenu"));
    SET_TO(lastDevice,              aml->GetSym(hGTASA, "lastDevice"));
    SET_TO(m_pWidgets,              aml->GetSym(hGTASA, "_ZN15CTouchInterface10m_pWidgetsE"));
    SET_TO(bDidWeProcessAnyCinemaCam, aml->GetSym(hGTASA, "bDidWeProcessAnyCinemaCam"));
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

    // Fix moon! (lack of rendering features)
    //if(cfg->Bind("MoonPhases", true, "Visual")->GetBool())
    //{
    //    MoonVisual_1_BackTo = pGTASA + 0x59ED90 + 0x1;
    //    MoonVisual_2_BackTo = pGTASA + 0x59EE4E + 0x1;
    //    Redirect(pGTASA + 0x59ED80 + 0x1, (uintptr_t)MoonVisual_1_inject);
    //    Redirect(pGTASA + 0x59EE36 + 0x1, (uintptr_t)MoonVisual_2_inject);
    //}

    // Fix corona's on wet roads
    //if(cfg->Bind("FixCoronasReflectionOnWetRoads", true, "Visual")->GetBool())
    //{
    //    // Nothing
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
    if(cfg->Bind("MaddDoggMansionSaveFix", true, "Gameplay")->GetBool())
    {
        HOOKPLT(GenerateNewPickup_MaddDogg, pGTASA + 0x674DE4);
    }

    // Fix broken basketball minigame by placing the save icon away from it
    if(cfg->Bind("FixStarBribeInSFBuilding", true, "Gameplay")->GetBool())
    {
        HOOKPLT(GenerateNewPickup_SFBribe, pGTASA + 0x674DE4);
    }

    // Fix rifle pickup that stuck inside the stadium
    if(cfg->Bind("FixSFStadiumRiflePickup", true, "Gameplay")->GetBool())
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
    if(cfg->Bind("DesiredNumOfCarsLoadedBuff", true, "Gameplay")->GetBool())
    {
        aml->Write(aml->GetSym(hGTASA, "_ZN10CStreaming24desiredNumVehiclesLoadedE"), (uintptr_t)"\x7F\x00\x00\x00", 4);
    }

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

    // Default pickup's interiors to 0 instead of 13 (MTA:SA)
    if(cfg->Bind("DefaultPickupInteriorIs0", true, "Gameplay")->GetBool())
    {
        aml->Unprot(pGTASA + 0x452E34, 1);
        *(char*)(pGTASA + 0x452E34) = 0x00;
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
        HOOKPLT(PlayerInfoProcess, pGTASA + 0x673E84);
    }
    
    // Fix Skimmer plane
    if (cfg->Bind("SkimmerPlaneFix", true, "Gameplay")->GetBool())
    {
        FixValue = 30.0 * (*ms_fTimeStep / magic);
        jump_addr_00589ADC = pGTASA + 0x00589ADC + 0x1;
        Redirect(pGTASA + 0x00589AD4 + 0x1, (uintptr_t)SkimmerPlaneFix_00589AD4);
    }

    // Fix those freakin small widgets!
    //if(cfg->Bind("FixWidgetsSizeDropping", true, "Gameplay")->GetBool())
    //{
    //    // Nothing
    //}
}

// AircraftMax  0x585674
// -AircraftMax 0x585678