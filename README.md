[![AML Mod Compiler](https://github.com/AndroidModLoader/JPatch/actions/workflows/main.yml/badge.svg?branch=main)](https://github.com/AndroidModLoader/JPatch/actions/workflows/main.yml)

# What am i?
 I am a little mod that wants to fix your game!
 Need a preview version of upcoming changes? Get it there: https://github.com/AndroidModLoader/JPatch/actions

 Currently i can fix those things:
 
 **GTA:SA**

 **Planned/TODO (not ready or broken now)**
 - Damaged components should not be colored by the previously damaged vehicle of that type
 - Fix camera zoom is not that far

**Since v1.4.2**
 - Removed "Please rate the game" message
 - Removed IDE flag "ExtraAirResistance"
 - Implement NoRadioCuts
 - Smaller grenades collision
 - MTA: Fixed a glitch with projectiles (see https://github.com/multitheftauto/mtasa-blue/issues/1123)
 - MTA: Fix slippery floor
 - Another fix for aiming + walking
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
 - A bit darker HUD colours (disabled by default)
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
 - Fixed emergency vehicles (probably?)
 - Moved save pickup in Madd Dogg's Mansion
 - Moved a bribe to the broken bridge from SF building
 - Moved a rifle pickup outside of the SF Stadium wall
 - Fixed cutscene FOV (disabled by default, incomplete)
 - Fixed sky multitude (a glitch on high speeds)
 - Fixed vehicles backlight light state (not sure what it does)
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
