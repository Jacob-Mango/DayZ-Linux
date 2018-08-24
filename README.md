# DayZ-Linux

This is an **UNOFFICIAL** modification of the game to enable Linux support, so don't blame the DayZ development team if this is not working as you expect. 

-- 

Note to other developers:  
Yes the code might be messy, yes there are things that do not look like they should work or do anything!  
DayZ is not opened for modding or scripting, so this is pretty much a hacky solution to everything.  
If you want to improve something, go ahead and create a pull-request ;)

# Installation:
Make sure you have downloaded and installed Proton. If you have done everything successfully and nothing happens, download 'Coloring Pixels' on steam (300mb size, very small, works) as suggested by [u/airspeedmph](https://www.reddit.com/user/airspeedmph) on Reddit. Fixed it for me.

Make sure you are on the "stress test" beta branch on steam. See [this link](https://dayz.com/blog/0-63-stress-tests) for details on how to switch on it!  

Place the ```scripts``` folder inside your DayZ game directory (```For example: C:\Program Files (x86)\Steam\steamapps\common\DayZ```).
You will not be banned by BattlEye due to the BattlEye process not even being able to run!

Now you now start the script ```DayZ-Linux.sh``` to start the game. You will be loaded directly into a rudimetry offline mode. No loot or infected will spawn due to proper mission loading not working.

To uninstall this modification, simple delete the folder ```scripts```.

# Logfiles:
You can currently not have log files saved with the use of SteamPlay (Proton). If someone can figure this out, that would be great and can you also make a pull-request for that?

In case you want to report errors to us or the offical dayz dev team, you might need logfile info.
We also save the positions you printed ingame in it so that you might revisit them later on by saving them in some textfile.
Locations are stored inside the script.log for now.

You find your logfiles here: ```Press WINDOWS + R  -> Type in %localappdata%/DayZ -> Hit enter```. 

# Future:

* Figure out how to incorporate this into the [Community Offline Mode](https://github.com/Arkensor/DayZCommunityOfflineMode).  