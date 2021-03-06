# LoRa-GPIO
This is a main container solution of several other projects for remote control
and monitoring of GPIO pins on a demote device from a local console using LoRa.

Normally, one might code in only this main solution, however unresolved conflicts
arose when uploading binary to different COM ports in different projects.
So for now, the projects contained in this solutions are opened in different
instances of Visual Studio 2017 with the VisualMicro add-in installed.

See also: https://gojimmypi.blogspot.com/2018/02/lora-range-issues.html

and: https://gojimmypi.blogspot.com/2018/02/m5stack-lora-range-issues.html

The 4 main projects of interest:

# Console
32u4 based DIYMALL LoRa32u4 using RadioHead library with good range as local control console.
There's currently no display implemented on this Console

# Device
32u4 based DIYMALL LoRa32u4 using RadioHead library with good range as remote GPIO device.

# M5StackConsole
ESP32 based M5Stack LoRa that initially was working with Sandeep Mistry's LoRa library, but
had rather poor range. The M5Stack has a nice enclosure and built-in display.

The working, but poor range code can be found here (NOTE it was saved with frequency=433.375E6):
https://github.com/gojimmypi/LoRa-GPIO/blob/8e96d83eca89cd9ac19fb5eeca8fb276b7a5dddf/M5StackConsole/M5StackConsole.ino

The current objective is to get the RadioHead library working with this project.

See the M5-RadioHead branch for this project: https://github.com/gojimmypi/LoRa-GPIO/tree/M5-RadioHead

The 11-month old Adafruit fork of RadioHead: https://github.com/gojimmypi/RadioHead/tree/development

The Master RadioHead overwritten with working download from Adafruit tutorial (missing ESP32 support)
https://learn.adafruit.com/adafruit-feather-32u4-radio-with-lora-radio-module/using-the-rfm-9x-radio
which can be found on this local fork branch:  https://github.com/gojimmypi/RadioHead/tree/master

Then new changes to most recent code will be applied to the M5Stack local branch: https://github.com/gojimmypi/RadioHead/tree/M5Stack (not yet updated with most recent, 11-month old code)

See source "A github'ified version of http://www.airspayce.com/mikem/arduino/RadioHead/":
https://github.com/adafruit/RadioHead

# RogueDeviceTest
We'll want to ignore transmissions from 3rd party devices. This is an example 3rd party.



# M5StackConsole-bad
something went amuck here and could not be resolved. Code moved to new project.
Error:
VM_Boards.h: 653:2: error: #error "Please edit Boards.h with a hardware abstraction for this board"
   error "Please edit Boards.h with a hardware abstraction for this board"
   for (analogPin = 0; analogPin < TOTAL_ANALOG_PINS; analogPin++)
...
Error compiling libraries
   Debug Build Error: This hardware might not support the default Visual Micro Debugger.
	If this project is using OTA/WiFi upload then please use the vMicro>Debugger menu to set the Serial COM and Remote port for the debugger.
	Otherwise switch the toolbar from 'Debug' to 'Release' and build again.
		or switch off Automatic Debugging
		or configure the SoftwareSerial debugger option.
Debug build failed for project 'M5StackConsole' 

Answer:
"The debug issue was that you had enabled automatic analog, digital or other graphical views in the debug options for the project. These views require information about the board such as number of digital pins. Your hardware is unknown hence the "error" telling you the hardware is unknown."

See http://www.visualmicro.com/forums/YaBB.pl?num=1518378800/0#3

(but I don't actually know how to fix this in the IDE; this -bad project saved for that)

Also note: "Yes v10 of the USB driver is not for your op sys it is for windows universal"
http://www.visualmicro.com/forums/YaBB.pl?num=1518378800/2#2

Apparently I had not read the full description properly and inadvertantly installed the most recent "Universal" drivers that don't work well with Windows 10.  :|

# Installation

### Download Library

#### Using the Arduino IDE Library Manager

1. Choose ```Sketch``` -> ```Include Library``` -> ```Manage Libraries...```
2. Type ```m5stack``` into the search box.
3. Click the row to select the library.
4. Click the ```Install``` button to install the library.
5. Click "File-> Examples". Here are some test programs in "M5Stack->"

#### Using Git (most environments)
```sh
cd ~/Documents/Arduino/libraries/
git clone https://github.com/gojimmypi/RadioHead.git
git clone https://github.com/gojimmypi/M5Stack
```

#### Using Git (Windows)
```sh
c:
cd %USERPROFILE%\documents\libraries
git clone https://github.com/gojimmypi/RadioHead.git
git clone https://github.com/gojimmypi/M5Stack
```


# Notes
Reminder: Sync a fork of a repository to keep it up-to-date with the upstream repository.
https://help.github.com/articles/syncing-a-fork/