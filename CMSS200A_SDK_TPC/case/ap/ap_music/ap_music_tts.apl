# generated by APBuild 3.0

# full path of HEX filename.
hexfile=.\Obj\ap_music.hex
mask=0x0
# now supports srec & ihex, default to srec
hextype=intel

# full path of MAP filename.
mapfile=.\Obj\ap_music.map

# full path of output AP filename.
apfile=.\music.ap

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
seg_text(0xff2000-0xff23FF)=API30,?PR?AP_SUB_INT,?PR?COM_MSG,?PR?AP_SEND_COMMAND,?PR?COM_EQTOOL_RCODE,C_RESERVE0
seg_text(0xfea000)=?C_INITEDATA
seg_text(0xfee000)=?C_INITHDATA
seg_text(0xffF000)=?C_C51STARTUP

#常驻数据段 
seg_data(0x680-0xa80)=?ED?COM_COM,?ED?AP_MAIN,?ED?MW_EH_FSEL_API,?ED?AP_KALACBK,?ED?MW_EH_KARAOKE,?ED?MW_EH_TPCHK_MP3,?ED?MW_EH_ID31,?ED?MW_CC_AD_DEC_R,?ED?MW_BS_AD_CMDR,?ED?MO_EH_USERPL,?ED?MW_BS_AD_CMDR_TTS,?ED?COM_REPORT,D_RESERVE0
#seg_data(0x6c00-0x6DFF)=?ED?DIR_MENU_1,?ED?DIR_MAIN,?ED?DIR_SEL_API,D_RESERVE1
seg_data(0xd80-0xdff)=?ED?MW_EH_FSEL_SHUFFLE,D_RESERVE2
#未初始化数据段 
#seg_bss(0x3400)=?ED?DIR_MENU_1
#seg_bss(0xF000)=?ED?DATA_BUFFER?MW_EH_LRC_SUB4
#seg_bss(0x9000)=?ED?RESULT_BUFFER?MW_EH_LRC_SUB1
#seg_bss(0x7E00)=?ED?DATA_BUFFER?MW_EH_LRC_SUB4
#seg_bss(0x8000)=?ED?RESULT_BUFFER?MW_EH_LRC_SUB1

#CTRL bank group
#//FUNCTION//
CTRL_bank0(0x2400-0x2CFF)=?HC?AP_MAIN,?PR?AP_MAIN,?HC?AP_CLOCK_1,?PR?AP_CLOCK_1,C_RESERVE1
CTRL_bank1(0x2400-0x2CFF)=?HC?AP_SUB_DIS,?PR?AP_SUB_DIS,?PR?FILE_SELECTOR,C_RESERVE2
CTRL_bank2(0x2400-0x2CFF)=?HC?AP_UI_PLY,?PR?AP_UI_PLY,C_RESERVE3
CTRL_bank3(0x2400-0x2CFF)=?HC?GET_MUSICINFO,?PR?GET_MUSICINFO,C_RESERVE4
CTRL_bank4(0x2400-0x2CFF)=?HC?SUB_FUNCTION,?PR?SUB_FUNCTION,C_RESERVE5
CTRL_bank5(0x2400-0x2CFF)=?PR?AP_CLOCK,?HC?AP_CLOCK,C_RESERVE6
CTRL_bank6(0x2400-0x2cFF)=?PR?MUSIC_REPORT,?FD?MUSIC_REPORT,?HC?AP_KALACBK,?PR?AP_KALACBK,C_RESERVE7
CTRL_bank7(0x2400-0x2cFF)=COM_REPORT,?HC?COM_REPORT,?PR?COM_REPORT_SUB,C_RESERVE8
CTRL_bank8(0x2400-0x2CFF)=?HC?COM_EQTOOL_SAVE,?FD?COM_EQTOOL_SAVE,?PR?COM_EQTOOL_SAVE,?PR?COM_EQTOOL
#//ALARM//
#CTRL_bank14(0x2300-0x2bFF)=?HC?APC_UI_MAIN,?PR?APC_UI_MAIN,?PR?ALARM_UI_PLY,?HC?ALARM_UI_PLY,?PR?ALARM_UI_NAVI,?HC?ALARM_UI_NAVI,C_RESERVE67
#CTRL_bank15(0x2300-0x2bFF)=?PR?ALARM_UI_PLY,?HC?ALARM_UI_PLY,C_RESERVE68
#CTRL_bank16(0x2300-0x2bFF)=?PR?ALARM_UI_SDPLY,?PR?COM_ALARM,C_RESERVE69
#CTRL_bank17(0x2300-0x2bFF)=?PR?ALARM_UI_NAVI,?HC?ALARM_UI_NAVI,C_RESERVE70
#CTRL_bank18(0x2300-0x2bFF)=?PR?COM_ALARM,C_RESERVE71


#CTRL_bank13(0x2400-0x2bFF)=?HC?AP_KALACBK,?PR?AP_KALACBK,C_RESERVE14



#UI bank group
#//资源文件处理和消息处理*/
UI_bank0(0x2d00-0x2fFF)=?PR?COM_MSG2,?HC?COM_MSG2,C_RESERVE17
UI_bank1(0x2d00-0x2fFF)=?PR?UNI_TO_CHAR,?FD?UNI_TO_CHAR,?HC?UNI_TO_CHAR
UI_bank2(0x2d00-0x2fFF)=?HC?COM_COM2,?PR?COM_COM2
UI_bank3(0x2d00-0x2fFF)=?PR?COM_COM
#//TTS sub function
#UI_bank3(0x2c00-0x2fFF)=?PR?COM_REPORT_SUB


