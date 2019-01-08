#ifndef _MID_ENTRY_H
#define _MID_ENTRY_H

#include "srom.h"

//#define MID_ENTRY	0xFFAA00
//#define MID_CALL(x,y,z){int (*midcall)(unsigned int, unsigned int ,  unsigned int)=MID_ENTRY; midcall(x,y,z);}

////////////////////////////////////////////////////////API NUM
#define MID_BASAL_TYPE		       0x0100
#define MID_ENHANCED_TYPE	       0x0200
#define MID_CODEC_TYPE		       0x0400
//////////////////////////////////////////////basal command
#define BASAL_MP3_PLAY           	0x0101
#define BASAL_WMA_PLAY           	0x0102
#define BASAL_OGG_PLAY          	0x0103

//////////////////////////////////////////////enhanced command
#define ENHANCED_LRC_SHOW        	0x0201
#define ENHANCED_ID3_SHOW        	0x0202
//////////////////////////////////////////////enhanced command
#define CODEC_MP3_ENC	        	0x0401
#define CODEC_MP3_DEC	        	0x0402
#define CODEC_WMA_ENC	        	0x0403
#define CODEC_WMA_DEC	        	0x0404
#define CODEC_OGG_ENC	        	0x0405
#define CODEC_OGG_DEC	        	0x0406

//////////////////////////////////////////////////////////bcall declare
#define BCALL0(x,y)  {B=x;y();}
#define BCALL1(x,y,z)  {B=x;y(z);}
#define BCALL2(x,y,z,m)  {B=x;y(z,m);}
#define BCALL3(x,y,z,m,n)  {B=x;y(z,m,n);}
#define BCALL4(x,y,z,m,n,p)  {B=x;y(z,m,n,p);}
#define BCALL5(x,y,z,m,n,p,q)  {B=x;y(z,m,n,p,q);}

////////////////////////////////////////////////////////////call declare
SROM_PS(basal_wma_play_BASAL_WMA)
SROM_PS(basal_mp3_play_BASAL_MP3)
SROM_PS(basal_ogg_play_BASAL_OGG)
SROM_PS(enhanced_lrc_show_EN_LRC)
SROM_PS(enhanced_id3_show_EN_ID3)
SROM_PS(codec_wma_enc_WMA_ENC)
SROM_PS(codec_wma_dec_WMA_DEC)
SROM_PS(codec_mp3_enc_MP3_ENC)
SROM_PS(codec_mp3_dec_MP3_DEC)
SROM_PS(codec_ogg_enc_OGG_ENC)
SROM_PS(codec_ogg_dec_OGG_DEC)

#define CALL_basal_wma_play(x,y)    	BCALL2(SROM_PS_BNK(basal_wma_play_BASAL_WMA),basal_wma_play,x,y)
#define CALL_basal_mp3_play(x,y)    	BCALL2(SROM_PS_BNK(basal_mp3_play_BASAL_MP3),basal_mp3_play,x,y)
#define CALL_basal_ogg_play(x,y)    	BCALL2(SROM_PS_BNK(basal_ogg_play_BASAL_OGG),basal_ogg_play,x,y)

#define CALL_enhanced_lrc_show(x,y)     BCALL2(SROM_PS_BNK(enhanced_lrc_show_EN_LRC),enhanced_lrc_show,x,y)
#define CALL_enhanced_id3_show(x,y)     BCALL2(SROM_PS_BNK(enhanced_id3_show_EN_ID3),enhanced_id3_show,x,y)

#define CALL_codec_wma_enc(x,y)   	BCALL2(SROM_PS_BNK(codec_wma_enc_WMA_ENC),codec_wma_enc,x,y)
#define CALL_codec_wma_dec(x,y)    	BCALL2(SROM_PS_BNK(codec_wma_dec_WMA_DEC),codec_wma_dec,x,y)
#define CALL_codec_mp3_enc(x,y)   	BCALL2(SROM_PS_BNK(codec_mp3_enc_MP3_ENC),codec_mp3_enc,x,y)
#define CALL_codec_mp3_dec(x,y)   	BCALL2(SROM_PS_BNK(codec_mp3_dec_MP3_DEC),codec_mp3_dec,x,y)
#define CALL_codec_ogg_enc(x,y)    	BCALL2(SROM_PS_BNK(codec_ogg_enc_OGG_ENC),codec_ogg_enc,x,y)
#define CALL_codec_ogg_dec(x,y)   	BCALL2(SROM_PS_BNK(codec_ogg_dec_OGG_DEC),codec_ogg_dec,x,y)

///////////////////////////////////////////////////////////extern declare
extern void basal_wma_play(unsigned int cmd, unsigned  int arg)	;
extern void basal_mp3_play(unsigned int cmd, unsigned  int arg) ;
extern void basal_ogg_play(unsigned int cmd, unsigned  int arg) ;

extern void enhanced_lrc_show(unsigned int cmd, unsigned  int arg);
extern void enhanced_id3_show(unsigned int cmd, unsigned  int arg);

extern void codec_wma_enc(unsigned int cmd, unsigned  int arg);
extern void codec_mp3_dec(unsigned int cmd, unsigned  int arg);
extern void codec_mp3_enc(unsigned int cmd, unsigned  int arg);
extern void codec_ogg_dec(unsigned int cmd, unsigned  int arg);
extern void codec_ogg_enc(unsigned int cmd, unsigned  int arg);
extern void codec_wma_dec(unsigned int cmd, unsigned  int arg);


#endif
