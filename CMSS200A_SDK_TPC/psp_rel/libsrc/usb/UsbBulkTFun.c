/*
 *********************************************************************************************************
 *                                       ACTOS GL3976
 *
 *
 *                           (c) Copyright 2002-2007, Actions Co,Ld.
 *                                   All Right Reserved
 *
 * File   : UsbBulkFun.msa
 * By     : zhouxl
 * Version: v1.02     first version     2004-12-14 9:09
 *********************************************************************************************************
 */

#include "udisklib.h"

#pragma name(BULkFUN_C)

#pragma renamecode(?PR?BULkFUN_C)
#pragma ROM(HUGE)
/*
 *********************************************************************************************************
 *                                           DMA_Data_Transfer
 *
 * Description: 使用通用DMA在USB FIFO与RAM之间传输数据, USB normal模式
 *
 * Arguments  : DMA_DT_Trnsfr_t
 *
 * Returns    : None.
 *
 * Note(s)    :
 *********************************************************************************************************
 */
void DMA_Data_Transfer(DMA_DT_Trnsfr_t *dma_struct)
{
    if (fifo_trans_mode == 0x01)
    {
        SFR_BANK = BANK_DMA_CARD;
    }
    else
    {
        SFR_BANK = BANK_DMA0;
    }
    DMAnCTL1 = dma_struct->pagenum;

    if ((dma_struct->pagenum & 0x0f) == 0x0a)
    {
        DMAnSADDR0H = 0xff;
    }
    else
    {
        DMAnSADDR0H = 0x00;
    }
    DMAnSADDR0L = (uint8) dma_struct->source_addr;
    DMAnSADDR0M = (uint8) (dma_struct->source_addr >> 8);

    if ((dma_struct->pagenum & 0xf0) == 0xa0)
    {
        DMAnDADDR0H = 0xff;
    }
    else
    {
        DMAnDADDR0H = 0x00;
    }    
	
	DMAnDADDR0L = (uint8) dma_struct->dst_addr;
    DMAnDADDR0M = (uint8) (dma_struct->dst_addr >> 8);

    DMAnFrameLenL = (uint8) dma_struct->length;
    DMAnFrameLenH = (uint8) (dma_struct->length >> 8);

    SFR_BANK = BANK_USB;
    if ((dma_struct->pagenum & 0x0f) == 0x04) //源地址为usbfifo
    {
        EP2DMALENL = (uint8) dma_struct->length;
        EP2DMALENH = (uint8) (dma_struct->length >> 8);               
		UDMAM = UDMAM | 0x02; //DMAEPSEL = OUT2

    }
    else
    {
		EP1DMALENL = (uint8) dma_struct->length;
        EP1DMALENH = (uint8) (dma_struct->length >> 8);         
		UDMAM = UDMAM | 0x01; //DMAST = DMA
    }

    if (fifo_trans_mode != 0x01)
    {
        SFR_BANK = BANK_DMA0;
		DMAnCTL0 = DMAnCTL0 | 0x01; //Nomal 模式
    }   

    SFR_BANK = BANK_USB;

}

/*
 *********************************************************************************************************
 *                                           Wait_PrevDMA_TransEnd
 *
 * Description: 等待上一次DMA传输完成.
 *
 * Arguments  : None
 *
 * Returns    : None.
 *
 * Note(s)    : 对于短包要特殊处理.
 *********************************************************************************************************
 */
