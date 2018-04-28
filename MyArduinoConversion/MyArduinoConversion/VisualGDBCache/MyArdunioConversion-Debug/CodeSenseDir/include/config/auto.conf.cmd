deps_config := \
	/esp-idf/v3.0/components/app_trace/Kconfig \
	/esp-idf/v3.0/components/aws_iot/Kconfig \
	/esp-idf/v3.0/components/bt/Kconfig \
	/esp-idf/v3.0/components/esp32/Kconfig \
	/esp-idf/v3.0/components/esp_adc_cal/Kconfig \
	/esp-idf/v3.0/components/ethernet/Kconfig \
	/esp-idf/v3.0/components/fatfs/Kconfig \
	/esp-idf/v3.0/components/freertos/Kconfig \
	/esp-idf/v3.0/components/heap/Kconfig \
	/esp-idf/v3.0/components/libsodium/Kconfig \
	/esp-idf/v3.0/components/log/Kconfig \
	/esp-idf/v3.0/components/lwip/Kconfig \
	/esp-idf/v3.0/components/mbedtls/Kconfig \
	/esp-idf/v3.0/components/openssl/Kconfig \
	/esp-idf/v3.0/components/pthread/Kconfig \
	/esp-idf/v3.0/components/spi_flash/Kconfig \
	/esp-idf/v3.0/components/spiffs/Kconfig \
	/esp-idf/v3.0/components/tcpip_adapter/Kconfig \
	/esp-idf/v3.0/components/wear_levelling/Kconfig \
	/esp-idf/v3.0/components/bootloader/Kconfig.projbuild \
	/esp-idf/v3.0/components/esptool_py/Kconfig.projbuild \
	/cygdrive/c/workspace-git/MyArduinoConversion/MyArduinoConversion/main/Kconfig.projbuild \
	/esp-idf/v3.0/components/partition_table/Kconfig.projbuild \
	/esp-idf/v3.0/Kconfig

include/config/auto.conf: \
	$(deps_config)


$(deps_config): ;
