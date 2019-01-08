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
 * Description: ʹ��ͨ��DMA��USB FIFO��RAM֮�䴫������, USB normalģʽ
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
    if ((dma_struct->pagenum & 0x0f) == 0x04) //Դ��ַΪusbfifo
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
		DMAnCTL0 = DMAnCTL0 | 0x01; //Nomal ģʽ
    }   

    SFR_BANK = BANK_USB;

}

/*
 *********************************************************************************************************
 *                                           Wait_PrevDMA_TransEnd
 *
 * Description: �ȴ���һ��DMA�������.
 *
 * Arguments  : None
 *
 * Returns    : None.
 *
 * Note(s)    : ���ڶ̰�Ҫ���⴦��.
 *********************************************************************************************************
 */
void Wait_PrevDMA_TransEnd(void)
{
    if (fifo_trans_mode == 0x01)
    {
        /*
          ����flash fifoֱͨ��flash��������flash
          �ڲ����ݴ���ֳɺܶ�С�飻�Ǻ��п��ܰ��ߺ�
          dma�Ѿ������������޷��˳�U�̡�
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
        //���ߴ���
        ifIsConnect();
    }

    //wait usb fifo empty
    if (((DMAnCTL1 >> 4) & 0x0f) == 0x04) //Ŀ�ĵ�ַΪusbfifo
    {
        SFR_BANK = BANK_USB;
        while ((UDMAM & 0x01) != 0x00)
        {
            //���ߴ���
            ifIsConnect();
        }
        while ((In1cs_hcout1cs & 0x0E) != 0x0C) //FIFO Empty?
        {
            //���ߴ���
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
    //FIFOֱͨģʽ������ر�ֱͨ���ã�����DMA�����ô���fifoֱͨģʽ,���º����洢���ʴ����쳣
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
    //CWW�׶�����ɣ���fifoΪ��һ��IN׼��
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
            In1cs_hcout1cs = In1cs_hcout1cs | 0x02; //�̰�,��Ҫ�ֶ�����Busyλ
        }

        while ((In1cs_hcout1cs & 0x0E) != 0x0C) //FIFO Empty?
        {
            //���ߴ���
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
 * Note(s)    : Memory�����и�EPB.
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
 * Note(s)    : Memory�����и�EPA.
 *********************************************************************************************************
 */
void EPA_In_Rdy(void)
{

}
#pragma ROM(HUGE)
