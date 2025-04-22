Fujicom
ASCOM compliant driver for Fujifilm cameras. 

I made this driver to solve my own problem of not being able to use my GFX camera with NINA, my usual astrophotography software. The code is probably done in a non-optimal way I'm sure, but for now it work. I will be trying to learn more about optimization going forward, other software may work but NINA is all I've validated. 

While the code has worked for me, I cannot guarantee it will work for you and honestly have no clue how it will behave under different circumstances. 

That being said, so far, everything seems to work. I can connect my GFX 100S, set the ISO, set exposure time, take the exposure, and have it properly show up in NINA. I have not tested with other software. 

This driver is currently for testing purposes, and will have to be compiled and registered locally on your machine for now. I'm working on getting an installer to release but we're not there yet. 

While the GFX 100S is the only currently working camera, II do have a list of other cameras that could **theoretically** function. Those cameras are as follows: 
1) 50R
2) 50S
3) 50S II
4) 100
5) 100 II (Video as well in theory)
6) 100S
7) 100S II (Video as well in theory)
8) X-H2 (Video as well in theory)
9) X-H2S (Video as well in theory)
10) X-M5 (Video as well in theory)
11) X-Pro3
12) X-S10
13) X-S20 (Video as well in theory)
14) X-T3
15) X-T4
16) X-T5 (Video as well in theory)

The limitation on models is not from me, Fujifilm only released header files for some models, meaning the others would have to be reverse engineered or I'd have to guess some of the header values.
If in the future more headers are released for me to work with, I can revisit this but until then it would be pracitcally impossible. 

If you happen to use this driver, please let me know if there are any issues encountered, I will try to keep up with development as best I can. 

**INSTALLATION**

To install:
1) Download the whole repository
2) Open in visual studio
3) Clean and build
4) Open cmd
5) cd to the repo's debug or release folder, whereever you built it
6) run ASCOM.ScdouglasFujifilm.exe /register (Any news here is bad news, should run quietly)
7) Test, ideally on NINA but can be elsewhere 
