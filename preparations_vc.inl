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

    // Fix pushing force
    //if(cfg->GetBool("FixPhysicalPushForce", true, "Gameplay"))
    //{
    //    HOOK(ApplyCollision_HighFPS, aml->GetSym(hGTAVC, "_ZN9CPhysical14ApplyCollisionEPS_R9CColPointRfS3_"));
    //    //ApplyCollision_BackTo = pGTAVC + 0x167588 + 0x1;
    //    //aml->Redirect(pGTAVC + 0x167580 + 0x1, (uintptr_t)ApplyCollision_Inject);
    //    //HOOKBL(ApplyCollision_MoveForce, pGTAVC + 0x166C60 + 0x1);
    //    //HOOKBL(ApplyCollision_TurnForce, pGTAVC + 0x166C84 + 0x1);
    //}
    
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
        HOOKBL(GameTick_RsEventHandler, pGTAVC + 0x21E7C8 + 0x1);
    }

    // Fix water scroll speed on high FPS
    if(cfg->GetBool("FixWaterUVScrollSpeed", true, "Visual"))
    {
        RenderWater_BackTo = pGTAVC + 0x2645D2 + 0x1;
        aml->Redirect(pGTAVC + 0x2645C6 + 0x1, (uintptr_t)RenderWater_Inject);
    }

    // Bringing back missing render states (WarDumb`s optimization) (everything goes black)
    if(cfg->GetBool("FixRenderStates", true, "Visual"))
    {
        HOOK(RwRenderState_Patch, aml->GetSym(hGTAVC, "_Z16RwRenderStateSet13RwRenderStatePv"));
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






    
    
    // An improved ForceDXT (everything goes black, no textures! :( )
    /*if(cfg->GetBool("ForceLoadDXT", false, "Gameplay"))
    {
        aml->Write8(pGTAVC + 0x2A8F70, 0x01);
        aml->Write8(pGTAVC + 0x2A8EBE, 0x01);
        HOOKBL(LoadEntries_DXT, pGTAVC + 0x2A8EC8 + 0x1);
    }*/