#midware enhance bank group
seg_bss(0x3000-0x31ff)=?ED?MW_EH_FSEL_NEXT[!]
EH1_bank0(0x3000-0x37FF)=?HC?MW_EH_FSEL_NEXT,MW_EH_FSEL_NEXT,C_RESERVE34
EH1_bank1(0x3000-0x37FF)=?HC?MW_EH_FSEL_PREV1,MW_EH_FSEL_PREV1,C_RESERVE35
EH1_bank2(0x3000-0x37FF)=?HC?MW_EH_FSEL_PREV,MW_EH_FSEL_PREV,C_RESERVE36
EH1_bank3(0x3000-0x37FF)=?FD?MW_EH_FSEL_API,MW_EH_FSEL_API,?HC?MW_EH_FSEL_API,C_RESERVE37

#//ID3
EH1_bank4(0x3000-0x37FF)=?FD?MW_EH_ID3,?HC?MW_EH_ID3,MW_EH_ID3,C_RESERVE38
EH1_bank5(0x3000-0x37FF)=?FD?MW_EH_ID31,?HC?MW_EH_ID31,MW_EH_ID31,C_RESERVE39

#//TypeCheck
seg_bss(0x3000-0x3200)=	?FD?MW_EH_TPCHK_MP3
EH1_bank6(0x3000-0x3fFF)=MW_EH_TPCHK_MP3,MW_EH_TPCHK_MP31,?HC?MW_EH_TPCHK_MP31,C_RESERVE40
EH1_bank7(0x3000-0x3fFF)=MW_EH_TPCHK_WMA,?FD?MW_EH_TPCHK_WMA,?HC?MW_EH_TPCHK_WMA,C_RESERVE41
EH1_bank8(0x3000-0x3fFF)=MW_EH_TPCHK_WAV,?FD?MW_EH_TPCHK_WAV,?HC?MW_EH_TPCHK_WAV,C_RESERVE42


#//enhance(caraoke) bank
EH1_bank13(0x32D0-0x37FF)=MW_EH_KARAOKE,?HC?MW_EH_KARAOKE,C_RESERVE73
EH1_bank14(0x32D0-0x37FF)=MW_EH_KARAOKE_SUB,?HC?MW_EH_KARAOKE_SUB,C_RESERVE74

#//enhance(favourite) bank
seg_bss(0x3000)=?ED?MO_EH_USERPL1[!]
EH1_bank15(0x3000-0x37FF)=?HC?MO_EH_USERPL,MO_EH_USERPL,?HC?MO_EH_USERPL1,MO_EH_USERPL1,C_RESERVE75
EH1_bank16(0x3000-0x37FF)=?HC?MW_EH_FSEL_SHUFFLE,MW_EH_FSEL_SHUFFLE,C_RESERVE76

#basal bank group
BS_bank1(0x3000-0x301f)[BR]=?HC?MW_BS_AD_CMDR,C_RESERVE44
BS_bank1(0x3020-0x32bf)[BR]=MW_BS_AD_CMDR,C_RESERVE90
BS_bank1(0x32C0-0x37FF)=MW_BS_AD_CMD,C_RESERVE45
BS_bank2(0x32C0-0x37FF)=MW_BS_AD_CMD1,C_RESERVE46
BS_bank3(0x32C0-0x37FF)=MW_BS_AD_CMD2,C_RESERVE47

#BS_bank4(0x3000-0x321F)[BR]=MW_BS_AD_CMDR_SD,C_RESERVE63
#BS_bank4(0x3220-0x37FF)=MW_BS_AD_CMD_SD,C_RESERVE64
#BS_bank6(0x3220-0x37FF)=MW_BS_AD_CMD1_SD,C_RESERVE65
#BS_bank7(0x3220-0x37FF)=MW_BS_AD_CMD2_SD,C_RESERVE66

#//TTS Basal
#BS_bank8(0x3000-0x302F)=?FD?MW_BS_AD_CMDR_TTS
BS_bank9(0x3030-0x357F)[BR]=MW_BS_AD_CMDR_TTS
BS_bank9(0x3580-0x35AF)[BR]=?HC?MW_BS_AD_CMDR_TTS
BS_bank9(0x35B0-0x37FF)=MW_BS_AD_CMD_TTS
BS_bank10(0x35B0-0x37FF)=MW_BS_AD_CMD1_TTS

#codec bank group
CC_bank2(0x3800-0x3c2F)[BR]=MW_CC_AD_DEC_R,C_RESERVE48
CC_bank1(0x3c30-0x3fFF)=MW_CC_AD_DEC_B1,C_RESERVE49
CC_bank2(0x3c30-0x3fFF)=MW_CC_AD_DEC_B2,C_RESERVE50
CC_bank3(0x3c30-0x3fFF)=MW_CC_AD_DEC_B3,C_RESERVE51
CC_bank4(0x3c30-0x3fFF)=MW_CC_AD_DEC_B4,C_RESERVE52
CC_bank5(0x3c30-0x3fFF)=MW_CC_AD_DEC_B6,C_RESERVE54