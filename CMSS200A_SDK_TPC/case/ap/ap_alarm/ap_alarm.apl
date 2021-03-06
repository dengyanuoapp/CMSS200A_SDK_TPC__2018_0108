# generated by APBuild 3.0

# full path of HEX filename.
hexfile=.\Obj\ap_alarm.hex
mask=0x0
# now supports srec & ihex, default to srec
hextype=intel

# full path of MAP filename.
mapfile=.\Obj\ap_alarm.map

# full path of output AP filename.
apfile=.\alarm.ap

# entry of AP.
entry=main

# segments define.

linkcfg0=begin
NOOVERLAY
NODEFAULTLIBRARY
CLASSES(BIT(0x21-0x2f))
CLASSES(ECODE(0x800000-0xFFFFFF))
CLASSES(HDATA(0x800000-0xFFFFFF))
CLASSES(HCONST(0x800000-0xFFFFFF))
CLASSES(SROM(0xFEC000-0xFFFFFF))
linkcfg0=end

#常驻代码段 
seg_text(0xff2000-0xff2bFF)=API30,?PR?ALARM_MAIN,?HC?ALARM_MAIN,?PR?COM_MSG,?PR?ALARM_UI_PLY,?HC?ALARM_UI_PLY,C_RESERVE0
seg_text(0xfea000)=?C_INITEDATA
seg_text(0xffF000)=?C_C51STARTUP

#常驻数据段 
seg_data(0x680-0xa80)=?ED?COM_COM,?ED?ALARM_MAIN,?ED?MW_EH_TPCHK_MP3,?ED?MW_CC_AD_DEC_R,?ED?MW_BS_AD_CMDR,?ED?MW_BS_AD_CMDR_SD,D_RESERVE0

#UI bank group
#//资源文件处理和消息处理*/
UI_bank0(0x2c00-0x2fFF)=?PR?COM_MSG2,?HC?COM_MSG2,C_RESERVE17
UI_bank1(0x2c00-0x2fFf)=?HC?COM_COM2,?PR?COM_COM2
UI_bank2(0x2d00-0x2fFF)=?PR?COM_COM

#//TypeCheck
seg_bss(0x3000-0x3200)=	?FD?MW_EH_TPCHK_MP3
EH1_bank6(0x3000-0x3fFF)=MW_EH_TPCHK_MP3,MW_EH_TPCHK_MP31,?HC?MW_EH_TPCHK_MP31,C_RESERVE40
EH1_bank7(0x3000-0x3fFF)=MW_EH_TPCHK_WMA,?FD?MW_EH_TPCHK_WMA,?HC?MW_EH_TPCHK_WMA,C_RESERVE41
EH1_bank8(0x3000-0x3fFF)=MW_EH_TPCHK_WAV,?FD?MW_EH_TPCHK_WAV,?HC?MW_EH_TPCHK_WAV,C_RESERVE42


#basal bank group
BS_bank1(0x3000-0x301f)[BR]=?HC?MW_BS_AD_CMDR,C_RESERVE44
BS_bank1(0x3020-0x32bf)[BR]=MW_BS_AD_CMDR,C_RESERVE90
BS_bank1(0x32C0-0x37FF)=MW_BS_AD_CMD,C_RESERVE45
BS_bank2(0x32C0-0x37FF)=MW_BS_AD_CMD1,C_RESERVE46
BS_bank3(0x32C0-0x37FF)=MW_BS_AD_CMD2,C_RESERVE47

BS_bank4(0x3000-0x321F)[BR]=MW_BS_AD_CMDR_SD,C_RESERVE63
BS_bank4(0x3220-0x37FF)=MW_BS_AD_CMD_SD,C_RESERVE64
BS_bank6(0x3220-0x37FF)=MW_BS_AD_CMD1_SD,C_RESERVE65
BS_bank7(0x3220-0x37FF)=MW_BS_AD_CMD2_SD,C_RESERVE66

#codec bank group
CC_bank2(0x3800-0x3c2F)[BR]=MW_CC_AD_DEC_R,C_RESERVE48
CC_bank1(0x3c30-0x3fFF)=MW_CC_AD_DEC_B1,C_RESERVE49
CC_bank2(0x3c30-0x3fFF)=MW_CC_AD_DEC_B2,C_RESERVE50
CC_bank3(0x3c30-0x3fFF)=MW_CC_AD_DEC_B3,C_RESERVE51
CC_bank4(0x3c30-0x3fFF)=MW_CC_AD_DEC_B4,C_RESERVE52
CC_bank5(0x3c30-0x3fFF)=MW_CC_AD_DEC_B6,C_RESERVE54
