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
        HOOK(TrFix_RenderEffects, aml->GetSym(hGTASA, "_Z13RenderEffectsv"));
        HOOK(TrFix_InitGame2nd, aml->GetSym(hGTASA, "_ZN5CGame5Init2EPKc"));
    }
    
    // Water Quadrant
    int dist = cfg->GetInt("DetailedWaterDrawDistance", 48 * 3, "Visual");
    if(dist > 0)
    {
        if(dist < 24) dist = 24;
        else if(dist <= 48 * 3) dist = 48 * 3;
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
    if(fxMultiplier != 1 && fxMultiplier > 0.1)
    {
        aml->Unprot(pGTASA + 0x744490, sizeof(float));
        SET_TO(fxCreateParticlesFloat, pGTASA + 0x744490);
        HOOK(CreateParticles_FxMult, pGTASA + 0x438D64);
        
        aml->Unprot(pGTASA + 0x740AE4, sizeof(float));
        SET_TO(fxUpdateFloat, pGTASA + 0x740AE4);
        HOOKBL(FxUpdate_FxMult, pGTASA + 0x43E054);
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
        HOOKPLT(DrawRadar, pGTASA + 0x66F618);
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
        HOOKBL(GetCustomCarPlateText_SetModelIdx, pGTASA + 0x5829F2 + 0x1);
    }

    // Fix "You have worked out enough for today, come back tomorrow!"
    if(cfg->GetBool("FixGymWorkoutDate", true, "SCMFixes"))
    {
        Opcode0835_BackTo = pGTASA + 0x3378CE + 0x1;
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
        HOOKBL(TaskSwim_ProcessInput, pGTASA + 0x6562C8);
        HOOKBL(GetPedWalkUpDown_Swimming, pGTASA + 0x656A1C);
        aml->Write32(pGTASA + 0x6571F4, 0xD000072A);
        aml->Write32(pGTASA + 0x6571F8, 0xBD4E9D44);
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
    }
    
    // Fix Skimmer plane
    if (cfg->GetBool("SkimmerPlaneFix", true, "Gameplay"))
    {
        SkimmerWaterResistance_BackTo = pGTASA + 0x6AD4D4;
        aml->Redirect(pGTASA + 0x6AD4C4, (uintptr_t)SkimmerWaterResistance_Inject);
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
    
    // Fixes a weird glitch from there: https://github.com/multitheftauto/mtasa-blue/issues/1123
    if(cfg->GetBool("MTA_FixProjectiles", true, "Gameplay"))
    {
        aml->PlaceB(pGTASA + 0x6FEB6C, pGTASA + 0x6FEC84);
    }

    // Car generators in an interior now work properly
    if(cfg->GetBool("CorrectPoliceScannerLocations", true, "Gameplay"))
    {
        aml->Write(pGTASA + 0x74AC98, "WESTP", 6);
        aml->Write(pGTASA + 0x74AD40, "????", 5);
    }