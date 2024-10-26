## This is a DMA cheating detector
Inspired by the files provided on the Github: https://github.com/IntelSDM/RustDMACheat https://github.com/Metick/CheatEngine-DMA
We can find that all the DMA cheating systems need to obtain and revise data in the game progress files. Therefore, we can make code to detect whether the player has access to directly get information just in the game progress files to avoid cheating.
This program is currently compatible only with the Windows operating system. To increase its capability, game developers and administrators could set up remote servers to monitor player pastime and apply this system to discover dishonest. Via doing so, a fairer and more balanced FPS gaming environment can be accomplished.

## Update in Detector.cpp
std::wstring gameProcessName = L"XXX.exe"; should be placed in the main function of the code. XXX is the process of the game.
![image](https://github.com/user-attachments/assets/5f653da7-ef4c-48ce-b39a-8e4a4c043fee)
