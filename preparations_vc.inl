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
    /*if(cfg->GetBool("FixPhysicalPushForce", true, "Gameplay"))
    {
        ApplyCollision_BackTo = pGTAVC + 0x167588 + 0x1;
        aml->Redirect(pGTAVC + 0x167580 + 0x1, (uintptr_t)ApplyCollision_Inject);
        //HOOKBL(ApplyCollision_MoveForce, pGTAVC + 0x166C60 + 0x1);
        //HOOKBL(ApplyCollision_TurnForce, pGTAVC + 0x166C84 + 0x1);
    }*/
    
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
    }