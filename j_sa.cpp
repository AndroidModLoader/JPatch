#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>
#include <dlfcn.h>
#include <vector>
#include <cctype>
#include <array>
#include <utility>
#include <unistd.h>

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

extern int androidSdkVer;
extern uintptr_t pGTASA;
extern void *hGTASA;

#define BUNCHTAILS_EX 1024

namespace GTA_SA
{

union ScriptVariables
{
    int      i;
    float    f;
    uint32_t u;
    void*    p;
};

// fMagic = 1.6666667:
// ARM64: Closest is 1.665 at 0x739EF4

static constexpr float ar43 = 4.0f / 3.0f;
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
float fAspectCorrection = 0.0f, fAspectCorrectionDiv = 0.0f;
#define fAspectCorrection (*ms_fAspectRatio - ar43)
#define fAspectCorrectionDiv (*ms_fAspectRatio / ar43)
#define GetTimeStep() (*ms_fTimeStep)
#define GetTimeStepInSeconds() (GetTimeStep() / 50.0f)
#define GetTimeStepMagic() (GetTimeStep() / fMagic)
#define GetTimeStepInvMagic() (fMagic / GetTimeStep())

struct VehiclePartsPair
{
    RpMaterial* material;
    uintptr_t color;
    RwTexture* texture;
}; // 
VehiclePartsPair* gStoredMats;
bool *ms_bIsPlayerOnAMission, *m_UserPause, *m_CodePause, *m_aCheatsActive, *bDidWeProcessAnyCinemaCam, *bRunningCutscene, *bProcessingCutscene, *ms_bTakePhoto;
uint8_t *ms_currentCol, *ms_nGameClockDays, *ms_nGameClockMonths, *_bf_12c;
int32_t *DETAILEDWATERDIST, *ms_nNumGang, *StatTypesInt, *lastDevice, *NumberOfSearchLights, *ms_numAnimBlocks, *RasterExtOffset, *textureDetail, *ms_iActiveSequence;
uint32_t *gbCineyCamProcessedOnFrame, *CloudsIndividualRotation, *m_ZoneFadeTimer, *ms_memoryUsed, *ms_memoryAvailable, *m_FrameCounter, *m_snTimeInMilliseconds;
float *ms_fTimeStep, *ms_fFOV, *game_FPS, *CloudsRotation, *WeatherWind, *fSpriteBrightness, *m_fLightsOnGroundBrightness, *m_f3rdPersonCHairMultX, *m_f3rdPersonCHairMultY, *ms_fAspectRatio, *ms_fTimeScale, *mod_HandlingManager_off4;
CVector *m_vecDirnLightToSun;
CVector *m_VectorToSun;
int *m_CurrentStoredValue, *currArea;
float *fPlayerAimRotRate;
CVector2D *m_vecCachedPos;

CPlayerInfo                 *WorldPlayers;
CIntVector2D                *windowSize;
CCamera                     *TheCamera;
RsGlobalType                *RsGlobal;
MobileMenu                  *gMobileMenu;
CWidget                     **m_pWidgets;
ScriptVariables*            ScriptParams;
CLinkList<AlphaObjectInfo>  *m_alphaList;
CPool<CCutsceneObject>      **pObjectPool;
CZoneInfo                   **m_pCurrZoneInfo;
CWeaponInfo                 *aWeaponInfo;
CPolyBunch                  *aPolyBunches;
CBaseModelInfo              **ms_modelInfoPtrs;
CRGBA                       *ms_vehicleColourTable;
CRGBA                       *HudColour;
CTaskComplexSequence        *ms_taskSequence;
CRunningScript              **pActiveScripts;
void                        *g_surfaceInfos;
RwTexture                   **ms_pRemapTexture;
CIdleCam                    *gIdleCam;
CLinkList<CCollisionData*>  *ms_colModelCache;
TDBArray<RwTexture*>        *detailTextures;

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////     Funcs     ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////
int ret0(int a, ...) { return 0; } // Generic
int ret1(int a, ...) { return 1; } // Generic
void crash_this() { __builtin_trap(); } // Used for redirected funcs, to get the function it's called from (virtual funcs are hard!)

void (*BrightLightsInit)();
void (*BrightLightsRender)();
bool (*IsOnAMission)();
void (*AddToCheatString)(char);
void (*RwRenderStateSet)(RwRenderState, void*);
void (*RwRenderStateGet)(RwRenderState, void*);
void (*ClearPedWeapons)(CPed*);
eBulletFxType (*GetBulletFx)(void* self, unsigned int surfaceId);
void (*LIB_PointerGetCoordinates)(int, int*, int*, float*);
bool (*Touch_IsDoubleTapped)(WidgetIDs, bool doTapEffect, int idkButBe1);
bool (*Touch_IsHeldDown)(WidgetIDs, int idkButBe1);
void (*Touch_ClearTapHistory)(WidgetIDs);
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
#ifdef AML32
void (*GetTouchPosition)(CVector2D*, int cachedPosNum);
#else
CVector2D (*GetTouchPosition)(int cachedPosNum);
#endif
bool (*StoreStaticShadow)(uintptr_t id, uint8_t type, RwTexture* texture, CVector* posn, float frontX, float frontY, float sideX, float sideY, int16_t intensity, uint8_t red, uint8_t green, uint8_t blue, float zDistane, float scale, float drawDistance, bool temporaryShadow, float upDistance);
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
RpAtomic* (*SetEditableMaterialsCB)(RpAtomic *pAtomic, void *pData);
void (*RenderEntity)(CEntity*);
void (*GeneratePlateText)(char*, int);
char* (*GetCustomCarPlateText)(CVehicleModelInfo*);
int (*GetVehicleRef)(CVehicle*);
void (*RenderFontBuffer)();
void (*RwTextureDestroy)(RwTexture*);
double (*OS_TimeAccurate)();
CVehicle* (*VehicleCheat)(int);
bool (*WidgetIsTouched)(int, CVector2D*, int);
void (*InitIdleCam)(CIdleCam*);
void (*ProcessIdleCam)(CIdleCam*);
void (*ResetIdleCam)(CIdleCam*, bool);
void (*SetIdleCamTarget)(CIdleCam*, CEntity*);
void (*RunIdleCam)(CIdleCam*);
void (*SetColTrianglePlane)(CColTrianglePlane*, CVector*, CColTriangle*);
void (*SetVehicleColour)(CVehicleModelInfo*, uint8_t, uint8_t, uint8_t, uint8_t);
CTask* (*GetActiveTask)(CTaskManager*);
int16_t (*GetPedWalkLR)(void*);
int16_t (*GetPedWalkUD)(void*);
void (*DoSunGlare)(CVehicle*);
void (*SetTaxiLight)(CAutomobile*, bool);
RwImage* (*RwImageCreate)(int, int, int);
void (*RwImageAllocatePixels)(RwImage*);
void (*RwImageSetFromRaster)(RwImage*, RwRaster*);
void (*SetDirMyDocuments)();
void (*FileMgrSetDir)(const char* dirName);
void (*JPegCompressScreenToFile)(RwCamera*, const char*);
bool (*RwGrabScreen)(RwCamera*, const char*);
void (*ApplyTurnForce)(CPhysical*, CVector, CVector);

inline int GetSectorForCoord(int coord)
{
    return floor(0.02f * coord + 60.0f);
}
inline int GetSectorForCoord(float coord)
{
    return floor(0.02f * coord + 60.0f);
}

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
int nMaxStreamingMemForDynamic, nDynamicStreamingMemBumpStep;
inline void BumpStreamingMemory(int megabytes)
{
    *ms_memoryAvailable += megabytes * 1024 * 1024;
    if(*ms_memoryAvailable > nMaxStreamingMemForDynamic)
    {
        *ms_memoryAvailable = nMaxStreamingMemForDynamic;
    }
}

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
    #include "patches_sa.inl"
#else
    #include "patches_sa64.inl"
#endif

void JPatch()
{
    cfg->Bind("IdeasFrom", "", "About")->SetString("MTA:SA Team, re3 contributors, JuniorDjjr, ThirteenAG, Blackbird88, 0x416c69, Whitetigerswt, XMDS, Peepo"); cfg->ClearLast();

    // Functions Start //
    SET_TO(AddToCheatString,        aml->GetSym(hGTASA, "_ZN6CCheat16AddToCheatStringEc"));
    SET_TO(RwRenderStateSet,        aml->GetSym(hGTASA, "_Z16RwRenderStateSet13RwRenderStatePv"));
    SET_TO(RwRenderStateGet,        aml->GetSym(hGTASA, "_Z16RwRenderStateGet13RwRenderStatePv"));
    SET_TO(ClearPedWeapons,         aml->GetSym(hGTASA, "_ZN4CPed12ClearWeaponsEv"));
    SET_TO(GetBulletFx,             aml->GetSym(hGTASA, "_ZN14SurfaceInfos_c11GetBulletFxEj"));
    SET_TO(LIB_PointerGetCoordinates, aml->GetSym(hGTASA, "_Z25LIB_PointerGetCoordinatesiPiS_Pf"));
    SET_TO(Touch_IsDoubleTapped,    aml->GetSym(hGTASA, "_ZN15CTouchInterface14IsDoubleTappedENS_9WidgetIDsEbi"));
    SET_TO(Touch_IsHeldDown,        aml->GetSym(hGTASA, "_ZN15CTouchInterface10IsHeldDownENS_9WidgetIDsEi"));
    SET_TO(Touch_ClearTapHistory,   aml->GetSym(hGTASA, "_ZN15CTouchInterface15ClearTapHistoryENS_9WidgetIDsE"));
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
    SET_TO(StoreStaticShadow,       aml->GetSym(hGTASA, BYVER("_ZN8CShadows17StoreStaticShadowEjhP9RwTextureP7CVectorffffshhhfffbf", "_ZN8CShadows17StoreStaticShadowEyhP9RwTextureP7CVectorffffshhhfffbf")));
    SET_TO(TransformPoint,          aml->GetSym(hGTASA, "_Z14TransformPointR5RwV3dRK16CSimpleTransformRKS_"));
    SET_TO(IsOnAMission,            aml->GetSym(hGTASA, "_ZN11CTheScripts18IsPlayerOnAMissionEv"));
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
    SET_TO(TaskConstructor,         aml->GetSym(hGTASA, BYVER("_ZN5CTasknwEj", "_ZN5CTasknwEm")));
    SET_TO(TaskStartNamedAnim,      aml->GetSym(hGTASA, "_ZN23CTaskSimpleRunNamedAnimC2EPKcS1_ifibbbb"));
    SET_TO(SetTask,                 aml->GetSym(hGTASA, "_ZN12CTaskManager7SetTaskEP5CTaskib"));
    SET_TO(TaskComplexSequenceAddTask, aml->GetSym(hGTASA, "_ZN20CTaskComplexSequence7AddTaskEP5CTask"));
    SET_TO(BlendAnimation,          aml->GetSym(hGTASA, "_ZN12CAnimManager14BlendAnimationEP7RpClump12AssocGroupId11AnimationIdf"));
    SET_TO(RpAnimBlendGetNextAssociation, aml->GetSym(hGTASA, "_Z29RpAnimBlendGetNextAssociationP21CAnimBlendAssociation"));
    SET_TO(SetEditableMaterialsCB,  aml->GetSym(hGTASA, "_ZN17CVehicleModelInfo22SetEditableMaterialsCBEP8RpAtomicPv"));
    SET_TO(RenderEntity,            *(uintptr_t*)(pGTASA + BYVER(0x66F764, 0x83F610)));
    SET_TO(GeneratePlateText,       aml->GetSym(hGTASA, "_ZN18CCustomCarPlateMgr17GeneratePlateTextEPci"));
    SET_TO(GetCustomCarPlateText,   aml->GetSym(hGTASA, "_ZN17CVehicleModelInfo21GetCustomCarPlateTextEv"));
    SET_TO(GetVehicleRef,           aml->GetSym(hGTASA, "_ZN6CPools13GetVehicleRefEP8CVehicle"));
    SET_TO(RenderFontBuffer,        aml->GetSym(hGTASA, "_ZN5CFont16RenderFontBufferEv"));
    SET_TO(RwTextureDestroy,        aml->GetSym(hGTASA, "_Z16RwTextureDestroyP9RwTexture"));
    SET_TO(OS_TimeAccurate,         aml->GetSym(hGTASA, "_Z15OS_TimeAccuratev"));
    SET_TO(VehicleCheat,            aml->GetSym(hGTASA, "_ZN6CCheat12VehicleCheatEi"));
    SET_TO(WidgetIsTouched,         aml->GetSym(hGTASA, "_ZN15CTouchInterface9IsTouchedENS_9WidgetIDsEP9CVector2Di"));
    SET_TO(InitIdleCam,             aml->GetSym(hGTASA, "_ZN8CIdleCam4InitEv"));
    SET_TO(ProcessIdleCam,          aml->GetSym(hGTASA, "_ZN8CIdleCam7ProcessEv"));
    SET_TO(ResetIdleCam,            aml->GetSym(hGTASA, "_ZN8CIdleCam5ResetEb"));
    SET_TO(SetIdleCamTarget,        aml->GetSym(hGTASA, "_ZN8CIdleCam9SetTargetEP7CEntity"));
    SET_TO(RunIdleCam,              aml->GetSym(hGTASA, "_ZN8CIdleCam3RunEv"));
    SET_TO(SetColTrianglePlane,     aml->GetSym(hGTASA, "_ZN17CColTrianglePlane3SetEPK7CVectorR12CColTriangle"));
    SET_TO(GetActiveTask,           aml->GetSym(hGTASA, "_ZNK12CTaskManager13GetActiveTaskEv"));
    SET_TO(GetPedWalkLR,            aml->GetSym(hGTASA, "_ZN4CPad19GetPedWalkLeftRightEv"));
    SET_TO(GetPedWalkUD,            aml->GetSym(hGTASA, "_ZN4CPad16GetPedWalkUpDownEv"));
    SET_TO(DoSunGlare,              aml->GetSym(hGTASA, "_ZN8CVehicle10DoSunGlareEv"));
    SET_TO(SetTaxiLight,            aml->GetSym(hGTASA, "_ZN11CAutomobile12SetTaxiLightEb"));
    SET_TO(RwImageCreate,           aml->GetSym(hGTASA, "_Z13RwImageCreateiii"));
    SET_TO(RwImageAllocatePixels,   aml->GetSym(hGTASA, "_Z21RwImageAllocatePixelsP7RwImage"));
    SET_TO(RwImageSetFromRaster,    aml->GetSym(hGTASA, "_Z20RwImageSetFromRasterP7RwImageP8RwRaster"));
    SET_TO(SetDirMyDocuments,       aml->GetSym(hGTASA, "_ZN8CFileMgr17SetDirMyDocumentsEv"));
    SET_TO(FileMgrSetDir,           aml->GetSym(hGTASA, "_ZN8CFileMgr6SetDirEPKc"));
    SET_TO(JPegCompressScreenToFile, aml->GetSym(hGTASA, "_Z24JPegCompressScreenToFileP8RwCameraPKc"));
    SET_TO(RwGrabScreen,            aml->GetSym(hGTASA, "_Z12RwGrabScreenP8RwCameraPc"));
    SET_TO(ApplyTurnForce,          aml->GetSym(hGTASA, "_ZN9CPhysical14ApplyTurnForceE7CVectorS0_"));
    #ifdef AML32
        SET_TO(RpLightCreate,           aml->GetSym(hGTASA, "_Z13RpLightCreatei"));
        SET_TO(RpLightSetColor,         aml->GetSym(hGTASA, "_Z15RpLightSetColorP7RpLightPK10RwRGBAReal"));
        SET_TO(RpLightSetRadius,        aml->GetSym(hGTASA, "_Z16RpLightSetRadiusP7RpLightf"));
        SET_TO(RwFrameCreate,           aml->GetSym(hGTASA, "_Z13RwFrameCreatev"));
        SET_TO(_rwObjectHasFrameSetFrame, aml->GetSym(hGTASA, "_Z25_rwObjectHasFrameSetFramePvP7RwFrame"));
        SET_TO(RpWorldAddLight,         aml->GetSym(hGTASA, "_Z15RpWorldAddLightP7RpWorldP7RpLight"));
        SET_TO(RpWorldRemoveLight,      aml->GetSym(hGTASA, "_Z18RpWorldRemoveLightP7RpWorldP7RpLight"));
        SET_TO(RwFrameDestroy,          aml->GetSym(hGTASA, "_Z14RwFrameDestroyP7RwFrame"));
        SET_TO(RpLightDestroy,          aml->GetSym(hGTASA, "_Z14RpLightDestroyP7RpLight"));
    #endif // AML32
    // Functions End   //
    
    // Variables Start //
    SET_TO(ms_taskSequence,         aml->GetSym(hGTASA, "_ZN14CTaskSequences15ms_taskSequenceE"));
    SET_TO(pActiveScripts,          aml->GetSym(hGTASA, "_ZN11CTheScripts14pActiveScriptsE"));
    SET_TO(ms_fTimeStep,            aml->GetSym(hGTASA, "_ZN6CTimer12ms_fTimeStepE"));
    SET_TO(WorldPlayers,            *(void**)(pGTASA + BYVER(0x6783C8, 0x84E7A8))); // Patched CWorld::Players will work now!
    SET_TO(ms_fFOV,                 aml->GetSym(hGTASA, "_ZN5CDraw7ms_fFOVE"));
    SET_TO(game_FPS,                aml->GetSym(hGTASA, "_ZN6CTimer8game_FPSE"));
    SET_TO(TheCamera,               aml->GetSym(hGTASA, "TheCamera"));
    SET_TO(RsGlobal,                aml->GetSym(hGTASA, "RsGlobal"));
    SET_TO(g_surfaceInfos,          aml->GetSym(hGTASA, "g_surfaceInfos"));
    SET_TO(m_snTimeInMilliseconds,  aml->GetSym(hGTASA, "_ZN6CTimer22m_snTimeInMillisecondsE"));
    SET_TO(gMobileMenu,             aml->GetSym(hGTASA, "gMobileMenu"));
    SET_TO(NumberOfSearchLights,    aml->GetSym(hGTASA, "_ZN5CHeli20NumberOfSearchLightsE"));
    SET_TO(lastDevice,              aml->GetSym(hGTASA, "lastDevice"));
    SET_TO(m_pWidgets,              *(void**)(pGTASA + BYVER(0x67947C, 0x850910))); // Patched CTouchInterface::m_pWidgets will work now!
    SET_TO(bDidWeProcessAnyCinemaCam, aml->GetSym(hGTASA, "bDidWeProcessAnyCinemaCam"));
    SET_TO(bRunningCutscene,        aml->GetSym(hGTASA, "_ZN12CCutsceneMgr10ms_runningE"));
    SET_TO(bProcessingCutscene,     aml->GetSym(hGTASA, "_ZN12CCutsceneMgr21ms_cutsceneProcessingE"));
    SET_TO(ScriptParams,            *(void**)(pGTASA + BYVER(0x676F7C, 0x84BF38))); // Patched ScriptParams will work now!
    SET_TO(m_alphaList,             aml->GetSym(hGTASA, "_ZN18CVisibilityPlugins11m_alphaListE"));
    SET_TO(CloudsRotation,          aml->GetSym(hGTASA, "_ZN7CClouds13CloudRotationE"));
    SET_TO(CloudsIndividualRotation, aml->GetSym(hGTASA, "_ZN7CClouds18IndividualRotationE"));
    SET_TO(WeatherWind,             aml->GetSym(hGTASA, "_ZN8CWeather4WindE"));
    SET_TO(pObjectPool,             *(void**)(pGTASA + BYVER(0x676BBC, 0x84B7C0)));
    SET_TO(m_pCurrZoneInfo,         aml->GetSym(hGTASA, "_ZN9CPopCycle15m_pCurrZoneInfoE"));
    SET_TO(m_ZoneFadeTimer,         aml->GetSym(hGTASA, "_ZN4CHud15m_ZoneFadeTimerE"));
    SET_TO(ms_numAnimBlocks,        aml->GetSym(hGTASA, "_ZN12CAnimManager16ms_numAnimBlocksE"));
    SET_TO(ms_memoryUsed,           aml->GetSym(hGTASA, "_ZN10CStreaming13ms_memoryUsedE"));
    SET_TO(ms_memoryAvailable,      aml->GetSym(hGTASA, "_ZN10CStreaming18ms_memoryAvailableE"));
    SET_TO(fSpriteBrightness,       pGTASA + BYVER(0x966590, 0xBD760C));
    SET_TO(m_fLightsOnGroundBrightness, pGTASA + BYVER(0x9665A4, 0xBD7620));
    SET_TO(detailTextures,          aml->GetSym(hGTASA, "_ZN22TextureDatabaseRuntime14detailTexturesE"));
    SET_TO(textureDetail,           aml->GetSym(hGTASA, "textureDetail"));
    SET_TO(ms_iActiveSequence,      aml->GetSym(hGTASA, "_ZN14CTaskSequences18ms_iActiveSequenceE"));
    SET_TO(RasterExtOffset,         aml->GetSym(hGTASA, "RasterExtOffset"));
    SET_TO(m_f3rdPersonCHairMultX,  aml->GetSym(hGTASA, "_ZN7CCamera22m_f3rdPersonCHairMultXE"));
    SET_TO(m_f3rdPersonCHairMultY,  aml->GetSym(hGTASA, "_ZN7CCamera22m_f3rdPersonCHairMultYE"));
    SET_TO(ms_fAspectRatio,         aml->GetSym(hGTASA, "_ZN5CDraw15ms_fAspectRatioE"));
    SET_TO(ms_fTimeScale,           aml->GetSym(hGTASA, "_ZN6CTimer13ms_fTimeScaleE"));
    SET_TO(aWeaponInfo,             aml->GetSym(hGTASA, "aWeaponInfo"));
    SET_TO(windowSize,              aml->GetSym(hGTASA, "windowSize"));
    SET_TO(ms_bIsPlayerOnAMission,  aml->GetSym(hGTASA, "_ZN10CPedGroups22ms_bIsPlayerOnAMissionE"));
    SET_TO(DETAILEDWATERDIST,       aml->GetSym(hGTASA, "DETAILEDWATERDIST"));
    SET_TO(ms_nNumGang,             aml->GetSym(hGTASA, "_ZN11CPopulation11ms_nNumGangE"));
    SET_TO(aPolyBunches,            aml->GetSym(hGTASA, "_ZN8CShadows12aPolyBunchesE"));
    SET_TO(ms_modelInfoPtrs,        aml->GetSym(hGTASA, "_ZN10CModelInfo16ms_modelInfoPtrsE"));
    SET_TO(ms_currentCol,           aml->GetSym(hGTASA, "_ZN17CVehicleModelInfo13ms_currentColE"));
    SET_TO(ms_vehicleColourTable,   aml->GetSym(hGTASA, "_ZN17CVehicleModelInfo21ms_vehicleColourTableE"));
    SET_TO(ms_pRemapTexture,        aml->GetSym(hGTASA, "_ZN17CVehicleModelInfo16ms_pRemapTextureE"));
    SET_TO(ms_nGameClockDays,       aml->GetSym(hGTASA, "_ZN6CClock17ms_nGameClockDaysE"));
    SET_TO(ms_nGameClockMonths,     aml->GetSym(hGTASA, "_ZN6CClock19ms_nGameClockMonthsE"));
    SET_TO(StatTypesInt,            aml->GetSym(hGTASA, "_ZN6CStats12StatTypesIntE"));
    SET_TO(HudColour,               aml->GetSym(hGTASA, "HudColour"));
    SET_TO(m_UserPause,             aml->GetSym(hGTASA, "_ZN6CTimer11m_UserPauseE"));
    SET_TO(m_CodePause,             aml->GetSym(hGTASA, "_ZN6CTimer11m_CodePauseE"));
    SET_TO(m_aCheatsActive,         aml->GetSym(hGTASA, "_ZN6CCheat15m_aCheatsActiveE"));
    SET_TO(gIdleCam,                aml->GetSym(hGTASA, "gIdleCam"));
    SET_TO(m_FrameCounter,          aml->GetSym(hGTASA, "_ZN6CTimer14m_FrameCounterE"));
    SET_TO(gbCineyCamProcessedOnFrame, aml->GetSym(hGTASA, "gbCineyCamProcessedOnFrame"));
    SET_TO(ms_colModelCache,        aml->GetSym(hGTASA, "_ZN10CCollision16ms_colModelCacheE"));
    SET_TO(_bf_12c,                 pGTASA + BYVER(0x9EF9D8 + 0x12C, 0xC8C180 + 0x14C));
    SET_TO(SetVehicleColour,        aml->GetSym(hGTASA, "_ZN17CVehicleModelInfo16SetVehicleColourEhhhh"));
    SET_TO(gStoredMats,             pGTASA + BYVER(0x99E53C, 0xB8DB50));
    SET_TO(m_vecDirnLightToSun,     aml->GetSym(hGTASA, "_ZN10CTimeCycle19m_vecDirnLightToSunE"));
    SET_TO(m_VectorToSun,           aml->GetSym(hGTASA, "_ZN10CTimeCycle13m_VectorToSunE"));
    SET_TO(m_CurrentStoredValue,    aml->GetSym(hGTASA, "_ZN10CTimeCycle20m_CurrentStoredValueE"));
    SET_TO(fPlayerAimRotRate,       aml->GetSym(hGTASA, "fPlayerAimRotRate"));
    SET_TO(m_vecCachedPos,          aml->GetSym(hGTASA, "_ZN15CTouchInterface14m_vecCachedPosE"));
    SET_TO(mod_HandlingManager_off4, (*(uintptr_t*)(pGTASA + BYVER(0x6777C8, 0x84CFB8))) + 4); // FLA
    SET_TO(ms_bTakePhoto,           aml->GetSym(hGTASA, "_ZN7CWeapon13ms_bTakePhotoE"));
    SET_TO(currArea,                aml->GetSym(hGTASA, "_ZN5CGame8currAreaE"));
    // Variables End //

    // We need it for future fixes.
    HOOK(Global_TimerUpdate, aml->GetSym(hGTASA, "_ZN6CTimer6UpdateEv"));

    #ifdef AML32
        #include "preparations_sa.inl"
    #else
        #include "preparations_sa64.inl"
    #endif
}

}; // namespace GTA_SA
