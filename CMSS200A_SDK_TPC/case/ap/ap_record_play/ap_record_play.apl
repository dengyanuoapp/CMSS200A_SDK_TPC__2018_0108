# generated by APBuild 3.0

# full path of HEX filename.
hexfile=.\Obj\ap_record_play.hex
mask=0x0
# now supports srec & ihex, default to srec
hextype=intel

# full path of MAP filename.
mapfile=.\Obj\ap_record_play.map

# full path of output AP filename.
apfile=.\ap_rec_p.ap

# entry of AP.
entry=main

# segments define.

linkcfg0=begin
NOOVERLAY
NODEFAULTLIBRARY
CLASSES(bit(0x21-0x2f))
CLASSES(ECODE(0x800000-0xFFFFFF))
CLASSES(HCONST(0x800000-0xFFFFFF))
CLASSES(SROM(0xFEC000-0xFFFFFF))
linkcfg0=end

#常驻代码段 
seg_text(0xff2000-0xff24ff)=API30,?PR?COM_MSG,?PR?AP_RCODE,?PR?MW_PITCH_DEC,C_RESERVE0
seg_text(0xfea000)=?C_INITEDATA
seg_text(0xfee000)=?C_INITHDATA
seg_text(0xffF000)=?C_C51STARTUP

#常驻数据段 
seg_data(0x680-0xa7f)=?ED?COM_COM,?ED?AP_MAIN,?ED?MW_CC_AD_ENC_R,?FD?MW_BS_AD_EN,?ED?UI_PLAY,?ED?MW_CC_AD_DEC_R,?ED?MW_BS_AD_CMDR,?ED?MW_PITCH_DEC,D_RESERVE0

#CTRL bank group
#//FUNCTION//
CTRL_bank0(0x2500-0x2bFF)=?PR?AP_MAIN,?HC?AP_MAIN,?PR?AP_REC,C_RESERVE1
CTRL_bank1(0x2500-0x2bFF)=?HC?AP_STOP,?PR?AP_STOP,C_RESERVE5
CTRL_bank2(0x2500-0x2bFF)=?PR?AP_CLOCK,?HC?AP_CLOCK,C_RESERVE6
CTRL_bank4(0x2500-0x2bFF)=?PR?AP_CLOCK_1,?HC?AP_CLOCK_1
CTRL_bank5(0x2500-0x2bFF)=?PR?UI_PLAY,?HC?UI_PLAY

//pitch alog data segment
CTRL_bank6(0x200-0x9ff)=?HC?MW_PITCH_DEC,?FD?MW_PITCH_DEC

#UI bank group
#//资源文件处理和消息处理*/
UI_bank0(0x2c00-0x2FFF)=?PR?COM_MSG2,?HC?COM_MSG2,C_RESERVE18
UI_bank1(0x2c00-0x2FFF)=?HC?COM_COM2,?PR?COM_COM2,?PR?MW_ADDA_DEC
UI_bank2(0x2c00-0x2fFF)=?PR?COM_COM

#//复用enhanced 空间，减少设置闹钟时切换bank
##midware enhance bank group
#EH1_bank0(0x3000-0x39FF)=?HC?MW_EH_FSEL_NEXT,MW_EH_FSEL_NEXT,C_RESERVE30
#EH1_bank1(0x3000-0x39FF)=?HC?MW_EH_FSEL_PREV1,MW_EH_FSEL_PREV1,C_RESERVE31
#EH1_bank2(0x3000-0x39FF)=?HC?MW_EH_FSEL_PREV,MW_EH_FSEL_PREV,C_RESERVE32
#EH1_bank3(0x3000-0x39FF)=MW_EH_FSEL_API,?HC?MW_EH_FSEL_API,C_RESERVE33
#EH1_bank4(0x3000-0x39FF)=MW_EH_NAME,C_RESERVE34

#basal bank group
BS_bank1(0x3000-0x39FF)=MW_BS_AD_EN,C_RESERVE40

#codec bank group
CC_bank0(0x3a00-0x3bFF)[BR]=MW_CC_AD_ENC_R,C_RESERVE41
CC_bank1(0x3c00-0x3fFF)=MW_CC_AD_ENC_B1,C_RESERVE42
CC_bank0(0x3c00-0x3fFF)=MW_CC_AD_ENC_B2,C_RESERVE43

#basal bank group
BS_bank2(0x3000-0x301f)[BR]=?HC?MW_BS_AD_CMDR,C_RESERVE44
BS_bank2(0x3020-0x32bf)[BR]=MW_BS_AD_CMDR,C_RESERVE90
BS_bank2(0x32C0-0x37FF)=MW_BS_AD_CMD,C_RESERVE45
BS_bank3(0x32C0-0x37FF)=MW_BS_AD_CMD1,C_RESERVE46
BS_bank4(0x32C0-0x37FF)=MW_BS_AD_CMD2,C_RESERVE47

#codec bank group
CC_bank2(0x3800-0x3c2F)[BR]=MW_CC_AD_DEC_R,C_RESERVE48
CC_bank6(0x3c30-0x3fFF)=MW_CC_AD_DEC_B1,C_RESERVE49
CC_bank2(0x3c30-0x3fFF)=MW_CC_AD_DEC_B2,C_RESERVE50
CC_bank3(0x3c30-0x3fFF)=MW_CC_AD_DEC_B3,C_RESERVE51
CC_bank4(0x3c30-0x3fFF)=MW_CC_AD_DEC_B4,C_RESERVE52
CC_bank5(0x3c30-0x3fFF)=MW_CC_AD_DEC_B6,C_RESERVE54