void Wait_PrevDMA_TransEnd(void)
{
    if (fifo_trans_mode == 0x01)
    {
        /*
          由于flash fifo直通由flash主导，且flash
          内部数据传输分成很多小块；那很有可能拔线后
          dma已经结束，导致无法退出U盘。
        */
        SFR_BANK = BANK_USB;
#ifndef _FPGA_VERTION_
       if (((DPDMCTRL & 0x40) == 0x00) || ((Usbirq_hcusbirq & 0x10) != 0x00))
       {
          ifIsConnect();
       }
#endif
        SFR_BANK = BANK_DMA_CARD;
    }
    else
    {
        SFR_BANK = BANK_DMA0;
    }

    //wait DMA0 complete
    while ((DMAnCTL0 & 0x01) != 0x00)
    {
        //拔线处理
        ifIsConnect();
    }

    //wait usb fifo empty
    if (((DMAnCTL1 >> 4) & 0x0f) == 0x04) //目的地址为usbfifo
    {
        SFR_BANK = BANK_USB;
        while ((UDMAM & 0x01) != 0x00)
        {
            //拔线处理
            ifIsConnect();
        }
        while ((In1cs_hcout1cs & 0x0E) != 0x0C) //FIFO Empty?
        {
            //拔线处理
            ifIsConnect();
        }
    }

    if (fifo_trans_mode == 0x01)
    {
        SFR_BANK = BANK_DMA_CARD;
    }
    else
    {
        SFR_BANK = BANK_DMA0;
    }
    //FIFO直通模式结束后关闭直通配置，避免DMA的配置处于fifo直通模式,导致后续存储介质处理异常
    DMAnCTL1 = 0x00;

    SFR_BANK = BANK_USB;
}

/*
 *********************************************************************************************************
 *                                           Send_CSW
 *
 * Description: Send Command Status Wrapper(CSW).
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void Send_CSW(void)
{
    Send_Data_To_PC((uint8 *) &CSWBuffer, 13);
    //CWW阶段已完成，清fifo为下一个IN准备
    EPB_OutRdy_ShortPacketNAK();
}

/*
 *********************************************************************************************************
 *                                           Send_Data_To_PC
 *
 * Description: Upload request length data to PC through endpointA.
 *
 * Arguments  : de: Addr of Data for Sending;
 *            : bc: Data Length(The length MUST less than FIFO Buffer length);
 *
 * Returns    : None.
 *
 * Note(s)    : The shared fifo will be switched to in endpoint.
 *********************************************************************************************************
 */
void Send_Data_To_PC(uint8 *data_p, uint16 length)
{
    char *dataAddr;
    int16 i, sendLength, count;

    dataAddr = data_p;
    sendLength = length;

    while (sendLength != 0)
    {
        count = (sendLength >= max_pkt_size) ? max_pkt_size : sendLength;

        for (i = 0; i < count; i++)
        {
            Fifo1dat = (char *) dataAddr[i]; //fill epa fifo
        }

        if (count != max_pkt_size)
        {
            In1cs_hcout1cs = In1cs_hcout1cs | 0x02; //短包,需要手动设置Busy位
        }

        while ((In1cs_hcout1cs & 0x0E) != 0x0C) //FIFO Empty?
        {
            //拔线处理
            ifIsConnect();
        }
        sendLength = sendLength - count;
        dataAddr = dataAddr + count;
    }
}

/*
 *********************************************************************************************************
 *                                           EPB_OutRdy_ShortPacketNAK
 *
 * Description: NAK the endpointB if a short packet had been received.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : Memory必须切给EPB.
 *********************************************************************************************************
 */
void EPB_OutRdy_ShortPacketNAK(void)
{
    //reset all endpoint fifo
    EPRST = 0x42;
    //switch 2kbyte fifo to endpointB

    //disable out packet counter
    OUTPCKCNTL = 0x00;
    OUTPCKCNTH = 0x00;
    USBEIRQ = USBEIRQ | (uint8) 0x80;
    //clear epB interrupt to rx next out packet
    SHORTPCKIRQ = SHORTPCKIRQ | 0x20;
}

/*
 *********************************************************************************************************
 *                                           EPB_OutRdy_FIFOFullNAK
 *
 * Description: NAK if the useful space size of index endpoint FIFO is smaller than the maxpacket size.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : Memory mest convert to EPB.
 *********************************************************************************************************
 */
void EPB_OutRdy_FIFOFullNAK(void)
{
    //switch 2kbyte fifo to endpointB
    //clear epB interrupt to rx next out packet
    SHORTPCKIRQ = SHORTPCKIRQ | 0x20;
}

/*
 *********************************************************************************************************
 *                                           EPA_In_Rdy
 *
 * Description: Set the index in enpoint ready for upload data to host.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : Memory必须切给EPA.
 *********************************************************************************************************
 */
void EPA_In_Rdy(void)
{

}
#pragma ROM(HUGE)
