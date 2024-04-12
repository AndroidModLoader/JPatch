[![AML Mod Compiler](https://github.com/AndroidModLoader/JPatch/actions/workflows/main.yml/badge.svg?branch=main)](https://github.com/AndroidModLoader/JPatch/actions/workflows/main.yml)

# What am i?
 I am a little mod that wants to fix your game!
 Need a preview version of upcoming changes? Get it there: https://github.com/AndroidModLoader/JPatch/actions

 Currently i can fix those things:
 
### ***GTA:VC v1.09***
**Planned/TODO (not ready or broken now)**
 - The fog is back (the water shader is broken???)
 - Pushing vehicles on a high FPS (just doesnt work.)
 - Fix fighting range
 - Probably, fix sprint speed?
 - Not sure, maybe timer-related bugs in save files

**Since v1.5.1**
 - Bringing back a framelimiter (may be inaccurate)
 - Water texture scrolling speed fix on high FPS
 - Fixed incorrect physics target (fixes small physical glitches with the vehicles, but also probably creates some)
 - Clouds rotating speed on high FPS (it is few times faster than it should be)
 - Allows the game to choose Extra6 component on a vehicle when created
 - Bringing back the fake crater from explosion
 - Correct render flag for Dodo Banner (does it work?)
 - Corona sprites draw distance is 3 times bigger (partially, traffic lights do work)
 - Static light shadows (from traffic lights, poles and more) draw distance is 3 times bigger
 - Bumped a limit of static shadows!
 - Fixing a crash of "Unprotected" APK (FLA bug)

**Since v1.5**
 - Vertex Weights
 - Fixed boat speed on high FPS
 - Fix wheels rotation speed on high FPS
 - Fix car slowdown on high FPS
 - Fix heli rotor rotation speed on high FPS
 - Streaming distance fix
 - Wide coronas/sprites fix (this fix is disabled if you have ViceSkies v1.0)
 - RE3: Fix R* optimization that prevents peds to spawn
 - RE3: Road reflections
 - Fix traffic lights
 
### ***GTA:SA v2.10***
**Since v1.5.2**
 - Colored zone names are back

**Since v1.5.1**
 - **Crosshair should be placed at the correct place**
 - Aiming with Country Rifle is now in 3rd person

**Since v1.5**
 - Animated textures (UVAnim)
 - Vertex Weights
 - Swimming speed
 - Fixed primary color resetting to the white after doing paintjob
 - Fixed sky multitude (a glitch on high speeds)
 - Fixed vehicles backlight light state (should do vehicle tail light effect when ANY of the lights are broken)
 
### ***GTA:SA v2.00***
**Planned/TODO (not ready or broken now)**
 - Fix camera zoom is not that big (aspect ratio?)
 - FoodEatingFix by JuniorDjjr
 - Moon phases (probably far away...)
 - Doubled the directional light sources on models (to match the PC version) <- Shaders thingo :(
 - Cant use some weapons (fire ex., spray) while the car-enter widget is active (read: near the vehicle)
 - Fix opcode 08F8 that still shows "Updated stats" pop-up no matter what

**Since v1.5.2**
 - Falling star is now not a black line
 - Unused detonator animation fix can be enabled (you need to put animations in your game files like on PC!)
 - Jetpack hovering is now "lockable"
 - Planes now have a toggleable smoke/dust (thanks BengbuGuards!)
 - Wrong vehicle parts colors (thanks B1ack_&_Wh1te!)

**Since v1.5.1**
 - Fixed a small FX system leakage when using custom effects mod
 - Rewrote cheats hashes, check **GTASA_CHEATS.md** file for more info
 - BengbuGuards: Implement jetpack hover button
 - **AFK/Idle camera!**

**Since v1.5**
 - BengbuGuards: Second siren is now working
 - BengbuGuards: An animation fix for boats
 - BengbuGuards: Player's plane won't disappear after it explodes
 - CJ is no longer breathing with his ass underwater
 - Directional sunlight on entities (vehicles, peds)
 - Lamppost coronas are now rotating like on PS2
 - Vehicles license plates now random
 - Car generators in an interior now work properly
 - "You have worked out enough for today, come back tomorrow!" glitch is fixed
 - Fixing a wrong value in carcols.dat (a mistakenly placed dot instead of comma, # 98 malachite poly)
 - Inverse swimming controls to dive/go up (to match PC version)
 - Fix for the wrong position (from the beta) of Bravura in a mission "You've Had Your Chips"
 - Michelle dating fix
 - Stealth kill with a knife does NOT require to be crouched anymore (as on PC)
 - RLEDecompress crash is fixed! (allocates more memory, for some reason Android is failing in allocating small memory chunks)

**Since v1.4.3**
 - Fixed coronas stretching while the weather is foggy
 - SCM fix by JuniorDjrr for "IsPedDead" (broken sometimes?)
 - Fixed water physics (includes swimming speed fix)
 - Sprint button after aiming and dropping to the water is always visible
 - Parachute landing animation fix
 - FX particles draw distance scaler (default is x2.5)
 - Boat radio antenna animation
 - MixSets: Crouch with any weapons

**Since v1.4.2.2**
 - Fixed wall behind the fog (use default shaders!)

**Since v1.4.2.1**
 - Fixed broken CrosshairFix after v1.4.2, fix for Country Rifle
 - Changed the values of CrosshairFix so it's more correct
 - Fixed the Country Rifle camera in 3rd person

**Since v1.4.2**
 - Removed "Please rate the game" message
 - Removed IDE flag "ExtraAirResistance"
 - Implement NoRadioCuts
 - Smaller grenades collision
 - MTA: Fixed a glitch with projectiles (see https://github.com/multitheftauto/mtasa-blue/issues/1123)
 - MTA: Fix slippery floor (improved in v1.4.3)
 - Allow license plates to render on all vehicles
 - Fix a little mistake that doesnt let player to crouch if he's ~1 HP
 - Show muzzle flash for the last bullet
 - Disable a flag to allow shadows to be drawed on all surfaces
 - Weapon spread fix

**Since v1.4.1**
 - Better ForceDXT
 - Vehicle sun glare
 - Wet roads reflections
 
**Since v1.4**
 - Finally fixed BuffDistForLightCoronas!
 - Fixed peds are not speaking closer to the game end
 - Always draw wanted stars (disabled by default)
 - Rotor blur *by Peepo*
 - Cloud saves can be disabled (NOT disabled by default)
 - Always drawable wanted stars (disabled by default)
 - Fixed PED preloading (for Improved Streaming, disabled by default)
 - A count of max loading screens can be changed now (default is 7)
 - Fixed ped counting overflow for gang members calculating
 - Sprinting on any surface is allowed
 - Camera/Sniper zooming is faster and can be adjusted to be even faster!
 - Minimal FOV for Camera is adjustable (default is 70, set it to 90 or bigger if you need "fish eye" in a camera)

 **Since v1.3**
 - VitaPort: Fixed cheats hashtable
 - Cheats can now be entered using keyboard
 - Fix save loading crash (may work... no save with that glitch :( )
 - Fix Adjustable.cfg sizes lowering on smartphones with an aspect ratio bigger than 16:9 (has a lil glitch with x position shifting)
 - Taxi lights are activated
 - No minimap in interiors
 - PC-like money in HUD (disabled by default)
 - Aiming with Country Rifle is now in 3rd person
 - Force the game's inner check always return that we have only touch (disabled by default)
 - Fixed ped animation after being attacked with parachute (inspired *by Alexander Polyak*)
 - MixSets: Hostile gangs
 - Adjustable detailed water draw distance (known as water quadrant fix)
 - Peepo: Traffic lights fix
 
 **Since v1.2.4**
 - Corona sprites draw distance is 3 times bigger
 - Static shadows are moved to the ground a bit
 - **Radar streaming should be fixed**
 - **Climb-dying glitch SHOULD be fixed**
 - Do not use bias in shaders, probably makes the game faster
 - Re-fixed emergency vehicles disappearing
 - Fixed bikes generator at driving school (for new players)
 - **Crosshair should be placed at the correct place**
 
 **Since v1.2.3**
 - Goddamn static light shadows from PC!
 - Bumped a limit of static shadows!
 - Fixed green textures (moved from SkyGfx Mobile)
 - Static light shadows (from traffic lights, poles and more) draw distance is 3 times bigger
 
 **Since v1.2.1**
 - Remove stupid specular from peds
 - Include objects in checks of FindGroundZ functions (may fix opcode 02CE after using 0107)
 - Automatical streaming memory buff if not enough for the game
 - Fixed (probably?) an annoying glitch with colorpicker in Tenderfender
 - ImprovedStreaming: Preload LODs (causes the game to freeze for a few seconds)
 - ImprovedStreaming: Preload animations (disabled by default)
 - ImprovedStreaming: Unloading of unused entities
 
 **Since v1.2**
 - Give more space for opcodes 038D+038F
 - Lower threads sleeping timer
 - Dont kill peds when jacking their car, monster!
 - Colored zone names are back
 - Tell the game we allow it to create more peds
 - Tell the game we allow it to create more cars
 - re3: Fix R\* optimization that prevents peds from spawn
 - re3: Make cars and peds to not despawn when you look away
 - re3: Do not remove locked cars
 - re3: Correct clouds rotating speed
 - re3: Multiple instances of semaphore fix (may be not working or useless)
 - re3: Fix a little mistake by R\* in AskForObjectToBeRenderedInGlass
 - re3: Free the space for an object by deleting temporary objects from pool if not enough space
 
 **Since v1.1.5**
 - Hotfix of experimental fix of 034E opcode
 - Classic CJ's shadow
 - FramerateVigilante: CarSlowDownSpeedFix
 - FramerateVigilante: HeliRotorIncreaseSpeed
 
 **Since v1.1.4**
 - Hotfix of Skimmer plane
 
 **Since v1.1.3**
 - Bring back a missing shoot button on S.W.A.T.
 - Fuzzy seek (without a mod)
 - Fixed water cannons (firetruck/S.W.A.T.) on high FPS
 - An experimental fix of 034E opcode (move object) on high FPS
 - FramerateVigilante: Fixed physical objects push force
 - VitaPort: Fixed Heli/Plane camera
 
 **Since v1.1.2**
 - Skimmer Plane can be properly get out of water (XMDS)
 - Streaming memory is buffed now
 - Helis/Planes can now fly higher
 - Jetpack can now fly higher
 - 44100Hz audio support without a mod "OpenAL Update"
 - MTA: Disable GTA vehicle detachment at rotation
 
 **Since v1.1.1**
 - Dont burn player that are driving burning bmx
 - A fix for possible crash from GetCompositeMatrix (may be useless)
 - Cinematic camera when double tapping the camera mode switch button
 - MTA: Increase intensity of vehicle tail light corona
 - MTA: Disable setting the occupied's vehicles health to 75.0f when a burning ped enters it
 - MTA: Thrown projectiles throw more accurately
 
 **Since v1.1**
 - Fixed emergency vehicles
 - Moved save pickup in Madd Dogg's Mansion
 - Moved a bribe to the broken bridge from SF building
 - Moved a rifle pickup outside of the SF Stadium wall
 - Fixed cutscene FOV (disabled by default, incomplete)
 - Fixed sky multitude (a glitch on high speeds)
 - Fixed vehicles backlight light state (should do vehicle tail light effect when ANY of the lights are broken)
 - Sand/Dust particles from bullets are limited because they are creating a lot of lag
 - Fixed primary color resetting to the white after doing paintjob
 - Fixed red marker cannot be placed on the right side of the screen
 
 **Since v1.0**
 - Animated textures (UVAnim)
 - Vertex Weights
 - Swimming speed
 - CJ magnetting to stealable objects only when very close
 - Madd Dogg's Basketball glitch fixed by replacing save file
 - CJ can leave any vehicle at high speeds
 - CJ no more can drop JetPack while he's in air
 - Weapon penalty clean-up when CJ dies
