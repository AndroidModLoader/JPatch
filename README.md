[![AML Mod Compiler](https://github.com/AndroidModLoader/JPatch/actions/workflows/main.yml/badge.svg?branch=main)](https://github.com/AndroidModLoader/JPatch/actions/workflows/main.yml)

![jpatch_mini](https://github.com/AndroidModLoader/JPatch/assets/8864329/5e273283-baad-4f2c-8697-f75040991f64)

# What am i?
 I am a little mod that wants to fix your game!
 Need a preview version of upcoming changes? Get it there: https://github.com/AndroidModLoader/JPatch/actions

 Currently i can fix those things:

### ***GTA:VC v1.12***
**Since v1.8.2**
 - A small optimisation for hashing functions (slightly speed ups loading)

**Since v1.8.1**
 - Fixing drunk camera on high FPS
 
**Since v1.8**
 - GLAlphaFunc mod is now included (fixing a crash on Huawei phones)
 - Fixed boat speed on high FPS
 - Fix wheels rotation speed on high FPS
 - Fix car slowdown on high FPS
 - Fix heli rotor rotation speed on high FPS
 - Streaming distance fix (it's incomplete!)
 - Wide coronas/sprites fix
 - RE3: Fix R* optimization that prevents peds to spawn
 - RE3: Road reflections
 - Fix traffic lights
 - Bringing back a framelimiter (may be inaccurate)
 - Water texture scrolling speed fix on high FPS
 - Clouds rotating speed on high FPS (it is few times faster than it should be)
 - Bringing back the fake crater from explosion
 - Allows the game to choose Extra6 component on a vehicle when created
 - Corona sprites draw distance is 3 times bigger (partially, traffic lights do work)
 - Static light shadows (from traffic lights, poles and more) draw distance is 3 times bigger
 - Correct render flag for Dodo Banner (does it work?)
 - Bumped a limit of static shadows!
 - FX particles draw distance scaler (default is x2.5)
 - Now it's possible to remove ZERO's from money text
 - 44100Hz audio support without a mod "OpenAL Update"
 
### ***GTA:VC v1.09***
**Planned/TODO (not ready or broken now)**
 - The fog is back (the water shader is broken???)
 - Pushing vehicles on a high FPS (just doesnt work.)
 - Fix fighting range
 - Probably, fix sprint speed?
 - Not sure, maybe timer-related bugs in save files

**Since v1.8.2**
 - A small optimisation for hashing functions (slightly speed ups loading)

 **Since v1.8.1**
 - Fixing drunk camera on high FPS

**Since v1.8**
 - GLAlphaFunc mod is now included (fixing a crash on Huawei phones)
 - Now it's possible to remove ZERO's from money text
 - 44100Hz audio support without a mod "OpenAL Update"

**Since v1.7**
 - FX particles draw distance scaler (default is x2.5)

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

### ***GTA:SA v2.10 (64-bit)***
**Since v1.8.2**
 - A small optimisation for textures loading speed (hashing function)
 - Fixed crazy vehicle shaking (swinging chassis) on high FPS
 - SilentPatch: Fixed an AI issue where enemies became too accurate after the player had been in the car earlier
 - SilentPatch: Bilinear filtering for license plates
 - SilentPatch: Don't clean the car BEFORE Pay 'n Spray doors close
 - SilentPatch: Spawn lapdm1 (biker cop) correctly if the script requests one with PEDTYPE_COP
 - SilentPatch: Fix the logic behind exploding cars losing wheels
 - Allow more wheels to be exploded instead of only ONE RANDOM
 - SilentPatch: Disable building pipeline for skinned objects (like parachute)
 - A little mistake by WarDrum related to holding a finger on widgets (thanks AliAssassiN!)
 - Fixed cutscene FOV (disabled by default, incomplete, black borders are better?)
 - A fix for possible crash from GetCompositeMatrix (might be useless)
 - Streaming memory is buffed now
 - re3: Correct clouds rotating speed
 - re3: Free the space for an object by deleting temporary objects from pool if not enough space
 - FixRadar: Radar outline is not forced to be in pure black color

**Since v1.8.1**
 - SilentPatch: Impound garages
 - SilentPatch: Bigger mirror quality on medium-high settings
 - Fixed a glitch with the timer on high FPS
 - Allowes gang wars to have a wanted level (like on PC)
 - Fixing drunk camera on high FPS
 - When headlights are active, the windows are no longer transparent from one side
 - Returned back the game logic related to player aiming at peds
 - Now the game shuts opened doors at high speeds with high FPS
 - HUD colors are not brighter anymore
 - Water color fix (from SkyGFX)
 - Dont burn player that are driving burning BMX
 - Enabled CJ's clothes response to the wind in some vehicles

**Since v1.8**
 - Able to skip "drive" (tripskip)
 - Fixed "collideable" particles such as gunshells (reversed from FxsFuncs)
 - GLAlphaFunc mod is now included (fixing a crash on Huawei phones)
 - RLEDecompress crash is fixed! (allocates more memory)
 - Fixes some stupid nearclipping issues that are caused by WarDrum's dirty hands
 - Fixes that some data in CollisionData is not being set to zero
 - Fixes a little Rockstar mistake with coronas rendering

**Since v1.7.3**
 - Not only airplanes but also trains and boats have shadows
 - If car is far enough, shadow will be classic (instead of NO SHADOWS)
 - An experimental fix for crazy cruising speed for vehicles
 - Some other issues are fixed now... duh

**Since v1.7.2**
 - Another crash fix... I hate myself... *facepalm*
 - A count of max loading screens can be changed now (default is 7)
 - Probably fixed walking AND aiming on high FPS
 - Fixed water cannons (firetruck/S.W.A.T.) on high FPS
 - An experimental fix of 034E opcode (move object) on high FPS
 - FramerateVigilante: Fixed physical objects push force
 - MTA: Increase intensity of vehicle tail light corona
 - MTA: Thrown projectiles throw more accurately
 - Fixed red marker cannot be placed on the right side of the screen
 - VitaPort: Fixed Heli/Plane camera
 - An ability to remove FOV-effect while driving a car (NoVehicleFOVEffect, not applied by default)
 - MTA: Disable GTA vehicle detachment at rotation
 - Now all airplanes have shadows
 - **Cars (only cars because of a limit)** now have a bigger shadow draw distance

**Since v1.7.1**
 - Goddamn static light shadows from PC!
 - Bumped a limit of static shadows!
 - Static light shadows (from traffic lights, poles and more) draw distance is 3 times bigger
 - Fix accidentally broken "Colorpicker widget". I am sorry.

**Since v1.7**
 - **Fix broken Swimming Speed Fix** (bruh)
 - CJ is now able to exit vehicle without closing the door (https://github.com/AndroidModLoader/JPatch/issues/28)
 - Removed "Please rate the game" message
 - Removed IDE flag "ExtraAirResistance"
 - Implement NoRadioCuts
 - Sprinting on any surface is allowed
 - Fix traffic lights
 - Adjustable detailed water draw distance (known as water quadrant fix)
 - PC-like money in HUD (disabled by default)
 - **Climb-dying glitch SHOULD be fixed**
 - RE3: Road reflections
 - Tell the game we allow it to create more peds
 - Tell the game we allow it to create more cars
 - re3: Fix R\* optimization that prevents peds from spawn
 - Fuzzy seek (the game does not load useless data from audio)
 - 44100Hz audio support without a mod "OpenAL Update"
 - Fixed emergency vehicles (FixStreamingDistScale)
 - Fixed an annoying glitch with colorpicker in Tenderfender
 - The camera is not rotating like a crazy when mouse/keyboard is connected (thanks AliAssassiN!)
 - The camera buffer is getting properly cleared. This is fixing "ghosting" underworld (thanks AliAssassiN!)
 - Lamppost coronas are now rotating like on PS2
 - Directional sunlight on entities (vehicles, peds)
 - CJ is no longer breathing with his ass underwater
 - FX particles draw distance scaler (default is x2.5)
 - Fixed coronas stretching while the weather is foggy
 - BengbuGuards: Second siren is now working
 - Weapon spread fix
 - Disable a flag to allow shadows to be drawed on all surfaces
 - Fix a little mistake that doesnt let player to crouch if he's ~1 HP
 - Vehicle sun glare
 - No minimap in interiors
 - Fixed green textures (moved from SkyGFX Mobile)
 - **Radar streaming should be fixed**
 - Allow license plates to render on all vehicles
 - Show muzzle flash for the last bullet
 - Vehicles license plates now random
 - "You have worked out enough for today, come back tomorrow!" glitch is fixed
 - Michelle dating fix
 - Parachute landing animation fix
 - Unused detonator animation is working
 - Taxi lights are activated
 - Fix Adjustable.cfg sizes lowering on smartphones with an aspect ratio bigger than 16:9 (has a lil glitch with x position shifting)
 - Classic CJ's shadow
 - FramerateVigilante: CarSlowDownSpeedFix
 - Skimmer Plane can be properly get out of water
 - Cinematic camera when double tapping the camera mode switch button
 - re3: Make cars and peds to not despawn when you look away
 - Dont kill peds when jacking their car, monster!
 - Fixed ped animation after being attacked with parachute (inspired *by Alexander Polyak*)
 - Cloud saves can be disabled (NOT disabled by default)
 - Always drawable wanted stars (disabled by default)
 - Better ForceDXT
 - Fix wheels rotation speed on high FPS
 - Reflections are now centered on a camera instead of *player's neck*
 - Reflections are now bigger in quality (1024x1024 instead of default 512x512 at **max quality**)
 - Fixed a small FX system leakage when using custom effects mod

**Since v1.6.1**
 - A fix for vehicle's entering task (thanks AliAssassiN!)
 - **Wrong vehicle parts colors (thanks B1ack_&_Wh1te and AliAssassiN!)**
 - Planes now have a toggleable smoke/dust (thanks BengbuGuards!)
 - Falling star is now not a black line
 - BengbuGuards: Implement jetpack hover button, also "lockable"
 - Fixing a wrong value in carcols.dat (a mistakenly placed dot instead of comma, # 98 malachite poly)
 - Automatical streaming memory buff if not enough for the game
 - Fix planes generation coordinates (will partially fix planes that are crashing in front of a player)
 - Fix JPatch crash after CJ dies :(

**Since v1.6**
 - Colored zone names are back
 - Sand/Dust particles from bullets are limited because they are creating a lot of lag
 - CJ magnetting to stealable objects only when very close
 - Moved save pickup in Madd Dogg's Mansion
 - Moved a bribe to the broken bridge from SF building
 - Moved a rifle pickup outside of the SF Stadium wall
 - CJ no more can drop JetPack while he's in air
 - CJ can leave any vehicle at high speeds
 - Weapon penalty clean-up when CJ dies
 - **A fix for 2.10 crash (thanks fastman92!)**
 - A fix for vehicle's entering task (thanks AliAssassiN!)
 - Fixed wall behind the fog (only for default shaders!)
 - Remove stupid specular from peds

**Since v1.5.1**
 - **Crosshair should be placed at the correct place**
 - Aiming with Country Rifle is now in 3rd person

**Since v1.5**
 - Animated textures (UVAnim)
 - Vertex Weights
 - Swimming speed
 - Fixed sky multitude (a glitch on high speeds)
 - Fixed vehicles backlight light state (should do vehicle tail light effect when ANY of the lights are broken)
 
### ***GTA:SA v2.00***
**Planned/TODO (not ready or broken now)**
 - Fix camera zoom is not that big (aspect ratio?)
 - FoodEatingFix by JuniorDjjr
 - Doubled the directional light sources on models (to match the PC version) <- Shaders thingo :(
 - Fix opcode 08F8 that still shows "Updated stats" pop-up no matter what
 - Bring back saveable camera photos (framebuffer-related problems! The image is like... rotated, scaled and repeated?!)
   - Need to fix a framebuffer at the end of the rendering stage
 - Fixed a glitch with bushes/trees switching their color to black at some camera angles (camnorm issue)

**Since v1.8.2**
 - A small optimisation for textures loading speed (hashing function)
 - Fixed crazy vehicle shaking (swinging chassis) on high FPS
 - SilentPatch: Fixed an AI issue where enemies became too accurate after the player had been in the car earlier
 - SilentPatch: Bilinear filtering for license plates
 - SilentPatch: Don't clean the car BEFORE Pay 'n Spray doors close
 - SilentPatch: Spawn lapdm1 (biker cop) correctly if the script requests one with PEDTYPE_COP
 - SilentPatch: Fix the logic behind exploding cars losing wheels
 - Allow more wheels to be exploded instead of only ONE RANDOM
 - SilentPatch: Disable building pipeline for skinned objects (like parachute)
 - FixRadar: Radar outline is not forced to be in pure black color

**Since v1.8.1**
 - SilentPatch: Impound garages
 - SilentPatch: Bigger mirror quality on medium-high settings
 - Fixed a glitch with the timer on high FPS
 - Allowes gang wars to have a wanted level (like on PC)
 - Fixing drunk camera on high FPS
 - When headlights are active, the windows are no longer transparent from one side
 - Returned back the game logic related to player aiming at peds
 - Now the game shuts opened doors at high speeds with high FPS
 - HUD colors are not brighter anymore
 - Water color fix (from SkyGFX)
 - Enabled CJ's clothes response to the wind in some vehicles

**Since v1.8**
 - Able to skip "drive" (tripskip)
 - Fixed "collideable" particles such as gunshells (reversed from FxsFuncs)
 - GLAlphaFunc mod is now included (fixing a crash on Huawei phones)
 - Fixes some stupid nearclipping issues that are caused by WarDrum's dirty hands
 - Fixes that some data in CollisionData is not being set to zero
 - Fixes a little Rockstar mistake with coronas rendering

**Since v1.7.3**
 - Not only airplanes but also trains and boats have shadows

**Since v1.7.2**
 - Little fixes for previous JPatch fixes
 - An ability to remove FOV-effect while driving a car (NoVehicleFOVEffect, not applied by default)
 - Now all vehicles have shadows (Trains, some airplanes and etc)
 - **Cars (only cars because of a limit)** now have a bigger shadow draw distance

**Since v1.7**
 - CJ is now able to exit vehicle without closing the door (https://github.com/AndroidModLoader/JPatch/issues/28)
 - Fix wheels rotation speed on high FPS
 - Reflections are now centered on a camera instead of *player's neck*
 - Reflections are now bigger in quality (1024x1024 instead of default 512x512 at **max quality**)
 - Fixed FX distance multiplier (caused a glitch, reworked)

**Since v1.6.1**
 - Unused detonator animation is working
 - Improved AND fixed Dynamic Streaming Memory
 - Fixed a crash from **"Wrong vehicle parts colors"** fix
 - A fix for vehicle's entering task (thanks AliAssassiN!)
 - Fix planes generation coordinates (will partially fix planes that are crashing in front of a player)

**Since v1.6**
 - Falling star is now not a black line
 - Jetpack hovering is now "lockable"
 - Planes now have a toggleable smoke/dust (thanks BengbuGuards!)
 - **Wrong vehicle parts colors (thanks B1ack_&_Wh1te and AliAssassiN!)**
 - The camera is not rotating like a crazy when mouse/keyboard is connected (thanks AliAssassiN!)
 - The camera buffer is getting properly cleared. This is fixing "ghosting" underworld (thanks AliAssassiN!)
 - A little mistake by WarDrum related to holding a finger on widgets (thanks AliAssassiN!)

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
 - RLEDecompress crash is fixed! (allocates more memory)

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
 - Fixed green textures (moved from SkyGFX Mobile)
 - Static light shadows (from traffic lights, poles and more) draw distance is 3 times bigger
 
 **Since v1.2.1**
 - Remove stupid specular from peds
 - Include objects in checks of FindGroundZ functions (may fix opcode 02CE after using 0107)
 - Automatical streaming memory buff if not enough for the game
 - Fixed an annoying glitch with colorpicker in Tenderfender
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
 - Fuzzy seek (the game does not load useless data from audio)
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
 - Dont burn player that are driving burning BMX
 - A fix for possible crash from GetCompositeMatrix (might be useless)
 - Cinematic camera when double tapping the camera mode switch button
 - MTA: Increase intensity of vehicle tail light corona
 - MTA: Disable setting the occupied's vehicles health to 75.0f when a burning ped enters it
 - MTA: Thrown projectiles throw more accurately
 
 **Since v1.1**
 - Fixed emergency vehicles (FixStreamingDistScale)
 - Moved save pickup in Madd Dogg's Mansion
 - Moved a bribe to the broken bridge from SF building
 - Moved a rifle pickup outside of the SF Stadium wall
 - Fixed cutscene FOV (disabled by default, incomplete, black borders are better?)
 - Fixed sky multitude (a glitch on high speeds)
 - Fixed vehicles backlight light state (should do vehicle tail light effect when ANY of the lights are broken)
 - Sand/Dust particles from bullets are limited because they are creating a lot of lag
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
