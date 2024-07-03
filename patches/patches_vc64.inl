// Boat and Skimmer on high FPS
uintptr_t Boat_ApplyWaterResistance_BackTo;
extern "C" float Boat_ApplyWaterResistance_Patch(float resistance)
{
    return resistance * GetTimeStepMagic();
}
__attribute__((optnone)) __attribute__((naked)) void Boat_ApplyWaterResistance_Inject(void)
{
    asm volatile(
        "STR X8, [SP, #-16]!\n"
        "FMOV S11, #1.0\n"
        "FMADD S1, S2, S2, S1\n"
        "FCSEL S0, S3, S0, EQ\n"
        "FMOV W0, S0\n"
        "STR S1, [SP, #-16]!\n"
        "BL Boat_ApplyWaterResistance_Patch\n"
        "LDR S1, [SP], #16\n"
        "FMADD S0, S0, S1, S11\n");
    asm volatile(
        "MOV X0, %0"
    :: "r" (Boat_ApplyWaterResistance_BackTo));
    asm volatile(
        "LDR X8, [SP], #16\n"
        "BR X0");
}

// Fix wheels rotation speed on high FPS
DECL_HOOK(float, ProcessWheelRotation, void* self, int state, const CVector &fwd, const CVector &speed, float radius)
{
    return ProcessWheelRotation(self, state, fwd, speed, radius) / fMagic;
}

// Car Slowdown Fix
DECL_HOOKv(ProcessWheel_SlowDownFix, void *self, CVector &a1, CVector &a2, CVector &a3, CVector &a4, int a5, float a6, float a7, float a8, char a9, float *a10, void *a11, uint16_t a12)
{
    float& fWheelFriction = *(float*)(mod_HandlingManager + 4);
    float save = fWheelFriction; fWheelFriction *= 2.0f * GetTimeStepMagic();
    ProcessWheel_SlowDownFix(self, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12);
    fWheelFriction = save;
}
DECL_HOOKv(ProcessBikeWheel_SlowDownFix, void *self, CVector &a1, CVector &a2, CVector &a3, CVector &a4, int a5, float a6, float a7, float a8, float a9, char a10, float *a11, void *a12, int a13, uint16_t a14)
{
    float& fWheelFriction = *(float*)(mod_HandlingManager + 4);
    float save = fWheelFriction; fWheelFriction *= 2.0f * GetTimeStepMagic();
    ProcessBikeWheel_SlowDownFix(self, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14);
    fWheelFriction = save;
}

// Fix heli rotor rotation speed on high FPS
DECL_HOOKv(HeliRender_MatrixUpdate, void *self)
{
    HeliRender_MatrixUpdate(self);
    *fHeliRotorSpeed = 0.48307693f * GetTimeStepMagic();
}

// Streaming distance fix
float fStreamingDistFix;
uintptr_t CameraProcess_StreamDist_BackTo;
DECL_HOOKv(SetFOV_StreamingDistFix, float factor)
{
    // Someone is using broken mods
    // So here is the workaround + a little value clamping
    if(factor < 5.0f)
    {
        fStreamingDistFix = 70.0f / 5.0f;
    }
    else if(factor > 170.0f)
    {
        fStreamingDistFix = 70.0f / 170.0f;
    }
    else
    {
        fStreamingDistFix = 70.0f / factor;
    }

    fStreamingDistFix *= (*ms_fAspectRatio / ar43);

    SetFOV_StreamingDistFix(factor);
}
extern "C" void CameraProcess_StreamDist_Patch(uintptr_t camera)
{
    *(float*)(camera + 0xF4) = fStreamingDistFix;
}
__attribute__((optnone)) __attribute__((naked)) void CameraProcess_StreamDist_Inject(void)
{
    asm volatile(
        "STR S0, [X20, #0xF4]!\n" // im not sure what "!" does in ARMv8 assembly...
        "MOV X0, X20\n"
        "BL CameraProcess_StreamDist_Patch\n");
    asm volatile(
        "MOV X0, %0"
    :: "r" (CameraProcess_StreamDist_BackTo));
    asm volatile(
        "BR X0\n");
}

