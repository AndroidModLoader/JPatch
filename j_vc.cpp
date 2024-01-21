#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>

#ifdef AML32
    #include "AArch64_ModHelper/Thumbv7_ASMHelper.h"
    #include "GTAVC_STRUCTS.h"
    #define BYVER(__for32, __for64) (__for32)
    using namespace ThumbV7;
#else
    #include "AArch64_ModHelper/ARMv8_ASMHelper.h"
    #define BYVER(__for32, __for64) (__for64)
    using namespace ARMv8;
#endif

extern int androidSdkVer;
extern uintptr_t pGTAVC;
extern void *hGTAVC;

namespace GTA_VC
{

float fAspectCorrection = 0.0f, fAspectCorrectionDiv = 0.0f;
static constexpr float ar43 = 4.0f / 3.0f;
static constexpr float fMagic = 50.0f / 30.0f;
#define fAspectCorrection (*ms_fAspectRatio - ar43)
#define fAspectCorrectionDiv (*ms_fAspectRatio / ar43)
#define GetTimeStep() (*ms_fTimeStep)
#define GetTimeStepInSeconds() (*ms_fTimeStep / 50.0f)
#define GetTimeStepMagic() (*ms_fTimeStep / fMagic)

union ScriptVariables
{
    int      i;
    float    f;
    uint32_t u;
    void*    p;
};

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////     Vars      ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#if defined(AML32)
CStaticShadow* aStaticShadows_NEW;

float *ms_fTimeStep, *fHeliRotorSpeed, *ms_fAspectRatio;
char *mod_HandlingManager;
int *fpsLimit; // a part of RsGlobal
void *GTouchscreen;
bool *m_PrefsFrameLimiter;
uint32_t *m_snTimeInMilliseconds;
float *m_fCurrentFarClip, *m_fCurrentFogStart;
CPolyBunch *aPolyBunches;

// CPhysical::ApplyCollision
float *fl1679D4;
// CClouds::Update
float *fl1D4CF0, *fl1D4CF4;
#endif

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////     Funcs     ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void (*BrightLightsRender)();
void (*RsEventHandler)(int, void*);
void (*TouchscreenUpdate)(void*, void*, bool);
float (*GetCurrentTimeInCycles)();
uint32_t (*GetCyclesPerMillisecond)();
void (*OS_ThreadSleep)(int);
void (*tbDisplay)();
void (*emu_DistanceFogSetEnabled)(bool);
void (*emu_DistanceFogSetup)(float start, float end, float r, float g, float b);

#ifdef AML32
    #include "patches_vc.inl"
#else
    //#include "patches_vc64.inl" // No VC 1.12
#endif

void JPatch()
{
    #if defined(AML32)
    cfg->Bind("IdeasFrom", "", "About")->SetString("MTA:SA Team, re3 contributors, ThirteenAG, Peepo"); cfg->ClearLast();

    // Functions Start //
    SET_TO(BrightLightsRender, aml->GetSym(hGTAVC, "_ZN13CBrightLights6RenderEv"));
    SET_TO(RsEventHandler, aml->GetSym(hGTAVC, "_Z14RsEventHandler7RsEventPv"));
    SET_TO(TouchscreenUpdate, aml->GetSym(hGTAVC, "_ZN11Touchscreen6UpdateEfb"));
    SET_TO(GetCurrentTimeInCycles, aml->GetSym(hGTAVC, "_ZN6CTimer22GetCurrentTimeInCyclesEv"));
    SET_TO(GetCyclesPerMillisecond, aml->GetSym(hGTAVC, "_ZN6CTimer23GetCyclesPerMillisecondEv"));
    SET_TO(OS_ThreadSleep, aml->GetSym(hGTAVC, "_Z14OS_ThreadSleepi"));
    SET_TO(tbDisplay, aml->GetSym(hGTAVC, "_Z9tbDisplayv"));
    SET_TO(emu_DistanceFogSetEnabled, aml->GetSym(hGTAVC, "_Z25emu_DistanceFogSetEnabledh"));
    SET_TO(emu_DistanceFogSetup, aml->GetSym(hGTAVC, "_Z20emu_DistanceFogSetupfffff"));
    // Functions End   //
    
    // Variables Start //
    SET_TO(ms_fTimeStep, aml->GetSym(hGTAVC, "_ZN6CTimer12ms_fTimeStepE"));
    SET_TO(mod_HandlingManager, *(char**)(pGTAVC + 0x3956B4));
    SET_TO(fHeliRotorSpeed, pGTAVC + 0x255000); aml->Unprot(AS_ADDR(fHeliRotorSpeed), sizeof(float));
    SET_TO(ms_fAspectRatio, aml->GetSym(hGTAVC, "_ZN5CDraw15ms_fAspectRatioE"));
    SET_TO(fpsLimit, pGTAVC + 0x714F1C);
    SET_TO(GTouchscreen, aml->GetSym(hGTAVC, "GTouchscreen"));
    SET_TO(m_PrefsFrameLimiter, aml->GetSym(hGTAVC, "_ZN12CMenuManager19m_PrefsFrameLimiterE"));
    SET_TO(m_snTimeInMilliseconds, aml->GetSym(hGTAVC, "_ZN6CTimer22m_snTimeInMillisecondsE"));
    SET_TO(m_fCurrentFarClip, aml->GetSym(hGTAVC, "_ZN10CTimeCycle17m_fCurrentFarClipE"));
    SET_TO(m_fCurrentFogStart, aml->GetSym(hGTAVC, "_ZN10CTimeCycle18m_fCurrentFogStartE"));
    SET_TO(aPolyBunches, aml->GetSym(hGTAVC, "_ZN8CShadows12aPolyBunchesE"));
    SET_TO(fl1679D4, pGTAVC + 0x1679D4); UNPROT(fl1679D4, sizeof(float));
    SET_TO(fl1D4CF0, pGTAVC + 0x1D4CF0); UNPROT(fl1D4CF0, sizeof(float));
    SET_TO(fl1D4CF4, pGTAVC + 0x1D4CF4); UNPROT(fl1D4CF4, sizeof(float));
    // Variables End   //
    #endif // AML32

    #ifdef AML32
        #include "preparations_vc.inl"
    #else
        //#include "preparations_vc64.inl" // No VC 1.12
    #endif
}

}; // namespace GTA_VC