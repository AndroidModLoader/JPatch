    HOOKPLT(GameProcess, pGTASA + 0x840198);
    

    // I am using a function at 0x692B14 as my variables storage.
    // This function is unused and most likely never will be,
    // so should be safe

    
    // Animated textures
    if(cfg->GetBool("EnableAnimatedTextures", true, "Visual"))
    {
        aml->Write8(aml->GetSym(hGTASA, "RunUVAnim"), true);
        aml->PlaceNOP(pGTASA + 0x25EE0C, 1);
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
        HOOKPLT(GetBulletFx_Limited, pGTASA + 0x846068);
        if(cfg->GetBool("LimitSandDustBulletParticlesWithSparkles", false, "Visual"))
        {
            nLimitWithSparkles = BULLETFX_SPARK;
        }
    }

    // Fix walking while rifle-aiming
    if(cfg->GetBool("FixAimingWalkRifle", true, "Gameplay"))
    {
        HOOKPLT(ControlGunMove, pGTASA + 0x83F9D8);
    }

    // Fix water physics
    if(cfg->GetBool("FixWaterPhysics", true, "Gameplay"))
    {
        HOOK(ProcessSwimmingResistance, pGTASA + 0x657B88);
        HOOKBL(ProcessPedBuoyancy, pGTASA + 0x5980F0);
        SET_TO(buoyancyTimescaleReplacement, pGTASA + 0x692B14); UNPROT(buoyancyTimescaleReplacement, sizeof(float));
        aml->Write32(pGTASA + 0x691E80, 0xB0000009);
        aml->Write32(pGTASA + 0x691E84, 0xBD4B1522);
        aml->Write32(pGTASA + 0x691E90, 0xD503201F);
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
        HOOK(DrawCrosshair, pGTASA + 0x51C694);
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
        aml->Write32(pGTASA + 0x51D9D8, 0x39403102);
        aml->Write32(pGTASA + 0x51D9DC, 0x39403503);
        aml->Write32(pGTASA + 0x51D9E0, 0x35000061);
        aml->Write32(pGTASA + 0x51D9E4, 0x35000042);
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
        aml->PlaceB(pGTASA + 0x4EF274, pGTASA + 0x4EF2F4);
        HOOKBL(Patch_ExitVehicleJustDown, pGTASA + 0x4EF414);
    }

    // Fixes farclip glitch with wall (wardumb be like)
    if(cfg->GetBool("FixFogWall", true, "Visual"))
    {
        aml->Write(pGTASA + 0x712095, "\x31\x2E\x30\x30", 4);
        HOOKPLT(DistanceFogSetup_FogWall, pGTASA + 0x8465E0);
    }

    // Frick your "improved characters models", War Dumb
    if(cfg->GetBool("FixPedSpecInShaders", true, "Visual"))
    {
        aml->Write32(pGTASA + 0x263810, ARMv8::MOVRegBits::Create(8, 23, false));
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
            int valllue = cfg->GetInt("DynamicStreamingMem_MaxMBs", 2048, "Gameplay");
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
        HOOKPLT(FindPlaneCoors_CheckCol, pGTASA + 0x844158);
    }

    // Now CJ is able to exit a vehicle and start moving immediately, without being forced to close the door
    if(cfg->GetBool("NotForcedToCloseVehDoor", true, "Gameplay"))
    {
        HOOKBL(DoorClosing_PadTarget, pGTASA + 0x60F990);
    }
    
    // Removes "plis give us 5 stars plis plis"
    if(cfg->GetBool("RemoveAskingToRate", true, "Others"))
    {
        aml->PlaceB(pGTASA + 0x411ACC, pGTASA + 0x411AD8);
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
        aml->PlaceB(pGTASA + 0x47CEF4, pGTASA + 0x47CFC0); // QueueUpTracksForStation
        aml->PlaceB(pGTASA + 0x47F680, pGTASA + 0x47F738); // ChooseMusicTrackIndex
        aml->PlaceB(pGTASA + 0x47F844, pGTASA + 0x47F93C); // ChooseIdentIndex
        aml->PlaceB(pGTASA + 0x47FBE0, pGTASA + 0x47FC9C); // ChooseAdvertIndex
        aml->PlaceB(pGTASA + 0x4805EC, pGTASA + 0x4806B8); // ChooseTalkRadioShow
        aml->PlaceB(pGTASA + 0x4807A4, pGTASA + 0x480870); // ChooseDJBanterIndexFromList
    }
    
    // Some kind of "Sprint Everywhere"
    if(cfg->GetBool("SprintOnAnySurface", true, "Gameplay"))
    {
        aml->Redirect(aml->GetSym(hGTASA, "_ZN14SurfaceInfos_c12CantSprintOnEj"), (uintptr_t)ret0);
    }
    
    // Peepo: Fix traffic lights
    if(cfg->GetBool("FixTrafficLights", true, "Visual"))
    {
        HOOK(TrFix_RenderShinyTexts, aml->GetSym(hGTASA, "_ZN11CShinyTexts6RenderEv"));
        HOOK(TrFix_InitGame2nd, aml->GetSym(hGTASA, "_ZN5CGame5Init2EPKc"));
    }
    
    // Water Quadrant
    int dist = cfg->GetInt("DetailedWaterDrawDistance", 48 * 2, "Visual");
    if(dist > 0)
    {
        if(dist < 24) dist = 24;
        else if(dist >= 48 * 4) dist = 48 * 4;
        *DETAILEDWATERDIST = dist;
    }
    
    // Money have 8 digits now? Exciting!
    if(cfg->GetBool("PCStyledMoney", false, "Visual"))
    {
        HOOKBL(DrawMoney_sprintf, pGTASA + 0x37D4B8);
    }

    // Sweet's roof is not that tasty anymore
    if(cfg->GetBool("FixClimbDying", true, "Gameplay"))
    {
        HOOKPLT(ClimbProcessPed, pGTASA + 0x83A6A0);
    }

    // Fix color picker widget
    if(cfg->GetBool("FixColorPicker", true, "Visual"))
    {
        HOOKPLT(GetColorPickerValue, pGTASA + 0x829BE0);
    }

    // RE3: Road reflections
    if(cfg->GetBool("Re3_WetRoadsReflections", true, "Visual"))
    {
        aml->PlaceNOP(pGTASA + 0x6C6770);
        aml->Write32(pGTASA + 0x6C6774, 0xBD407BE2);
    }

    // Bigger max count of peds
    if(cfg->GetBool("BuffMaxPedsCount", true, "Gameplay"))
    {
        *(int*)aml->GetSym(hGTASA, "_ZN11CPopulation20MaxNumberOfPedsInUseE") = 0x23;
        aml->Write32(pGTASA + 0x4D71E4, 0x52800469);
        aml->Write32(pGTASA + 0x5CB4E4, 0x52800469);
        aml->Write32(pGTASA + 0x5CC0E8, 0x52800468);
        aml->Write32(pGTASA + 0x5CC0EC, 0x5280038A);
    }

    // Bigger max count of cars
    if(cfg->GetBool("BuffMaxCarsCount", true, "Gameplay"))
    {
        *(int*)aml->GetSym(hGTASA, "_ZN8CCarCtrl20MaxNumberOfCarsInUseE") = 0x14;
        aml->Write32(pGTASA + 0x4D71E8, 0x5280028B);
    }

    // RE3: Fix R* optimization that prevents peds to spawn
    if(cfg->GetBool("Re3_PedSpawnDeoptimize", true, "Gameplay"))
    {
        aml->Write32(pGTASA + 0x4D6420, 0x71000ABF);
    }

    // Just a fuzzy seek. Tell MPG123 to not load useless data.
    if(cfg->GetBool("FuzzySeek", true, "Gameplay"))
    {
        aml->Write32(pGTASA + 0x339134, 0x52846C02);
        aml->Write32(pGTASA + 0x339404, 0x52846C02);
    }

    // 44100 Hz Audio support (without a mod OpenAL Update but works with it anyway)
    if(cfg->GetBool("Allow44100HzAudio", true, "Gameplay"))
    {
        aml->Unprot(pGTASA + 0x749AA4, sizeof(int));
        *(int*)(pGTASA + 0x749AA4) = 44100;
    }

    // Fixes emergency vehicles
    if(cfg->GetBool("FixStreamingDistScale", true, "Gameplay"))
    {
        EmergencyVeh_BackTo = pGTASA + 0x4BBB50;
        aml->Redirect(pGTASA + 0x4BBB38, (uintptr_t)EmergencyVeh_Inject);
        HOOKPLT(SetFOV_Emergency, pGTASA + 0x846898);
        aml->Write32(pGTASA + 0x4BBB70, 0x1E204002); // NOP "* 0.875f"
        aml->PlaceNOP(pGTASA + 0x4BBB68, 1); // NOP "* 0.8f"
    }

    // AliAssassiN: Camera does not go crazy with mouse connected
    if(cfg->GetBool("MouseFix", true, "Gameplay"))
    {
        aml->Write32(pGTASA + 0x4DB614, 0xD0001BAB);
        aml->Write32(pGTASA + 0x4DB618, 0xF947996B);
    }

    // AliAssassiN: Fixes "ghosting" when looking underground
    if(cfg->GetBool("CompletelyClearCameraBuffer", true, "Visual"))
    {
        aml->PlaceB(pGTASA + 0x4D7DB4, pGTASA + 0x4D7E2C);
    }

    // PS2 thingo
    if(cfg->GetBool("PS2CoronaRotation", true, "Visual"))
    {
        HOOKBL(RenderOneXLUSprite_Rotate_Aspect_PS2, pGTASA + 0x6C6158);
    }

    // SilentPatch fix
    if(cfg->GetBool("DirectionalSunLight", true, "Visual"))
    {
        HOOKPLT(SetLightsWithTimeOfDayColour_DirLight, pGTASA + 0x846C88);
    }

    // Fix airbubbles from the jaw (CJ is breathing with his ass, lololololol)
    if(cfg->GetBool("AirBubblesFromJaw", true, "Visual"))
    {
        aml->Write32(pGTASA + 0x6586E4, 0xF9411C08);
        aml->Write32(pGTASA + 0x6586EC, 0xF9411808);
    }

    // FX particles distance multiplier!
    fxMultiplier = cfg->GetFloat("FxDistanceMult", 2.5f, "Visual");
    if(fxMultiplier != 1)
    {
        if(fxMultiplier < 0.1) fxMultiplier = 0.1f;
        else if(fxMultiplier > 20) fxMultiplier = 20.0f;
        HOOKBL(LoadFX_sscanf, pGTASA + 0x559AC8);
        HOOKBL(LoadFX_sscanf, pGTASA + 0x559F48);
        HOOKBL(LoadFX_sscanf, pGTASA + 0x559F78);
    }

    // Fixes Corona sprites stretching at foggy weather
    if(cfg->GetBool("FixCoronasStretching", true, "Visual"))
    {
        aml->Write32(pGTASA + 0x6C6120, 0x1E204044);
    }

    // BengbuGuards' idea #1
    if(cfg->GetBool("FixSecondSiren", true, "Gameplay"))
    {
        aml->PlaceNOP(pGTASA + 0x6B41E8, 1);
        aml->PlaceNOP(pGTASA + 0x6B4214, 1);
    }

    // Spread fix
    if(cfg->GetBool("WeaponSpreadFix", true, "Gameplay"))
    {
        HOOK(FireInstantHit, aml->GetSym(hGTASA, "_ZN7CWeapon14FireInstantHitEP7CEntityP7CVectorS3_S1_S3_S3_bb"));
    }

    // Renders shadows on all surfaces -> disables a flag
    if(cfg->GetBool("DrawShadowsOnAllSurfaces", true, "Visual"))
    {
        aml->Write32(pGTASA + 0x6DF67C, 0x52800033);
    }

    // Game is checking if HP is < 1.0 but it may be lower!
    if(cfg->GetBool("AllowCrouchWith1HP", true, "Gameplay"))
    {
        aml->Write32(pGTASA + 0x660DC8, 0x1E281001);
    }
    
    // Vehicle sun glare
    if(cfg->GetBool("VehicleSunGlare", true, "Visual"))
    {
        HOOK(RenderVehicle_SunGlare, aml->GetSym(hGTASA, "_ZN8CVehicle6RenderEv"));
    }
    
    // Minimap in interiors? Hell nah!
    if(cfg->GetBool("NoInteriorRadar", true, "Visual"))
    {
        HOOK(DrawRadar, pGTASA + 0x51CFF0);
    }

    // Fix greenish detail tex
    if(cfg->GetBool("FixGreenTextures", true, "Visual"))
    {
        aml->PlaceNOP(pGTASA + 0x2418B8, 1); // Dont set textureDetail variable! We'll handle it by ourselves!
        HOOK(emu_TextureSetDetailTexture, aml->GetSym(hGTASA, "_Z27emu_TextureSetDetailTexturePvj"));
    }

    // Radar
    if(cfg->GetBool("FixRadarStreaming", true, "Visual"))
    {
        aml->PlaceB(pGTASA + 0x5283D4, pGTASA + 0x5283EC);
    }

    // AllowLicensePlatesForAllCars
    if(cfg->GetBool("AllowLicensePlatesForAllCars", true, "Visual"))
    {
        aml->PlaceB(pGTASA + 0x6A5E84, pGTASA + 0x6A5EC8);
        aml->PlaceB(pGTASA + 0x6A5F58, pGTASA + 0x6A5F68);
    }

    // Show muzzle flash for the last bullet in magazine
    if(cfg->GetBool("MuzzleFlashForLastBullet", true, "Visual"))
    {
        aml->PlaceNOP(pGTASA + 0x5DFF28, 1);
    }

    // Random license plates
    if(cfg->GetBool("RandomLicensePlates", true, "Visual"))
    {
        HOOKBL(GetCustomCarPlateText_SetModelIdx, pGTASA + 0x6A6668);
    }

    // Fix "You have worked out enough for today, come back tomorrow!"
    if(cfg->GetBool("FixGymWorkoutDate", true, "SCMFixes"))
    {
        Opcode0835_BackTo = pGTASA + 0x403768;
        aml->Redirect(pGTASA + 0x401E84, (uintptr_t)Opcode0835_Inject);
    }

    // Michelle dating fix
    if(cfg->GetBool("MichelleDatingFix", true, "SCMFixes"))
    {
        Opcode039E_BackTo = pGTASA + 0x415DA0;
        aml->Redirect(pGTASA + 0x414798, (uintptr_t)Opcode039E_Inject);
    }

    // Inverse swimming controls to dive/go up (to match PC version)
    if(cfg->GetBool("InverseSwimmingDivingControl", true, "Gameplay"))
    {
        HOOK(TaskSwim_ProcessInput, pGTASA + 0x6569C4);
        HOOKBL(GetPedWalkUpDown_Swimming, pGTASA + 0x656A1C);
        aml->Write32(pGTASA + 0x6571F4, 0xD000072A);
        aml->Write32(pGTASA + 0x6571F8, 0xBD4E9D44);
    }

    // Guess by the name
    if(cfg->GetBool("FixParachuteLandingAnim", true, "Visual"))
    {
        HOOKPLT(PlayerInfoProcess_ParachuteAnim, pGTASA + 0x846998);
    }

    // Unused detonator animation is in the ped.ifp, lol
    if(cfg->GetBool("UnusedDetonatorAnimation", true, "Visual"))
    {
        HOOKPLT(UseDetonator, pGTASA + 0x840048);
    }
    
    // Taxi lights (obviously)
    if(cfg->GetBool("TaxiLights", true, "Visual"))
    {
        HOOK(AutomobileRender, aml->GetSym(hGTASA, "_ZN11CAutomobile6RenderEv"));
    }
    
    // Fix Adjustable.cfg loading?
    // UPD: Introduced another glitch, so its unfixed. yet.
    // UD2: Fixed with a much better way. But another glitch arrived with X-coord shifting
    if(cfg->GetBool("FixAdjustableSizeLowering", true, "Visual"))
    {
        aml->Write32(pGTASA + 0x33D6E8, 0x1E229000);
    }

    // Classic CJ shadow
    if(cfg->GetBool("FixClassicCJShadow", true, "Gameplay"))
    {
        aml->PlaceB(pGTASA + 0x6DCFF0, pGTASA + 0x6DD014);
    }

    // Car Slowdown Fix
    if(cfg->GetBool("FixCarSlowdownHighFPS", true, "Gameplay"))
    {
        HOOKPLT(ProcessVehicleWheel, pGTASA + 0x83FE78);
        HOOKPLT(ProcessBikeWheel, pGTASA + 0x846A90);
    }
    
    // Fix Skimmer plane
    if (cfg->GetBool("SkimmerPlaneFix", true, "Gameplay"))
    {
        //SkimmerWaterResistance_BackTo = pGTASA + 0x6AD4D8;
        //aml->Redirect(pGTASA + 0x6AD4C8, (uintptr_t)SkimmerWaterResistance_Inject);
        HOOK(ApplyBoatWaterResistance, pGTASA + 0x6AD450);
    }

    // Cinematic vehicle camera on double tap
    if(cfg->GetBool("CinematicCameraOnDoubleTap", true, "Gameplay"))
    {
        HOOKPLT(PlayerInfoProcess_Cinematic, pGTASA + 0x846998);
    }

    // RE3: Make cars and peds to not despawn when you look away
    if(cfg->GetBool("Re3_ExtOffscreenDespRange", true, "Gameplay"))
    {
        aml->PlaceB(pGTASA + 0x3B06E0, pGTASA + 0x3B0728); // Vehicles
        aml->PlaceB(pGTASA + 0x5CDAF8, pGTASA + 0x5CDBE4); // Peds
    }

    // Dont kill peds when jacking their car, monster!
    if(cfg->GetBool("DontKillPedsOnCarJacking", true, "Gameplay"))
    {
        aml->PlaceB(pGTASA + 0x5FF4C4, pGTASA + 0x5FF4DC);
    }
    
    // Equipped parachute attacked anim fix
    if(cfg->GetBool("EquippedParaAttackAnimFix", true, "Visual"))
    {
        HOOK(ComputeDamageAnim, aml->GetSym(hGTASA, "_ZN12CEventDamage17ComputeDamageAnimEP4CPedb"));
    }
    
    if(cfg->GetBool("DisableCloudSaves", false, "Gameplay"))
    {
        aml->Write8(aml->GetSym(hGTASA, "UseCloudSaves"), 0x00);
    }
    
    // Always show wanted stars even if we're not breakin the law
    if(cfg->GetBool("AlwaysDrawWantedStars", false, "Visual"))
    {
        aml->PlaceB(pGTASA + 0x37E174, pGTASA + 0x37E1A4);
    }
    
    // An improved ForceDXT
    if(cfg->GetBool("ForceLoadDXT", false, "Gameplay"))
    {
        HOOK(LoadTexDBThumbs, aml->GetSym(hGTASA, "_ZN22TextureDatabaseRuntime4LoadEPKcb21TextureDatabaseFormat"));
    }

    // Car generators in an interior now work properly
    if(cfg->GetBool("CorrectPoliceScannerLocations", true, "Gameplay"))
    {
        aml->Write(pGTASA + 0x74AC98, "WESTP", 6);
        aml->Write(pGTASA + 0x74AD40, "????", 5);
    }
    
    // Fix wheels rotation speed on high FPS
    if(cfg->GetBool("FixWheelsRotationSpeed", true, "Visual"))
    {
        HOOKBL(ProcessWheelRotation_FPS, pGTASA + 0x67875C);
    }
    
    // Reflections are based on player's neck when it's not a cutscene (WTF???)
    if(cfg->GetBool("FixReflectionsCenter", true, "Visual"))
    {
        aml->PlaceNOP(pGTASA + 0x6E9508);
    }
    
    // Reflection's quality
    if(cfg->GetBool("DoubleReflectionsRenderSize", true, "Visual"))
    {
        aml->Write32(pGTASA + 0x6E8F60, 0x52808009);
    }
    
    // Fix FX memory leak
    if(cfg->GetBool("FixFXLeak", true, "Gameplay"))
    {
        HOOKPLT(FxInfoMan_FXLeak, pGTASA + 0x841D00);
    }

    // Bigger distance for light coronas
    if(cfg->GetBool("BuffDistForLightCoronas", true, "Visual"))
    {
        aml->Write32(pGTASA + 0x6C7A68, 0xF0000389); // CEntity::ProcessLightsForEntity
        aml->Write32(pGTASA + 0x6C7A78, 0xBD4C1520); // CEntity::ProcessLightsForEntity

        aml->Write32(pGTASA + 0x4331A0, 0xF0001828); // CTrafficLights::DisplayActualLight
        aml->Write32(pGTASA + 0x4331A4, 0xBD4C1506); // CTrafficLights::DisplayActualLight

        aml->Write32(pGTASA + 0x6864C0, 0x900005AA); // CBike::PreRender
        aml->Write32(pGTASA + 0x6864E8, 0xBD4C1541); // CBike::PreRender

        aml->Write32(pGTASA + 0x686434, 0x900005AA); // CBike::PreRender
        aml->Write32(pGTASA + 0x68645C, 0xBD4C1541); // CBike::PreRender

        aml->Write32(pGTASA + 0x695F18, 0xB0000529); // CHeli::SearchLightCone
        aml->Write32(pGTASA + 0x695F3C, 0xBD4C1529); // CHeli::SearchLightCone

        aml->Write32(pGTASA + 0x67ADC0, 0x90000608); // CAutomobile::PreRender
        aml->Write32(pGTASA + 0x67ADD0, 0xBD4C1503); // CAutomobile::PreRender

        aml->Write32(pGTASA + 0x67B0B4, 0xF00005E9); // CAutomobile::PreRender
        aml->Write32(pGTASA + 0x67B0C8, 0xBD4C152B); // CAutomobile::PreRender

        aml->Write32(pGTASA + 0x67C1AC, 0xD00005E8); // CAutomobile::PreRender
        aml->Write32(pGTASA + 0x67C1B4, 0xBD4C1501); // CAutomobile::PreRender
    }

    // Bigger distance for light shadows
    if(cfg->GetBool("BuffDistForLightShadows", true, "Visual"))
    {
        aml->Write32(pGTASA + 0x4331A4, 0xBD419506); // CTrafficLights::DisplayActualLight, 40 -> 120
        aml->Write32(pGTASA + 0x4D3904, 0xBD419508); // CFireManager::Update, 40 -> 120
        fLightDist = 120.0f; // For thingies below \/
    }

    // Bring back light shadows from poles!
    if(cfg->GetBool("BackPolesLightShadow", true, "Visual"))
    {
        HOOKBL(GetBaseEffect_AddLight, pGTASA + 0x6C7AC0);
        HOOKPLT(ProcessLightsForEntity_ItSelf, pGTASA + 0x849858);
        HOOKBL(ProcessLightsForEntity_AddLight, pGTASA + 0x6C8564);
        HOOKBL(ProcessLightsForEntity_AddLight, pGTASA + 0x6C84F8);

        // We are not passing argument "pArgCastingEntity" (always NULL)
        // We will use it for "bCurrentlyActive" variable.
        aml->Write32(pGTASA + 0x6C84EC, 0x2A1A03E3);
        aml->Write32(pGTASA + 0x6C8560, 0x2A1A03E3);
    }

    // Allows the game to render even more light shadows on the ground
    if(cfg->GetBool("BuffStaticShadowsCount", true, "Gameplay"))
    {
        // Static shadows?
        asShadowsStored_NEW = new CRegisteredShadow[0xFF + 1]; memset(asShadowsStored_NEW, 0, sizeof(CRegisteredShadow) * (0xFF + 1));
        aStaticShadows_NEW = new CStaticShadow[0xFF + 1] {0}; memset(aStaticShadows_NEW, 0, sizeof(CStaticShadow) * (0xFF + 1));
        aml->Write(pGTASA + 0x84D7F8, (uintptr_t)&asShadowsStored_NEW, sizeof(void*));
        aml->Write(pGTASA + 0x8511F8, (uintptr_t)&aStaticShadows_NEW, sizeof(void*));
        
        // Registered Shadows:
        // CShadows::StoreShadowToBeRendered
        aml->Write32(pGTASA + 0x6DDC3C, 0xF103F93F);
        aml->Write32(pGTASA + 0x6DDC5C, 0xF103F93F);
        aml->Write32(pGTASA + 0x6DDC7C, 0xF103F93F);
        aml->Write32(pGTASA + 0x6DDC9C, 0xF103F93F);
        aml->Write32(pGTASA + 0x6DDCBC, 0xF103F93F);
        aml->Write32(pGTASA + 0x6DDCE8, 0xF103F93F);
        // CShadows::StoreShadowToBeRendered (2nd arg is RwTexture*)
        aml->Write32(pGTASA + 0x6DDD74, 0xF103F93F);
        // CShadows::StoreShadowForVehicle
        aml->Write32(pGTASA + 0x6DE328, 0x7103F95F);
        aml->Write32(pGTASA + 0x6DE37C, 0x7103F95F);
        // CShadows::StoreShadowForPedObject
        aml->Write32(pGTASA + 0x6DE738, 0xF103F95F);
        // CShadows::StoreRealTimeShadow
        aml->Write32(pGTASA + 0x6DEA5C, 0xF103F93F);
        // CShadows::RenderExtraPlayerShadows
        aml->Write32(pGTASA + 0x6E2480, 0x7103F93F);
        aml->Write32(pGTASA + 0x6E24A4, 0x7103F93F);

        // Static Shadows:
        // CShadows::StoreStaticShadow
        aml->Write32(pGTASA + 0x6DD798, 0xF103FD1F);
        aml->Write32(pGTASA + 0x6DD7C4, 0x7103FD7F);
        // CShadows::RenderStaticShadows
        aml->Write32(pGTASA + 0x6E024C, 0xF104025F);
        aml->Write32(pGTASA + 0x6E0260, 0xF104029F);
        // CShadows::UpdateStaticShadows
        aml->Write32(pGTASA + 0x6E1BB4, 0xF104011F);
        
        HOOKPLT(RenderStaticShadows, pGTASA + 0x846A00);
        HOOKPLT(InitShadows, pGTASA + 0x83EFC0);
    }

    // Can now use a gun!
    if(cfg->GetBool("HighFPSAimingWalkingFix", true, "Gameplay"))
    {
        aml->Write32(pGTASA + 0x5DF790, 0x90000AA9);
        aml->Write32(pGTASA + 0x5DF794, 0xBD48D521);
    }
    
    // Max mobilesc0,mobilesc1,...,mobilesc### for us
    mobilescCount = cfg->GetInt("MaxLoadingScreens", 7, "Visual");
    if(mobilescCount > 0 && mobilescCount != 7)
    {
        HOOKBL(LoadSplash_sscanf, pGTASA + 0x520154);
    }

    // Fix water cannon on a high fps
    if(cfg->GetBool("FixHighFPSWaterCannons", true, "Gameplay"))
    {
        aml->PlaceNOP(pGTASA + 0x6EF5AC);
        aml->Write32(pGTASA + 0x6EF598, 0xD0000249);
        aml->Write32(pGTASA + 0x6EF59C, 0xBD4EF520);
    }

    // Fix moving objects on a high fps (through the scripts)
    if(cfg->GetBool("FixHighFPSOpcode034E", true, "SCMFixes"))
    {
        HOOKBL(CollectParams_034E, pGTASA + 0x4129B8);
    }

    // Fix pushing force
    if(cfg->GetBool("FixPhysicalPushForce", true, "Gameplay"))
    {
        HOOKBL(ApplyForce_Collision, pGTASA + 0x4E6634);
        HOOKBL(ApplyForce_Collision, pGTASA + 0x4E6A04);
    }

    // Increase intensity of vehicle tail light corona (MTA:SA)
    // Is this even working on Android?
    if(cfg->GetBool("IncreaseTailLightIntensity", true, "Gameplay"))
    {
        aml->Write32(pGTASA + 0x6B503C, 0x52801E05);
    }

    // THROWN projectiles throw more accurately (MTA:SA)
    if(cfg->GetBool("ThrownProjectilesAccuracy", true, "Gameplay"))
    {
        aml->PlaceB(pGTASA + 0x700F70, pGTASA + 0x701074);
    }

    // Fix red marker that cannot be placed in a menu on ultrawide screens
    // Kinda trashy fix...
    // This glitch is annoying me on v2.10!!! Time to fix it!!!
    if(cfg->GetBool("FixRedMarkerUnplaceable", true, "Gameplay"))
    {
        aml->Write32(pGTASA + 0x368D20, 0xF0001E49);
        aml->Write32(pGTASA + 0x368D24, 0xBD48C920);
        aml->Write32(pGTASA + 0x368D54, 0x1E2D1863);
        HOOK(PlaceRedMarker_MarkerFix, pGTASA + 0x3688A0);
    }

    // Can now rotate the camera inside the heli/plane?
    // https://github.com/TheOfficialFloW/gtasa_vita/blob/6417775e182b0c8b789cc9a0c1161e6f1b43814f/loader/main.c#L736
    if(cfg->GetBool("UnstuckHeliCamera", true, "Gameplay"))
    {
        aml->Write32(pGTASA + 0x4A02E0, 0xD2800000);
        aml->Write32(pGTASA + 0x4A0E94, 0xD2800000);
        aml->Write32(pGTASA + 0x4A1258, 0xD2800000);
        aml->Write32(pGTASA + 0x4DF9F0, 0xD2800000);
        aml->Write32(pGTASA + 0x4DFDB4, 0xD2800000);
    }

    // An ability to remove FOV-effect while driving a car
    if(cfg->GetBool("NoVehicleFOVEffect", false, "Gameplay"))
    {
        aml->PlaceNOP(pGTASA + 0x4A025C, 1);
        aml->PlaceNOP(pGTASA + 0x4A02A4, 1);
        aml->PlaceNOP(pGTASA + 0x4A02C8, 1);
    }

    // Disable GTA vehicle detachment at rotation awkwardness
    if(cfg->GetBool("FixVehicleDetachmentAtRot", true, "Visual"))
    {
        aml->PlaceB(pGTASA + 0x4EA3B8, pGTASA + 0x4EA5D0);
    }

    // Now all vehicles should have a shadow
    if(cfg->GetBool("FixVehicleShadows", true, "Visual"))
    {
        aml->PlaceNOP(pGTASA + 0x6DDEFC);
        aml->PlaceNOP(pGTASA + 0x6DDF04);
        aml->PlaceNOP(pGTASA + 0x6DE028);
        aml->PlaceB(pGTASA + 0x6DDF14, pGTASA + 0x6DDFDC);
        HOOKPLT(TrainPreRender, pGTASA + 0x83D0E8);
        HOOKPLT(BoatPreRender, pGTASA + 0x83C330);
    }

    // That's for the fix above. Cuz they're ugly as hell by default, EWWW...
    if(cfg->GetBool("BiggerCarShadowsDistance", true, "Visual"))
    {
        aml->Write32(pGTASA + 0x6DDFE0, 0x900002E9);
        aml->Write32(pGTASA + 0x6DDFE4, 0xBD418922);
        aml->Write32(pGTASA + 0x6DDFE8, 0x1E204041);
    }

    // Drawing shadows as it's classic shadows for really far vehicles
    if(cfg->GetBool("DrawCarShadowsClassicAtDist", true, "Visual"))
    {
        aml->Write32(pGTASA + 0x6DDFFC, 0x5400220A);
    }

    // The fix "PCDirLightsCount" is not gonna work now. So lets remove an optimisation instead.
    if(cfg->GetBool("BiggerLightsCountOutside", true, "Visual"))
    {
        aml->Write32(pGTASA + 0x6F621C, 0x52800075);
        aml->PlaceNOP(pGTASA + 0x6F6220, 1);
    }

    // Missing effects that are on PC but not on Mobile (from SkyGFX)
    /*if(cfg->GetBool("CutEffects", true, "Visual"))
    {
        aml->PlaceNOP(pGTASA + 0x6DA9E0, 1);
        HOOKPLT(RenderPostEffects, pGTASA + 0x842CF0);
        HOOK(PostProcess_CCTV, pGTASA + 0x6DA0B4);
        HOOKPLT(RenderEffects_WaterCannons, pGTASA + 0x8426F0);
        HOOKBL(SpeedFX_Raster, pGTASA + 0x6D8E08);
        HOOKB(SpeedFX_RestoreStates, pGTASA + 0x6D8EC4);
    }*/

    // Cant skip drive
    if(cfg->GetBool("BringBackTripSkip", true, "Gameplay"))
    {
        skiptripChangeTex = cfg->GetBool("BringBackSkipButton_Texture", skiptripChangeTex, "Gameplay");
        SET_TO(bDisplayedSkipTripMessage, pGTASA + 0xC20E90);
        HOOKB(UpdateSkip_SkipCanBeActivated, pGTASA + 0x3CEA24);
        HOOKPLT(DrawHud_SkipTrip, pGTASA + 0x848C48);
    }

    // A particles with "check ground" flag are falling through the world
    if(cfg->GetBool("ParticlesJumpOffGround", true, "Visual"))
    {
        HOOKPLT(FXInfoGroundCollide_GetVal, pGTASA + 0x82C6C0);
        bCheckMoreObjects = cfg->GetBool("ParticlesJumpOffGround_CheckObjects", true);
    }

    // OpenGL-related crash in huawei (poop ass phone moment)
    if(cfg->GetBool("FixHuaweiCrash", true, "Gameplay"))
    {
        void* sym;
        if((sym = TryLoadAlphaFunc("libGLESv2.so"))       || (sym = TryLoadAlphaFunc("libGLESv3.so"))    ||
           (sym = TryLoadAlphaFunc("libGLESv2_mtk.so"))   || (sym = TryLoadAlphaFunc("libGLES_mali.so")) ||
           (sym = TryLoadAlphaFunc("libGLES_android.so")) || (sym = TryLoadAlphaFunc("libGLES.so")))
        {
            *(void**)(pGTASA + 0x89A1B0) = sym;
        }
    }

    // Fix
    if(cfg->GetBool("FixCruisingVehiclesSpeed", true, "Gameplay"))
    {
        //HOOKBL(CurvePoint_SpeedFPS, pGTASA + 0x3AD0A4);
        //HOOKBL(CurvePoint_SpeedFPS, pGTASA + 0x3B0F60);
        HOOKPLT(CurvePoint_SpeedFPS, pGTASA + 0x844DA8);
    }
    
    // Fix a dumb Android 10+ RLEDecompress fix crash (that's an issue of TXD tools)
    if(cfg->GetBool("RLEDecompressCrashFix", androidSdkVer < 33, "Gameplay"))
    {
        aml->Write(pGTASA + 0x2858CC, "\x14\x81\x00\x11", 4);
    }

    // Fixes some stupid issues that are caused my WarDrum's dirty hands
    if(cfg->GetBool("FixNearClippingIssues", true, "Visual"))
    {
        aml->Write32(pGTASA + 0x4D8664, 0x1E204120);
    }

    // Fixes that some data in CollisionData is not being set to zero
    if(cfg->GetBool("FixCollisionAllocatingData", true, "Gameplay"))
    {
        HOOK(ColModel_AllocData, aml->GetSym(hGTASA, "_ZN9CColModel12AllocateDataEiiiiib"));
        // Maybe also in CColModel::MakeMultipleAlloc?
    }

    // Fixes a little Rockstar mistake with coronas rendering
    if(cfg->GetBool("FixSomeCoronasRendering", true, "Gameplay"))
    {
        HOOKBL(CoronasRender_Headlight, pGTASA + 0x6C6300);
    }

    // SilentPatchSA: ImpoundGarages
    if(cfg->GetBool("FixImpoundGarages", true, "Gameplay"))
    {
        aml->Write32(pGTASA + 0x3B05BC, 0x97F9C489);
        aml->Write32(pGTASA + 0x3B0674, 0x97F9C45B);
        aml->Write32(pGTASA + 0x3B07BC, 0x97F9C409);
        aml->PlaceNOP(pGTASA + 0x3CE088, 1);
    }

    // SilentPatchSA: Bigger mirrors quality
    if(cfg->GetBool("BiggerMirrorsQuality", true, "Visual"))
    {
        aml->Write32(pGTASA + 0x6E8EB4, 0x52810000);
        aml->Write32(pGTASA + 0x6E8EB8, 0x52808001);

        aml->Write32(pGTASA + 0x6E8ED0, 0x52810000);
        aml->Write32(pGTASA + 0x6E8ED4, 0x52808001);
    }

    // Faml->Write32weird ass glitches with the timer value clamping
    if(cfg->GetBool("FixHighFPSTimer", true, "Gameplay"))
    {
        aml->Write32(pGTASA + 0x5045F4, 0xD00010AB);
        aml->Write32(pGTASA + 0x504600, 0xBD41E161);
    }

    // Allowes gang wars to have a wanted level (like on PC)
    if(cfg->GetBool("AllowGangWarsWantedLvl", true, "Gameplay"))
    {
        aml->PlaceNOP4(pGTASA + 0x4DBB8C, 1);
        aml->PlaceNOP4(pGTASA + 0x4DBB9C, 1);
        aml->PlaceNOP4(pGTASA + 0x4DBBBC, 1);
        aml->PlaceNOP4(pGTASA + 0x4DBBCC, 1);
    }

    // Fixing drunk camera on high FPS
    if(cfg->GetBool("FixDrunkCameraHighFPS", true, "Visual"))
    {
        HOOKPLT(CameraProcess_HighFPS, pGTASA + 0x8429D8);
    }

    // When headlights are active, the windows are no longer transparent from one side.
    if(cfg->GetBool("FixWindowsCullingWithHeadlights", true, "Visual"))
    {
        aml->Write32(pGTASA + 0x6B4B00, 0x52800021);
    }

    // SilentPatch: Skimmer 20th Anniversary glitch (doesnt happen for us but set it to PC values)
    if(cfg->GetBool("Skimmer20HandlingLittleFix", true, "Gameplay"))
    {
        aml->Write32(pGTASA + 0x554EDC, 0x290D27E9);
        aml->Write32(pGTASA + 0x554ED0, 0x52A7E68A);
    }

    // Fixes everything related to the player aiming at peds
    if(cfg->GetBool("FixPlayerAimingTasks", true, "Gameplay"))
    {
        aml->PlaceNOP4(pGTASA + 0x5C6F3C, 1);
        aml->PlaceNOP4(pGTASA + 0x653478, 1); // ?
        aml->PlaceNOP4(pGTASA + 0x653668, 1);
        aml->PlaceNOP4(pGTASA + 0x654790, 1);
        aml->PlaceNOP4(pGTASA + 0x41C8D8, 1);
        aml->PlaceNOP4(pGTASA + 0x5C83D4, 1);
        aml->PlaceNOP4(pGTASA + 0x5C3CAC, 1);
    }

    // Now the game shuts opened doors at high speeds with high FPS
    if(cfg->GetBool("FixShutDoorAtHighSpeed", true, "Gameplay"))
    {
        //aml->Write32(pGTASA + 0x6914F8, 0xD0000508);
        //aml->Write32(pGTASA + 0x6914FC, 0xBD488101);
        ShutDoorAtHighSpeed_BackTo = pGTASA + 0x691508;
        aml->Redirect(pGTASA + 0x6914F8, (uintptr_t)ShutDoorAtHighSpeed_Inject);
    }

    // HudColors
    if(cfg->GetBool("PCHudColors", true, "Visual"))
    {
        aml->PlaceRET(pGTASA + 0x252CE4);
    }

    // SkyGFX: Water color fix. You now have a choice to use JPatch if you dont need SkyGFX
    if(cfg->GetBool("WaterColorFix", true, "Visual"))
    {
        aml->PlaceNOP4(pGTASA + 0x6BD1C4, 1);
    }

    // Open top cars are now extended
    if(cfg->GetBool("ExtendOpenTopVehicles", true, "Gameplay"))
    {
        aml->PlaceNOP4(pGTASA + 0x59C7B0, 1);
        HOOK(IsOpenTopAutomobile, aml->GetSym(hGTASA, "_ZNK11CAutomobile12IsOpenTopCarEv"));
        HOOKPLT(IsOpenTopVehicle, pGTASA + 0x83BF80);
        HOOKPLT(IsOpenTopVehicle, pGTASA + 0x83C1B0);
        HOOKPLT(IsOpenTopVehicle, pGTASA + 0x83C3E0);
        HOOKPLT(IsOpenTopVehicle, pGTASA + 0x83D198);
        HOOKPLT(IsOpenTopVehicle, pGTASA + 0x83D3C0);

        g_bOpenTopQuadBike = cfg->GetBool("ExtendOpenTopVehicles_QuadBike", true, "Gameplay");
        g_bOpenTopBoats = cfg->GetBool("ExtendOpenTopVehicles_Boats", true, "Gameplay");
        g_bOpenTopVortex = cfg->GetBool("ExtendOpenTopVehicles_Vortex", true, "Gameplay");
        g_bOpenTopKart = cfg->GetBool("ExtendOpenTopVehicles_Kart", true, "Gameplay");
    }

    // Dont set player on fire when he's on burning BMX (MTA:SA)
    if(cfg->GetBool("DontBurnPlayerOnBurningBMX", true, "Gameplay"))
    {
        aml->Write32(pGTASA + 0x4D3F04, 0x8B1B0388);
        aml->Write32(pGTASA + 0x4D3F08, 0xF942D100);
        aml->Write32(pGTASA + 0x4D3F10, 0x8B1B0388);
        aml->Write32(pGTASA + 0x4D3F14, 0xF942D100);
        HOOK(Fire_DoStuffToGoOnFire, aml->GetSym(hGTASA, "_ZN10CPlayerPed17DoStuffToGoOnFireEv"));
        HOOK(Fire_StartFire, aml->GetSym(hGTASA, "_ZN12CFireManager9StartFireEP7CEntityS1_fhja"));
    }

    // Optimise textures searching
    if(cfg->GetBool("OptimiseTextureSearching", true, "Gameplay"))
    {
        HOOKPLT(HashStringOpt, pGTASA + 0x83F930);
        // TextureDatabase::LoadEntry
        aml->Write32(pGTASA + 0x283D1C, 0x14000009);
        aml->Write32(pGTASA + 0x283D40, 0x97FE64F0);
        aml->Write32(pGTASA + 0x283D44, 0x2A0003FC);
        // TextureDatabase::LoadEntries
        aml->Write32(pGTASA + 0x284478, 0x97FE6322);
        aml->Write32(pGTASA + 0x28447C, 0x2A0003FB);
        aml->Write32(pGTASA + 0x284480, 0x14000008);
        aml->Write32(pGTASA + 0x2844AC, 0xD503201F);
    }

    // The vehicle doesnt shake like a crazy on high FPS
    if(cfg->GetBool("FixChassisSwingingHighFPS", true, "Gameplay"))
    {
        ChassisSwingAngle_BackTo = pGTASA + 0x67B744;
        aml->Redirect(pGTASA + 0x67B730, (uintptr_t)ChassisSwingAngle_Inject);
    }

    // SilentPatch: Fixed an AI issue where enemies became too accurate after the player had been in the car earlier
    if(cfg->GetBool("SP_AIAccuracyAfterVehicle", true, "Gameplay"))
    {
        AIAccuracyAfterVehicle_BackTo_Next = pGTASA + 0x704A88;
        AIAccuracyAfterVehicle_BackTo_Continue = pGTASA + 0x704B04;
        aml->Redirect(pGTASA + 0x704A74, (uintptr_t)AIAccuracyAfterVehicle_Inject);
    }

    // SilentPatch: Bilinear filtering for license plates
    if(cfg->GetBool("SP_BilinearFilterForPlate", true, "Visual"))
    {
        aml->Write32(pGTASA + 0x6C8D2C, ARMv8::MOVBits::Create(2, 8, false));
    }

    // SilentPatch: Don't clean the car BEFORE Pay 'n Spray doors close
    if(cfg->GetBool("SP_DontEarlyCleanTheCar", true, "Gameplay"))
    {
        aml->PlaceNOP4(pGTASA + 0x3D6A24, 1);
    }

    // SilentPatch: Spawn lapdm1 (biker cop) correctly if the script requests one with PEDTYPE_COP
    if(cfg->GetBool("SP_AllowCopBikerPedFromScript", true, "Gameplay"))
    {
        HOOKPLT(GetCorrectPedModelIndexForEmergencyServiceType, pGTASA + 0x847528);
    }

    // SilentPatch: Fix the logic behind exploding cars losing wheels
    if(cfg->GetBool("SP_FixExplodedCarWheels", true, "Visual"))
    {
        aml->Write32(pGTASA + 0x690ED8, 0x1E221001);
        aml->PlaceNOP4(pGTASA + 0x67D5A4, 1);
        aml->PlaceNOP4(pGTASA + 0x67D988, 1);
        aml->PlaceNOP4(pGTASA + 0x696BB0, 1);
        aml->PlaceNOP4(pGTASA + 0x69D880, 1);
        HOOK(FixWheelVisibility_SpawnFlyingComponent, aml->GetSym(hGTASA, "_ZN11CAutomobile20SpawnFlyingComponentEij"));
    }

    // Allow more wheels to be exploded instead of only ONE RANDOM
    if(cfg->GetBool("AllowMultipleWheelsBeExploded", true, "Visual"))
    {
        aml->PlaceNOP4(pGTASA + 0x690EF0, 1);
        HOOK(FlickCarCompletely, aml->GetSym(hGTASA, "_ZN14CDamageManager17FuckCarCompletelyEb"));
    }

    // SilentPatch: Disable building pipeline for skinned objects (like parachute)
    if(cfg->GetBool("SP_FixSkinnedObjectsPipeline", true, "Visual"))
    {
        HOOKPLT(SetupDNPipeline, pGTASA + 0x83E968);
    }

    // Fix widget's holding radius
    if(cfg->GetBool("WidgetHoldingMaxShift", true, "Gameplay"))
    {
        WidgetUpdateHold_BackTo = pGTASA + 0x371F54;
        aml->Redirect(pGTASA + 0x371F44, (uintptr_t)WidgetUpdateHold_Inject);
    }

    // Fix cutscene FOV (disabled by default right now, causes the camera being too close on ultrawide screens)
    if(cfg->GetBool("FixCutsceneFOV", false, "Visual"))
    {
        HOOKPLT(SetFOV, pGTASA + 0x846898);
    }

    // Disable call to FxSystem_c::GetCompositeMatrix in CAEFireAudioEntity::UpdateParameters 
    // that was causing a crash - spent ages debugging, the crash happens if you create 40 or 
    // so vehicles that catch fire (upside down) then delete them, repeating a few times.
    // (MTA:SA)
    if(cfg->GetBool("GetCompositeMatrixFixPossibleCrash", true, "Gameplay"))
    {
        aml->PlaceNOP4(pGTASA + 0x470EF8, 6);
    }

    // Buff streaming
    if(cfg->GetBool("BuffStreamingMem", true, "Gameplay"))
    {
        int wantsMB = cfg->GetInt("BuffStreamingMem_CountMB", 384, "Gameplay");
        if(wantsMB >= 20)
        {
            aml->PlaceNOP4(pGTASA + 0x55774C, 1);
            aml->PlaceNOP4(pGTASA + 0x55EE54, 1);
            aml->PlaceNOP4(pGTASA + 0x55EE6C, 1);
            if(*ms_memoryAvailable < wantsMB*1024*1024)
            {
                *ms_memoryAvailable = wantsMB * 1024 * 1024;
            }
        }
    }

    // RE3: Correct clouds rotating speed
    if(cfg->GetBool("Re3_CloudsRotationHighFPS", true, "Visual"))
    {
        HOOKPLT(CloudsUpdate_Re3, pGTASA + 0x840968);
    }

    // RE3: Free the space for an object in a pool by deleting temp objects if there is no space
    if(cfg->GetBool("Re3_FreePlaceInObjectPool", true, "Gameplay"))
    {
        HOOKPLT(Object_New, pGTASA + 0x83FA78);
    }

    // FixRadar: Radar outline is not forced to be in pure black color
    if(cfg->GetBool("FixRadarOutlineColor", true, "Visual"))
    {
        aml->Write32(pGTASA + 0x51D450, ARMv8::MOVBits::Create(0xFF, 1, false));
        aml->Write32(pGTASA + 0x51D454, ARMv8::MOVBits::Create(0xFF, 2, false));
        aml->Write32(pGTASA + 0x51D468, ARMv8::MOVBits::Create(0xFF, 3, false));
        aml->Write32(pGTASA + 0x51D4A0, ARMv8::MOVBits::Create(0xFF, 1, false));
        aml->Write32(pGTASA + 0x51D4A4, ARMv8::MOVBits::Create(0xFF, 2, false));
        aml->Write32(pGTASA + 0x51D4A8, ARMv8::MOVBits::Create(0xFF, 3, false));
        aml->Write32(pGTASA + 0x51D4D4, ARMv8::MOVBits::Create(0xFF, 1, false));
        aml->Write32(pGTASA + 0x51D4D8, ARMv8::MOVBits::Create(0xFF, 2, false));
        aml->Write32(pGTASA + 0x51D4DC, ARMv8::MOVBits::Create(0xFF, 3, false));
        aml->Write32(pGTASA + 0x51D504, ARMv8::MOVBits::Create(0xFF, 1, false));
        aml->Write32(pGTASA + 0x51D508, ARMv8::MOVBits::Create(0xFF, 2, false));
        aml->Write32(pGTASA + 0x51D50C, ARMv8::MOVBits::Create(0xFF, 3, false));
    }
    
    // Re-implement idle camera like on PC/PS2
    /*if(cfg->GetBool("IdleCamera", true, "Gameplay"))
    {
        InitIdleCam(gIdleCam);

        ProcessCamFollowPed_BackTo1 = pGTASA + 0x3C4C52 + 0x1; // reset idle cam // fix
        ProcessCamFollowPed_BackTo2 = pGTASA + 0x3C4B84 + 0x1; // skip after patch 1 // fix
        ProcessCamFollowPed_BackTo3 = pGTASA + 0x3C4BBE + 0x1; // skip after patch 2 // fix
        aml->Redirect(pGTASA + 0x3C4B7C + 0x1, (uintptr_t)ProcessCamFollowPed_IdleCam1); // fix
        aml->Redirect(pGTASA + 0x3C4BB2 + 0x1, (uintptr_t)ProcessCamFollowPed_IdleCam2); // fix

        aml->PlaceNOP4(pGTASA + 0x49EF10, 1);
        HOOK(CamProcess_IdleCam, pGTASA + 0x49EEBC);
        HOOKPLT(DrawAllWidgets, pGTASA + 0x83D9A0);

        // Speed 3.0f is like on PC: 1,5 minutes
        // Some weird glitch exists i dont wanna deal with...
        float idleCamSpeed = cfg->GetFloat("IdleCameraStartSpeed", 3.0f, "Gameplay");
        if(idleCamSpeed < 0.34f) idleCamSpeed = 0.34f; // min 10 seconds
        else if(idleCamSpeed > 20.0f) idleCamSpeed = 20.0f; // max 10 minutes
        gIdleCam->timeControlsIdleForIdleToKickIn = idleCamSpeed * 90000.0f;
    }*/

    









    // Skip that dumb EULA. We accepted it years ago, shut up
    /*if(cfg->GetBool("SkipAnnoyingEULA", true, "Gameplay"))
    {
        aml->Write8(aml->GetSym(hSC, "LegalScreenShown"), 0x01);
    }*/

    // This fixes black bushes and more things
    //if(cfg->GetBool("FixCamNormColorOverflow", true, "Visual"))
    {
        //HOOKBL(VTXShader_CamBasedNormal_snprintf, pGTASA + 0x264944);
    }

    // Searchlights are too fast... (TODO: untested lol, there's no sqrtf at these addresses)
    /*if(cfg->GetBool("FixSearchlightHighFPS", true, "Gameplay"))
    {
        HOOKBL(SearchLight_sqrtf, pGTASA + 0x425C2C);
        HOOKBL(SearchLight_sqrtf, pGTASA + 0x425D5C);
        HOOKBL(SearchLight_sqrtf, pGTASA + 0x425CA0);
        HOOKBL(SearchLight_sqrtf, pGTASA + 0x425CF8);
    }*/
    
    // Fixes a weird glitch from there: https://github.com/multitheftauto/mtasa-blue/issues/1123
    //if(cfg->GetBool("MTA_FixProjectiles", true, "Gameplay"))
    //{
    //    aml->PlaceB(pGTASA + 0x6FEB6C, pGTASA + 0x6FEC84);
    //}

    // Camera is saving screenshots?
    /*if(cfg->GetBool("FixScreenGrabber", true, "Gameplay"))
    {
        // 821F20

        HOOK(Patch_psGrabScreen, aml->GetSym(hGTASA, "_Z12psGrabScreenP8RwCamera"));
        //HOOKBL(TakePhoto, pGTASA + 0x7010EC);
        //HOOKBL(ShowRasterIdle, pGTASA + 0x4D8FE0);
        HOOKBL(ShowRasterIdle, pGTASA + 0x4D9254);
        HOOKBL(jpeg_samplecopy, pGTASA + 0x56A0E8);
        HOOKBL(jpeg_samplecopy, pGTASA + 0x56A1EC);
    }*/
