dnl
dnl ecmd_defs.m4
dnl
dnl  You can declare your ecmd commands here.  The code will be expanded
dnl  by the make system (which in turn uses GNU m4) to build the
dnl  ecmd_defs.c file.
dnl
dnl  Lines starting with `dnl' are comments.
dnl

ecmd_ifndef(TEENSY_SUPPORT)
  ecmd_ifdef(UIP_SUPPORT)
    ecmd_ifdef(ENC28J60_SUPPORT)
      ecmd_ifndef(DISABLE_IPCONF_SUPPORT)
        ecmd_feature(mac, "mac ",xx:xx:xx:xx:xx:xx,Set the MAC address.)
      ecmd_endif()
      ecmd_feature(show_mac, "show mac",,Display the MAC address.)
    ecmd_endif()
    ecmd_feature(show_ip, "show ip",,Display the IP address.)
    ecmd_ifndef(IPV6_SUPPORT)
      ecmd_feature(show_netmask, "show netmask",,Display the network mask.)
    ecmd_endif()
    ecmd_feature(show_gw, "show gw",,Display the IP address of the default router.)
    ecmd_feature(show_version, "show version",,Display the version number.)

    ecmd_ifndef(IPV6_SUPPORT)
      ecmd_ifndef(BOOTP_SUPPORT)
        ecmd_ifndef(DISABLE_IPCONF_SUPPORT)
	  ecmd_feature(ip, "ip ",IP,Set the IP address.)
	  ecmd_feature(netmask, "netmask ",IP,Set the network mask.)
	  ecmd_feature(gw, "gw ",IP,Set the address of the default router.)
        ecmd_endif()
      ecmd_endif()
    ecmd_endif()

    ecmd_ifdef(IPV6_STATIC_SUPPORT)
      ecmd_ifndef(DISABLE_IPCONF_SUPPORT)
        ecmd_feature(ip, "ip ")
        ecmd_feature(gw, "gw ")
      ecmd_endif()
    ecmd_endif()
  ecmd_endif()

  ecmd_ifndef(DISABLE_REBOOT_SUPPORT)
    block(Resetting the controller)

    ecmd_feature(reset, "reset",,Reset the Ethersex.)
    ecmd_feature(wdreset, "wdreset",,Go into endless loop to trigger a watchdog timeout.)
  ecmd_endif()
ecmd_endif()

ecmd_ifdef(IPSTATS_SUPPORT)
  ecmd_feature(ipstats, "ipstats")
ecmd_endif()

ecmd_ifdef(FREE_SUPPORT)
  ecmd_feature(free, "free")
ecmd_endif()

ecmd_ifdef(STELLA_SUPPORT)
  block(Stella commands)
  ecmd_feature(stella_eeprom_store, "stella store",, Store values in eeprom)
  ecmd_feature(stella_eeprom_load, "stella load",, Load values from eeprom)
  ecmd_ifdef(CRON_SUPPORT)
  ecmd_ifdef(ECMD_SPEED_SUPPORT)
  ecmd_feature(stella_cron, "stella cron", CHANNEL VALUE MINUTE HOUR DAY MONTH DAYOFWEEK, Add a cronjob for stella to set the CHANNEL to VALUE. You may use fade channel constants.)
  ecmd_endif()
  ecmd_endif()
  ecmd_feature(stella_channels, "channels",, Return stella channel size)
  ecmd_ifdef(STELLA_MOODLIGHT)
  ecmd_feature(stella_moodlight_get, "moodlight get",CHANNEL, Return on if channel CHANNEL is moodlighted)
  ecmd_feature(stella_moodlight_set, "moodlight set",CHANNEL ONOFF, Set CHANNEL moodlight on=1 or off=0)
  ecmd_endif()
  ecmd_feature(stella_fadestep_set, "stella fadestep set", FADESTEP, Set stella fade step)
  ecmd_feature(stella_fadestep_get, "stella fadestep get",, Get stella fade step)
  ecmd_feature(stella_channel_set, "channel set", CHANNEL VALUE FUNCTION,Set stella channel to value. The third parameter is optional. You may use 's' for instant set, 'f' for fade and 'y' for flashy fade. )
  ecmd_feature(stella_channel_get, "channel get", CHANNEL, Get stella channel value)
ecmd_endif()

ecmd_ifdef(CRON_SUPPORT)
  block(Cron commands)
  ecmd_feature(cron_list, "cron_list",, Show all cron entries)
  ecmd_feature(cron_rm, "cron_rm", POSITION, Remove one cron entry)
ecmd_endif()

ecmd_ifndef(DISABLE_REBOOT_SUPPORT)
  ecmd_feature(bootloader, "bootloader",,Call the bootloader.)
ecmd_endif()

ecmd_ifdef(PORTIO_SUPPORT)
  block(Port I/O)
  ecmd_feature(io_set_ddr, "io set ddr", PORTNUM HEXVALUE [MASK], Set the DDR of port PORTNUM to VALUE (possibly using the provided MASK).)
  ecmd_feature(io_get_ddr, "io get ddr", PORTNUM, Display the current value of the DDR PORTNUM.)
  ecmd_feature(io_set_port, "io set port", NUM HEXVALUE [MASK], Set the PORT NUM to VALUE (possibly using the provided MASK).)
  ecmd_feature(io_get_port, "io get port", NUM, Display the current value of the PORT NUM.)
  ecmd_feature(io_get_pin, "io get pin", PORTNUM, Display the current value of the PIN-register of the port PORTNUM.)
  ecmd_feature(io_get_mask, "io get mask", PORTNUM, Display the mask of the port PORTNUM.)
ecmd_endif()

ecmd_ifdef(PORTIO_SIMPLE_SUPPORT)
  ecmd_feature(io, "io ")
ecmd_endif()

ecmd_ifdef(CONTROL6_SUPPORT)
  ecmd_ifdef(C6_ECMD_USED)
    ecmd_feature(c6_get, "c6 get ", VARNAME, Display the current value of the ECMD_GLOBAL Variable)
    ecmd_feature(c6_set, "c6 set ", VARNAME VALUE, Set an new value on the ECMD_GLOBAL Variable)
  ecmd_endif()
ecmd_endif()

ecmd_ifdef(NAMED_PIN_SUPPORT)
  ecmd_ifdef(PORTIO_SUPPORT)
    block(Named Pin)
    ecmd_feature(pin_get, "pin get", NAME, Read and display the status of pin NAME.)
    ecmd_feature(pin_set, "pin set", NAME STATUS, Set the status of pin NAME to STATUS.)
    ecmd_feature(pin_toggle, "pin toggle", NAME, Toggle the status of pin NAME.)
  ecmd_endif()
ecmd_endif()

ecmd_ifdef(FS20_SUPPORT)
  ecmd_ifdef(FS20_SEND_SUPPORT)
    ecmd_feature(fs20_send, "fs20 send")
  ecmd_endif()

  ecmd_ifdef(FS20_RECEIVE_SUPPORT)
    ecmd_feature(fs20_receive, "fs20 receive")
  ecmd_endif()

  ecmd_ifdef(FS20_RECEIVE_WS300_SUPPORT)
    ecmd_feature(fs20_ws300, "fs20 ws300")
  ecmd_endif()
ecmd_endif()

ecmd_ifdef(HD44780_SUPPORT)
  block(HD44780 LCD Display)
  ecmd_feature(lcd_clear, "lcd clear", [LINE], Clear line LINE (0..3) or the whole display (if parameter is omitted))
  ecmd_feature(lcd_write, "lcd write", TEXT, Write TEXT to the current cursor location)
  ecmd_feature(lcd_goto, "lcd goto", LINE COL, Move cursor to LINE and column COL (origin is 0/0))
  ecmd_feature(lcd_char, "lcd char", N D1 D2 D3 D4 D5 D6 D7 D8, Define use-definable char N with data D1..D8 (provide DATA in hex))
  ecmd_feature(lcd_init, "lcd init", CURSOR BLINK, Reinitialize the display, set whether to show the cursor (CURSOR, 0 or 1) and whether the cursor shall BLINK)
  ecmd_feature(lcd_shift, "lcd shift", DIR, Shift the display to DIR (either ''left'' or ''right''))
ecmd_endif()

ecmd_ifdef(ONEWIRE_SUPPORT)
  block(Dallas 1-wire)
  ecmd_ifdef(ONEWIRE_DETECT_SUPPORT)
    ecmd_feature(onewire_list, "1w list",,Return a list of the connected onewire devices)
  ecmd_endif()
  ecmd_feature(onewire_get, "1w get", DEVICE, Return temperature value of onewire DEVICE (provide 64-bit ID as 16-hex-digits))
  ecmd_feature(onewire_convert, "1w convert", [DEVICE], Trigger temperatur conversion of either DEVICE or all connected devices)
ecmd_endif()

ecmd_ifdef(RC5_SUPPORT)
  ecmd_feature(ir_send, "ir send")
  ecmd_feature(ir_receive, "ir receive")
ecmd_endif()

ecmd_ifdef(DNS_SUPPORT)
  block(DNS)
  ecmd_feature(nslookup, "nslookup ", HOSTNAME, Do DNS lookup for HOSTNAME (call twice).)
  ecmd_feature(show_dns, "show dns",, Display the IP address of the configured DNS server.)
  ecmd_ifndef(BOOTP_SUPPORT)
    ecmd_feature(dns, "dns ", IPADDR, Set the IP address of the DNS server to use to IPADDR.)
  ecmd_endif()
ecmd_endif()

ecmd_ifdef(CLOCK_SUPPORT)
  block(Clock)
  ecmd_ifdef(CLOCK_DATETIME_SUPPORT)
    ecmd_feature(time, "time",, Display the current time.)
    ecmd_feature(date, "date",, Display the current date.)
  ecmd_endif()
ecmd_endif()

ecmd_ifdef(WHM_SUPPORT)
  ecmd_feature(whm, "whm",, Display the uptime.)
ecmd_endif()

ecmd_ifdef(ADC_SUPPORT)
  block(ADC)
  ecmd_feature(adc_get, "adc get", [CHANNEL], Get the ADC Value in HEX of CHANNEL or if noch channel set of all channels.)
ecmd_endif()

ecmd_ifdef(KTY_SUPPORT)
  block(KTY)
  ecmd_feature(kty_get, "kty get", [CHANNEL], Get the temperature in xxx.x °C of CHANNEL or if noch channel set of all channels.)
  ecmd_feature(kty_cal_get, "kty cal get",, Return the calibration difference to 2k2 Resistor.)
  ecmd_feature(kty_calibration, "kty autocalibrate", CHANNEL, Calibrate to 1000 Ohm precision Resistor.)
ecmd_endif()

ecmd_ifdef(DATAFLASH_SUPPORT)
  block(Dataflash)
  ecmd_feature(df_status, "df status",, Display internal status.)

  ecmd_feature(fs_format, "fs format",, Format the filesystem.)
  ecmd_feature(fs_list, "fs list",, List the directory.)
  ecmd_feature(fs_mkfile, "fs mkfile ", NAME, Create a new file NAME.)
  ecmd_feature(fs_remove, "fs remove ", NAME, Delete the file NAME.)
  ecmd_feature(fs_truncate, "fs truncate ", NAME LEN, Truncate the file NAME to LEN bytes.)
  ecmd_ifdef(DEBUG_FS)
    ecmd_feature(fs_inspect_node, "fs inspect node ", NODE, Inspect NODE and dump to serial.)
    ecmd_feature(fs_inspect_inode, "fs inspect inode ", INODE, Inspect INODE (and associated page).)
  ecmd_endif()
ecmd_endif()

ecmd_ifdef(SD_READER_SUPPORT)
  block(SD/MMC Card Reader)
  ecmd_feature(sd_dir, "sd dir",, List contents of current SD directory.)
ecmd_endif

ecmd_ifndef(TEENSY_SUPPORT)
  block(RFM12)
  ecmd_ifdef(RFM12_SUPPORT)
    ecmd_feature(rfm12_status, "rfm12 status",, Display internal status.)
  ecmd_endif()
  ecmd_ifdef(RFM12_IP_SUPPORT)
    ecmd_feature(rfm12_setbaud, "rfm12 setbaud", BAUD, Set baudrate to BAUD.)
    ecmd_feature(rfm12_setbandwidth, "rfm12 setbandwidth", BW, Set RX bandwidth to BW.)
    ecmd_feature(rfm12_setmod, "rfm12 setmod", MOD, Set modulation to MOD.)
    ecmd_feature(rfm12_setgain, "rfm12 setgain", GAIN, Set preamplifier gain to GAIN.)
    ecmd_feature(rfm12_setdrssi, "rfm12 setdrssi", DRSSI, Set the drssi to DRSSI.)
  ecmd_endif()
ecmd_endif()

ecmd_ifdef(RFM12_ASK_SENDER_SUPPORT)
  ecmd_ifdef(RFM12_ASK_TEVION_SUPPORT)
  ecmd_feature(rfm12_ask_tevion_send, "rfm12 tevion", , housecode command delay cnt)
  ecmd_endif()
  ecmd_ifdef(RFM12_ASK_2272_SUPPORT)
    ecmd_feature(rfm12_ask_2272_send, "rfm12 2272", , housecodeCommand delay cnt)
  ecmd_endif()
ecmd_endif()
ecmd_ifdef(RFM12_ASK_EXTERNAL_FILTER_SUPPORT)
  ecmd_feature(rfm12_ask_external_filter, "rfm12 external filter", , Toggel receiver)
ecmd_endif()
ecmd_ifdef(RFM12_ASK_SENSING_SUPPORT)
  ecmd_feature(rfm12_ask_sense, "rfm12 ask sense")
ecmd_endif()

block(Miscelleanous)

ecmd_ifndef(TEENSY_SUPPORT)
  ecmd_feature(d, "d ", ADDR, Dump the memory at ADDR (16 bytes).)
  ecmd_feature(help, "help",, List which commands are available.)

dnl  ecmd_ifdef(USART_SUPPORT)
dnl    ecmd_feature(usart_baud, "usart baud", BAUD, Set the USART baudrate to BAUD.)
dnl  ecmd_endif()

ecmd_endif()

ecmd_ifdef(MODBUS_SUPPORT)
  ecmd_feature(modbus_recv, "mb recv ")
ecmd_endif()

ecmd_ifdef(HTTPD_AUTH_SUPPORT)
  ecmd_feature(http_passwd, "http passwd")
ecmd_endif()

ecmd_ifdef(DC3840_SUPPORT)
  block(DC3840 Handycam support)
  ecmd_feature(dc3840_capture, "dc3840 capture",, Make a picture)
  ecmd_feature(dc3840_send, "dc3840 send ", A B C D E, Send provided command bytes to the camera.)
  ecmd_feature(dc3840_sync, "dc3840 sync",, Re-sync to the camera)
ecmd_endif

ecmd_ifdef(PWM_MELODY_SUPPORT)
  ecmd_feature(pwm, "pwm", , start pwm)
ecmd_endif

ecmd_ifdef(I2C_DETECT_SUPPORT)
  block(I2C (TWI))
  ecmd_feature(i2c_detect, "i2c detect",,list detected I2C Chips)
ecmd_endif

ecmd_ifdef(I2C_LM75_SUPPORT)
  ecmd_feature(i2c_lm75, "lm75", ADDR, Get temperature)
ecmd_endif
ecmd_ifdef(I2C_PCA9531_SUPPORT)
  ecmd_feature(i2c_pca9531, "pca9531", ADDR PERIODPWM1 DUTYPWM1 PERIODPWM2 DUTYPWM2 LED0..3 LED4..7, set PWM1 and PWM2 and LED states)
ecmd_endif
ecmd_ifdef(I2C_PCF8574X_SUPPORT)
  ecmd_feature(i2c_pcf8574x_read, "pcf8574x read", ADDR CHIP, Get bits)
  ecmd_feature(i2c_pcf8574x_set, "pcf8574x set", ADDR CHIP HEXVALUE, Set bits)
ecmd_endif

ecmd_ifdef(EEPROM_SUPPORT)
  ecmd_feature(eeprom_reinit, "eeprom reinit",, Force reinitialization of the EEPROM config area)
ecmd_endif

ecmd_ifdef(MCUF_SUPPORT)
  ecmd_ifdef(MCUF_CLOCK_SUPPORT)
    ecmd_feature(mcuf_show_clock, "mcuf showclock")
  ecmd_endif
  ecmd_ifdef(MCUF_SCROLLTEXT_SUPPORT)
   ecmd_feature(mcuf_show_string, "mcuf showstring")
  ecmd_endif
  ecmd_ifdef(MCUF_MODUL_SUPPORT)
   ecmd_feature(mcuf_modul_list, "mcuf modul list",, List all modules)
   ecmd_feature(mcuf_modul, "mcuf modul")
  ecmd_endif
ecmd_endif

ecmd_ifdef(MYSQL_SUPPORT)
  ecmd_feature(mysql_query, "mysql query ")
ecmd_endif

ecmd_ifdef(ALIASCMD_SUPPORT)
  ecmd_feature(alias_list, "alias list")
ecmd_endif

ecmd_ifdef(ECMD_SCRIPT_SUPPORT)
  ecmd_feature(goto, "goto ")
  ecmd_feature(exit, "exit")
  ecmd_feature(wait, "wait ")
  ecmd_feature(set, "set ")
  ecmd_feature(get, "get ")
  ecmd_feature(inc, "inc ")
  ecmd_feature(dec, "dec ")
  ecmd_feature(call, "call ")
  ecmd_feature(if, "if ")
ecmd_endif

ecmd_ifdef(BUTTONS_INPUT_SUPPORT)
  ecmd_feature(push, "push ")
ecmd_endif

ecmd_ifdef(TWITTER_SUPPORT)
  ecmd_feature(tw, "tw ")
ecmd_endif
