V1 release is now out! Installation is now simply an exe download and install. Let me know if you have issues. 

I made this driver to solve my own problem of not being able to use my GFX camera with NINA, my usual astrophotography software. This isn't a NINA specific driver or anything, so it should work with other software, but I honestly just have no idea. Functionality so far in NINA is connecting, reading sensor data, getting/setting ISO, exposing, downloading to NINA. 

Supported Cameras (No other cameras can be supported without being explicitly supported by the Fujifilm SDK, only other course of action is to fully reverse engineer each camera model, sorry!)
1) 50R
2) 50S
3) 50S II
4) 100
5) 100 II 
6) 100S
7) 100S II 
8) X-H2 
9) X-H2S 
10) X-M5
11) X-Pro3
12) X-S10
13) X-S20
14) X-T3
15) X-T4
16) X-T5

**NOTE FOR NON PASM CAMERAS (X-T# or X-Pro#)**

For cameras that have the manual dials on the top, you have to make sure they are fully in manual mode. This means set ISO to the base, set shutter speed to 'B', set aperture to manual (if lens is attached), and make sure image quality is set to Raw only and uncompressed. I will work on having this functionality set by the driver, but for now this is fine. If done right, the shutter speed should read 'BULB' on the back of your camera. 

Cameras like the X-S# and GFX series don't have the manual dials, so the driver sets it to BULB or non bulb mode depending on what is required. These cameras still need to be set to Raw only and uncompressed. 

**INSTALLATION (If building manually)**
1. IMPORTANT: This driver will not function without the ASCOM platform installed, no ASCOM driver will. Make sure it's installed before beginning
2. Download the whole repo to your local machine
3. Open the project in visual studio
4. Make sure visual studio is set to Debug and x64 in the top row
5. Make sure the solution explorer shows 2 of 2 projects (Fuji and LibRawWrapper)
6. Right click Solution 'Fuji' at the top of the solution explorer
7. Click clean solution, wait for it to finish
8. Click build solution, wait for it to finish
9. Open cmd and cd to the Debug/Release folder of the repo (typically \bin\Debug)
10. type ASCOM.ScdouglasFujifilm.exe /register
11. Driver should now be good to go, so go ahead and test it 
