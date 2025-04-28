Fujicom
ASCOM compliant driver for Fujifilm cameras. 

I made this driver to solve my own problem of not being able to use my GFX camera with NINA, my usual astrophotography software. The code is probably done in a non-optimal way I'm sure, but for now it works. I will be trying to learn more about optimization going forward and other software may work but NINA is all I've validated. 

This driver is currently for testing purposes, and will have to be compiled and registered locally on your machine for now. I'm working on getting an installer to release but we're not there yet. 

While the GFX 100S is the only currently working camera, II do have a list of other cameras that could **theoretically** function. Those cameras are as follows: 
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

The limitation on models is not from me, Fujifilm only released header files for some models, meaning the others would have to be reverse engineered or I'd have to guess some of the header values.
If in the future more headers are released for me to work with, I can revisit this but until then it would be pracitcally impossible. 

**INSTALLATION**
1. IMPORTANT: This driver will not function without the ASCOM platform installed, no ASCOM driver will. Make sure it's installed before beginning
2. Download the whole repo to your local machine
3. Open the project in visual studio
4. Make sure visual studio is set to Debug and x64 in the top row
5. Make sure the solution explorer shows 2 of 2 projects (Fuji and LibRawWrapper)
6. Right click Solution 'Fuji' at the top of the solution explorer
7. Click clean solution, wait for it to finish
8. Click build solution, wait for it to finish
9. Open cmd and cd to the Debug folder of the repo
10. type ASCOM.ScdouglasFujifilm.exe /register
11. Driver should now be good to go, so go ahead and test it 
