# generated by APBuild 3.0

# full path of HEX filename.
hexfile=.\Obj\ap_standby.hex
mask=0x0
# now supports srec & ihex, default to srec
hextype=intel

# full path of MAP filename.
mapfile=.\Obj\ap_standby.map

# full path of output AP filename.
apfile=.\standby.ap

# entry of AP.
entry=main

# segments define.

linkcfg0=begin
CASE
NOOVERLAY
NOJMPTAB
NODEFAULTLIBRARY
CLASSES(BIT(0x21-0x2f))
CLASSES(ECODE(0x800000-0xFFFFFF))
CLASSES(HCONST(0x800000-0xFFFFFF))
linkcfg0=end

����� 
seg_text(0xff2000-0xff3Fff)=API30,?HC?AP_STANDBY,?PR?AP_STANDBY,?FD?AP_STANDBY,?PR?COM_MSG2,?PR?COM_COM2,?PR?COM_MSG,?HC?COM_MSG2,?PR?COM_COM,C_RESERVE0
seg_text(0xfea000)=?C_INITEDATA
seg_text(0xffF000)=?C_C51STARTUP
פ���ݶ� 
seg_data(0x680-0xa80)=?ED?COM_COM,D_RESERVE0

