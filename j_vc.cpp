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
float *ms_fTimeStep, *fHeliRotorSpeed, *ms_fAspectRatio;
char *mod_HandlingManager;

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////     Funcs     ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void (*BrightLightsRender)();


#ifdef AML32
    #include "patches_vc.inl"
#else
    //#include "patches_vc64.inl" // No VC 1.12
#endif

void JPatch()
{
    // Functions Start //
    SET_TO(BrightLightsRender, aml->GetSym(hGTAVC, "_ZN13CBrightLights6RenderEv"));
    // Functions End   //
    
    // Variables Start //
    SET_TO(ms_fTimeStep, aml->GetSym(hGTAVC, "_ZN6CTimer12ms_fTimeStepE"));
    SET_TO(mod_HandlingManager, *(char**)(pGTAVC + 0x3956B4));
    SET_TO(fHeliRotorSpeed, pGTAVC + 0x255000); aml->Unprot(AS_ADDR(fHeliRotorSpeed), sizeof(float));
    SET_TO(ms_fAspectRatio, aml->GetSym(hGTAVC, "_ZN5CDraw15ms_fAspectRatioE"));
    // Variables End   //

    #ifdef AML32
        #include "preparations_vc.inl"
    #else
        //#include "preparations_vc64.inl" // No VC 1.12
    #endif
}

}; // namespace GTA_VC