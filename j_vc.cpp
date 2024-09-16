#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>

#ifdef AML32
    #include "AArch64_ModHelper/Thumbv7_ASMHelper.h"
    #include "GTAVC_STRUCTS.h"
    using namespace ThumbV7;
#else
    #include "AArch64_ModHelper/ARMv8_ASMHelper.h"
    #include "GTAVC_STRUCTS_112.h"
    using namespace ARMv8;
#endif

extern int androidSdkVer;
extern uintptr_t pGTAVC;
extern void *hGTAVC;

#define BUNCHTAILS_EX 6144 // More bunchtails because who knows about Project2DFX?

// Usage of aligned memory chunks:
// 9D1DCC, 9D301C : Clouds rotation patch

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
CStaticShadow* aStaticShadows_NEW;
CPolyBunch *aPolyBunches;

float *ms_fTimeStep, *fHeliRotorSpeed, *ms_fAspectRatio, *ms_fTimeScale;
char *mod_HandlingManager;
int *fpsLimit; // a part of RsGlobal
void *GTouchscreen;
bool *m_PrefsFrameLimiter;
uint32_t *m_snTimeInMilliseconds, *m_FrameCounter;
float *m_fCurrentFarClip, *m_fCurrentFogStart;
bool *m_UserPause, *m_CodePause;

// CPhysical::ApplyCollision
float *fl1679D4;
// CClouds::Update
float *fl1D4CF0, *fl1D4CF4;

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////     Funcs     ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////
int ret0(int a, ...) { return 0; } // Generic
int ret1(int a, ...) { return 1; } // Generic
void crash_this() { __builtin_trap(); } // Used for redirected funcs, to get the function it's called from (virtual funcs are hard!)
inline void* TryLoadAlphaFunc(const char* szlib)
{
    void* lib = dlopen(szlib, RTLD_LAZY);
    if(!lib) return NULL;

    void* sym = dlsym(lib, "glAlphaFuncQCOM");
    if(!sym) sym = dlsym(lib, "glAlphaFunc");
    return sym;
}

void (*BrightLightsRender)();
void (*RsEventHandler)(int, void*);
void (*TouchscreenUpdate)(void*, void*, bool);
float (*GetCurrentTimeInCycles)();
uint32_t (*GetCyclesPerMillisecond)();
void (*OS_ThreadSleep)(int);
void (*tbDisplay)();
void (*emu_DistanceFogSetEnabled)(bool);
void (*emu_DistanceFogSetup)(float start, float end, float r, float g, float b);
double (*OS_TimeAccurate)();

// Global Hooks (no need to enable patches)
double m_dblTimeInMicroseconds = 0.0f;
double m_dblOldTimeInMicroseconds = 0.0f;
uint32_t m_snOwnTimeInMilliseconds = 0;

uint32_t m_LogicalFramesPassed = 0, m_LogicalFrameCounter = 0;
DECL_HOOKv(Global_TimerUpdate)
{
    Global_TimerUpdate();

    m_dblOldTimeInMicroseconds = m_dblTimeInMicroseconds;
    m_dblTimeInMicroseconds = OS_TimeAccurate();

    // re3:
    static double frameTimeLogical = 0.0, frameTimeFractionScaled = 0.0;

    m_LogicalFramesPassed = 0;
    frameTimeLogical += m_dblTimeInMicroseconds;
    while (frameTimeLogical >= 1.0 / 30.0)
    {
        frameTimeLogical -= 1.0 / 30.0;
        m_LogicalFramesPassed++;
    }
    m_LogicalFrameCounter += m_LogicalFramesPassed;

    if(!*m_UserPause && !*m_CodePause)
    {
        frameTimeFractionScaled += m_dblTimeInMicroseconds * *ms_fTimeScale;
        m_snOwnTimeInMilliseconds += (uint32_t)frameTimeFractionScaled;
        frameTimeFractionScaled -= (uint32_t)frameTimeFractionScaled;
    }
}

#ifdef AML32
    #include "patches_vc.inl"
#else
    #include "patches_vc64.inl"
#endif

void JPatch()
{
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
    SET_TO(OS_TimeAccurate, aml->GetSym(hGTAVC, "_Z15OS_TimeAccuratev"));
    // Functions End   //
    
    // Variables Start //
    SET_TO(ms_fTimeStep, aml->GetSym(hGTAVC, "_ZN6CTimer12ms_fTimeStepE"));
    SET_TO(mod_HandlingManager, *(char**)(pGTAVC + BYBIT(0x3956B4, 0x578DE8)));
    SET_TO(fHeliRotorSpeed, pGTAVC + BYBIT(0x255000, 0x4CAEE0)); UNPROT(fHeliRotorSpeed, sizeof(float));
    SET_TO(ms_fAspectRatio, aml->GetSym(hGTAVC, "_ZN5CDraw15ms_fAspectRatioE"));
    SET_TO(ms_fTimeScale, aml->GetSym(hGTAVC, "_ZN6CTimer13ms_fTimeScaleE"));
    SET_TO(fpsLimit, pGTAVC + BYBIT(0x714F1C, 0x991C38));
    SET_TO(GTouchscreen, aml->GetSym(hGTAVC, "GTouchscreen"));
    SET_TO(m_PrefsFrameLimiter, aml->GetSym(hGTAVC, "_ZN12CMenuManager19m_PrefsFrameLimiterE"));
    SET_TO(m_snTimeInMilliseconds, aml->GetSym(hGTAVC, "_ZN6CTimer22m_snTimeInMillisecondsE"));
    SET_TO(m_FrameCounter, aml->GetSym(hGTAVC, "_ZN6CTimer14m_FrameCounterE"));
    SET_TO(m_fCurrentFarClip, aml->GetSym(hGTAVC, "_ZN10CTimeCycle17m_fCurrentFarClipE"));
    SET_TO(m_fCurrentFogStart, aml->GetSym(hGTAVC, "_ZN10CTimeCycle18m_fCurrentFogStartE"));
    SET_TO(aPolyBunches, aml->GetSym(hGTAVC, "_ZN8CShadows12aPolyBunchesE"));
    SET_TO(m_UserPause, aml->GetSym(hGTAVC, "_ZN6CTimer11m_UserPauseE"));
    SET_TO(m_CodePause, aml->GetSym(hGTAVC, "_ZN6CTimer11m_CodePauseE"));
    SET_TO(fl1679D4, pGTAVC + BYBIT(0x1679D4, 0x0)); UNPROT(fl1679D4, sizeof(float));
    SET_TO(fl1D4CF0, pGTAVC + BYBIT(0x1D4CF0, 0x9D1DCC)); UNPROT(fl1D4CF0, sizeof(float));
    SET_TO(fl1D4CF4, pGTAVC + BYBIT(0x1D4CF4, 0x9D301C)); UNPROT(fl1D4CF4, sizeof(float));
    // Variables End   //

    // We need it for future fixes.
    HOOK(Global_TimerUpdate, aml->GetSym(hGTAVC, "_ZN6CTimer6UpdateEv"));

  #ifdef AML32
    #include "preparations_vc.inl"
  #else
    #include "preparations_vc64.inl"
  #endif
}

}; // namespace GTA_VC