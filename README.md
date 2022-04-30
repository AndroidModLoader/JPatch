# What am i?
 I am a little mod that wants to fix your game!
 Currently i can fix those things:
 
 **GTA:SA**
 
 **Since v1.2.4*
 - Corona sprites draw distance is 3 times bigger
 - Static shadows are moved to the ground a bit
 - Radar streaming should be fixed
 - Climb-dying glitch SHOULD be fixed
 - Do not use bias in shaders, probably makes the game faster
 - Re-fixed emergency vehicles disappearing
 
 **Since v1.2.3**
 - Goddamn static light shadows from PC!
 - Bumped a limit of static shadows!
 - Fixed green textures (moved from SkyGfx)
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