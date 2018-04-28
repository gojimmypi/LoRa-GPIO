#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

COMPONENT_EXTRA_INCLUDES += C:/Users/jscarletta.WINGS/Documents/libraries/RadioHead C:/Users/jscarletta.WINGS/Documents/hardware/arduino-esp32/cores/esp32 C:/Users/jscarletta.WINGS/Documents/hardware/arduino-esp32/libraries/SPI/src C:/Users/jscarletta.WINGS/Documents/hardware/arduino-esp32/variants/esp32
CXXFLAGS += $(addprefix -D,$(COMPONENT_PRIV_DEFINES))
CFLAGS += $(addprefix -D,$(COMPONENT_PRIV_DEFINES))
COMPONENT_PRIV_DEFINES += ESP32
COMPONENT_SRCDIRS += ../../../../Users/jscarletta.WINGS/Documents/hardware/arduino-esp32/cores/esp32 ../../../../Users/jscarletta.WINGS/Documents/hardware/arduino-esp32/libraries/SPI/src ../../../../Users/jscarletta.WINGS/Documents/hardware/arduino-esp32/variants/esp32 ../../../../Users/jscarletta.WINGS/Documents/libraries/RadioHead