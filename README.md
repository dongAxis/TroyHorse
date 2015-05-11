#TroyHorse

This project run on the mac. It can used for creating virus or create anti-malware, this framework is under developing.

###Aim
This project is aimed for creating rootkit on Mac OSX 10.10.<br>
This rootkit can hide some specific directories, files and process. <br/>
TroyHorse will be a framework finally, and it will have UI, user space module and kernel extension. 

###How to run it on Mac
Now, I only test it on Yosemite, so you must run it on yousemite rather than other versions.<br/>
Because all the kernel extension must be signed on Yosemite and later, and i have not found effective way to bypass code sign security mechanism. so you must set your yousemite as developing mode with command:<br/>
sudo nvram boot-args="kext-dev-mode=1" <br/>
then restart the computer. After doing this, the kext without being signed can be loaded.<br/>

Then, you should use the following command to load kernel extension:
    kextload ./Troy.kext.
if the kernel extension is loaded successfully, please run the user mode execute file:
    ./Horse
You can follow the help menu to do some operations.

###MileStone
May 10: complete hide process, and control module in user space land
May 11: complete hide direct

Happy

Axis