// fat-ass coronas!
DECL_HOOKb(CoronaSprite_CalcScreenCoors, CVector *worldPos, CVector *screenPos, float *scalex, float *scaley, bool a)
{
    bool ret = CoronaSprite_CalcScreenCoors(worldPos, screenPos, scalex, scaley, a);
    *scalex /= *ms_fAspectRatio;
    return ret;
}

// Vertex weight fix
/*uintptr_t VertexWeightFix_BackTo1, VertexWeightFix_BackTo2;
extern "C" uintptr_t VertexWeightFix_Patch(RpSkin *skin)
{
    return (skin->vertexMaps.maxWeights == 4) ? VertexWeightFix_BackTo2 : VertexWeightFix_BackTo1;
}
__attribute__((optnone)) __attribute__((naked)) void VertexWeightFix_Inject(void)
{
    asm volatile(
        "LDR R3, [R0, #0x18]\n"
        "PUSH {R1-R5}\n"
        "BL VertexWeightFix_Patch\n"
        "POP {R1-R5}\n"
        "MOV PC, R0");
}*/

// Fix traffic lights
DECL_HOOKv(TrFix_RenderEffects)
{
    BrightLightsRender();
    TrFix_RenderEffects();
}

// Framelimiter
static double frameMcs = (1.0 / 30.0);
static double nextMcsTime = 0.0;
DECL_HOOKv(GameTick_TouchscreenUpdate, void *self, float a1, bool a2)
{
    if(*m_PrefsFrameLimiter)
    {
        // Touch handling is completely fucked up.
        // Pausing thread is the only way...
        double accurateTime = OS_TimeAccurate();
        if(nextMcsTime < accurateTime - frameMcs) nextMcsTime = accurateTime + frameMcs;
        else nextMcsTime += frameMcs;

        OS_ThreadSleep(1000 * (nextMcsTime - accurateTime));
    }

    RsEventHandler(0x1A, (void*)true);
    GameTick_TouchscreenUpdate(self, a1, a2);
}
DECL_HOOKv(OS_ScreenSetRefresh, int newLimit)
{
    if(newLimit < 10) newLimit = 10;
    else if(newLimit > 240) newLimit = 240;
    
    *fpsLimit = newLimit;
    frameMcs = 1.0 / (double)newLimit;
}

// Water UV Scroll
uintptr_t RenderWater_BackTo;
extern "C" float RenderWater_Patch(float windScale)
{
    return windScale * GetTimeStepMagic();
}
__attribute__((optnone)) __attribute__((naked)) void RenderWater_Inject(void)
{
    asm volatile(
        "AND W8, W8, #0xFFF\n"
        "UCVTF S3, W8\n"
        "FMOV W0, S10\n"

        "STR S0, [SP, #-16]!\n"
        "STR S1, [SP, #-16]!\n"
        "BL RenderWater_Patch\n"
        "FMOV S10, S0\n"
        "LDR S1, [SP], #16\n"
        "LDR S0, [SP], #16\n"

        "FMADD S12, S10, S0, S1\n"
        "LDP S0, S1, [X19, #0x4]");

    asm volatile(
        "MOV X8, %0"
    :: "r" (RenderWater_BackTo));
    asm volatile(
        "BR X8\n");
}

// Render States
static int fogState = 0;
CRGBA fogColor;
DECL_HOOKb(RwRenderState_Patch, RwRenderState state, int val)
{
    switch(state)
    {
        default:
            return RwRenderState_Patch(state, val);

        case rwRENDERSTATEFOGENABLE:
            if(fogState != val)
            {
                fogState = val;
                emu_DistanceFogSetEnabled(fogState != 0);
                if(val)
                {
                    emu_DistanceFogSetup(*m_fCurrentFogStart, 0.7f * *m_fCurrentFarClip, (float)fogColor.b / 255.0f, (float)fogColor.g / 255.0f, (float)fogColor.r / 255.0f);
                }
            }
            return true;

        case rwRENDERSTATEFOGCOLOR:
            fogColor.val = val;
            return true;

        // Shader does not support any other type of the fog...
        // Always rwFOGTYPELINEAR, no exponential (is this even used tho?)
        case rwRENDERSTATEFOGTYPE:
            return true;
    }
}

