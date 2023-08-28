#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>
#include <dlfcn.h>
#include <vector>
#include <cctype>

#define GL_GLEXT_PROTOTYPES
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES2/gl2platform.h>

#ifdef AML32
    #include "AArch64_ModHelper/Thumbv7_ASMHelper.h"
    #include "GTASA_STRUCTS.h"
    #define BYVER(__for32, __for64) (__for32)
    using namespace ThumbV7;
#else
    #include "AArch64_ModHelper/ARMv8_ASMHelper.h"
    #include "GTASA_STRUCTS_210.h"
    #define BYVER(__for32, __for64) (__for64)
    using namespace ARMv8;
#endif

MYMODCFG(net.rusjj.jpatch, JPatch, 1.5, RusJJ)
BEGIN_DEPLIST()
    ADD_DEPENDENCY_VER(net.rusjj.aml, 1.0.2.1)
END_DEPLIST()

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
uintptr_t pGTASA, pSC;
void *hGTASA, *hSC;
static constexpr float ar43 = 4.0f / 3.0f;
static constexpr float fMagic = 50.0f / 30.0f;
static constexpr int nMaxScriptSprites = 384; // Changing it wont make it bigger.

bool bIsRWReady = false;
float fEmergencyVehiclesFix;
CSprite2d** pNewScriptSprites = new CSprite2d*[nMaxScriptSprites] {NULL}; // 384*4=1536 0x600
void* pNewIntroRectangles = new void*[15*nMaxScriptSprites] {NULL}; // 384*60=23040 0x5A00
CRegisteredShadow* asShadowsStored_NEW;
CStaticShadow* aStaticShadows_NEW;

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////     Vars      ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#define GetTimeStep() (*ms_fTimeStep)
#define GetTimeStepInSeconds() (*ms_fTimeStep / 50.0f)

CPlayerInfo* WorldPlayers;
CIntVector2D* windowSize;
CCamera* TheCamera;
RsGlobalType* RsGlobal;
MobileMenu *gMobileMenu;
CWidget** m_pWidgets;
ScriptVariables* ScriptParams;
CLinkList<AlphaObjectInfo>* m_alphaList;
CPool<CCutsceneObject>** pObjectPool;
CZoneInfo** m_pCurrZoneInfo;
CWeaponInfo* aWeaponInfo;
int keys[538];
bool *ms_bIsPlayerOnAMission;
int *DETAILEDWATERDIST;
int *ms_nNumGang;
CPolyBunch* aPolyBunches;
CBaseModelInfo** ms_modelInfoPtrs;
CRGBA* ms_vehicleColourTable;

CTaskComplexSequence* ms_taskSequence;
CRunningScript** pActiveScripts;
float *ms_fTimeStep, *ms_fFOV, *game_FPS, *CloudsRotation, *WeatherWind, *fSpriteBrightness, *m_f3rdPersonCHairMultX, *m_f3rdPersonCHairMultY, *ms_fAspectRatio;
void *g_surfaceInfos;
unsigned int *m_snTimeInMilliseconds;
int *lastDevice, *NumberOfSearchLights, *ms_numAnimBlocks, *RasterExtOffset, *detailTexturesStorage, *textureDetail, *ms_iActiveSequence;
bool *bDidWeProcessAnyCinemaCam, *bRunningCutscene, *bProcessingCutscene;
uint32_t *CloudsIndividualRotation, *m_ZoneFadeTimer, *ms_memoryUsed, *ms_memoryAvailable;
static uint32_t CCheat__m_aCheatHashKeys[] = { 
   0xDE4B237D, 0xB22A28D1, 0x5A783FAE, 
   // WEAPON4, TIMETRAVEL, SCRIPTBYPASS, SHOWMAPPINGS 
   0x5A1B5E9A, 0x00000000, 0x00000000, 0x00000000, 
   // INVINCIBILITY, SHOWTAPTOTARGET, SHOWTARGETING 
   0x7B64E263, 0x00000000, 0x00000000, 
   0xEECCEA2B, 
   0x42AF1E28, 0x555FC201, 0x2A845345, 0xE1EF01EA, 
   0x771B83FC, 0x5BF12848, 0x44453A17, 0x00000000, 
   0xB69E8532, 0x8B828076, 0xDD6ED9E9, 0xA290FD8C, 
   0x00000000, 0x43DB914E, 0xDBC0DD65, 0x00000000, 
   0xD08A30FE, 0x37BF1B4E, 0xB5D40866, 0xE63B0D99, 
   0x675B8945, 0x4987D5EE, 0x2E8F84E8, 0x00000000, 
   0x00000000, 0x0D5C6A4E, 0x00000000, 0x00000000, 
   0x66516EBC, 0x4B137E45, 0x00000000, 0x00000000, 
   0x3A577325, 0xD4966D59, 
   // THEGAMBLER 
   0x00000000, 
   0x5FD1B49D, 0xA7613F99, 
   0x1792D871, 0xCBC579DF, 0x4FEDCCFF, 0x44B34866, 
   0x2EF877DB, 0x2781E797, 0x2BC1A045, 0xB2AFE368, 
   0x00000000, 0x00000000, 0x1A5526BC, 0xA48A770B, 
   0x00000000, 0x00000000, 0x00000000, 0x7F80B950, 
   0x6C0FA650, 0xF46F2FA4, 0x70164385, 0x00000000, 
   0x885D0B50, 0x151BDCB3, 0xADFA640A, 0xE57F96CE, 
   0x040CF761, 0xE1B33EB9, 0xFEDA77F7, 0x00000000, 
   0x00000000, 0xF53EF5A5, 0xF2AA0C1D, 0xF36345A8, 
   0x00000000, 0xB7013B1B, 0x00000000, 0x31F0C3CC, 
   0x00000000, 0x00000000, 0x00000000, 0x00000000, 
   0x00000000, 0xF01286E9, 0xA841CC0A, 0x31EA09CF, 
   0xE958788A, 0x02C83A7C, 0xE49C3ED4, 0x171BA8CC, 
   0x86988DAE, 0x2BDD2FA1, 0x00000000, 0x00000000, 
   0x00000000, 0x00000000, 0x00000000, 0x00000000, 
   0x00000000, 0x00000000, 0x00000000, 0x00000000, 
   0x00000000, 0x00000000, 0x00000000, 
}; // VitaPort

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////     Funcs     ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////
int ret0(int a, ...) { return 0; } // Generic
int ret1(int a, ...) { return 1; } // Generic

float fAspectCorrection = 0.0f, fAspectCorrectionDiv = 0.0f;
CRGBA OwnHudColour[HUD_COLOUR_COUNT];
DECL_HOOK(bool, InitRenderWare)
{
    if(!InitRenderWare()) return false;
    
    bIsRWReady = true;
    
    OwnHudColour[HUD_COLOUR_RED] = CRGBA(180, 25, 29);
    OwnHudColour[HUD_COLOUR_GREEN] = CRGBA(54, 104, 44);
    OwnHudColour[HUD_COLOUR_DARK_BLUE] = CRGBA(50, 60, 127);
    OwnHudColour[HUD_COLOUR_LIGHT_BLUE] = CRGBA(172, 203, 241);
    OwnHudColour[HUD_COLOUR_LIGHT_GRAY] = CRGBA(225, 225, 225);
    OwnHudColour[HUD_COLOUR_WHITE] = CRGBA(255, 255, 255);
    OwnHudColour[HUD_COLOUR_BLACK] = CRGBA(0, 0, 0);
    OwnHudColour[HUD_COLOUR_GOLD] = CRGBA(144, 98, 16);
    OwnHudColour[HUD_COLOUR_PURPLE] = CRGBA(180, 25, 29);
    OwnHudColour[HUD_COLOUR_DARK_GRAY] = CRGBA(180, 25, 29);
    OwnHudColour[HUD_COLOUR_DARK_RED] = CRGBA(180, 25, 29);
    OwnHudColour[HUD_COLOUR_DARK_GREEN] = CRGBA(180, 25, 29);
    OwnHudColour[HUD_COLOUR_CREAM] = CRGBA(180, 25, 29);
    OwnHudColour[HUD_COLOUR_NIGHT_BLUE] = CRGBA(180, 25, 29);
    OwnHudColour[HUD_COLOUR_BLUE] = CRGBA(180, 25, 29);
    OwnHudColour[HUD_COLOUR_YELLOW] = CRGBA(180, 25, 29);

    return true;
}
#define fAspectCorrection (*ms_fAspectRatio - ar43)
#define fAspectCorrectionDiv (*ms_fAspectRatio / ar43)
/*DECL_HOOKv(CalculateAspectRatio)
{
    CalculateAspectRatio();
    fAspectCorrection = (*ms_fAspectRatio - ar43);
    fAspectCorrectionDiv = *ms_fAspectRatio / ar43;
}*/
void (*BrightLightsInit)();
void (*BrightLightsRender)();
void (*emu_glEnable)(GLenum);
void (*emu_glDisable)(GLenum);
void (*emu_glAlphaFunc)(GLenum, GLclampf);
bool (*IsOnAMission)();
void (*AddToCheatString)(char);
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
CPlayerPed* (*FindPlayerPed)(int playerId);
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
CAnimBlendAssociation* (*RpAnimBlendClumpGetAssociation)(RpClump*, const char*);
CAnimBlendAssociation* (*RpAnimBlendClumpGetAssociationU)(RpClump*, uint32_t);
CObject* (*CreateObject)(int mdlIdx, bool create);
C2dEffect* (*Get2dEffect)(CBaseModelInfo*, int);
void (*RwFrameForAllObjects)(RwFrame*, RwObject* (*)(RwObject*, void*), void*);
RwObject* (*GetCurrentAtomicObjectCB)(RwObject*, void*);
void (*RpGeometryForAllMaterials)(RpGeometry*, RpMaterial* (*)(RpMaterial*, void*), void*);
void (*SetComponentAtomicAlpha)(RpAtomic*, int);
void (*ApplyMoveForce)(CPhysical*,float,float,float);
bool (*GetWaterLevel)(CVector, float&, bool, CVector*);
CTask* (*GetTaskSwim)(CPedIntelligence*);
CTask* (*TaskConstructor)();
void (*TaskStartNamedAnim)(CTask*, const char* animName, const char* animGroupName, uint32_t animFlags, float blendDelta, int32_t endTime, bool bDontInterrupt, bool bRunInSequence, bool bOffsetPed, bool bHoldLastFrame);
void (*SetTask)(CTaskManager*, CTask*, int, bool);
bool (*TaskComplexSequenceAddTask)(CTaskComplexSequence*, CTask*);
CAnimBlendAssociation* (*BlendAnimation)(RpClump* clump, AssocGroupId groupId, AnimationId animId, float clumpAssocBlendData);
CAnimBlendAssociation* (*RpAnimBlendGetNextAssociation)(CAnimBlendAssociation *pAssociation);


