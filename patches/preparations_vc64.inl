    // Boat and Skimmer on high FPS
    if(cfg->GetBool("HighFPSBoatSpeed", true, "Gameplay"))
    {
        Boat_ApplyWaterResistance_BackTo = pGTAVC + 0x3418F8;
        aml->Redirect(pGTAVC + 0x3418E8, (uintptr_t)Boat_ApplyWaterResistance_Inject);
    }
    
    // Fix wheels rotation speed on high FPS
    if(cfg->GetBool("FixWheelsRotationSpeed", true, "Visual"))
    {
        HOOK(ProcessWheelRotation, aml->GetSym(hGTAVC, "_ZN8CVehicle20ProcessWheelRotationE11tWheelStateRK7CVectorS3_f"));
    }
    
    // Car Slowdown Fix
    if(cfg->GetBool("FixCarSlowdownHighFPS", true, "Gameplay"))
    {
        HOOK(ProcessWheel_SlowDownFix, aml->GetSym(hGTAVC, "_ZN8CVehicle12ProcessWheelER7CVectorS1_S1_S1_ifffcPfP11tWheelStatet"));
        HOOK(ProcessBikeWheel_SlowDownFix, aml->GetSym(hGTAVC, "_ZN8CVehicle16ProcessBikeWheelER7CVectorS1_S1_S1_iffffcPfP11tWheelState17eBikeWheelSpecialt"));
    }
    
    // Fix heli rotor rotation speed on high FPS
    if(cfg->GetBool("FixRotorRotationSpeed", true, "Visual"))
    {
        HOOKBL(HeliRender_MatrixUpdate, pGTAVC + 0x3489B0);
    }
    
    // Fixes a streaming distance that gets bugged because of dumb R* logic (aspect ratio moment)
    if(cfg->GetBool("FixStreamingDistance", true, "Visual"))
    {
        CameraProcess_StreamDist_BackTo = pGTAVC + 0x1D8310;
        aml->Redirect(pGTAVC + 0x1D8300, (uintptr_t)CameraProcess_StreamDist_Inject);
        HOOK(SetFOV_StreamingDistFix, aml->GetSym(hGTAVC, "_ZN5CDraw6SetFOVEf"));
        
        aml->Write32(pGTAVC + 0x1D83BC, 0xF0001649);
        aml->Write32(pGTAVC + 0x1D83C0, 0xBD43A122);
    }
    
    // Fixing fat ass coronas
    if(cfg->GetBool("FixCoronasAspectRatio", true, "Visual"))
    {
        HOOK(CoronaSprite_CalcScreenCoors, aml->GetSym(hGTAVC, "_ZN7CSprite15CalcScreenCoorsERK5RwV3dPS0_PfS4_b"));
    }

    // Fix Vertex Weights
    if(cfg->GetBool("FixVertexWeight", true, "Visual"))
    {
        aml->Write32(pGTAVC + 0x39058C, 0x540005A0);
        aml->Write32(pGTAVC + 0x390640, MOVBits::Create(1, 27, false));
    }

    // RE3: Fix R* optimization that prevents peds to spawn
    if(cfg->GetBool("Re3_PedSpawnDeoptimize", true, "Gameplay"))
    {
        aml->Write(pGTAVC + 0x1EE2A8, "\x7F\x0A\x00\x71", 4);
    }

    // RE3: Road reflections
    if(cfg->GetBool("Re3_WetRoadsReflections", true, "Visual"))
    {
        aml->Write(pGTAVC + 0x1D6552, "\xE2\x6B\x40\xBD", 4);
    }

    // Fix traffic lights
    if(cfg->GetBool("FixTrafficLights", true, "Visual"))
    {
        HOOK(TrFix_RenderEffects, pGTAVC + 0x2DFB64);

        // Why does vehicle's lights are in that list?!
        aml->PlaceNOP4(pGTAVC + 0x32DB34, 1);
        aml->PlaceNOP4(pGTAVC + 0x32E2CC, 1);
        aml->PlaceNOP4(pGTAVC + 0x32E330, 1);
        aml->PlaceNOP4(pGTAVC + 0x32E6F0, 1);
        aml->PlaceNOP4(pGTAVC + 0x32E74C, 1);
        aml->PlaceNOP4(pGTAVC + 0x32EB54, 1);
        aml->PlaceNOP4(pGTAVC + 0x32EBB0, 1);

        // Why does bike's lights are in that list?!
        aml->PlaceNOP4(pGTAVC + 0x339394, 1);
        aml->PlaceNOP4(pGTAVC + 0x339770, 1);
        aml->PlaceNOP4(pGTAVC + 0x3398E0, 1);
    }

    // Fixes framelimiter (doesnt limit the FPS, ayo lol)
    // P.S. IT WAS NEVER WORKING, OH MY GOD
    if(cfg->GetBool("FixGameTimer", true, "Gameplay"))
    {
        aml->PlaceNOP(pGTAVC + 0x30C930, 1);
        HOOKBL(GameTick_TouchscreenUpdate, pGTAVC + 0x30C948);
        HOOK(OS_ScreenSetRefresh, aml->GetSym(hGTAVC, "_Z19OS_ScreenSetRefreshj"));

        aml->Write(pGTAVC + 0x30C9A0, "\xE0\x0F\x1F\x32", 4);
    }

    // Fix water scroll speed on high FPS
    if(cfg->GetBool("FixWaterUVScrollSpeed", true, "Visual"))
    {
        RenderWater_BackTo = pGTAVC + 0x357920;
        aml->Redirect(pGTAVC + 0x357910, (uintptr_t)RenderWater_Inject);
    }

    // Bringing back missing render states (WarDumb`s optimization)
    /*if(cfg->GetBool("FixRenderStates", true, "Visual"))
    {
        // Disabled for now, dont let users to check broken things :P
        //HOOK(RwRenderState_Patch, aml->GetSym(hGTAVC, "_Z16RwRenderStateSet13RwRenderStatePv"));
        aml->Write(pGTAVC + 0x352A10 + 17, "fcolor.xyz      ", 16);
        aml->Write(pGTAVC + 0x3533F0 + 103, "1.00", 4);
        aml->Write(pGTAVC + 0x3539CC + 65, "0.01111111", 10);
        aml->Write(pGTAVC + 0x353A84 + 42, "0.65000000", 10);
        
        // Not exactly like in GTA:SA. Should be a different thing.
        aml->Write(pGTAVC + 0x353978 + 44, "0.50000000", 10);
        aml->Write(pGTAVC + 0x353978 + 63, "0.50000000", 10);
        aml->Write(pGTAVC + 0x353924 + 42, "0.50000000", 10);
        aml->Write(pGTAVC + 0x353924 + 61, "0.50000000", 10);

        aml->PlaceNOP4(pGTAVC + 0x287526, 1);
        aml->PlaceNOP4(pGTAVC + 0x287554, 1);
        aml->PlaceNOP4(pGTAVC + 0x2874EE, 1);

        // shading
        aml->PlaceNOP4(pGTAVC + 0x286E44, 1);
    }*/

    // Fix clouds rotating speed
    if(cfg->GetBool("FixCloudsRotateSpeed", true, "Visual"))
    {
        aml->Write32(pGTAVC + 0x2AA5A0, 0xF0003928);
        aml->Write32(pGTAVC + 0x2AA5A4, 0xBD4DCD01);
        
        aml->Write32(pGTAVC + 0x2AA5C8, 0xB0003949);
        aml->Write32(pGTAVC + 0x2AA5CC, 0xBD401D24);
        
        HOOKPLT(CloudsUpdate_Speedo, pGTAVC + 0x56F1A8);
    }

    // The explosion "shadow" is missing
    if(cfg->GetBool("FixExplosionCraterTexture", true, "Visual"))
    {
        HOOKBL(AddExplosion_AddShadow, pGTAVC + 0x35E610);
    }

    // Allows the game to choose Extra6 component on a vehicle when created
    if(cfg->GetBool("AllowExtra6Part", true, "Visual"))
    {
        aml->Write(pGTAVC + 0x2F5FD0, "\x01\x10\x23\x1E", 4);
    }

    // Bigger distance for light coronas
    if(cfg->GetBool("BuffDistForLightCoronas", true, "Visual"))
    {

        aml->Write32(pGTAVC + 0x2AD0A8, 0xBD487520); // CEntity::ProcessLightsForEntity, 120 -> 400
        aml->Write32(pGTAVC + 0x1C2B80, 0xB0001709); // CTrafficLights::DisplayActualLight, 50 -> 400
        aml->Write32(pGTAVC + 0x1C2BAC, 0xBD487521); // CTrafficLights::DisplayActualLight, 50 -> 400
        aml->Write32(pGTAVC + 0x1C2B14, 0xB0001709); // CTrafficLights::DisplayActualLight, 50 -> 400
        aml->Write32(pGTAVC + 0x1C2B40, 0xBD487521); // CTrafficLights::DisplayActualLight, 50 -> 400
        aml->Write32(pGTAVC + 0x1C2F54, 0xB0001709); // CTrafficLights::DisplayActualLight, 50 -> 400
        aml->Write32(pGTAVC + 0x1C2F58, 0xBD487521); // CTrafficLights::DisplayActualLight, 50 -> 400
        aml->Write32(pGTAVC + 0x1C2F7C, 0xB0001709); // CTrafficLights::DisplayActualLight, 50 -> 400
        aml->Write32(pGTAVC + 0x1C2F80, 0xBD487521); // CTrafficLights::DisplayActualLight, 50 -> 400

        aml->Write32(pGTAVC + 0x2AD3BC, MOVBits::Create(0, 8, false)); // CEntity::ProcessLightsForEntity, Force off useNearDist
        aml->Write32(pGTAVC + 0x2AD67C, MOVBits::Create(0, 8, false)); // CEntity::ProcessLightsForEntity, Force off useNearDist
    }

    // Bigger distance for light shadows
    if(cfg->GetBool("BuffDistForLightShadows", true, "Visual"))
    {
        aml->Write32(pGTAVC + 0x2AD0A0, 0xD0000FAB); // \/
        aml->Write32(pGTAVC + 0x2AD0B0, 0xBD47796C); // CEntity::ProcessLightsForEntity, 40 -> 120
        aml->Write32(pGTAVC + 0x1EC830, 0xF00015A8); // \/
        aml->Write32(pGTAVC + 0x1EC834, 0xBD477906); // CFire::ProcessFire, 40 -> 120
        aml->Write32(pGTAVC + 0x1C27C4, 0xB0001708); // \/
        aml->Write32(pGTAVC + 0x1C27D0, 0xBD477906); // CTrafficLights::DisplayActualLight, 40 -> 120
        aml->Write32(pGTAVC + 0x1C2A58, 0xB0001708); // \/
        aml->Write32(pGTAVC + 0x1C2A64, 0xBD477906); // CTrafficLights::DisplayActualLight, 40 -> 120
        aml->Write32(pGTAVC + 0x1C2E68, 0xF00015A8); // \/
        aml->Write32(pGTAVC + 0x1C2E74, 0xBD477906); // CTrafficLights::DisplayActualLight, 40 -> 120
        aml->Write32(pGTAVC + 0x2D8F7C, 0xF0000E48); // \/
        aml->Write32(pGTAVC + 0x2D8F90, 0xBD477906); // CShadows::StoreShadowForPole, 40 -> 120

        // We do count vehicle shadows as light shadows?
        // Yeah, why not!
        HOOKBL(StoreShadowForVehicle, pGTAVC + 0x2D82C4);
        HOOKBL(StoreStaticShadow_LS, pGTAVC + 0x2D84A8);
        // still need patch at 1FC420 and 1FC424 (VC1.09)
        aml->Write32(pGTAVC + 0x2D83F4, 0xF0000E4A); // \/
        aml->Write32(pGTAVC + 0x2D83F8, 0xBD477542); // CShadows::StoreCarLightShadow, 27*27 -> 120*120
        aml->WriteFloat(pGTAVC + 0x4C61B4, 120.0f * 0.75f); // CShadows::StoreCarLightShadow, 27 -> 120
        aml->PlaceNOP(pGTAVC + 0x2D7F4C, 1);
        
        DoCollectableEffects_BackTo = pGTAVC + 0x18F7B0;
        aml->Redirect(pGTAVC + 0x18F7A0, (uintptr_t)DoCollectableEffects_Inject);
        DoPickUpEffects_BackTo = pGTAVC + 0x18EDD0;
        aml->Redirect(pGTAVC + 0x18EDC0, (uintptr_t)DoPickUpEffects_Inject);
    }
    
    // Just a little fix for banner
    if(cfg->GetBool("CorrectBannerRenderFlag", true, "Visual"))
    {
        aml->Write(pGTAVC + 0x2B812C, "\x23\x03\x80\x52", 4);
    }

    // Allows the game to render even more light shadows on the ground
    if(cfg->GetBool("BuffStaticShadowsCount", true, "Gameplay"))
    {
        aStaticShadows_NEW = new CStaticShadow[MAX_STATIC_SHADOWS + 1] {0}; memset(aStaticShadows_NEW, 0, sizeof(CStaticShadow) * (MAX_STATIC_SHADOWS + 1));
        aml->Write(pGTAVC + 0x576FF0, (uintptr_t)&aStaticShadows_NEW, sizeof(void*));

        // Static Shadows:
        // CShadows::StoreStaticShadow
        aml->Write32(pGTAVC + 0x2D7530, CMPBits::Create(MAX_STATIC_SHADOWS-1, 8, true)); // CMP X8, #0x2F
        // CShadows::RenderStaticShadows
        aml->Write32(pGTAVC + 0x2DA10C, CMPBits::Create(MAX_STATIC_SHADOWS, 28, true)); // CMP X28, #0x30
        aml->Write32(pGTAVC + 0x2DA120, CMPBits::Create(MAX_STATIC_SHADOWS, 28, true)); // CMP X28, #0x30
        // CShadows::UpdateStaticShadows
        aml->Write32(pGTAVC + 0x2DB4D8, CMPBits::Create(MAX_STATIC_SHADOWS, 8, true)); // CMP X8, #0x30

        HOOKPLT(InitShadows, pGTAVC + 0x574740);

        StoreStaticShadow_BackTo = pGTAVC + 0x2D76B8;
        aml->Redirect(pGTAVC + 0x2D7538, (uintptr_t)StoreStaticShadow_Inject);
        aml->Write32(pGTAVC + 0x2D76B8, 0x3100051F);
        aml->Write32(pGTAVC + 0x2D76BC, 0x54001780);
    }

    // Bigger max count of peds
    if(cfg->GetBool("BuffMaxPedsCount", true, "Gameplay"))
    {
        *(int*)aml->GetSym(hGTAVC, "_ZN11CPopulation20MaxNumberOfPedsInUseE") = 0x23;
    }

    // An attempt to fix fighting... (incomplete, the radius is small tho)
    if(cfg->GetBool("FixFighting", true, "Gameplay"))
    {
        aml->Write(pGTAVC + 0x291FC0, "\x01\x10\x22\x1E", 4); // CPed::Fight -> unlock move FIGHTMOVE_KNEE
        aml->Write(pGTAVC + 0x293458, "\x1F\x05\x00\x71", 4); // CheckForPedsOnGroundToAttack -> 4 >> PED_IN_FRONT_OF_ATTACKER
    }

    // FX particles distance multiplier!
    fxMultiplier = cfg->GetFloat("FxDistanceMult", 2.5f, "Visual");
    if(fxMultiplier != 1)
    {
        if(fxMultiplier < 0.1) fxMultiplier = 0.1f;
        else if(fxMultiplier > 20) fxMultiplier = 20.0f;
        HOOKBL(LoadFX_atof, pGTAVC + 0x2D4034);
    }

    // Skip that dumb EULA. We accepted it years ago, shut up
    if(cfg->GetBool("SkipAnnoyingEULA", true, "Gameplay"))
    {
        aml->Write8(aml->GetSym(hGTAVC, "shownLegalScreen"), 0x01);
    }

    // A style of a SA mobile
    if(cfg->GetBool("MoneysWithoutZeros", false, "Visual"))
    {
        aml->Write(pGTAVC + 0x4C599E, "$%d", 4);
    }

    // OpenGL-related crash in huawei (poop ass phone moment)
    if(cfg->GetBool("FixHuaweiCrash", true, "Gameplay"))
    {
        void* sym;
        if((sym = TryLoadAlphaFunc("libGLESv2.so"))       || (sym = TryLoadAlphaFunc("libGLESv3.so"))    ||
           (sym = TryLoadAlphaFunc("libGLESv2_mtk.so"))   || (sym = TryLoadAlphaFunc("libGLES_mali.so")) ||
           (sym = TryLoadAlphaFunc("libGLES_android.so")) || (sym = TryLoadAlphaFunc("libGLES.so")))
        {
            *(void**)(pGTAVC + 0x9AEA70) = sym;
        }
    }

    // 44100 Hz Audio support (without a mod OpenAL Update but works with it anyway)
    if(cfg->GetBool("Allow44100HzAudio", true, "Gameplay"))
    {
        aml->Unprot(pGTAVC + 0x4AED68, sizeof(int));
        *(int*)(pGTAVC + 0x4AED68) = 44100;
    }

    // Fix threads performance
    if(cfg->GetBool("FixFPS", true, "Gameplay"))
    {
        aml->Write32(pGTAVC + 0x460330, 0x52801E40);
        aml->Write32(pGTAVC + 0x460384, 0x52801E20);
    }

    // Fixing drunk camera on high FPS
    if(cfg->GetBool("FixDrunkCameraHighFPS", true, "Visual"))
    {
        HOOKPLT(CameraProcess_HighFPS, pGTAVC + 0x5705C0);
    }