// Speedo cloudz
DECL_HOOKv(CloudsUpdate_Speedo)
{
    *fl1D4CF0 = 0.001f * GetTimeStepMagic();
    *fl1D4CF4 = 0.3f * GetTimeStepMagic();
    CloudsUpdate_Speedo();
}

// The Shadow. Of explosion. It's missing. Yup.
DECL_HOOKv(AddExplosion_AddShadow, uint8_t ShadowType, RwTexture *pTexture, CVector *pPosn,
                                   float fFrontX, float fFrontY, float fSideX, float fSideY,
                                   int16_t nIntensity, uint8_t nRed, uint8_t nGreen, uint8_t nBlue,
                                   float fZDistance, uint32_t nTime, float fScale)
{
    AddExplosion_AddShadow(ShadowType, pTexture, pPosn, 8.0f, 0.0f, 0.0f, -8.0f, 200, 0, 0, 0, 10.0f, 30000, 1.0f);
}

// Light shadow tweaked distance
DECL_HOOKv(StoreShadowForVehicle, uint32_t nId, uint8_t ShadowType, void *pTexture, CVector *pPosn, float fFrontX, float fFrontY, float fSideX, float fSideY, int16_t nIntensity, uint8_t nRed, uint8_t nGreen, uint8_t nBlue, float fZDistance, float fScale, float fDrawDistance, bool bTempShadow, float fUpDistance)
{
    StoreShadowForVehicle(nId, ShadowType, pTexture, pPosn, fFrontX, fFrontY, fSideX, fSideY, nIntensity, nRed, nGreen, nBlue, 15.0f, fScale, 120.0f, bTempShadow, 0.0f);
}

// Static shadows
/*DECL_HOOKv(InitShadows)
{
    static CPolyBunch bunchezTail[BUNCHTAILS_EX];
    
    InitShadows();
    for(int i = 0; i < BUNCHTAILS_EX-1; ++i)
    {
        bunchezTail[i].m_pNext = &bunchezTail[i+1];
    }
    bunchezTail[BUNCHTAILS_EX-1].m_pNext = NULL;
    aPolyBunches[380-1].m_pNext = &bunchezTail[0]; // GTA:VC has 380 instead of 360 in SA?! LOL, DOWNGRADE
}

uintptr_t DoCollectableEffects_BackTo, DoPickUpEffects_BackTo;
__attribute__((optnone)) __attribute__((naked)) void DoCollectableEffects_Inject(void)
{
    asm volatile(
        "LDM.W R3, {R0-R2}\n"
        "MOVS R10, #0x0\n"
        "MOVT R10, #0x4220\n"
        "VMOV.F32 S17, R10\n"
        "SUB SP, SP, #0x54\n"
        "PUSH {R0}\n"
    );
    asm volatile(
        "MOV R10, %0"
    :: "r" (DoCollectableEffects_BackTo));
    asm volatile(
        "POP {R0}\n"
        "MOV PC, R10\n"
    );
}
__attribute__((optnone)) __attribute__((naked)) void DoPickUpEffects_Inject(void)
{
    asm volatile(
        "VLDR S13, [R4, #0x38]\n"
        "STM.W R7, {R0-R2}\n"
        "MOVS R11, #0x0\n"
        "MOVT R11, #0x41F0\n"
        "VMOV.F32 S17, R11\n"
        "PUSH {R0}\n"
    );
    asm volatile(
        "MOV R11, %0"
    :: "r" (DoPickUpEffects_BackTo));
    asm volatile(
        "POP {R0}\n"
        "MOV PC, R11\n"
    );
}*/

// Bigger spawn distance for peds
DECL_HOOK(float, PedCreationDistMult_Offscreen)
{
    return 2.6f * PedCreationDistMult_Offscreen();
}

// FX particles distance multiplier!
float fxMultiplier;
DECL_HOOK(float, LoadFX_atof, char* str)
{
    return fxMultiplier * LoadFX_atof(str);
}