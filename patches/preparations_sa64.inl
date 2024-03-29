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
    //if(cfg->GetBool("LimitSandDustBulletParticles", true, "Visual"))
    {
        
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