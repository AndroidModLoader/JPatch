    // Boat and Skimmer on high FPS
    if(cfg->GetBool("HighFPSBoatSpeed", true, "Gameplay"))
    {
        Boat_ApplyWaterResistance_BackTo = pGTAVC + 0x247650 + 0x1;
        aml->Redirect(pGTAVC + 0x247648 + 0x1, (uintptr_t)Boat_ApplyWaterResistance_Inject);
    }
    
    // Fix wheels rotation speed on high FPS
    if(cfg->GetBool("FixWheelsRotationSpeed", true, "Visual"))
    {
        aml->Redirect(pGTAVC + 0x2584EC + 0x1, (uintptr_t)ProcessWheelRotation_Inject);
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
        HOOKBL(HeliRender_MatrixUpdate, pGTAVC + 0x254F76 + 0x1);
    }
    
    // Fixes a streaming distance that gets bugged because of dumb R* logic (aspect ratio moment)
    if(cfg->GetBool("FixStreamingDistance", true, "Visual"))
    {
        CameraProcess_StreamDist_BackTo = pGTAVC + 0x13FB34 + 0x1;
        aml->Redirect(pGTAVC + 0x13FB1A + 0x1, (uintptr_t)CameraProcess_StreamDist_Inject);
        HOOK(SetFOV_StreamingDistFix, aml->GetSym(hGTAVC, "_ZN5CDraw6SetFOVEf"));

        aml->PlaceNOP(pGTAVC + 0x13FB0C + 0x1, 1);
        aml->Write(pGTAVC + 0x13FB0E, "\x9F\xED\xBB\x7A", 4);
        aml->Write(pGTAVC + 0x13FB12, "\xD7\xED\x0E\x6A", 4);
        aml->Write(pGTAVC + 0x13FB16, "\xF4\xEE\xC7\x6A", 4);
    }
    
    // Fixing fat ass coronas
    if(cfg->GetBool("FixCoronasAspectRatio", true, "Visual"))
    {
        HOOK(CoronaSprite_CalcScreenCoors, aml->GetSym(hGTAVC, "_ZN7CSprite15CalcScreenCoorsERK5RwV3dPS0_PfS4_b"));
    }

    // Fix Vertex Weights
    if(cfg->GetBool("FixVertexWeight", true, "Visual"))
    {
        VertexWeightFix_BackTo1 = pGTAVC + 0x28EA7A + 0x1;
        VertexWeightFix_BackTo2 = pGTAVC + 0x28EAA4 + 0x1;
        aml->Redirect(pGTAVC + 0x28E970 + 0x1, (uintptr_t)VertexWeightFix_Inject);
    }

    // RE3: Fix R* optimization that prevents peds to spawn
    if(cfg->GetBool("Re3_PedSpawnDeoptimize", true, "Gameplay"))
    {
        aml->Write(pGTAVC + 0x14CA68, (uintptr_t)"\x02", 1);
    }

    // RE3: Road reflections
    if(cfg->GetBool("Re3_WetRoadsReflections", true, "Visual"))
    {
        RoadReflections_BackTo = pGTAVC + 0x1D6528 + 0x1;
        aml->Redirect(pGTAVC + 0x1D6520 + 0x1, (uintptr_t)RoadReflections_Inject);
        aml->PlaceNOP(pGTAVC + 0x1D6552 + 0x1, 1);
    }

    // Fix traffic lights
    if(cfg->GetBool("FixTrafficLights", true, "Visual"))
    {
        HOOKBL(TrFix_RenderEffects, pGTAVC + 0x202F98 + 0x1);

        // Why does vehicle's lights are in that list?!
        aml->PlaceNOP4(pGTAVC + 0x231138 + 0x1, 1);
        aml->PlaceNOP4(pGTAVC + 0x231198 + 0x1, 1);
        aml->PlaceNOP4(pGTAVC + 0x2334C4 + 0x1, 1);
        aml->PlaceNOP4(pGTAVC + 0x233C16 + 0x1, 1);
        aml->PlaceNOP4(pGTAVC + 0x233CDE + 0x1, 1);
        aml->PlaceNOP4(pGTAVC + 0x235574 + 0x1, 1);
        aml->PlaceNOP4(pGTAVC + 0x23579E + 0x1, 1);

        // Why does bike's lights are in that list?!
        aml->PlaceNOP4(pGTAVC + 0x240E98 + 0x1, 1);
        aml->PlaceNOP4(pGTAVC + 0x240FD2 + 0x1, 1);
        aml->PlaceNOP4(pGTAVC + 0x241052 + 0x1, 1);
        aml->PlaceNOP4(pGTAVC + 0x2410C8 + 0x1, 1);
        aml->PlaceNOP4(pGTAVC + 0x2414B8 + 0x1, 1);
    }

    // Fixes framelimiter (doesnt limit the FPS, ayo lol)
    // P.S. IT WAS NEVER WORKING, OH MY GOD
    if(cfg->GetBool("FixGameTimer", true, "Gameplay"))
    {
        aml->PlaceB(pGTAVC + 0x21E7C4 + 0x1, pGTAVC + 0x21E7CC + 0x1);
        HOOKBL(GameTick_TouchscreenUpdate, pGTAVC + 0x21E7D6 + 0x1);
        HOOK(OS_ScreenSetRefresh, aml->GetSym(hGTAVC, "_Z19OS_ScreenSetRefreshj"));

        aml->Write8(pGTAVC + 0x21E994, 0x1E);
    }

    // Fix water scroll speed on high FPS
    if(cfg->GetBool("FixWaterUVScrollSpeed", true, "Visual"))
    {
        RenderWater_BackTo = pGTAVC + 0x2645D2 + 0x1;
        aml->Redirect(pGTAVC + 0x2645C6 + 0x1, (uintptr_t)RenderWater_Inject);
    }

    // Bringing back missing render states (WarDumb`s optimization)
    if(cfg->GetBool("FixRenderStates", true, "Visual"))
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
    }

    // Fixes a mistake by Rockstar (not WarDrum), a wrong physical target
    if(cfg->GetBool("FixPhysicsTargets", true, "Gameplay"))
    {
        aml->Write16(pGTAVC + 0x16777E, 0x4628);
        aml->Write16(pGTAVC + 0x1677A2, 0x4628);
        aml->Write8(pGTAVC + 0x16AC3C, 0x94);
    }

    // Fix clouds rotating speed
    if(cfg->GetBool("FixCloudsRotateSpeed", true, "Visual"))
    {
        HOOKBL(CloudsUpdate_Speedo, pGTAVC + 0x14CA8E + 0x1);
    }

    // The explosion "shadow" is missing
    if(cfg->GetBool("FixExplosionCraterTexture", true, "Visual"))
    {
        HOOKBL(AddExplosion_AddShadow, pGTAVC + 0x2656B4 + 0x1);
    }

    // Allows the game to choose Extra6 component on a vehicle when created
    if(cfg->GetBool("AllowExtra6Part", true, "Visual"))
    {
        aml->Write8(pGTAVC + 0x20E3A4 + 2, 0x08);
    }

    // Bigger distance for light coronas
    if(cfg->GetBool("BuffDistForLightCoronas", true, "Visual"))
    {
        aml->WriteFloat(pGTAVC + 0x1D7450, 400.0f); // CEntity::ProcessLightsForEntity, 120 -> 400
        aml->Write(pGTAVC + 0x127EA2, "\xC4\xF2\xC8\x33", 4); // CTrafficLights::DisplayActualLight, 50 -> 400
        aml->Write(pGTAVC + 0x127A0A, "\xC4\xF2\xC8\x33", 4); // CTrafficLights::DisplayActualLight, 50 -> 400
        aml->Write(pGTAVC + 0x127BE8, "\xC4\xF2\xC8\x33", 4); // CTrafficLights::DisplayActualLight, 50 -> 400
        aml->Write(pGTAVC + 0x1280B4, "\xC4\xF2\xC8\x33", 4); // CTrafficLights::DisplayActualLight, 50 -> 400

        aml->Write(pGTAVC + 0x1D7842, "\x4F\xF0\x00\x0E", 4); // CEntity::ProcessLightsForEntity, Force off useNearDist
        aml->Write(pGTAVC + 0x1D74A6, "\x4F\xF0\x00\x0E", 4); // CEntity::ProcessLightsForEntity, Force off useNearDist
    }

    // Bigger distance for light shadows
    if(cfg->GetBool("BuffDistForLightShadows", true, "Visual"))
    {
        aml->WriteFloat(pGTAVC + 0x1D744C, 120.0f); // CEntity::ProcessLightsForEntity, 40 -> 120
        aml->Write(pGTAVC + 0x14BAD4, "\xC4\xF2\xF0\x2E", 4); // CFire::ProcessFire, 40 -> 120
        aml->Write(pGTAVC + 0x128440, "\xC4\xF2\xF0\x2E", 4); // CTrafficLights::DisplayActualLight, 40 -> 120
        aml->Write(pGTAVC + 0x127FB0, "\xC4\xF2\xF0\x2A", 4); // CTrafficLights::DisplayActualLight, 40 -> 120
        aml->Write(pGTAVC + 0x128150, "\xC4\xF2\xF0\x25", 4); // CTrafficLights::DisplayActualLight, 40 -> 120
        aml->WriteFloat(pGTAVC + 0x1FC780, 120.0f); // CShadows::StoreShadowForPole, 40 -> 120

        // We do count vehicle shadows as light shadows?
        // Yeah, why not!
        HOOKBL(StoreShadowForVehicle, pGTAVC + 0x1FC2F4 + 0x1);
        // still need patch at 1FC420 and 1FC424
        aml->WriteFloat(pGTAVC + 0x1FC5EC, 120.0f * 120.0f); // CShadows::StoreCarLightShadow, 27 -> 120
        aml->WriteFloat(pGTAVC + 0x1FC5E4, 120.0f * 0.75f); // CShadows::StoreCarLightShadow, 27 -> 120
        aml->Write(pGTAVC + 0x1FC5AA, "\xC4\xF2\xF0\x24", 4); // CShadows::StoreCarLightShadow, 27 -> 120
        aml->PlaceB(pGTAVC + 0x1FBEE8 + 0x1, pGTAVC + 0x1FC09A + 0x1);
        
        DoCollectableEffects_BackTo = pGTAVC + 0xFF058 + 0x1;
        aml->Redirect(pGTAVC + 0xFF04E + 0x1, (uintptr_t)DoCollectableEffects_Inject);
        DoPickUpEffects_BackTo = pGTAVC + 0xFFF8A + 0x1;
        aml->Redirect(pGTAVC + 0xFFF7E + 0x1, (uintptr_t)DoPickUpEffects_Inject);
    }
    
    // Just a little fix for banner
    if(cfg->GetBool("CorrectBannerRenderFlag", true, "Visual"))
    {
        aml->Write16(pGTAVC + 0x1DF904, 0x2319);
    }

    // Fixing a weird error with access error in Unprotected version of VC apk
    if(cfg->GetBool("FixUnprotectedAccessCrash", true, "Gameplay"))
    {
        UNPROT(pGTAVC + 0x3D20B4, 1);
    }

    // Allows the game to render even more light shadows on the ground
    if(cfg->GetBool("BuffStaticShadowsCount", true, "Gameplay"))
    {
        aStaticShadows_NEW = new CStaticShadow[0xFF + 1] {0}; memset(aStaticShadows_NEW, 0, sizeof(CStaticShadow) * (0xFF + 1));
        aml->Write(pGTAVC + 0x394750, (uintptr_t)&aStaticShadows_NEW, sizeof(void*));

        // Static Shadows:
        // CShadows::StoreStaticShadow
        aml->Write(pGTAVC + 0x1FBBDE, (uintptr_t)"\xFE", 1);
        aml->Write(pGTAVC + 0x1FBC8C, (uintptr_t)"\xFF", 1);
        aml->Write(pGTAVC + 0x1FBCA0, (uintptr_t)"\xFF", 1);
        aml->Write(pGTAVC + 0x1FBBB8, (uintptr_t)"\x09\xF5\x1B\x5E", 4); // ADD.W LR, R9, #0x26C0
        // CShadows::RenderStaticShadows
        aml->Write(pGTAVC + 0x1FCB00, (uintptr_t)"\xFF", 1);
        aml->Write(pGTAVC + 0x1FCB2C, (uintptr_t)"\xFE", 1);
        aml->Write(pGTAVC + 0x1FCB54, (uintptr_t)"\xFF", 1);
        aml->Write(pGTAVC + 0x1FCEBA, (uintptr_t)"\xFF", 1);
        // CShadows::UpdateStaticShadows
        aml->Write(pGTAVC + 0x1F92D6, (uintptr_t)"\x04\xF5\x1B\x55", 4); // ADD.W R5, R4, #0x26C0

        HOOKBL(InitShadows, pGTAVC + 0x14C56A + 0x1);
    }

    // Bigger max count of peds
    if(cfg->GetBool("BuffMaxPedsCount", true, "Gameplay"))
    {
        *(int*)aml->GetSym(hGTAVC, "_ZN11CPopulation20MaxNumberOfPedsInUseE") = 0x23;
        //aml->Write(pGTASA + 0x3F4DE0, (uintptr_t)"\x23", 1);
        //aml->Write(pGTASA + 0x4CC284, (uintptr_t)"\x23", 1);
        //aml->Write(pGTASA + 0x4CCBE0, (uintptr_t)"\x23", 1);
        //aml->Write(pGTASA + 0x4CCBEA, (uintptr_t)"\x1C", 1);
    }

    // Bigger spawn distance for peds
    if(cfg->GetBool("BuffPedsDistance", true, "Gameplay"))
    {
        aml->WriteFloat(pGTAVC + 0x1D413C, 50.0f);
        aml->WriteFloat(pGTAVC + 0x1D4140, 90.0f);
        HOOKBL(PedCreationDistMult_Offscreen, pGTAVC + 0x1D40EE + 0x1);
        HOOKBL(PedCreationDistMult_Offscreen, pGTAVC + 0x1D40F6 + 0x1);
    }

    // An attempt to fix fighting... (incomplete, the radius is small tho)
    if(cfg->GetBool("FixFighting", true, "Gameplay"))
    {
        aml->Write(pGTAVC + 0x1C2E3C, "\xB1\xEE\x00\x7A", 4); // CPed::Fight -> unlock move FIGHTMOVE_KNEE
        aml->Write8(pGTAVC + 0x1C5520, 0x01); // CheckForPedsOnGroundToAttack -> 4 >> PED_IN_FRONT_OF_ATTACKER
    }

    // FX particles distance multiplier!
    fxMultiplier = cfg->GetFloat("FxDistanceMult", 2.5f, "Visual");
    if(fxMultiplier != 1)
    {
        if(fxMultiplier < 0.1) fxMultiplier = 0.1f;
        else if(fxMultiplier > 20) fxMultiplier = 20.0f;
        HOOKBLX(LoadFX_atof, pGTAVC + 0x1F6500);
    }












    // Do camera tweaks, get a better camera!
    //if(cfg->GetBool("CameraTweaks", true, "Gameplay"))
    //{
    //    // Need to check WellBufferMe
    //}

    // Fix pushing force
    //if(cfg->GetBool("FixPhysicalPushForce", true, "Gameplay"))
    //{
    //    HOOK(ApplyCollision_HighFPS, aml->GetSym(hGTAVC, "_ZN9CPhysical14ApplyCollisionEPS_R9CColPointRfS3_"));
    //    //ApplyCollision_BackTo = pGTAVC + 0x167588 + 0x1;
    //    //aml->Redirect(pGTAVC + 0x167580 + 0x1, (uintptr_t)ApplyCollision_Inject);
    //    //HOOKBL(ApplyCollision_MoveForce, pGTAVC + 0x166C60 + 0x1);
    //    //HOOKBL(ApplyCollision_TurnForce, pGTAVC + 0x166C84 + 0x1);
    //}

    // An improved ForceDXT (everything goes black, no textures! :( )
    /*if(cfg->GetBool("ForceLoadDXT", false, "Gameplay"))
    {
        aml->Write8(pGTAVC + 0x2A8F70, 0x01);
        aml->Write8(pGTAVC + 0x2A8EBE, 0x01);
        HOOKBL(LoadEntries_DXT, pGTAVC + 0x2A8EC8 + 0x1);
    }*/