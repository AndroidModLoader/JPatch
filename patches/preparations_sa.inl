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
        //aml->Write(pGTASA + 0x59128E, (uintptr_t)"\x02", 1);
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

    // Fixes emergency vehicles
    if(cfg->GetBool("FixStreamingDistScale", true, "Gameplay"))
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
    if(bDynStreamingMem)
    {
        fDynamicStreamingMemPercentage = 0.01f * cfg->GetInt("DynamicStreamingMem_Percentage", 80, "Gameplay");
        if(fDynamicStreamingMemPercentage < 0.01f || fDynamicStreamingMemPercentage > 0.99f) bDynStreamingMem = false;
        else
        {
            int valllue = cfg->GetInt("DynamicStreamingMem_MaxMBs", 1024, "Gameplay");
            if(valllue < 32) valllue = 32;
            else if(valllue > 2048) valllue = 2048;

            nMaxStreamingMemForDynamic = 1024 * 1024 * valllue;

            valllue = cfg->GetInt("DynamicStreamingMem_BumpStep", 8, "Gameplay");
            if(valllue < 2) valllue = 2;
            else if(valllue > 64) valllue = 64;
            nDynamicStreamingMemBumpStep = valllue;
        }
    }

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
    // TODO: WHY PREDATOR CANT GET IT?!
    if(cfg->GetBool("ReturnShootBtnForVehicles", true, "Gameplay"))
    {
        GetCarGunFired1_BackTo1 = pGTASA + 0x3F998C + 0x1;
        GetCarGunFired1_BackTo2 = pGTASA + 0x3F9948 + 0x1;
        aml->Redirect(pGTASA + 0x3F9914 + 0x1, (uintptr_t)GetCarGunFired_Inject1);

        GetCarGunFired2_BackTo1 = pGTASA + 0x3F99E8 + 0x1; // continue
        GetCarGunFired2_BackTo2 = pGTASA + 0x3F9908 + 0x1; // return
        aml->Redirect(pGTASA + 0x3F99C4 + 0x1, (uintptr_t)GetCarGunFired_Inject2);

        aml->Write8(pGTASA + 0x5E2940, 0x01); // CPad::GetCarGunFired(Pad, >1<, 0); // BengbuGuards

        //aml->PlaceB(pGTASA + 0x3F99B0 + 0x1, pGTASA + 0x3F99E8 + 0x1); // NO DIFFERENCE!
    }

    // Just a fuzzy seek. Tell MPG123 to not load useless data.
    if(cfg->GetBool("FuzzySeek", true, "Gameplay"))
    {
        HOOKPLT(mpg123_param, pGTASA + 0x66F3D4);
    }

    // Fix water cannon on a high fps
    if(cfg->GetBool("FixHighFPSWaterCannons", true, "Gameplay"))
    {
        HOOKPLT(WaterCannonsRender, pGTASA + 0x6715F0);
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
        HOOKPLT(ProcessVehicleWheel, pGTASA + 0x66FC7C);
        HOOKPLT(ProcessBikeWheel, pGTASA + 0x673F10);
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
        aml->Write(pGTASA + 0x3F40C0, (uintptr_t)"\x02", 1);
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
        //RoadReflections_BackTo = pGTASA + 0x5A2EA4 + 0x1;
        //aml->Redirect(pGTASA + 0x5A2E94 + 0x1, (uintptr_t)RoadReflections_Inject);
        aml->Write(pGTASA + 0x5A2E9C, "\x2F\x98\x00\xBF", 4);
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
        HOOKPLT(AddLight_LightPoles, pGTASA + 0x671A10);
    }

    // Fix greenish detail tex
    if(cfg->GetBool("FixGreenTextures", true, "Visual"))
    {
        aml->PlaceNOP(pGTASA + 0x1B00B0, 5); // Dont set textureDetail variable! We'll handle it by ourselves!
        HOOK(emu_TextureSetDetailTexture, aml->GetSym(hGTASA, "_Z27emu_TextureSetDetailTexturePvj"));
    }

    // Allows the game to render even more light shadows on the ground
    if(cfg->GetBool("BuffStaticShadowsCount", true, "Gameplay"))
    {
        // Static shadows?
        asShadowsStored_NEW = new CRegisteredShadow[0xFF + 1]; memset(asShadowsStored_NEW, 0, sizeof(CRegisteredShadow) * (0xFF + 1));
        aStaticShadows_NEW = new CStaticShadow[0xFF + 1] {0}; memset(aStaticShadows_NEW, 0, sizeof(CStaticShadow) * (0xFF + 1));
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
        float fH = cfg->GetFloat("MoveShadowsToTheGround_NewHeight", 0.03f, "Visual");
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
        static uint32_t m_aNewCheatHashKeys[0x6F] =
        { 
            0xDE4B237D, 0xB22A28D1, 0x5A783FAE, 0xBC39F5A2, // WEAPON4_CHEAT
            0x87237A8E, 0xBB5ADFD7, 0x37B458AA, 0xE1614CCB, // INVINCIBILITY_CHEAT
            0x438F0792, 0x1983B67D, 0xEECCEA2B, 0x42AF1E28, // WANTEDLEVELUP_CHEAT
            0x555FC201, 0x2A845345, 0xE1EF01EA, 0x771B83FC, // CLOUDYWEATHER_CHEAT
            0x5BF12848, 0x44453A17, 0xFCFF1D08, 0xB69E8532, // FASTTIME_CHEAT
            0x8B828076, 0xDD6ED9E9, 0xA290FD8C, 0x3484B5A7, // WEAPONSFORALL_CHEAT
            0x43DB914E, 0xDBC0DD65, 0x7CBC2154, 0xD08A30FE, // STOCKCAR3_CHEAT
            0x37BF1B4E, 0xB5D40866, 0xE63B0D99, 0x675B8945, // TRASHMASTER_CHEAT
            0x4987D5EE, 0x2E8F84E8, 0x1A9AA3D6, 0xE842F3BC, // STRONGGRIP_CHEAT
            0x0D5C6A4E, 0x74D4FCB1, 0xB01D13B8, 0x66516EBC, // PINKCARS_CHEAT
            0x4B137E45, 0x6652908F, 0x78520E33, 0x3A577325, // FAT_CHEAT
            0xD4966D59, 0x6A562342, 0x5FD1B49D, 0xA7613F99, // ELVISLIVES_CHEAT
            0x1792D871, 0xCBC579DF, 0x4FEDCCFF, 0x44B34866, // GANGLAND_CHEAT
            0x2EF877DB, 0x2781E797, 0x2BC1A045, 0xB2AFE368, // ALLCARSAREGREAT_CHEAT
            0xFA8DD45B, 0x8DED75BD, 0x1A5526BC, 0xA48A770B, // VORTEX_CHEAT
            0xB07D3B32, 0x80C1E54B, 0x5DAD0087, 0x7F80B950, // MIDNIGHT_CHEAT
            0x6C0FA650, 0xF46F2FA4, 0x70164385, 0xF51F7ED1, // PREDATOR_CHEAT
            0x885D0B50, 0x151BDCB3, 0xADFA640A, 0xE57F96CE, // PARACHUTE_CHEAT
            0x040CF761, 0xE1B33EB9, 0xFEDA77F7, 0x8CA870DD, // SUPERPUNCH_CHEAT
            0x9A629401, 0xF53EF5A5, 0xF2AA0C1D, 0xF36345A8, // ADRENALINE_CHEAT
            0x8990D5E1, 0xB7013B1B, 0xCAEC94EE, 0x31F0C3CC, // COUNTRYSIDEINVASION_CHEAT
            0xB3B3E72A, 0xC25CDBFF, 0xD5CF4EFF, 0x680416B1, // RESPECT_CHEAT
            0xCF5FDA18, 0xF01286E9, 0xA841CC0A, 0x31EA09CF, // VEHICLESKILLS_CHEAT
            0xE958788A, 0x02C83A7C, 0xE49C3ED4, 0x171BA8CC, // DOZER_CHEAT
            0x86988DAE, 0x2BDD2FA1, 0x57E1CA95, 0xF689F99F, // TAXINITRO_CHEAT
            0x00000000, 0x00000000, 0x00000000, 0x00000000, // SLOT_SHOTGUN
            0x00000000, 0x00000000, 0x00000000, 0x00000000, // SLOT_HEAVY_ARTILLERY
            0x00000000, 0x00000000, 0x068AFAD9,             // XBOX_HELPER
        };

        HOOKPLT(DoCheats, pGTASA + 0x675458);
        HOOKPLT(KBEvent, pGTASA + 0x6709B8);
        aml->Write(aml->GetSym(hGTASA, "_ZN6CCheat16m_aCheatHashKeysE"), (uintptr_t)m_aNewCheatHashKeys, sizeof(m_aNewCheatHashKeys));
        aml->WriteAddr(pGTASA + 0x68627C, (uintptr_t)Cheat_Predator); // crash?
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
    int dist = cfg->GetInt("DetailedWaterDrawDistance", 48 * 3, "Visual");
    if(dist > 0)
    {
        if(dist < 24) dist = 24;
        else if(dist <= 48 * 3) dist = 48 * 3;
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
        aml->Write8(aml->GetSym(hGTASA, "UseCloudSaves"), 0x00);
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
        aml->PlaceB(pGTASA + 0x3A152A + 0x1, pGTASA + 0x3A1602 + 0x1); // QueueUpTracksForStation
        aml->PlaceB(pGTASA + 0x3A35F6 + 0x1, pGTASA + 0x3A369A + 0x1); // ChooseMusicTrackIndex
        aml->PlaceB(pGTASA + 0x3A37C2 + 0x1, pGTASA + 0x3A385E + 0x1); // ChooseIdentIndex
        aml->PlaceB(pGTASA + 0x3A3A1E + 0x1, pGTASA + 0x3A3AA2 + 0x1); // ChooseAdvertIndex
        aml->PlaceB(pGTASA + 0x3A4374 + 0x1, pGTASA + 0x3A4416 + 0x1); // ChooseTalkRadioShow
        aml->PlaceB(pGTASA + 0x3A44D6 + 0x1, pGTASA + 0x3A4562 + 0x1); // ChooseDJBanterIndexFromList
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
        aml->PlaceB(pGTASA + 0x5D991E + 0x1, pGTASA + 0x5D9A04 + 0x1);
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
        *(float*)(pGTASA + 0x4DD9E8) = 0.015f;
        //SET_TO(float_4DD9E8, pGTASA + 0x4DD9E8);
        //HOOK(TaskSimpleUseGunSetMoveAnim, aml->GetSym(hGTASA, "_ZN17CTaskSimpleUseGun11SetMoveAnimEP4CPed"));
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
        aml->Write(pGTASA + 0x54316C, "\xB4", 1);
    }

    // Show muzzle flash for the last bullet in magazine
    if(cfg->GetBool("MuzzleFlashForLastBullet", true, "Visual"))
    {
        aml->PlaceNOP(pGTASA + 0x4DDCCA, 10);
    }

    // Renders shadows on all surfaces -> disables a flag
    if(cfg->GetBool("DrawShadowsOnAllSurfaces", true, "Visual"))
    {
        aml->Write(pGTASA + 0x5BAE23, "\x00", 1);
    }

    // Spread fix
    if(cfg->GetBool("WeaponSpreadFix", true, "Gameplay"))
    {
        HOOK(FireInstantHit, aml->GetSym(hGTASA, "_ZN7CWeapon14FireInstantHitEP7CEntityP7CVectorS3_S1_S3_S3_bb"));
    }

    // Fixes farclip glitch with wall (wardumb be like)
    if(cfg->GetBool("FixFogWall", true, "Visual"))
    {
        aml->Write(pGTASA + 0x5EB9D9, "\x31\x2E\x30\x30", 4);
        HOOKBLX(DistanceFogSetup_FogWall, pGTASA + 0x1AF4AE);
    }

    // Fixes Corona sprites stretching at foggy weather
    if(cfg->GetBool("FixCoronasStretching", true, "Visual"))
    {
        aml->Write(pGTASA + 0x5A27EC, "\xB0\xEE\x44\x0A", 4);
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

    // Unused detonator animation is in the ped.ifp, lol
    if(cfg->GetBool("UnusedDetonatorAnimation", true, "Visual"))
    {
        HOOKPLT(UseDetonator, pGTASA + 0x66FD94);
    }

    // FX particles distance multiplier!
    fxMultiplier = cfg->GetFloat("FxDistanceMult", 2.5f, "Visual");
    if(fxMultiplier != 1)
    {
        if(fxMultiplier < 0.1) fxMultiplier = 0.1f;
        else if(fxMultiplier > 20) fxMultiplier = 20.0f;
        HOOKBLX(LoadFX_sscanf, pGTASA + 0x46DC28);
        HOOKBLX(LoadFX_sscanf, pGTASA + 0x46E00C);
        HOOKBLX(LoadFX_sscanf, pGTASA + 0x46E03A);
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
        //HOOKBLX(ProcessHierarchy_BoatRadar, pGTASA + 0x387246 + 0x1);
        aml->Write8(pGTASA + 0x6107D9, 0x00); // Animation name: boat_moving_hi -> boat_moving
    }

    // Fix airbubbles from the jaw (CJ is breathing with his ass, lololololol)
    if(cfg->GetBool("AirBubblesFromJaw", true, "Visual"))
    {
        aml->Write(pGTASA + 0x53C4A0, "\xD0\xED\x8C\x0B\xD0\xF8\x38\x02", 8);
    }

    // EXPERIMENTAL: Removes entity's nowritez logic at rendering stage
    //if(cfg->GetBool("NoModelFlagNoWriteZ", true, "Visual"))
    //{
    //    aml->PlaceNOP4(pGTASA + 0x5D6874 + 0x1, 1);
    //}

    // SilentPatch fix
    if(cfg->GetBool("DirectionalSunLight", true, "Visual"))
    {
        HOOKPLT(SetLightsWithTimeOfDayColour_DirLight, pGTASA + 0x674048);
    }

    // PS2 thingo
    if(cfg->GetBool("PS2CoronaRotation", true, "Visual"))
    {
        HOOKBLX(RenderOneXLUSprite_Rotate_Aspect_PS2, pGTASA + 0x5A2816 + 0x1);
        //HOOKPLT(RenderOneXLUSprite_Rotate_Aspect_PS2, pGTASA + 0x675988);
    }

    // Random license plates
    if(cfg->GetBool("RandomLicensePlates", true, "Visual"))
    {
        HOOKBLX(GetCustomCarPlateText_SetModelIdx, pGTASA + 0x5829F2 + 0x1);
    }

    // Car generators in an interior now work properly
    if(cfg->GetBool("InteriorCarGenerators", true, "Gameplay"))
    {
        HOOKBLX(CarGen_WorldAdd1, pGTASA + 0x56DA9E + 0x1);
        HOOKBLX(CarGen_WorldAdd2, pGTASA + 0x56DA76 + 0x1);
    }

    // Fix "You have worked out enough for today, come back tomorrow!"
    if(cfg->GetBool("FixGymWorkoutDate", true, "SCMFixes"))
    {
        Opcode0835_BackTo = pGTASA + 0x3378CE + 0x1;
        aml->Redirect(pGTASA + 0x3378AE + 0x1, (uintptr_t)Opcode0835_Inject);
    }

    // Car generators in an interior now work properly
    if(cfg->GetBool("CorrectPoliceScannerLocations", true, "Gameplay"))
    {
        aml->Write(pGTASA + 0x614258, "WESTP", 6);
        aml->Write(pGTASA + 0x614300, "????", 5);
    }

    // Fixing a wrong value in carcols.dat
    if(cfg->GetBool("CarColsDat_FixWrongValue", true, "Visual"))
    {
        HOOKBLX(CarColsDatLoad_sscanf, pGTASA + 0x46C55E + 0x1);
    }

    // Inverse swimming controls to dive/go up (to match PC version)
    if(cfg->GetBool("InverseSwimmingDivingControl", true, "Gameplay"))
    {
        HOOKPLT(TaskSwim_ProcessInput, pGTASA + 0x674CBC);
        HOOKBLX(GetPedWalkUpDown_Swimming, pGTASA + 0x53A928 + 0x1);
        //aml->WriteFloat(pGTASA + 0x53B31C, -0.08f); // not what we want!
        aml->WriteFloat(pGTASA + 0x53B338,  0.08f);
    }

    // Fixes the wrong position (from the beta) for a Bravura in a mission "You've Had Your Chips"
    if(cfg->GetBool("FourDragonsBravuraCoordFix", true, "SCMFixes"))
    {
        HOOKBLX(CreateCarForScript_Bravura_00A5, pGTASA + 0x32CBDC + 0x1);
        HOOKBLX(CollectParameters_Bravura_0175, pGTASA + 0x342474 + 0x1);
    }

    // Michelle dating fix
    if(cfg->GetBool("MichelleDatingFix", true, "SCMFixes"))
    {
        Opcode039E_BackTo = pGTASA + 0x349E7E + 0x1;
        aml->Redirect(pGTASA + 0x348CCE + 0x1, (uintptr_t)Opcode039E_Inject);
        //aml->Redirect(pGTASA + 0x348CA6 + 0x1, pGTASA + 0x349E7E + 0x1); // disable opcode completely
    }

    // HudColors
    if(cfg->GetBool("PCHudColors", true, "Visual"))
    {
        // A function CWidgetVitalStats::Draw (0x2C84EC) has something that brings back normal colors!!!
        // Temporary fix. I found an interesting connection between real colors and those:

        // Color rendering is broken, wadahel !!!
        aml->Write8(pGTASA + 0x2BDE14, HUD_COLOUR_LIGHT_GRAY); // Ammo
        aml->Write8(pGTASA + 0x2BD102, HUD_COLOUR_WHITE); // Clock
        aml->Write8(pGTASA + 0x2BD228, HUD_COLOUR_DARK_GREEN); // Money
        aml->Write8(pGTASA + 0x2BD232, HUD_COLOUR_DARK_RED); // Money (negative)
        aml->Write8(pGTASA + 0x2BD6C8, HUD_COLOUR_DARK_RED); // Health bar
        aml->Write8(pGTASA + 0x2BD876, HUD_COLOUR_WHITE); // Armor bar
        aml->Write8(pGTASA + 0x2BD9C4, (uint8_t)(magicColorVal * 172)); // Oxygen bar (red channel)
        aml->Write8(pGTASA + 0x2BD9CA, (uint8_t)(magicColorVal * 203)); // Oxygen bar (green channel)
        aml->Write8(pGTASA + 0x2BD9D0, (uint8_t)(magicColorVal * 241)); // Oxygen bar (blue channel)
        HOOKBLX(DrawAmmo_PrintString, pGTASA + 0x2BDEFA + 0x1); // check this fn for bit more info
    }

    // Fix the issue that player cannot kill with a knife if not crouching
    if(cfg->GetBool("FixUncrouchedStealthKill", true, "Gameplay"))
    {
        aml->PlaceB(pGTASA + 0x537988 + 0x1, pGTASA + 0x538BBE + 0x1);
    }
    
    // Fix a dumb Android 10+ RLEDecompress fix crash
    if(cfg->GetBool("RLEDecompressCrashFix", androidSdkVer < 33, "Gameplay"))
    {
        RLE_BackTo = pGTASA + 0x1E9244 + 0x1;
        aml->Redirect(pGTASA + 0x1E9238 + 0x1, (uintptr_t)RLE_Inject);
    }

    // Fix plane disappear after explode. It's still there, but is invisible. Does it have any meaning, Rockstar?
    if(cfg->GetBool("PlaneExplodeDisappearFix", true, "Visual"))
    {
        aml->Write(pGTASA + 0x579ECC, "\x4C\xE0", 2); // B  loc_579F68
        aml->PlaceNOP(pGTASA + 0x579F7C + 0x1, 1);
    }
    
    // Fix FX memory leak
    if(cfg->GetBool("FixFXLeak", true, "Gameplay"))
    {
        HOOKBLX(FxInfoMan_FXLeak, pGTASA + 0x36DB7C + 0x1);
    }
    
    // BengbuGuards: Jetpack Hover Button
    if(cfg->GetBool("JetpackHovering", true, "Gameplay"))
    {
        HOOKBLX(Jetpack_IsHeldDown, pGTASA + 0x3FA4A0 + 0x1);
    }
    
    // Re-implement idle camera like on PC/PS2
    if(cfg->GetBool("IdleCamera", true, "Gameplay"))
    {
        InitIdleCam(gIdleCam);

        ProcessCamFollowPed_BackTo1 = pGTASA + 0x3C4C52 + 0x1; // reset idle cam
        ProcessCamFollowPed_BackTo2 = pGTASA + 0x3C4B84 + 0x1; // skip after patch 1
        ProcessCamFollowPed_BackTo3 = pGTASA + 0x3C4BBE + 0x1; // skip after patch 2
        aml->Redirect(pGTASA + 0x3C4B7C + 0x1, (uintptr_t)ProcessCamFollowPed_IdleCam1);
        aml->Redirect(pGTASA + 0x3C4BB2 + 0x1, (uintptr_t)ProcessCamFollowPed_IdleCam2);

        aml->PlaceNOP4(pGTASA + 0x3BF2DC, 1);
        HOOKBLX(CamProcess_IdleCam, pGTASA + 0x3DCA2C + 0x1);
        HOOKPLT(DrawAllWidgets, pGTASA + 0x66E5E4);

        // Speed 3.0f is like on PC: 1,5 minutes
        // Some weird glitch exists i dont wanna deal with...
        float idleCamSpeed = cfg->GetFloat("IdleCameraStartSpeed", 3.0f, "Gameplay");
        if(idleCamSpeed < 0.34f) idleCamSpeed = 0.34f; // min 10 seconds
        else if(idleCamSpeed > 20.0f) idleCamSpeed = 20.0f; // max 10 minutes
        gIdleCam->timeControlsIdleForIdleToKickIn = idleCamSpeed * 90000.0f;
    }
    
    // Do a bigger size for collision-processing cache
    newColCacheSize = cfg->GetInt("CollisionCacheSizeBump", 128, "Gameplay");
    if(newColCacheSize > 50) // default is 50, we dont allow to set it to lower value
    {
        aml->PlaceB(pGTASA + 0x2D96B4 + 0x1, pGTASA + 0x2D9730 + 0x1); // disable default limit
        HOOKBLX(InitCollisions_BumpCache, pGTASA + 0x471DB8 + 0x1);
    }

    // Falling star.
    if(cfg->GetBool("FallingStarColor", true, "Visual"))
    {
        HOOKBLX(RenderState_Star, pGTASA + 0x59F570 + 0x1);
    }

    // Stunt smoke
    if(cfg->GetBool("FixStuntSmoke", true, "Gameplay"))
    {
        HOOKBLX(Plane_ProcessControl_Horn, pGTASA + 0x575CC4 + 0x1);
    }

    // Wrong vehicle's parts colors!
    if(cfg->GetBool("FixWrongCarDetailsColor", true, "Visual"))
    {
        HOOKBLX(ObjectRender_VehicleParts, pGTASA + 0x454F5A + 0x1);
    }

    // AliAssassiN: Camera does not go crazy with mouse connected
    if(cfg->GetBool("MouseFix", true, "Gameplay"))
    {
        aml->Write32(pGTASA + 0x3F8C8C, 0x679F90 - 0x3F8C5E);
        aml->Write32(pGTASA + 0x3F8C94, 0x679F90 - 0x3F8B7E);
    }

    // AliAssassiN: Fixes "ghosting" when looking underground
    if(cfg->GetBool("CompletelyClearCameraBuffer", true, "Visual"))
    {
        aml->PlaceB(pGTASA + 0x3F58A0 + 0x1, pGTASA + 0x3F58D0 + 0x1);
    }

    // Fix enter-vehicle tasks
    if(cfg->GetBool("FixEnterVehicleTasks", true, "Gameplay"))
    {
        aml->PlaceNOP(pGTASA + 0x40AA36);
        aml->PlaceB(pGTASA + 0x40AA68 + 0x1, pGTASA + 0x40AACC + 0x1);
        HOOKBLX(Patch_ExitVehicleJustDown, pGTASA + 0x40AB80 + 0x1);
    }

    // Fix widget's holding radius
    if(cfg->GetBool("WidgetHoldingMaxShift", true, "Gameplay"))
    {
        WidgetUpdateHold_BackTo = pGTASA + 0x2B2C98 + 0x1;
        aml->Redirect(pGTASA + 0x2B2C8E + 0x1, (uintptr_t)WidgetUpdateHold_Inject);
    }

    // Fix planes generation coordinates
    if(cfg->GetBool("FixPlanesGenerationCoords", true, "Gameplay"))
    {
        HOOKBLX(FindPlaneCoors_CheckCol, pGTASA + 0x578FA8 + 0x1);
    }

    // Now CJ is able to exit a vehicle and start moving immediately, without being forced to close the door
    if(cfg->GetBool("NotForcedToCloseVehDoor", true, "Gameplay"))
    {
        HOOKBLX(DoorClosing_PadTarget, pGTASA + 0x501BD0 + 0x1);
    }
    
    // Fix wheels rotation speed on high FPS
    if(cfg->GetBool("FixWheelsRotationSpeed", true, "Visual"))
    {
        HOOKBLX(ProcessWheelRotation_FPS, pGTASA + 0x557FC6);
    }
    
    // Reflections are based on player's neck when it's not a cutscene (WTF???)
    if(cfg->GetBool("FixReflectionsCenter", true, "Visual"))
    {
        aml->PlaceNOP(pGTASA + 0x5C4F24);
    }
    
    // Reflection's quality
    if(cfg->GetBool("DoubleReflectionsRenderSize", true, "Visual"))
    {
        aml->Write(pGTASA + 0x5C49E6, "\x4F\xF4\x80\x64", 4);
    }

    // An ability to remove FOV-effect while driving a car
    if(cfg->GetBool("NoVehicleFOVEffect", false, "Gameplay"))
    {
        aml->PlaceNOP(pGTASA + 0x3C07E6 + 0x1, 2);
        aml->PlaceNOP(pGTASA + 0x3C082C + 0x1, 2);
    }

    // Now all vehicles should have a shadow
    if(cfg->GetBool("FixVehicleShadows", true, "Visual"))
    {
        aml->PlaceNOP(pGTASA + 0x5B95CE + 0x1);
        aml->PlaceNOP(pGTASA + 0x5B95D2 + 0x1);
        aml->PlaceNOP(pGTASA + 0x5B97E6 + 0x1, 3);
        aml->PlaceB(pGTASA + 0x5B95D6, pGTASA + 0x5B9798 + 0x1);
        HOOKPLT(TrainPreRender, pGTASA + 0x66E154);
        HOOKPLT(BoatPreRender, pGTASA + 0x66DA78);
    }

    // That's for the fix above. Cuz they're ugly as hell by default, EWWW...
    if(cfg->GetBool("BiggerCarShadowsDistance", true, "Visual"))
    {
        aml->Write(pGTASA + 0x5B979C, "\x9F\xED\xD0\x2A\xB0\xEE\x42\x1A", 8);
        aml->WriteFloat(pGTASA + 0x5B9AE0, 320.0f);
    }

    // The fix "PCDirLightsCount" is not gonna work now. So lets remove an optimisation instead.
    if(cfg->GetBool("BiggerLightsCountOutside", true, "Visual"))
    {
        aml->Write8(pGTASA + 0x5D1AE4, 0x03);
    }

    // Missing effects that are on PC but not on Mobile (from SkyGFX)
    if(cfg->GetBool("CutEffects", true, "Visual"))
    {
        aml->PlaceNOP(pGTASA + 0x5B678A + 0x1, 1);
        HOOKBLX(PostProcess_CCTV, pGTASA + 0x5B678C + 0x1);
        HOOKBLX(RenderEffects_WaterCannons, pGTASA + 0x3F63A2 + 0x1);
    }

    // Cant skip drive
    if(cfg->GetBool("BringBackSkipButton", true, "Gameplay"))
    {
        HOOKBLX(UpdateSkip_SkipCanBeActivated, pGTASA + 0x3092AE + 0x1);
    }

    // This fixes black bushes and more things
    //if(cfg->GetBool("FixCamNormColorOverflow", true, "Visual"))
    {
       // HOOKBL(VTXShader_CamBasedNormal_snprintf, pGTASA + 0x1CF44C);
    }
    




    


    // Mobile has 2x times less directional light sources. Lets fix this, it's not 2013 anymore
    // UPD: shaders are limited to 1 (or 2?) only...
    /*if(cfg->GetBool("PCDirLightsCount", true, "Visual"))
    {
        uint8_t patchval = 3 + EXTRA_DIRLIGHTS;
        aml->Write8(pGTASA + 0x5D1B0C, patchval);
        aml->Write8(pGTASA + 0x5D1AE4, (uint8_t)(patchval * 0.6666666f));
        aml->WriteAddr(pGTASA + 0x678854, pNewExtraDirectionals);
        aml->WriteAddr(pGTASA + 0x67604C, NewLightStrengths);
        HOOKPLT(LightsCreate,  pGTASA + 0x671548);
        HOOKPLT(LightsDestroy, pGTASA + 0x6754E0);
        HOOKPLT(RemoveExtraDirectionalLights, pGTASA + 0x670240);

        HOOK(ADDDD, aml->GetSym(hGTASA, "_Z26AddAnExtraDirectionalLightP7RpWorldffffff"));
    }*/

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
    
    // JuniorDjjr, W.I.P.
    /*if(cfg->GetBool("FoodEatingModelFix", true, "Gameplay"))
    {
        HOOKPLT(PlayerInfoProcess_Food, pGTASA + 0x673E84);
    }*/