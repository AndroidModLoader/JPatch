#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>
#include <dlfcn.h>

#include "GTASA_STRUCTS.h"

MYMODCFG(net.rusjj.jpatch, JPatch, 1.0.1, RusJJ)

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////     Saves     ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////
uintptr_t pGTASA;
void* hGTASA;
static constexpr float fMagic = 50.0f / 30.0f;

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////     Vars      ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////
float *ms_fTimeStep;
CPlayerInfo* WorldPlayers;

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////     Funcs     ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Redirect(uintptr_t addr, uintptr_t to)
{
    if(!addr) return;
    if(addr & 1)
    {
        addr &= ~1;
        if (addr & 2)
        {
            aml->PlaceNOP(addr, 1);
            addr += 2;
        }
        uint32_t hook[2];
        hook[0] = 0xF000F8DF;
        hook[1] = to;
        aml->Write(addr, (uintptr_t)hook, sizeof(hook));
    }
    else
    {
        uint32_t hook[2];
        hook[0] = 0xE51FF004;
        hook[1] = to;
        aml->Write(addr, (uintptr_t)hook, sizeof(hook));
    }
}
void (*_rwOpenGLSetRenderState)(RwRenderState, int);
void (*_rwOpenGLGetRenderState)(RwRenderState, void*);
void (*ClearPedWeapons)(CPed*);

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////     Hooks     ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////
extern "C" void adadad(void)
{
    asm("VMOV.F32 S0, #0.5");
}
DECL_HOOKv(EntityRender, CEntity* ent)
{
    if(ent->m_nType == ENTITY_TYPE_VEHICLE)
    {
        EntityRender(ent);
        return;
    }

    //RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLNONE);
    EntityRender(ent);
    //RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLBACK);
}
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
DECL_HOOKv(ControlGunMove, void* self, CVector2D* vec2D) // AimingRifleWalkFix
{
    float save = *ms_fTimeStep; *ms_fTimeStep = fMagic;
    ControlGunMove(self, vec2D);
    *ms_fTimeStep = save;
}

uintptr_t SwimmingResistanceBack_BackTo;
float saveStep;
DECL_HOOKv(ProcessSwimmingResistance, void* self, CPed* ped) // SwimSpeedFix
{
    saveStep = *ms_fTimeStep;
    if(ped->m_nPedType == PED_TYPE_PLAYER1) *ms_fTimeStep *= 0.7854f/fMagic;
    else *ms_fTimeStep *= 1.0f/fMagic;
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

DECL_HOOKv(ProcessBuoyancy, void* self, CPhysical* phy, float unk, CVector* vec1, CVector* vec2) // BuoyancySpeedFix
{
    if(phy->m_nType == ENTITY_TYPE_PED && ((CPed*)phy)->m_nPedType == PED_TYPE_PLAYER1)
    {
        float save = *ms_fTimeStep; *ms_fTimeStep = (1.0f + ((save / fMagic) / 1.5f)) * (save / fMagic);
        ProcessBuoyancy(self, phy, unk, vec1, vec2);
        *ms_fTimeStep = save;
        return;
    }
    ProcessBuoyancy(self, phy, unk, vec1, vec2);
}
DECL_HOOK(ScriptHandle, GenerateNewPickup, float x, float y, float z, int16_t modelId, ePickupType pickupType, int ammo, int16_t moneyPerDay, bool isEmpty, const char* msg)
{
    if(modelId == 1277 && x == 1263.05f && y == -773.67f && z == 1091.39f)
    {
        return GenerateNewPickup(1291.2f, -798.0f, 1089.39f, modelId, pickupType, ammo, moneyPerDay, isEmpty, msg);
    }
    return GenerateNewPickup(x, y, z, modelId, pickupType, ammo, moneyPerDay, isEmpty, msg);
}
DECL_HOOKv(DropJetPackTask, void* task, CPed* ped)
{
    if(!ped->m_PedFlags.bIsStanding) return;
    DropJetPackTask(task, ped);
}
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

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////     Funcs     ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////
extern "C" void OnModLoad()
{
    pGTASA = aml->GetLib("libGTASA.so");
    hGTASA = dlopen("libGTASA.so", RTLD_LAZY);

    // Functions Start //
    SET_TO(_rwOpenGLSetRenderState, aml->GetSym(hGTASA, "_Z23_rwOpenGLSetRenderState13RwRenderStatePv"));
    SET_TO(_rwOpenGLGetRenderState, aml->GetSym(hGTASA, "_Z23_rwOpenGLGetRenderState13RwRenderStatePv"));
    SET_TO(ClearPedWeapons, aml->GetSym(hGTASA, "_ZN4CPed12ClearWeaponsEv"));
    // Functions End   //
    
    // Variables Start //
    SET_TO(ms_fTimeStep, aml->GetSym(hGTASA, "_ZN6CTimer12ms_fTimeStepE"));
    SET_TO(WorldPlayers, aml->GetSym(hGTASA, "_ZN6CWorld7PlayersE"));
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

    // Fix vehicle's backface culling that should be disable
    /*if(cfg->Bind("DisableVehiclesBackfaceCulling", true, "Visual")->GetBool())
    {
        HOOKPLT(EntityRender, pGTASA + 0x66F764);
    }*/

    // Fix moon!
    /*if(cfg->Bind("MoonPhases", true, "Visual")->GetBool())
    {
        Redirect(pGTASA + 0x59ED80 + 0x1, (uintptr_t)MoonVisual_1_inject);
        MoonVisual_1_BackTo = pGTASA + 0x59ED90 + 0x1;
        Redirect(pGTASA + 0x59EE36 + 0x1, (uintptr_t)MoonVisual_2_inject);
        MoonVisual_2_BackTo = pGTASA + 0x59EE4E + 0x1;

        //Redirect(pGTASA + 0x59ED6E + 0x1, pGTASA + 0x59EE36 + 0x1);
        //Redirect(pGTASA + 0x59EE36 + 0x1, pGTASA + 0x59EED8 + 0x1);
    }*/

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

    // Buoyancy speed fix (not working)
    /*if(cfg->Bind("BuoyancySpeedFix", true, "Gameplay")->GetBool())
    {
        HOOKPLT(ProcessBuoyancy, pGTASA + 0x67130C);
    }*/

    // Fix stealable items sucking
    if(cfg->Bind("ClampObjectToStealDist", true, "Gameplay")->GetBool())
    {
        aml->Write(pGTASA + 0x40B162, (uintptr_t)"\xB6\xEE\x00\x0A", 4);
    }

    // Fix broken basketball minigame by placing the save icon away from it
    if(cfg->Bind("MaddDoggMansionSaveFix", true, "Gameplay")->GetBool())
    {
        HOOKPLT(GenerateNewPickup, pGTASA + 0x674DE4);
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
    if(cfg->Bind("BrindBackWeaponPenaltyIfDied", true, "Gameplay")->GetBool())
    {
        DiedPenalty_BackTo = pGTASA + 0x3088E0 + 0x1;
        Redirect(pGTASA + 0x3088BE + 0x1, (uintptr_t)DiedPenalty_inject);
    }
}