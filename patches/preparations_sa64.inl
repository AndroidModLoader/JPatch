    HOOKPLT(GameProcess, pGTASA + 0x840198);
    
    
    // Animated textures
    if(cfg->GetBool("EnableAnimatedTextures", true, "Visual"))
    {
        //aml->Write8(aml->GetSym(hGTASA, "RunUVAnim"), true);
        aml->PlaceNOP(pGTASA + 0x25F5EC, 1);
        aml->PlaceNOP(pGTASA + 0x25EDC8, 1);
    }

    // Vertex weight
    if(cfg->GetBool("FixVertexWeight", true, "Visual"))
    {
        // openglSkinAllInOneAtomicInstanceCB
        aml->Write32(pGTASA + 0x25C278, MOVBits::Create(1, 27, false));
        aml->PlaceNOP(pGTASA + 0x25C28C, 1);
        aml->Write32(pGTASA + 0x25C290, MOVBits::Create(1, 27, false));
    }

    // Fix sky multitude
    if(cfg->GetBool("FixSkyMultitude", true, "Visual"))
    {
        aml->Unprot(pGTASA + 0x7630D8, sizeof(float)); *(float*)(pGTASA + 0x7630D8) = -10.0f;
        aml->PlaceNOP(pGTASA + 0x6C39F8, 1);
        aml->Write(pGTASA + 0x6C3A04, "\x03\x90\x24\x1E", 4);
    }

    // Fix vehicles backlights light state
    if(cfg->GetBool("FixCarsBacklightLightState", true, "Visual"))
    {
        aml->Write32(pGTASA + 0x6B52E0, MOVBits::Create(2, 1, false));
    }

    // Limit sand/dust particles on bullet impact (they are EXTREMELY dropping FPS)
    if(cfg->GetBool("LimitSandDustBulletParticles", true, "Visual"))
    {
        HOOKBL(GetBulletFx_Limited, pGTASA + 0x434A38);
        if(cfg->GetBool("LimitSandDustBulletParticlesWithSparkles", false, "Visual"))
        {
            nLimitWithSparkles = BULLETFX_SPARK;
        }
    }

    // Do not set primary color to the white on vehicles paintjob
    if(cfg->GetBool("PaintJobDontSetPrimaryToWhite", true, "Visual"))
    {
        aml->PlaceNOP(pGTASA + 0x6A5EC0, 1);
    }

    // Fix walking while rifle-aiming
    if(cfg->GetBool("FixAimingWalkRifle", true, "Gameplay"))
    {
        HOOKPLT(ControlGunMove, pGTASA + 0x83F9D8);
    }

    // Fix water physics
    if(cfg->GetBool("FixWaterPhysics", true, "Gameplay"))
    {
        HOOKBL(ProcessSwimmingResistance, pGTASA + 0x6565F0);
        ProcessBuoyancy_BackTo = pGTASA + 0x691E94;
        aml->Redirect(pGTASA + 0x691E80, (uintptr_t)ProcessBuoyancy_Inject);
    }

    // Fix stealable items sucking
    if(cfg->GetBool("ClampObjectToStealDist", true, "Gameplay"))
    {
        aml->Write(pGTASA + 0x4EF9C4, (uintptr_t)"\x00\x10\x2E\x1E", 4);
    }

    // Fix broken basketball minigame by placing the save icon away from it
    if(cfg->GetBool("MaddDoggMansionSaveFix", true, "SCMFixes"))
    {
        HOOKPLT(GenerateNewPickup_MaddDogg, pGTASA + 0x848278);
    }

    // Fix broken basketball minigame by placing the save icon away from it
    if(cfg->GetBool("FixStarBribeInSFBuilding", true, "SCMFixes"))
    {
        HOOKPLT(GenerateNewPickup_SFBribe, pGTASA + 0x848278);
    }

    // Fix rifle pickup that stuck inside the stadium
    if(cfg->GetBool("FixSFStadiumRiflePickup", true, "SCMFixes"))
    {
        HOOKPLT(GenerateNewPickup_SFRiflePickup, pGTASA + 0x848278);
    }

    // Remove jetpack leaving on widget press while in air?
    if(cfg->GetBool("DisableDropJetPackInAir", true, "Gameplay"))
    {
        HOOKPLT(DropJetPackTask, pGTASA + 0x849738);
    }

    // Dont stop the car before leaving it
    if(cfg->GetBool("ImmediatelyLeaveTheCar", true, "Gameplay"))
    {
        HOOK(CanPedStepOutCar, aml->GetSym(hGTASA, "_ZNK8CVehicle16CanPedStepOutCarEb"));
    }

    // Bring back penalty when CJ dies!
    if(cfg->GetBool("WeaponPenaltyIfDied", true, "Gameplay"))
    {
        DiedPenalty_BackTo = pGTASA + 0x3CE2A8;
        aml->Redirect(pGTASA + 0x3CE290, (uintptr_t)DiedPenalty_Inject);
    }

    // Fixing a crosshair position by very stupid math
    if(cfg->GetBool("FixCrosshair", true, "Visual"))
    {
        HOOKBL(DrawCrosshair, pGTASA + 0x51F7E4);
    }
    
    // Country. Rifle. Is. 3rd. Person.
    if(cfg->GetBool("FixCountryRifleAim", true, "Gameplay"))
    {
        aml->PlaceNOP(pGTASA + 0x653408, 2);
        aml->Write32(pGTASA + 0x6539E0, CMPBits::Create(0xFF, 28, false));
    }

    // Colored zone names are back
    if(cfg->GetBool("ColoredZoneNames", true, "Visual"))
    {
        aml->PlaceNOP(pGTASA + 0x51D9C0, 1);
    }

    // A fix for 2.10 crash (thanks fastman92!)
    if(cfg->GetBool("Fix210Crash", true, "Gameplay"))
    {
        aml->Write32(pGTASA + 0x278E50, 0x8B090108);
        aml->Write32(pGTASA + 0x278E60, 0x8B090108);
        aml->Write32(pGTASA + 0x278E70, 0xAA0903E9);
    }

    // Fix enter-vehicle tasks
    if(cfg->GetBool("FixEnterVehicleTasks", true, "Gameplay"))
    {
        aml->PlaceNOP(pGTASA + 0x4EF23C);
        aml->PlaceB(pGTASA + 0x4EF27C, pGTASA + 0x4EF2F4);
        HOOKBL(Patch_ExitVehicleJustDown, pGTASA + 0x4EF414);
    }

    // Fixes farclip glitch with wall (wardumb be like)
    if(cfg->GetBool("FixFogWall", true, "Visual"))
    {
        aml->Write(pGTASA + 0x712095, "\x31\x2E\x30\x30", 4);
        HOOKBL(DistanceFogSetup_FogWall, pGTASA + 0x240B3C);
    }

    // Frick your "improved characters models", War Dumb
    if(cfg->GetBool("FixPedSpecInShaders", true, "Visual"))
    {
        aml->Write32(pGTASA + 0x263810, ARMv8::MOVRegBits::CreateMOV(23, 8, false));
        aml->PlaceB(pGTASA + 0x264A94, pGTASA + 0x264B34);
    }

    // Wrong vehicle's parts colors!
    if(cfg->GetBool("FixWrongCarDetailsColor", true, "Visual"))
    {
        HOOKBL(ObjectRender_VehicleParts, pGTASA + 0x53E17C);
    }

    // Stunt smoke
    if(cfg->GetBool("FixStuntSmoke", true, "Gameplay"))
    {
        HOOKBL(Plane_ProcessControl_Horn, pGTASA + 0x699418);
    }

    // Falling star.
    if(cfg->GetBool("FallingStarColor", true, "Visual"))
    {
        HOOKBL(RenderState_Star, pGTASA + 0x6C34C4);
    }
    
    // BengbuGuards: Jetpack Hover Button
    if(cfg->GetBool("JetpackHovering", true, "Gameplay"))
    {
        HOOKBL(Jetpack_IsHeldDown, pGTASA + 0x4DD104);
    }

    // Fix the issue that player cannot kill with a knife if not crouching
    if(cfg->GetBool("FixUncrouchedStealthKill", true, "Gameplay"))
    {
        aml->PlaceB(pGTASA + 0x65354C, pGTASA + 0x654A20);
    }

    // Fixing a wrong value in carcols.dat
    if(cfg->GetBool("CarColsDat_FixWrongValue", true, "Visual"))
    {
        HOOKBL(CarColsDatLoad_sscanf, pGTASA + 0x557F5C);
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
            else if(valllue > 4096) valllue = 4096;

            nMaxStreamingMemForDynamic = 1024 * 1024 * valllue;

            valllue = cfg->GetInt("DynamicStreamingMem_BumpStep", 8, "Gameplay");
            if(valllue < 2) valllue = 2;
            else if(valllue > 64) valllue = 64;
            nDynamicStreamingMemBumpStep = valllue;
        }
    }

    // Fix planes generation coordinates
    if(cfg->GetBool("FixPlanesGenerationCoords", true, "Gameplay"))
    {
        HOOKBL(FindPlaneCoors_CheckCol, pGTASA + 0x69C660);
    }