inline void TransformFromObjectSpace(CEntity* self, CVector& outPos, const CVector& offset)
{
    if(self->m_matrix)
    {
        outPos = *self->m_matrix * offset;
        return;
    }
    TransformPoint((RwV3d&)outPos, self->m_placement, (RwV3d&)offset);
}
inline CVector TransformFromObjectSpace(CEntity* ent, const CVector& offset)
{
    auto result = CVector();
    if (ent->m_matrix) {
        result = *ent->m_matrix * offset;
        return result;
    }

    TransformPoint((RwV3d&)result, ent->m_placement, (RwV3d&)offset);
    return result;
}
inline void BumpStreamingMemory(int megabytes)
{
    *ms_memoryAvailable += megabytes * 1024 * 1024;
}

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////     Hooks     ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////

#include "patches.inl"

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////     Funcs     ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////
extern "C" const char* OnUpdaterURLRequested()
{
    return "https://raw.githubusercontent.com/AndroidModLoader/JPatch/main/updater.txt";
}
extern "C" void OnModLoad()
{
    logger->SetTag("JPatch");
    
    //aml->PatchForThumb(true); // Auto but may be enabled to be sure
    
    cfg->Bind("Author", "", "About")->SetString("[-=KILL MAN=-]"); cfg->ClearLast();
    cfg->Bind("IdeasFrom", "", "About")->SetString("MTA:SA Team, re3 contributors, JuniorDjjr, ThirteenAG, Blackbird88, 0x416c69, Whitetigerswt, XMDS, Peepo"); cfg->ClearLast();
    cfg->Bind("Discord", "", "About")->SetString("https://discord.gg/2MY7W39kBg"); cfg->ClearLast();
    cfg->Bind("GitHub", "", "About")->SetString("https://github.com/AndroidModLoader/JPatch"); cfg->ClearLast();
    cfg->Save();

    pGTASA = aml->GetLib("libGTASA.so");
    hGTASA = aml->GetLibHandle("libGTASA.so");
    
    pSC = aml->GetLib("libSCAnd.so");
    hSC = aml->GetLibHandle("libSCAnd.so");

    // Functions Start //
    SET_TO(AddToCheatString,        aml->GetSym(hGTASA, "_ZN6CCheat16AddToCheatStringEc"));
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
    SET_TO(FindPlayerPed,           aml->GetSym(hGTASA, "_Z13FindPlayerPedi"));
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
    SET_TO(IsOnAMission,            aml->GetSym(hGTASA, "_ZN11CTheScripts18IsPlayerOnAMissionEv"));
    SET_TO(emu_glEnable,            aml->GetSym(hGTASA, "_Z12emu_glEnablej"));
    SET_TO(emu_glDisable,           aml->GetSym(hGTASA, "_Z13emu_glDisablej"));
    SET_TO(BrightLightsInit,        aml->GetSym(hGTASA, "_ZN13CBrightLights4InitEv"));
    SET_TO(BrightLightsRender,      aml->GetSym(hGTASA, "_ZN13CBrightLights6RenderEv"));
    SET_TO(CreateObject,            aml->GetSym(hGTASA, "_ZN7CObject6CreateEib"));
    SET_TO(Get2dEffect,             aml->GetSym(hGTASA, "_ZN14CBaseModelInfo11Get2dEffectEi"));
    SET_TO(RpAnimBlendClumpGetAssociation,aml->GetSym(hGTASA, "_Z30RpAnimBlendClumpGetAssociationP7RpClumpPKc"));
    SET_TO(RpAnimBlendClumpGetAssociationU,aml->GetSym(hGTASA, "_Z30RpAnimBlendClumpGetAssociationP7RpClumpj"));
    SET_TO(RwFrameForAllObjects,    aml->GetSym(hGTASA, "_Z20RwFrameForAllObjectsP7RwFramePFP8RwObjectS2_PvES3_"));
    SET_TO(GetCurrentAtomicObjectCB,aml->GetSym(hGTASA, "_Z24GetCurrentAtomicObjectCBP8RwObjectPv"));
    SET_TO(RpGeometryForAllMaterials,aml->GetSym(hGTASA, "_Z25RpGeometryForAllMaterialsP10RpGeometryPFP10RpMaterialS2_PvES3_"));
    SET_TO(SetComponentAtomicAlpha, aml->GetSym(hGTASA, "_ZN8CVehicle23SetComponentAtomicAlphaEP8RpAtomici"));
    SET_TO(ApplyMoveForce,          aml->GetSym(hGTASA, "_ZN9CPhysical14ApplyMoveForceE7CVector"));
    SET_TO(GetWaterLevel,           aml->GetSym(hGTASA, "_ZN11CWaterLevel13GetWaterLevelEfffPfbP7CVector"));
    SET_TO(GetTaskSwim,             aml->GetSym(hGTASA, "_ZNK16CPedIntelligence11GetTaskSwimEv"));
    SET_TO(TaskConstructor,         aml->GetSym(hGTASA, "_ZN5CTasknwEj"));
    SET_TO(TaskStartNamedAnim,      aml->GetSym(hGTASA, "_ZN23CTaskSimpleRunNamedAnimC2EPKcS1_ifibbbb"));
    SET_TO(SetTask,                 aml->GetSym(hGTASA, "_ZN12CTaskManager7SetTaskEP5CTaskib"));
    SET_TO(TaskComplexSequenceAddTask, aml->GetSym(hGTASA, "_ZN20CTaskComplexSequence7AddTaskEP5CTask"));
    SET_TO(BlendAnimation,          aml->GetSym(hGTASA, "_ZN12CAnimManager14BlendAnimationEP7RpClump12AssocGroupId11AnimationIdf"));
    SET_TO(RpAnimBlendGetNextAssociation, aml->GetSym(hGTASA, "_Z29RpAnimBlendGetNextAssociationP21CAnimBlendAssociation"));
    HOOKPLT(InitRenderWare,         pGTASA + 0x66F2D0);
    //HOOK(CalculateAspectRatio,      aml->GetSym(hGTASA, "_ZN5CDraw20CalculateAspectRatioEv"));
    // Functions End   //
    
    // Variables Start //
    SET_TO(ms_taskSequence,         aml->GetSym(hGTASA, "_ZN14CTaskSequences15ms_taskSequenceE"));
    SET_TO(pActiveScripts,          aml->GetSym(hGTASA, "_ZN11CTheScripts14pActiveScriptsE"));
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
    SET_TO(bProcessingCutscene,     aml->GetSym(hGTASA, "_ZN12CCutsceneMgr21ms_cutsceneProcessingE"));
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
    SET_TO(ms_iActiveSequence,      aml->GetSym(hGTASA, "_ZN14CTaskSequences18ms_iActiveSequenceE"));
    SET_TO(RasterExtOffset,         aml->GetSym(hGTASA, "RasterExtOffset"));
    SET_TO(m_f3rdPersonCHairMultX,  aml->GetSym(hGTASA, "_ZN7CCamera22m_f3rdPersonCHairMultXE"));
    SET_TO(m_f3rdPersonCHairMultY,  aml->GetSym(hGTASA, "_ZN7CCamera22m_f3rdPersonCHairMultYE"));
    SET_TO(ms_fAspectRatio,         aml->GetSym(hGTASA, "_ZN5CDraw15ms_fAspectRatioE"));
    SET_TO(aWeaponInfo,             aml->GetSym(hGTASA, "aWeaponInfo"));
    SET_TO(windowSize,              aml->GetSym(hGTASA, "windowSize"));
    SET_TO(ms_bIsPlayerOnAMission,  aml->GetSym(hGTASA, "_ZN10CPedGroups22ms_bIsPlayerOnAMissionE"));
    SET_TO(DETAILEDWATERDIST,       aml->GetSym(hGTASA, "DETAILEDWATERDIST"));
    SET_TO(ms_nNumGang,             aml->GetSym(hGTASA, "_ZN11CPopulation11ms_nNumGangE"));
    SET_TO(aPolyBunches,            aml->GetSym(hGTASA, "_ZN8CShadows12aPolyBunchesE"));
    SET_TO(ms_modelInfoPtrs,        aml->GetSym(hGTASA, "_ZN10CModelInfo16ms_modelInfoPtrsE"));
    SET_TO(ms_vehicleColourTable,   aml->GetSym(hGTASA, "_ZN17CVehicleModelInfo21ms_vehicleColourTableE"));
    // Variables End //
    
    // Animated textures
    if(cfg->GetBool("EnableAnimatedTextures", true, "Visual"))
    {
        aml->Write(aml->GetSym(hGTASA, "RunUVAnim"), (uintptr_t)"\x01", 1);
    }

    // Vertex weight
    if(cfg->GetBool("FixVertexWeight", true, "Visual"))
    {
        aml->Write(pGTASA + 0x1C8064, (uintptr_t)"\x01", 1);
        aml->Write(pGTASA + 0x1C8082, (uintptr_t)"\x01", 1);
    }

    // Fix moon!
    // War Drum moment: cannot get Alpha testing to work
    /*if(cfg->GetBool("MoonPhases", true, "Visual"))
    {
        //aml->Write(pGTASA + 0x1AF5C2, (uintptr_t)"\x4F\xF0\x00\x03", 4);
        MoonVisual_1_BackTo = pGTASA + 0x59ED90 + 0x1;
        MoonVisual_2_BackTo = pGTASA + 0x59EE4E + 0x1;
        aml->Redirect(pGTASA + 0x59ED80 + 0x1, (uintptr_t)MoonVisual_1_Inject);
        aml->Redirect(pGTASA + 0x59EE36 + 0x1, (uintptr_t)MoonVisual_2_Inject);
    };*/

    // Fix sky multitude
    if(cfg->GetBool("FixSkyMultitude", true, "Visual"))
    {
        aml->Unprot(pGTASA + 0x59FB8C, 2*sizeof(float));
        *(float*)(pGTASA + 0x59FB8C) = -10.0f;
        *(float*)(pGTASA + 0x59FB90) =  10.0f;
    }

    // Fix vehicles backlights light state
    if(cfg->GetBool("FixCarsBacklightLightState", true, "Visual"))
    {
        aml->Write(pGTASA + 0x591272, (uintptr_t)"\x02", 1);
        aml->Write(pGTASA + 0x59128E, (uintptr_t)"\x02", 1);
    }

    // Limit sand/dust particles on bullet impact (they are EXTREMELY dropping FPS)
    if(cfg->GetBool("LimitSandDustBulletParticles", true, "Visual"))
    {
        AddBulletImpactFx_BackTo = pGTASA + 0x36478E + 0x1;
        aml->Redirect(pGTASA + 0x36477C + 0x1, (uintptr_t)AddBulletImpactFx_Inject);
        if(cfg->GetBool("LimitSandDustBulletParticlesWithSparkles", false, "Visual"))
        {
            nLimitWithSparkles = BULLETFX_SPARK;
        }
    }

    // Do not set primary color to the white on vehicles paintjob
    if(cfg->GetBool("PaintJobDontSetPrimaryToWhite", true, "Visual"))
    {
        aml->PlaceNOP(pGTASA + 0x582328, 2);
    }

    // Fix walking while rifle-aiming
    if(cfg->GetBool("FixAimingWalkRifle", true, "Gameplay"))
    {
        HOOKPLT(ControlGunMove, pGTASA + 0x66F9D0);
    }

    // Fix water physics
    if(cfg->GetBool("FixWaterPhysics", true, "Gameplay"))
    {
        HOOKPLT(ProcessSwimmingResistance, pGTASA + 0x66E584);
        ProcessBuoyancy_BackTo = pGTASA + 0x56F946 + 0x1;
        aml->Redirect(pGTASA + 0x56F930 + 0x1, (uintptr_t)ProcessBuoyancy_Inject);
    }

    // Fix stealable items sucking
    if(cfg->GetBool("ClampObjectToStealDist", true, "Gameplay"))
    {
        aml->Write(pGTASA + 0x40B162, (uintptr_t)"\xB7\xEE\x00\x0A", 4);
    }

    // Fix broken basketball minigame by placing the save icon away from it
    if(cfg->GetBool("MaddDoggMansionSaveFix", true, "SCMFixes"))
    {
        HOOKPLT(GenerateNewPickup_MaddDogg, pGTASA + 0x674DE4);
    }

    // Fix broken basketball minigame by placing the save icon away from it
    if(cfg->GetBool("FixStarBribeInSFBuilding", true, "SCMFixes"))
    {
        HOOKPLT(GenerateNewPickup_SFBribe, pGTASA + 0x674DE4);
    }

    // Fix rifle pickup that stuck inside the stadium
    if(cfg->GetBool("FixSFStadiumRiflePickup", true, "SCMFixes"))
    {
        HOOKPLT(GenerateNewPickup_SFRiflePickup, pGTASA + 0x674DE4);
    }

    // Remove jetpack leaving on widget press while in air?
    if(cfg->GetBool("DisableDropJetPackInAir", true, "Gameplay"))
    {
        HOOKPLT(DropJetPackTask, pGTASA + 0x675AA8);
    }

    // Dont stop the car before leaving it
    if(cfg->GetBool("ImmediatelyLeaveTheCar", true, "Gameplay"))
    {
        HOOK(CanPedStepOutCar, aml->GetSym(hGTASA, "_ZNK8CVehicle16CanPedStepOutCarEb"));
    }

    // Bring back penalty when CJ dies!
    if(cfg->GetBool("WeaponPenaltyIfDied", true, "Gameplay"))
    {
        DiedPenalty_BackTo = pGTASA + 0x3088E0 + 0x1;
        aml->Redirect(pGTASA + 0x3088BE + 0x1, (uintptr_t)DiedPenalty_Inject);
    }

    // Fix emergency vehicles
    if(cfg->GetBool("FixEmergencyVehicles", true, "Gameplay"))
    {
        EmergencyVeh_BackTo = pGTASA + 0x3DD88C + 0x1;
        aml->Redirect(pGTASA + 0x3DD87A + 0x1, (uintptr_t)EmergencyVeh_Inject);
        HOOKPLT(SetFOV_Emergency, pGTASA + 0x673DDC);
        aml->Write(pGTASA + 0x3DD8A0, (uintptr_t)"\xB0\xEE\x42\x1A", 4); // WarDumbs are multiplying it by 0.8? Reasonable for 2013 but why didnt they remove that in 2.00?
        aml->Write(pGTASA + 0x3DD8A4, (uintptr_t)"\xB0\xEE\x40\x2A", 4); // Same thing but by 0.875... Cringe.
    }

    // Fix cutscene FOV (disabled by default right now, causes the camera being too close on ultrawide screens)
    if(cfg->GetBool("FixCutsceneFOV", false, "Visual"))
    {
        HOOKPLT(SetFOV, pGTASA + 0x673DDC);
    }

    // Fix red marker that cannot be placed in a menu on ultrawide screens
    // Kinda trashy fix...
    if(cfg->GetBool("FixRedMarkerUnplaceable", true, "Gameplay"))
    {
        aml->Unprot(pGTASA + 0x2A9E60, sizeof(float));
        *(float*)(pGTASA + 0x2A9E60) /= 1.2f;
        aml->Write(pGTASA + 0x2A9D42, (uintptr_t)"\x83\xEE\x0C\x3A", 4);
        HOOKPLT(PlaceRedMarker_MarkerFix, pGTASA + 0x6702C8);
    }

    // Dont set player on fire when he's on burning BMX (MTA:SA)
    if(cfg->GetBool("DontBurnPlayerOnBurningBMX", true, "Gameplay"))
    {
        aml->PlaceB(pGTASA + 0x3F1ECC + 0x1, pGTASA + 0x3F1F24 + 0x1);
    }

    // Increase the number of vehicles types (not actual vehicles) that can be loaded at once (MTA:SA)
    // Causes crash and completely useless
    //if(cfg->GetBool("DesiredNumOfCarsLoadedBuff", true, "Gameplay"))
    //{
    //    *(unsigned char*)(aml->GetSym(hGTASA, "_ZN10CStreaming24desiredNumVehiclesLoadedE")) = 50; // Game hardcoded to 50 max (lazy to fix crashes for patches below)
    //    aml->PlaceNOP(pGTASA + 0x46BE1E, 1);
    //    aml->PlaceNOP(pGTASA + 0x47269C, 2);
    //}

    // THROWN projectiles throw more accurately (MTA:SA)
    if(cfg->GetBool("ThrownProjectilesAccuracy", true, "Gameplay"))
    {
        aml->PlaceB(pGTASA + 0x5DBBC8 + 0x1, pGTASA + 0x5DBD0C + 0x1);
    }

    // Disable call to FxSystem_c::GetCompositeMatrix in CAEFireAudioEntity::UpdateParameters 
    // that was causing a crash - spent ages debugging, the crash happens if you create 40 or 
    // so vehicles that catch fire (upside down) then delete them, repeating a few times.
    // (MTA:SA)
    if(cfg->GetBool("GetCompositeMatrixFixPossibleCrash", true, "Gameplay"))
    {
        aml->PlaceNOP(pGTASA + 0x395E6A, 7);
    }

    // Disable setting the occupied's vehicles health to 75.0f when a burning ped enters it (MTA:SA)
    if(cfg->GetBool("DontGiveCarHealthFromBurningPed", true, "Gameplay"))
    {
        aml->PlaceNOP(pGTASA + 0x3F1CAC, 0xD);
    }

    // Increase intensity of vehicle tail light corona (MTA:SA)
    // Is this even working on Android?
    if(cfg->GetBool("IncreaseTailLightIntensity", true, "Gameplay"))
    {
        aml->Write(pGTASA + 0x591016, (uintptr_t)"\xF0", 1);
    }

    // Cinematic vehicle camera on double tap
    if(cfg->GetBool("CinematicCameraOnDoubleTap", true, "Gameplay"))
    {
        HOOKPLT(PlayerInfoProcess_Cinematic, pGTASA + 0x673E84);
    }
    
    // Fix Skimmer plane ( https://github.com/XMDS )
    if (cfg->GetBool("SkimmerPlaneFix", true, "Gameplay"))
    {
        SkimmerWaterResistance_BackTo = pGTASA + 0x589ADC + 0x1;
        aml->Redirect(pGTASA + 0x589AD4 + 0x1, (uintptr_t)SkimmerWaterResistance_Inject);
    }

    // Buff streaming
    if(cfg->GetBool("BuffStreamingMem", true, "Gameplay"))
    {
        int wantsMB = cfg->GetInt("BuffStreamingMem_CountMB", 512, "Gameplay");
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

    // Buff streaming memory (dynamic)
    bDynStreamingMem = cfg->GetBool("DynamicStreamingMem", true, "Gameplay");
    fDynamicStreamingMemPercentage = 0.001f * cfg->GetInt("DynamicStreamingMem_Percentage", 80, "Gameplay");

    // Buff planes max height
    if(cfg->GetBool("BuffPlanesMaxHeight", true, "Gameplay"))
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
    if(cfg->GetBool("BuffJetpackMaxHeight", true, "Gameplay"))
    {
        aml->Unprot(pGTASA + 0x5319D0, sizeof(float));
        *(float*)(pGTASA + 0x5319D0) *= 2.0f;
    }

    // 44100 Hz Audio support (without a mod OpenAL Update but works with it anyway)
    if(cfg->GetBool("Allow44100HzAudio", true, "Gameplay"))
    {
        aml->Unprot(pGTASA + 0x613E0A, sizeof(int));
        *(int*)(pGTASA + 0x613E0A) = 44100;
    }

    // Disable GTA vehicle detachment at rotation awkwardness
    if(cfg->GetBool("FixVehicleDetachmentAtRot", true, "Visual"))
    {
        aml->PlaceB(pGTASA + 0x407344 + 0x1, pGTASA + 0x407016 + 0x1);
    }

    // Bring back missing "Shoot" button for S.W.A.T. when we dont have a weapon. WarDrum forgot about it.
    // And boat Predator. The fix dont work without the SWAT fix below
    if(cfg->GetBool("FixMissingShootBtnForSWATAndPredator", true, "Gameplay"))
    {
        GetCarGunFired_BackTo1 = pGTASA + 0x3F99E8 + 0x1;
        GetCarGunFired_BackTo2 = pGTASA + 0x3F9908 + 0x1;
        aml->Redirect(pGTASA + 0x3F99C4 + 0x1, (uintptr_t)GetCarGunFired_Inject);

        aml->Write8(pGTASA + 0x5E2940, 0x01); // CPad::GetCarGunFired(Pad, >1<, 0);
    }

    // Just a fuzzy seek. Tell MPG123 to not load useless data.
    if(cfg->GetBool("FuzzySeek", true, "Gameplay"))
    {
        HOOKPLT(mpg123_param, pGTASA + 0x66F3D4);
    }

    // Fix water cannon on a high fps
    if(cfg->GetBool("FixHighFPSWaterCannons", true, "Gameplay"))
    {
        HOOKPLT(WaterCannonRender, pGTASA + 0x67432C);
        HOOKPLT(WaterCannonUpdate, pGTASA + 0x6702EC);
    }

    // Fix moving objects on a high fps (through the scripts)
    if(cfg->GetBool("FixHighFPSOpcode034E", true, "SCMFixes"))
    {
        ProcessCommands800To899_BackTo = pGTASA + 0x347866 + 0x1;
        aml->Redirect(pGTASA + 0x346E84 + 0x1, (uintptr_t)ProcessCommands800To899_Inject);
    }

    // Fix pushing force
    if(cfg->GetBool("FixPhysicalPushForce", true, "Gameplay"))
    {
        PhysicalApplyCollision_BackTo = pGTASA + 0x402B72 + 0x1;
        aml->Redirect(pGTASA + 0x402B68 + 0x1, (uintptr_t)PhysicalApplyCollision_Inject);
    }

    // Can now rotate the camera inside the heli/plane?
    // https://github.com/TheOfficialFloW/gtasa_vita/blob/6417775e182b0c8b789cc9a0c1161e6f1b43814f/loader/main.c#L736
    if(cfg->GetBool("UnstuckHeliCamera", true, "Gameplay"))
    {
        aml->Write(pGTASA + 0x3C0866, (uintptr_t)"\x00\x20\x00\xBF", 4);
        aml->Write(pGTASA + 0x3C1518, (uintptr_t)"\x00\x20\x00\xBF", 4);
        aml->Write(pGTASA + 0x3C198A, (uintptr_t)"\x00\x20\x00\xBF", 4);
        aml->Write(pGTASA + 0x3FC462, (uintptr_t)"\x00\x20\x00\xBF", 4);
        aml->Write(pGTASA + 0x3FC754, (uintptr_t)"\x00\x20\x00\xBF", 4);
    }

    // Classic CJ shadow
    if(cfg->GetBool("FixClassicCJShadow", true, "Gameplay"))
    {
        aml->PlaceNOP(pGTASA + 0x5B86C4, 7);
    }

    // Car Slowdown Fix
    if(cfg->GetBool("FixCarSlowdownHighFPS", true, "Gameplay"))
    {
        SET_TO(mod_HandlingManager_off4, (*(uintptr_t*)(pGTASA + 0x6777C8)) + 4); // FLA
        HOOKPLT(ProcessVehicleWheel, pGTASA + 0x66FC7C);
    }

    // Heli rotor speed fix
    if(cfg->GetBool("FixHeliRotorSpeedHighFPS", true, "Visual"))
    {
        aml->Unprot(pGTASA + 0x572604, sizeof(float)*5);
        SET_TO(fRotorFinalSpeed, pGTASA + 0x572604);
        SET_TO(fRotor1Speed, pGTASA + 0x572610);
        SET_TO(fRotor2Speed, pGTASA + 0x572608);
        HOOK(Heli_ProcessFlyingStuff, aml->GetSym(hGTASA, "_ZN5CHeli21ProcessFlyingCarStuffEv"));
    }

    // Give more space for opcodes 038D+038F
    if(cfg->GetBool("FixOpcodes038D/F", true, "SCMFixes"))
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
    if(cfg->GetBool("Re3_PedSpawnDeoptimize", true, "Gameplay"))
    {
        aml->Write(pGTASA + 0x3F40E8, (uintptr_t)"\x03", 1);
    }

    // RE3: Make cars and peds to not despawn when you look away
    if(cfg->GetBool("Re3_ExtOffscreenDespRange", true, "Gameplay"))
    {
        aml->PlaceB(pGTASA + 0x2EC660 + 0x1, pGTASA + 0x2EC6D6 + 0x1); // Vehicles
        aml->PlaceB(pGTASA + 0x4CE4EA + 0x1, pGTASA + 0x4CE55C + 0x1); // Peds
    }

    // RE3: Do not remove locked cars
    if(cfg->GetBool("Re3_DontRemoveLockedCars", true, "Gameplay"))
    {
        HOOKPLT(PossiblyRemoveVehicle_Re3, pGTASA + 0x6736E4);
    }

    // RE3: Correct clouds rotating speed
    if(cfg->GetBool("Re3_CloudsRotationHighFPS", true, "Visual"))
    {
        HOOKPLT(CloudsUpdate_Re3, pGTASA + 0x670358);
    }

    // RE3: multiple instances of semaphore fix
    if(cfg->GetBool("Re3_CdStreamMultipleInst", true, "Gameplay"))
    {
        aml->Unprot(pGTASA + 0x26C0DA, 4);
        aml->Write(pGTASA + 0x2C97DC, (uintptr_t)"\x00\x23", 2);
        SET_TO(ThreadLaunch_GagNameSet, pGTASA + 0x26C0DA);
        HOOKPLT(OS_ThreadLaunch, pGTASA + 0x66E8A0);
    }

    // RE3: Fix a lil mistake in AskForObjectToBeRenderedInGlass
    if(cfg->GetBool("Re3_InGlassRenderedPlus1", true, "Visual"))
    {
        aml->Write(pGTASA + 0x5AC61C, (uintptr_t)"\x1F", 1);
    }

    // RE3: Free the space for an object in a pool by deleting temp objects if there is no space
    if(cfg->GetBool("Re3_FreePlaceInObjectPool", true, "Gameplay"))
    {
        HOOKPLT(Object_New, pGTASA + 0x6726EC);
    }

    // Lower threads sleeping timer
    if(cfg->GetBool("LowerThreadsSleeping", true, "Gameplay"))
    {
        aml->Write(pGTASA + 0x1D248E, (uintptr_t)"\x08", 1);
        aml->Write(pGTASA + 0x266D3A, (uintptr_t)"\x08", 1);
        aml->Write(pGTASA + 0x26706E, (uintptr_t)"\x08", 1);
        aml->Write(pGTASA + 0x26FDCC, (uintptr_t)"\x08", 1);
    }

    // Dont kill peds when jacking their car, monster!
    if(cfg->GetBool("DontKillPedsOnCarJacking", true, "Gameplay"))
    {
        aml->PlaceB(pGTASA + 0x4F5FC4 + 0x1, pGTASA + 0x4F5FD6 + 0x1);
    }

    // Colored zone names are back
    if(cfg->GetBool("ColoredZoneNames", true, "Visual"))
    {
        ColoredZoneNames_BackTo = pGTASA + 0x438404 + 0x1;
        aml->Redirect(pGTASA + 0x4383D6 + 0x1, (uintptr_t)ColoredZoneNames_Inject);
    }

    // Bigger max count of peds
    if(cfg->GetBool("BuffMaxPedsCount", true, "Gameplay"))
    {
        *(int*)aml->GetSym(hGTASA, "_ZN11CPopulation20MaxNumberOfPedsInUseE") = 0x23;
        aml->Write(pGTASA + 0x3F4DE0, (uintptr_t)"\x23", 1);
        aml->Write(pGTASA + 0x4CC284, (uintptr_t)"\x23", 1);
        aml->Write(pGTASA + 0x4CCBE0, (uintptr_t)"\x23", 1);
        aml->Write(pGTASA + 0x4CCBEA, (uintptr_t)"\x1C", 1);
    }

    // Bigger max count of cars
    if(cfg->GetBool("BuffMaxCarsCount", true, "Gameplay"))
    {
        *(int*)aml->GetSym(hGTASA, "_ZN8CCarCtrl20MaxNumberOfCarsInUseE") = 0x14;
        aml->Write(pGTASA + 0x3F4DD2, (uintptr_t)"\x14", 1);
    }

    // Frick your "improved characters models", War Dumb
    if(cfg->GetBool("FixPedSpecInShaders", true, "Visual"))
    {
        aml->Write(pGTASA + 0x1CE2F0, (uintptr_t)"\x40\x46\x00\xBF", 4);
        aml->Write(pGTASA + 0x1CEDC4, (uintptr_t)"\x40\xF2\x40\x60", 4);
        aml->Write(pGTASA + 0x1CEF1A, (uintptr_t)"\x58\x46\x00\xBF", 4);
        aml->PlaceB(pGTASA + 0x1CF5C8 + 0x1, pGTASA + 0x1CF658 + 0x1);
    }

    // Tells "FindGroundZ" functions that we need "can teleport on objects" too
    if(cfg->GetBool("IncludeObjectsForFindZ", true, "Gameplay"))
    {
        HOOKPLT(FindGroundZ2D, pGTASA + 0x66EDB8);
        HOOKPLT(FindGroundZ3D, pGTASA + 0x67022C);
    }

    // RE3: Road reflections
    if(cfg->GetBool("Re3_WetRoadsReflections", true, "Visual"))
    {
        RoadReflections_BackTo = pGTASA + 0x5A2EA4 + 0x1;
        aml->Redirect(pGTASA + 0x5A2E94 + 0x1, (uintptr_t)RoadReflections_Inject);
    }

    // Helicopter's rotor blur
    bool heliblur = cfg->GetBool("HeliRotorBlur", true, "Visual");
    bool planeblur = cfg->GetBool("PlaneRotorBlur", false, "Visual");
    if(heliblur || planeblur)
    {
        aml->Redirect(pGTASA + 0x572D6C + 0x1, (uintptr_t)RotorBlurRender_Inject1);
        aml->Redirect(pGTASA + 0x572D8C + 0x1, (uintptr_t)RotorBlurRender_Inject2);
        RotorBlurRender_BackTo1 = pGTASA + 0x572D76 + 0x1;
        RotorBlurRender_BackTo2 = pGTASA + 0x572D96 + 0x1;
        
        if(heliblur)
        {
            HOOK(HeliRender, aml->GetSym(hGTASA, "_ZN5CHeli6RenderEv"));
            nRotorMdlIgnore = cfg->GetBool("HeliRotorBlur_NotMaverick", true, "Visual") ? 487 : 0;
        }
        // Glitchy planes
        if(planeblur) HOOK(PlaneRender, aml->GetSym(hGTASA, "_ZN6CPlane6RenderEv"));
    }

    /* ImprovedStreaming by ThirteenAG & Junior_Djjr */
    /* ImprovedStreaming by ThirteenAG & Junior_Djjr */
    /* ImprovedStreaming by ThirteenAG & Junior_Djjr */
    
    // Preload LOD models
    bPreloadLOD = cfg->GetBool("IS_PreloadLODs", false, "Gameplay");
    bPreloadAnim = cfg->GetBool("IS_PreloadAnims", false, "Gameplay");
    bPreloadPed = cfg->GetBool("IS_PreloadPeds", false, "Gameplay");
    if(bPreloadLOD || bPreloadAnim || bPreloadPed || bDynStreamingMem)
    {
        HOOKPLT(GameProcess, pGTASA + 0x66FE58);
        if(bPreloadLOD)
        {
            LoadScene_BackTo = pGTASA + 0x4691E2 + 0x1;
            aml->Redirect(pGTASA + 0x4691D6 + 0x1, (uintptr_t)LoadScene_Inject);
        }
        if(bPreloadPed)
        {
            HOOKPLT(AddPedModel, pGTASA + 0x675D98);
            aml->Redirect(pGTASA + 0x40CC20 + 0x1, (uintptr_t)InitPools_Inject);
            InitPools_BackTo = pGTASA + 0x40CC8A + 0x1;
        }
    }
    bUnloadUnusedModels = cfg->GetBool("IS_UnloadUnusedModels", true, "Gameplay");
    if(bUnloadUnusedModels)
    {
        bDontUnloadInCutscenes = !cfg->GetBool("IS_UnloadUnusedModels_InCutscene", false, "Gameplay");
        fRemoveUnusedStreamMemPercentage = 0.001f * cfg->GetInt("IS_UnloadUnusedModels_Percentage", 80, "Gameplay");
        nRemoveUnusedInterval = cfg->GetInt("IS_UnloadUnusedModels_Interval", 60, "Gameplay");

        if(fRemoveUnusedStreamMemPercentage < 0.001f ||
           fRemoveUnusedStreamMemPercentage > 0.99f  ||
           nRemoveUnusedInterval < 0)
        {
            bUnloadUnusedModels = false;
        }
    }

    // Fix color picker widget
    if(cfg->GetBool("FixColorPicker", true, "Visual"))
    {
        HOOKPLT(GetColorPickerValue, pGTASA + 0x6645C4);
    }

    // Bigger distance for light coronas
    if(cfg->GetBool("BuffDistForLightCoronas", true, "Visual"))
    {
        aml->Write(pGTASA + 0x5A4960, (uintptr_t)"\x00\x22\xC4\xF2\xC8\x32", 6); // CEntity::ProcessLightsForEntity
        aml->Write(pGTASA + 0x362EC6, (uintptr_t)"\x00\x20\xC4\xF2\xC8\x30", 6); // CTrafficLights::DisplayActualLight
        aml->Write(pGTASA + 0x56585E, (uintptr_t)"\x00\x21\xC4\xF2\xF0\x21", 6); // CBike::PreRender
        aml->Write(pGTASA + 0x5658FC, (uintptr_t)"\x00\x20\xC4\xF2\xF0\x20", 6); // CBike::PreRender
        aml->Write(pGTASA + 0x573826, (uintptr_t)"\x00\x20\xC4\xF2\x96\x30", 6); // CHeli::SearchLightCone
        aml->Unprot(pGTASA + 0x55BAD0, sizeof(float)); *(float*)(pGTASA + 0x55BAD0) = 300.0f; // CAutomobile::PreRender
    }

    // Bigger distance for light shadows
    if(cfg->GetBool("BuffDistForLightShadows", true, "Visual"))
    {
        aml->Write(pGTASA + 0x36311C, (uintptr_t)"\xC4\xF2\xF0\x21", 4); // CTrafficLights::DisplayActualLight, 40 -> 120
        aml->Write(pGTASA + 0x3F1996, (uintptr_t)"\xC4\xF2\xF0\x21", 4); // CFireManager::Update, 40 -> 120
        fLightDist = 120.0f; // For thingies below \/
    }

    // Bring back light shadows from poles!
    if(cfg->GetBool("BackPolesLightShadow", true, "Visual"))
    {
        ProcessLightsForEntity_BackTo = pGTASA + 0x5A4DA8 + 0x1;
        aml->Redirect(pGTASA + 0x5A4578 + 0x1, (uintptr_t)ProcessLightsForEntity_Inject);
        HOOK(AddLight, aml->GetSym(hGTASA, "_ZN12CPointLights8AddLightEh7CVectorS0_ffffhbP7CEntity"));
    }

    // Fix greenish detail tex
    if(cfg->GetBool("FixGreenTextures", true, "Visual"))
    {
        aml->PlaceNOP(pGTASA + 0x1B00B0, 5); // Dont set textureDetail variable! We'll handle it by ourselves!
        HOOK(emu_TextureSetDetailTexture, aml->GetSym(hGTASA, "_Z27emu_TextureSetDetailTexturePvj"));
    }

    // Bring back light shadows from poles!
    if(cfg->GetBool("BuffStaticShadowsCount", true, "Gameplay"))
    {
        // Static shadows?
        asShadowsStored_NEW = new CRegisteredShadow[0xFF]; memset(asShadowsStored_NEW, 0, sizeof(CRegisteredShadow) * 0xFF);
        aStaticShadows_NEW = new CStaticShadow[0xFF] {0}; memset(aStaticShadows_NEW, 0, sizeof(CStaticShadow) * 0xFF);
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
        
        HOOK(RenderStaticShadows, aml->GetSym(hGTASA, "_ZN8CShadows19RenderStaticShadowsEb"));
        HOOK(InitShadows, aml->GetSym(hGTASA, "_ZN8CShadows4InitEv"));
    }

    // Move shadows closer to the ground
    if(cfg->GetBool("MoveShadowsToTheGround", true, "Visual"))
    {
        float fH = cfg->GetFloat("MoveShadowsToTheGround_Height", 0.02f, "Visual");
        aml->Unprot(pGTASA + 0x5A224C, sizeof(float)); *(float*)(pGTASA + 0x5A224C) = -fH;
        aml->Unprot(pGTASA + 0x5B3ED4, sizeof(float)); *(float*)(pGTASA + 0x5B3ED4) = fH;
        aml->Unprot(pGTASA + 0x5BB80C, sizeof(float)); *(float*)(pGTASA + 0x5BB80C) = fH;
        aml->Unprot(pGTASA + 0x5BC188, sizeof(float)); *(float*)(pGTASA + 0x5BC188) = fH;
        aml->Unprot(pGTASA + 0x5E03D0, sizeof(float)); *(float*)(pGTASA + 0x5E03D0) = fH;
    }

    // Radar
    if(cfg->GetBool("FixRadarStreaming", true, "Visual"))
    {
        aml->PlaceB(pGTASA + 0x44313A + 0x1, pGTASA + 0x443146 + 0x1);
    }

    // texture2D bias? In theory, this thing (below) is giving better FPS + better textures
    if(cfg->GetBool("NoShaderTextureBias", true, "Visual"))
    {
        aml->Write(pGTASA + 0x5EAB20 + 52, (uintptr_t)"      ", 6);
        aml->Write(pGTASA + 0x5EAB94 + 52, (uintptr_t)"      ", 6);
        aml->Write(pGTASA + 0x5EAC96 + 75, (uintptr_t)"      ", 6);
        aml->Write(pGTASA + 0x5EABE8 + 53, (uintptr_t)"      ", 6);
        aml->Write(pGTASA + 0x5EABE8 + 99, (uintptr_t)"      ", 6);
    }

    // Sweet's roof is not that tasty anymore
    if(cfg->GetBool("FixClimbDying", true, "Gameplay"))
    {
        HOOKPLT(ClimbProcessPed, pGTASA + 0x66CC28);
    }

    // For new save only, fixes 3 bikes spawn that are inside each other
    if(cfg->GetBool("FixDrivingSchoolBikesSpawn", true, "SCMFixes"))
    {
        HOOKPLT(CreateCarGenerator, pGTASA + 0x672808);
    }

    // Fixing a crosshair position by very stupid math
    if(cfg->GetBool("FixCrosshair", true, "Visual"))
    {
        HOOKPLT(DrawCrosshair, pGTASA + 0x672880);
        //HOOK(CalculateAspectRatio_CrosshairFix, aml->GetSym(hGTASA, "_ZN5CDraw20CalculateAspectRatioEv"));
    }

    // Fixed cheats
    if(cfg->GetBool("FixCheats", true, "Gameplay"))
    {
        HOOKPLT(DoCheats, pGTASA + 0x675458);
        HOOKPLT(KBEvent, pGTASA + 0x6709B8);
        aml->Write(aml->GetSym(hGTASA, "_ZN6CCheat16m_aCheatHashKeysE"), (uintptr_t)CCheat__m_aCheatHashKeys, sizeof(CCheat__m_aCheatHashKeys));
    }
    
    // Save file loading crash fix
    if(cfg->GetBool("FixSaveLoadingCrash1", true, "Gameplay"))
    {
        HOOKPLT(EntMdlNoCreate, pGTASA + 0x673844);
    }
    
    // Fix Adjustable.cfg loading?
    // UPD: Introduced another glitch, so its unfixed. yet.
    // UD2: Fixed with a much better way. But another glitch arrived with X-coord shifting
    if(cfg->GetBool("FixAdjustableSizeLowering", true, "Visual"))
    {
        aml->Unprot(pGTASA + 0x28260C, sizeof(float)); *(float*)(pGTASA + 0x28260C) = 5.0f;
    }
    
    // Taxi lights (obviously)
    if(cfg->GetBool("TaxiLights", true, "Visual"))
    {
        SET_TO(SetTaxiLight, aml->GetSym(hGTASA, "_ZN11CAutomobile12SetTaxiLightEb"));
        HOOK(AutomobileRender, aml->GetSym(hGTASA, "_ZN11CAutomobile6RenderEv"));
    }
    
    // Minimap in interiors? Hell nah!
    if(cfg->GetBool("NoInteriorRadar", true, "Visual"))
    {
        HOOKPLT(DrawRadar, pGTASA + 0x66F618);
    }
    
    // Money have 8 digits now? Exciting!
    if(cfg->GetBool("PCStyledMoney", false, "Visual"))
    {
        DrawMoney_BackTo = pGTASA + 0x2BD260 + 0x1;
        aml->Redirect(pGTASA + 0x2BD258 + 0x1, (uintptr_t)DrawMoney_Inject);
    }
    
    // Idk how to fix it, yet
    //if(cfg->GetBool("ReColors", true, "Visual"))
    //{
    //    HOOK(GetIntColour, aml->GetSym(hGTASA, "_ZN11CHudColours12GetIntColourEh"));
    //    HOOK(GetRGBA, aml->GetSym(hGTASA, "_ZN11CHudColours7GetRGBAEh"));
    //}
    
    // Country. Rifle. Is. 3rd. Person.
    if(cfg->GetBool("FixCountryRifleAim", true, "Gameplay"))
    {
        // YES, THATS EXTREMELY EASY TO FIX, LMAO
        aml->PlaceNOP(pGTASA + 0x5378C0, 3);
        //aml->Write(pGTASA + 0x5378C0, (uintptr_t)"\xFF", 1);
        aml->Write(pGTASA + 0x53813C, (uintptr_t)"\xFF", 1); // 
    }
    
    // Haha, no gejmpat!!1
    if(cfg->GetBool("ForceTouchControls", false, "Gameplay"))
    {
        aml->Redirect(aml->GetSym(hGTASA, "_ZN4CHID12GetInputTypeEv"), (uintptr_t)ret0);
    }
    
    // Fix ped conversations are gone
    if(cfg->GetBool("FixPedConversation", true, "Gameplay"))
    {
        aml->PlaceB(pGTASA + 0x301BFE + 0x1, pGTASA + 0x301C0E + 0x1);
    }
    
    // Equipped parachute attacked anim fix
    if(cfg->GetBool("EquippedParaAttackAnimFix", true, "Visual"))
    {
        HOOK(ComputeDamageAnim, aml->GetSym(hGTASA, "_ZN12CEventDamage17ComputeDamageAnimEP4CPedb"));
    }
    
    // MixSets-SA: Hostile gangs
    if(cfg->GetBool("MIX_HostileGangs", true, "Gameplay"))
    {
        HOOKPLT(ProcessPedGroups, pGTASA + 0x670164);
        HOOKPLT(PedGroups_IsOnAMission, pGTASA + 0x670CDC);
    }
    
    // Water Quadrant
    int dist = cfg->GetInt("DetailedWaterDrawDistance", 48 * 5, "Visual");
    if(dist > 0)
    {
        if(dist < 24) dist = 24;
        *DETAILEDWATERDIST = dist;
    }
    
    // Peepo: Fix traffic lights
    if(cfg->GetBool("FixTrafficLights", true, "Visual"))
    {
        HOOK(TrFix_RenderEffects, aml->GetSym(hGTASA, "_Z13RenderEffectsv"));
        HOOK(TrFix_InitGame2nd, aml->GetSym(hGTASA, "_ZN5CGame5Init2EPKc"));
    }
    
    if(cfg->GetBool("DisableCloudSaves", false, "Gameplay"))
    {
        aml->Write(aml->GetSym(hGTASA, "UseCloudSaves"), (uintptr_t)"\x00", 1);
    }
    
    // Always show wanted stars even if we're not breakin the law
    if(cfg->GetBool("AlwaysDrawWantedStars", false, "Visual"))
    {
        aml->PlaceB(pGTASA + 0x2BDF82 + 0x1, pGTASA + 0x2BDFA4 + 0x1);
    }
    
    // Max mobilesc0,mobilesc1,...,mobilesc### for us
    mobilescCount = cfg->GetInt("MaxLoadingScreens", 7, "Visual");
    if(mobilescCount > 0 && mobilescCount != 7)
    {
        LoadSplashes_BackTo = pGTASA + 0x43AD00 + 0x1;
        aml->Redirect(pGTASA + 0x43ACEC + 0x1, (uintptr_t)LoadSplashes_Inject);
    }
    
    // A mistake by R* that overwrites "total num of X peds"
    if(cfg->GetBool("FixGangsCounterOverflow", true, "Gameplay"))
    {
        PedCountCalc_BackTo1 = pGTASA + 0x4D0CC2 + 0x1;
        PedCountCalc_BackTo2 = pGTASA + 0x4D0D0A + 0x1;
        aml->Redirect(pGTASA + 0x4D0CAE + 0x1, (uintptr_t)PedCountCalc_Inject1);
        aml->Redirect(pGTASA + 0x4D0CF6 + 0x1, (uintptr_t)PedCountCalc_Inject2);
    }
    
    // Some kind of "Sprint Everywhere"
    if(cfg->GetBool("SprintOnAnySurface", true, "Gameplay"))
    {
        aml->Redirect(aml->GetSym(hGTASA, "_ZN14SurfaceInfos_c12CantSprintOnEj"), (uintptr_t)ret0);
    }
    
    // An improved ForceDXT
    if(cfg->GetBool("ForceLoadDXT", false, "Gameplay"))
    {
        HOOK(LoadTexDBThumbs, aml->GetSym(hGTASA, "_ZN22TextureDatabaseRuntime4LoadEPKcb21TextureDatabaseFormat"));
    }
    
    // Vehicle sun glare
    if(cfg->GetBool("VehicleSunGlare", true, "Visual"))
    {
        SET_TO(DoSunGlare, aml->GetSym(hGTASA, "_ZN8CVehicle10DoSunGlareEv"));
        HOOK(RenderVehicle_SunGlare, aml->GetSym(hGTASA, "_ZN8CVehicle6RenderEv"));
    }
        
    aml->Unprot(pGTASA + 0x3C51E8, sizeof(float));
    *(float*)(pGTASA + 0x3C51E8) = 10000.0f * (1.0f / cfg->GetFloat("CameraZoomingSpeed", 2.5f, "Gameplay"));
    
    aml->Unprot(pGTASA + 0x3C51F0, sizeof(float));
    *(float*)(pGTASA + 0x3C51F0) = cfg->GetFloat("MinimalCameraZoomingFOV", 70.0f, "Gameplay");
    
    // Removes "plis give us 5 stars plis plis"
    if(cfg->GetBool("RemoveAskingToRate", true, "Others"))
    {
        aml->PlaceB(pGTASA + 0x345E9C + 0x1, pGTASA + 0x345EAE + 0x1);
    }
    
    // Remove "ExtraAirResistance" flag
    if(cfg->GetBool("NoExtraAirResistanceFlag", true, "Gameplay"))
    {
        aml->Redirect(aml->GetSym(hGTASA, "_ZN10CCullZones29DoExtraAirResistanceForPlayerEv"), (uintptr_t)ret0);
    }
    
    // Allow all radio tracks to be played!
    if(cfg->GetBool("NoRadioCuts", true, "Gameplay"))
    {
        aml->Redirect(aml->GetSym(hGTASA, "_Z14IsRemovedTracki"), (uintptr_t)ret0);
        aml->Redirect(pGTASA + 0x3A152A + 0x1, pGTASA + 0x3A1602 + 0x1); // QueueUpTracksForStation
        aml->Redirect(pGTASA + 0x3A35F6 + 0x1, pGTASA + 0x3A369A + 0x1); // ChooseMusicTrackIndex
        aml->Redirect(pGTASA + 0x3A37C2 + 0x1, pGTASA + 0x3A385E + 0x1); // ChooseIdentIndex
        aml->Redirect(pGTASA + 0x3A3A1E + 0x1, pGTASA + 0x3A3AA2 + 0x1); // ChooseAdvertIndex
        aml->Redirect(pGTASA + 0x3A4374 + 0x1, pGTASA + 0x3A4416 + 0x1); // ChooseTalkRadioShow
        aml->Redirect(pGTASA + 0x3A44D6 + 0x1, pGTASA + 0x3A4562 + 0x1); // ChooseDJBanterIndexFromList
    }
    
    // Make the grenade's collision smaller. Will this nade fit in this hole now?
    if(cfg->GetBool("SmallerGrenadeCollision", true, "Gameplay"))
    {
        HOOK(InitTempColModels, aml->GetSym(hGTASA, "_ZN14CTempColModels10InitialiseEv"));
        aml->Redirect(pGTASA + 0x4697CA + 0x1, (uintptr_t)LoadWeaponObject_Inject);
        LoadWeaponObject_BackTo = pGTASA + 0x4697D4 + 0x1;
        SET_TO(ColModelAllocateData, aml->GetSym(hGTASA, "_ZN9CColModel12AllocateDataEiiiiib"));
        SET_TO(ColSphereSet, aml->GetSym(hGTASA, "_ZN10CColSphere3SetEfRK7CVectorhhh"));
        SET_TO(ms_colModelWeapon, aml->GetSym(hGTASA, "_ZN14CTempColModels17ms_colModelWeaponE"));
    }
    
    // Fixes a weird glitch from there: https://github.com/multitheftauto/mtasa-blue/issues/1123
    if(cfg->GetBool("MTA_FixProjectiles", true, "Gameplay"))
    {
        aml->Redirect(pGTASA + 0x5D991E + 0x1, pGTASA + 0x5D9A04 + 0x1);
    }
    
    // Fixes a very slippery floor?
    if(cfg->GetBool("MTA_FixIceFloor", true, "Gameplay"))
    {
        //aml->Redirect(pGTASA + 0x4A1C3C + 0x1, pGTASA + 0x4A1C4E + 0x1);

        IceFloor_BackTo1 = pGTASA + 0x4A1C4E + 0x1; // skip
        IceFloor_BackTo2 = pGTASA + 0x4A1CA6 + 0x1; // continue
        aml->Redirect(pGTASA + 0x4A1C3C + 0x1, (uintptr_t)IceFloor_Inject);
    }

    // Can now use a gun!
    if(cfg->GetBool("HighFPSAimingWalkingFix", true, "Gameplay"))
    {
        aml->Unprot(pGTASA + 0x4DD9E8, sizeof(float));
        SET_TO(float_4DD9E8, pGTASA + 0x4DD9E8);
        HOOK(TaskSimpleUseGunSetMoveAnim, aml->GetSym(hGTASA, "_ZN17CTaskSimpleUseGun11SetMoveAnimEP4CPed"));
    }

    // AllowLicensePlatesForAllCars
    if(cfg->GetBool("AllowLicensePlatesForAllCars", true, "Visual"))
    {
        aml->PlaceNOP(pGTASA + 0x5822E6, 3);
        aml->PlaceNOP(pGTASA + 0x582398, 3);
    }

    // Game is checking if HP is < 1.0 but it may be lower!
    if(cfg->GetBool("AllowCrouchWith1HP", true, "Gameplay"))
    {
        aml->Write(pGTASA + 0x54316C, (uintptr_t)"\xB4", 1);
    }

    // Show muzzle flash for the last bullet in magazine
    if(cfg->GetBool("MuzzleFlashForLastBullet", true, "Visual"))
    {
        aml->PlaceNOP(pGTASA + 0x4DDCCA, 10);
    }

    // Renders shadows on all surfaces -> disables a flag
    if(cfg->GetBool("DrawShadowsOnAllSurfaces", true, "Visual"))
    {
        aml->Write(pGTASA + 0x5BAE23, (uintptr_t)"\x00", 1);
    }

    // Spread fix
    if(cfg->GetBool("WeaponSpreadFix", true, "Gameplay"))
    {
        SET_TO(fPlayerAimRotRate, aml->GetSym(hGTASA, "fPlayerAimRotRate"));
        HOOK(FireInstantHit, aml->GetSym(hGTASA, "_ZN7CWeapon14FireInstantHitEP7CEntityP7CVectorS3_S1_S3_S3_bb"));
    }

    // Fixes farclip glitch with wall (wardumb be like)
    if(cfg->GetBool("FixFogWall", true, "Visual"))
    {
        aml->Write(pGTASA + 0x5EB9D9, (uintptr_t)"\x31\x2E\x30\x30", 4);
    }

    // Fixes Corona sprites stretching at foggy weather
    if(cfg->GetBool("FixCoronasStretching", true, "Visual"))
    {
        aml->Write(pGTASA + 0x5A27EC, (uintptr_t)"\xB0\xEE\x44\x0A", 4);
    }

    // JuniorDjjr: Fix IS_CHAR_DEAD returning false even if health is 0.0
    if(cfg->GetBool("FixIsCharDead0HP", true, "SCMFixes"))
    {
        HOOKPLT(RunningScript_IsPedDead, pGTASA + 0x670D10);
    }

    // Sprint button after aiming and dropping to the water
    if(cfg->GetBool("FixSprintButtonSwimDisappear", true, "Gameplay"))
    {
        HOOKPLT(IsTargetingActiveForPlayer, pGTASA + 0x6708F0);
        HOOKPLT(IsTargetingActive, pGTASA + 0x674718);
    }

    // Guess by the name
    if(cfg->GetBool("FixParachuteLandingAnim", true, "Visual"))
    {
        HOOKPLT(PlayerInfoProcess_ParachuteAnim, pGTASA + 0x673E84);
    }

    // FX particles distance multiplier!
    float fxMultiplier = cfg->GetFloat("FxDistanceMult", 2.5f, "Visual");
    if(fxMultiplier != 1 && fxMultiplier > 0.1)
    {
        aml->Unprot(pGTASA + 0x368C7C, sizeof(float)); *(float*)(pGTASA + 0x368C7C) = 0.015625 * fxMultiplier;
        aml->Unprot(pGTASA + 0x36EC34, sizeof(float)); *(float*)(pGTASA + 0x36EC34) = 0.00390625 * fxMultiplier;
    }

    // A long awaiting radio animation... Why do you need it so bad?
    if(cfg->GetBool("BoatRadioAnimationFix", true, "Visual"))
    {
        BoatRadio_BackTo = pGTASA + 0x58C230 + 0x1;
        aml->Redirect(pGTASA + 0x58C224 + 0x1, (uintptr_t)BoatRadio_Inject);
    }

    // MixSets' DuckAnyWeapon
    if(cfg->GetBool("MIX_DuckAnyWeapon", true, "Gameplay"))
    {
        DuckAnyWeapon_BackTo1 = pGTASA + 0x543714 + 0x1; // ret 1
        DuckAnyWeapon_BackTo2 = pGTASA + 0x54376A + 0x1; // ret 0
        aml->Redirect(pGTASA + 0x5436EC + 0x1, (uintptr_t)DuckAnyWeapon_Inject);
    }

    // BengbuGuards' idea #1
    if(cfg->GetBool("FixSecondSiren", true, "Gameplay"))
    {
        aml->PlaceNOP(pGTASA + 0x590134 + 0x1, 2);
        aml->Write(pGTASA + 0x590168, "\xAF\xF3\x00\x80", 4);
    }

    // BengbuGuards' idea #2
    if(cfg->GetBool("BoatRotatingRadarFix", true, "Visual"))
    {
        aml->Write8(pGTASA + 0x6107D9, 0x00); // Animation name: boat_moving_hi -> boat_moving
    }

    // Fix airbubbles from the jaw (CJ is breathing with his ass, lololololol)
    if(cfg->GetBool("AirBubblesFromJaw", true, "Visual"))
    {
        aml->Write(pGTASA + 0x53C4A0, "\xD0\xED\x8C\x0B\xD0\xF8\x38\x02", 8);
    }

    // Fix plane disappear after explode. It's still there, but is invisible. Does it have any meaning, Rockstar?
    if(cfg->GetBool("PlaneExplodeDisappearFix", true, "Visual"))
    {
        aml->Write(pGTASA + 0x579ECC, "\x4C\xE0", 2); // B  loc_579F68
        aml->PlaceNOP(pGTASA + 0x579F7C + 0x1, 1);
    }
        

    // Disables backface culling for object with transparent textures
    /*if(cfg->GetBool("NoBFCullingForTransparents", true, "Visual"))
    {
        aml->Write8(pGTASA + 0x40FC64, 0x01);
    }*/

    // Fix camera zooming
    /*if(cfg->GetBool("FixCameraSniperZoomDist", true, "Gameplay"))
    {
        CamZoomProc_BackTo = pGTASA + 0x3C5160 + 0x1;
        aml->Redirect(pGTASA + 0x3C5158 + 0x1, (uintptr_t)CamZoomProc_Inject);

        FixSniperZoomingDistance_BackTo = pGTASA + 0x3C5060 + 0x1;
        aml->Redirect(pGTASA + 0x3C5054 + 0x1, (uintptr_t)FixSniperZoomingDistance_Inject);
        FixSniperZoomingDistance2_BackTo = pGTASA + 0x3C513C + 0x1;
        aml->Redirect(pGTASA + 0x3C5132 + 0x1, (uintptr_t)FixSniperZoomingDistance2_Inject);
    }*/
    
    // Fixes an opcode 08F8 which controls if the game should show you "Updated stats mate!"
    /*if(cfg->GetBool("FixOpcode08F8", true, "SCMFixes"))
    {
        HOOK(CheckForStatsMessage, aml->GetSym(hGTASA, "_ZN6CStats20CheckForStatsMessageEb"));
    }*/

    // Undone and so disabled
    /*if(cfg->GetBool("DamagedComponentsColorFix", true, "Visual"))
    {
        //HOOK(ChooseVehicleColour, aml->GetSym(hGTASA, "_ZN17CVehicleModelInfo19ChooseVehicleColourERhS0_S0_S0_i"));
        //HOOKPLT(SetComponentVisibility, pGTASA + 0x66ED8C);
        HOOK(PreRenderCar, aml->GetSym(hGTASA, "_ZN11CAutomobile9PreRenderEv"));
    }*/
    
    // JuniorDjjr, W.I.P.
    /*if(cfg->GetBool("FoodEatingModelFix", true, "Gameplay"))
    {
        HOOKPLT(PlayerInfoProcess_Food, pGTASA + 0x673E84);
    }*/
    
    // Michelle date: CTaskSimpleCarSetPedInAsPassenger?

    // No SetClumpAlpha for ped (probably fixes the problem with player's lighting when holding a weapon)
    //if(cfg->GetBool("FixPlayerLighting", true, "Visual"))
    //{
    //    aml->PlaceNOP(pGTASA + 0x4A2622 + 0x1, 2);
    //}

    // Detonator unused anim?
    // This should work but this animation is not in the files!
    //if(cfg->GetBool("UnusedDetonatorAnim", true, "Visual"))
    //{
    //    HOOKPLT(UseDetonator, pGTASA + 0x66FD94);
    //}
}