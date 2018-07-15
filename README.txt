This main LoRa-GPIO solution is simply a holder for the other projects:

Device - 32u2 that needs to have GPIO pins remotely controlled
Console - also a 32u2, but the "host" or controle (no display)
M5StackConsole - control the "device" with the ESP32-based module with display
RogueDeviceTest - we only want our Console and Device to talk; here we test 3rd party


Although in theory one should be able to compile and upload each of the projects from within 
a single instance of Visual Studio. I've never been able to get that to work. As such I 
open each of the Project's respective solution in separate Visual Studio windows. ymmv