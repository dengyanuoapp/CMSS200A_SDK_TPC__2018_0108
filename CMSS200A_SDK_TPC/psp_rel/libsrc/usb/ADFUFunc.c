#include "udisklib.h"

#pragma name(AUCODE_C)

#pragma ROM(HUGE)
static uint8 MBREC_Write_AndCheck(void);
static uint8 BREC_Write_AndCheck(void);
static uint8 Calculate_BREC_Checksum(void);
static void sFlash_Phy_Write(uint8 *dst_buffer);
static void sFlash_Phy_Read(uint8 *dst_buffer);
static uint16 Caculate_CheckSum(uint16 *addr, uint16 length);
#pragma ROM(HUGE)

#pragma renamecode(?PR?AUCODE_C)
#pragma ROM(HUGE)
/*
 *********************************************************************************************************
 *                                           Access_Internal_RAM_Command
 *
 * Description: ADFU protocol handle, read or write internal sram.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void Access_Internal_RAM_Command(void)
{
    //��ȡ����д�� 0:д��  01:��ȡ
    if ((CBW_data_buffer._CBWCB[1] & 0x80) == 0)
    {
        Write_Internal_RAM();
    }
    else
    {
        Read_Internal_RAM();
    }

    ReportCSW(0);
}

/*
 *********************************************************************************************************
 *                                           Access_Nand_Flash_Command
 *
 * Description: ADFU protocol handle, read or write nand flash.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void Access_Nand_Flash_Command(void)
{
    uint8 tmp;
    LogicOrPhy = CBW_data_buffer._CBWCB[0];

    //�ж��Ƿ���SPINor
    if (flash_type != 0x02)
    {
        //���������߶�ͬһ�豸�Ĳ�ͬLUN�����Ͷ�д����,��ADFU����û��ȥ����LUN��
        //���߶���ÿ��LUN�Ķ�д���ǲ���Flash,���¶�Flash���ж��ͬһsector�ŵĶ�д
        //ADFU��ֻ��Ϊ��LUN=1(Flash��)�Ķ�д�Ϸ�,������LUN����Ķ�д��������Ӧ;
        illegalLBAFlag = CBW_data_buffer._bCBWLUN - 1;
    }

    //DIR
    if ((CBW_data_buffer._CBWCB[1] & 0x80) != 0)
    {
        CMD_Read10_();

    }
    else
    {
        AccessFlash = 1;

        if ((LogicOrPhy & 0x01) == 0)
        {
            //for logical write
            CMD_Write10_();
        }
        else
        {
            sectors_trans_once = 1;

            //DIR & BREC type: 0:MBREC; 1:BREC
            if ((CBW_data_buffer._CBWCB[1] & 0x0f) != 0)
            {
                tmp = BREC_Write_AndCheck();
            }
            else
            {
                tmp = MBREC_Write_AndCheck();
            }

            CSWBuffer._bCSWStatus = tmp;

            ReportCSW(0);
        }
    }
}

/*
 *********************************************************************************************************
 *                                           Switch_Firmware_Command
 *
 * Description: ADFU protocol handle, switch pc.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void Switch_Firmware_Command(void)
{
    ReportCSW(0);
}

/*
 *********************************************************************************************************
 *                                           Detach_Device_Command
 *
 * Description: ADFU protocol handle, disconnect usb device.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void Detach_Device_Command(void)
{
    PullLineFlag = 1;

    ReportCSW(0);
}

/*
 *********************************************************************************************************
 *                                           Polling_Ready_Command
 *
 * Description: ADFU protocol handle, get status length.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void Polling_Ready_Command(void)
{
    //�ѱ�־�ϴ�
    CBW_data_buffer._dCBWDataTransferLength = 2;
    Send_Data_To_PC((uint8 *) &StatusLength, CBW_data_buffer._dCBWDataTransferLength);

    ReportCSW(0);
}

/*
 *********************************************************************************************************
 *                                           Get_Status_Command
 *
 * Description: ADFU protocol handle, get status.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void Get_Status_Command(void)
{
    //��״̬�ϴ�,Ҫ��������ݳ���
    CBW_data_buffer._dCBWDataTransferLength = *((uint16 *) &CBW_data_buffer._CBWCB[7]);
    EndianReverse((uint8 *) (&CBW_data_buffer._dCBWDataTransferLength), 4);

    Send_Data_To_PC((uint8 *) &CallingTaskResult, CBW_data_buffer._dCBWDataTransferLength);
}

/*
 *********************************************************************************************************
 *                                           Calling_Entry_Command
 *
 * Description: ADFU protocol handle, call the address.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void Calling_Entry_Command(void)
{
    uint8 tmp;

    //CallingTask��ʼ��ַ
    tmp = *((uint8 *) &CBW_data_buffer._CBWCB[2]);

    if (tmp < 0x90)
    {
        CallingTaskFlag = 1;

        EndianReverse((uint8 *) (&CBW_data_buffer._CBWCB[1]), 2);
        CallingTaskAdd = (uint32) (*((uint16 *) &CBW_data_buffer._CBWCB[1])) + 0xff0000; //תΪ�����ַ
    }
    else if (tmp == 0x90) //9000h
    {
        TransferFlag = 0x21; //��ʾDownLoad
    }
    else if (tmp == 0x91) //9100h
    {
        //prints("callentry exit-");
        detach_cmd_rcv = 1; //����������յ�detach����
        TransferFlag = 0x41; //��ʾSucc
    }
    else if (tmp == 0x92) //9200h
    {
        TransferFlag = 0x31; //��ʾUpLoad
    }
    else if (tmp == 0x93) //9300h
    {
        FWOpStatus = 2;
        TransferFlag = 0x41; //��ʾSucc
    }
    else if (tmp == 0x95) //9500h
    {
        TransferFlag = 0x11; //��ʾCommu
    }
    else //9600h
    {
        TransferFlag = 0x51; //��ʾReady
    }

    ReportCSW(0);
}

/*
 *********************************************************************************************************
 *                                   Write_Internal_RAM
 *
 * Description: Recieve data from USB host, and write it to ram.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void Write_Internal_RAM(void)
{
    uint16 sendLength;
    uint16 count;
    DMA_DT_Trnsfr_t dma_transfer;

    EPB_OutRdy_FIFOFullNAK();

    //Ҫ��������ݳ���
    EndianReverse((uint8 *) (&CBW_data_buffer._CBWCB[7]), 2);
    sendLength = *((uint16 *) &CBW_data_buffer._CBWCB[7]);

    while (sendLength != 0)
    {
        count = (sendLength >= max_pkt_size) ? max_pkt_size : sendLength;

        dma_transfer.source_addr = URAMSTARTADDR;
        dma_transfer.dst_addr = DATABUFFER;
        dma_transfer.length = count * 0x200;
        dma_transfer.pagenum = USBFIFO_TO_DRAM;
        DMA_Data_Transfer(&dma_transfer);
        Wait_PrevDMA_TransEnd();

        sendLength = sendLength - count;
    }
}

/*
 *********************************************************************************************************
 *                                   Read_Internal_RAM
 *
 * Description: Upload data to USB host.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : �����ж����ݸ������ܳ���0xC0.
 *********************************************************************************************************
 */
static void Read_Internal_RAM(void)
{
    uint8 i;
    ADFU_SysInfo_t *adfuSysInfo;

    EPA_In_Rdy();

    EndianReverse((uint8 *) (&CBW_data_buffer._CBWCB[2]), 2);
    EndianReverse((uint8 *) (&CBW_data_buffer._CBWCB[7]), 2);

    adfuSysInfo = (ADFU_SysInfo_t *) URAMYADDR;
    //Ҫ��������ݳ���ֻ�ܴ�Ram������512�ֽڸ�������,����Ϊα����,Ҫ�����������ʼ��ַֻ����data�ռ��0x800��λ��
    if ((*((uint16 *) &CBW_data_buffer._CBWCB[7]) <= 0x200) && (*((uint16 *) &CBW_data_buffer._CBWCB[2]) == 0x800))
    {
        //don't use URAMXADDR for buffer,because when calling SD_READ,need use it
        GetSysInfo((ADFU_SysInfo_t *) URAMYADDR);

        adfu_flag = 1;
        phy_block_addr = BREC_START_BLOCK; //BREC��һ����Flash ����block 4
        sectors_trans_once = 1;

        //�ܹ�4��BREC,�ҵ�У�����ȷ����һ��,������ID���Downgrade��Ϣ
        for (i = 1; i <= 1; i++)
        {
            //Return: BRECУ���,���õ�DATABUFFER������buffer������У���
            if (Calculate_BREC_Checksum() == 0)
            {
                break;
            }
            phy_block_addr++;
        }

        //�ж��Ƿ���SPINor,����Nor Base����ÿ�ξ�����MBREC,BREC
        if (flash_type == 0x02)
        {
            adfuSysInfo->adfu_fwscaninfo.BrecCheckSum[1] = 0;
        }
        else
        {
            //DATABUFFER + 0x1FE:brec checksum
            adfuSysInfo->adfu_fwscaninfo.BrecCheckSum[1] = *((uint16 *) ((uint8 *) DATABUFFER + 0x1FE));
        }

        adfuSysInfo->adfu_fwscaninfo.Logical_cap = flash_capacity;

        if (flash_type == 0x04)
        {
            adfuSysInfo->adfu_hwscaninfo.BootDiskType[0] = 0xf6;
            adfuSysInfo->adfu_hwscaninfo.BootDiskType[1] = 0x48;
            adfuSysInfo->adfu_hwscaninfo.BootDiskType[2] = 0xf6;
            adfuSysInfo->adfu_hwscaninfo.BootDiskType[3] = 0x48;
            adfuSysInfo->adfu_hwscaninfo.BootDiskType[4] = 0xf6;
            adfuSysInfo->adfu_hwscaninfo.BootDiskType[5] = 0x48;
            adfuSysInfo->adfu_hwscaninfo.BootDiskType[6] = 0xf6;
            adfuSysInfo->adfu_hwscaninfo.BootDiskType[7] = 0x48;
        }
        //BREC���һ��������ͷ64byte��Flash ID����Ϣ,֮��32byte��Downgrade��Ϣ
        //memcpy(URAMYADDR+160, DATABUFFER, 64);
        //memcpy(URAMYADDR+160+64, DATABUFFER+64, 32);

        //u����С�ɻ�����ʱ,���߷����̼���Ϣ����Ʒ���豸��Ϣʱ��z80ƽ̨�����ݽṹ��ȡ
        //��С�����жϵ��ǹ����ڶ�ȡ��Ϣʱ,��д��Ӧλ���͸�����.
        if (CBW_data_buffer._dCBWDataTransferLength == 0xc0)
        {
            memcpy(&adfuSysInfo->SysinfoFlag[0] + 8 + 64 + 6, &adfuSysInfo->adfu_fwscaninfo.FirmwareVersion,
                   sizeof(adfuSysInfo->adfu_fwscaninfo.FirmwareVersion));
            memcpy(&adfuSysInfo->SysinfoFlag[0] + 8 + 64 + 12, &adfuSysInfo->adfu_fwscaninfo.Productor[0], 32);
            memcpy(&adfuSysInfo->SysinfoFlag[0] + 8 + 64 + 44, &adfuSysInfo->adfu_fwscaninfo.DeviceName[0], 32);
        }
    }

    Send_Data_To_PC(URAMYADDR, CBW_data_buffer._dCBWDataTransferLength);
}

/*
 *********************************************************************************************************
 *                                   MBREC_Write_AndCheck
 *
 * Description: Write MBREC to flash physical area, then check if it success.
 *
 * Arguments  : None.
 *
 * Returns    : a: if write and verify success: 0: yes; 1:no.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
static uint8 MBREC_Write_AndCheck(void)
{
    uint8 i;
    uint8 count;
    uint8 ret;
    DMA_DT_Trnsfr_t dma_transfer;

    ret = 0;

    //prints("mrec enter-");
    //ak211x big endian,need convert
    EndianReverse((uint8 *) (&CBW_data_buffer._CBWCB[7]), 2);
    udisk_rw_sctr_num = *((uint16 *) &CBW_data_buffer._CBWCB[7]);

    EPB_OutRdy_FIFOFullNAK();

    //prints("mrec received 0-");
    dma_transfer.source_addr = URAMSTARTADDR;
    dma_transfer.dst_addr = DATABUFFER;
    dma_transfer.length = udisk_rw_sctr_num * 512;
    dma_transfer.pagenum = USBFIFO_TO_DRAM;
    DMA_Data_Transfer(&dma_transfer);
    Wait_PrevDMA_TransEnd();

    memcpy(URAMYADDR, DATABUFFER, udisk_rw_sctr_num * 512);

    phy_block_addr = 0;
    LBA = 0;

    if ((flash_type == 3) || (flash_type == 4))//card base or lba nand
    {
        sFlash_Phy_Read(DATABUFFER);
        if (memcmp(DATABUFFER, URAMYADDR, udisk_rw_sctr_num * 512) != 0)
        {
            //prints("mrec err-");
            //дMBREC����
            count = 4;
        }
        else
        {
            //prints("mrec OK-");
            count = 0;
            ret = 0;
        }
    }
    else if (flash_type == 2)//spi nor base
    {
        //�Ƿ�Snor,Nor����ÿ�ξ�����MBREC,���ڲ�����ƬNor,SPI NorFlashֻдһ��MBREC��BREC
        //����SPI Nor��Ƭ����,��������дMBREC��BREC
        count = 1;
    }
    else
    {
        //prints("mrec received 1-");
        //nand flash mrec is 1K,saved and write together;
        dma_transfer.source_addr = URAMSTARTADDR;
        dma_transfer.dst_addr = DATABUFFER;
        dma_transfer.length = udisk_rw_sctr_num * 512;
        dma_transfer.pagenum = USBFIFO_TO_DRAM;
        DMA_Data_Transfer(&dma_transfer);
        Wait_PrevDMA_TransEnd();
        memcpy(URAMYADDR + 0x200, DATABUFFER, 0x200);

        sFlash_Phy_Read(DATABUFFER);
        if ((memcmp(DATABUFFER, URAMYADDR, 0x1e0) != 0) || (memcmp(DATABUFFER + 0x200, URAMYADDR + 0x200, 0x200) != 0))
        {
            //prints("flash mrec err-");
            //дMBREC����
            count = 4;
        }
        else
        {
            //prints("flash mrec OK-");
            count = 0;
            ret = 0;
        }

    }

    //write MBREC
    if (count != 0)
    {
        //prints("mrec write-");
        for (i = 0; i < count; i++)
        {
            sFlash_Phy_Write(URAMYADDR);
            phy_block_addr++;
        }

        //д�������У��block0�� MBRECд���Ƿ���ȷ
        phy_block_addr = 0;
        sFlash_Phy_Read(DATABUFFER);

        ClearWatchdog();

        if ((flash_type == 0x02) || (flash_type == 0x03) || (flash_type == 0x04))
        {
            ret = (uint8) memcmp(DATABUFFER, URAMYADDR, udisk_rw_sctr_num * 512);
        }
        else
        {
            ret = (memcmp(DATABUFFER, URAMYADDR, 0x1e0)) || (memcmp(DATABUFFER + 0x200, URAMYADDR + 0x200, 0x200));
        }
    }

    return ret;
}

/*
 *********************************************************************************************************
 *                                   BREC_Write_AndCheck
 *
 * Description: Write BREC to flash physical area, then check if it success.
 *
 * Arguments  : None.
 *
 * Returns    : a: if write and verify success: 0: yes; 1:no.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
static uint8 BREC_Write_AndCheck(void)
{
    uint8 ret;
    ADFU_BrecHead_t *brecHead_p;
    DMA_DT_Trnsfr_t dma_transfer;

    //prints("brec enter-");

    ret = 0;

    EPB_OutRdy_FIFOFullNAK();

    //ak211x big endian,need convert
    EndianReverse((uint8 *) (&CBW_data_buffer._CBWCB[2]), 2);
    LBA = *((uint16 *) &CBW_data_buffer._CBWCB[2]);
    EndianReverse((uint8 *) (&CBW_data_buffer._CBWCB[7]), 2);
    udisk_rw_sctr_num = *((uint16 *) &CBW_data_buffer._CBWCB[7]);

    brecHead_p = DATABUFFER;
    phy_block_addr = BREC_START_BLOCK;
    //write brec
    while (udisk_rw_sctr_num != 0)
    {
        dma_transfer.source_addr = URAMSTARTADDR;
        dma_transfer.dst_addr = DATABUFFER;
        dma_transfer.length = 0x200;
        dma_transfer.pagenum = USBFIFO_TO_DRAM;
        DMA_Data_Transfer(&dma_transfer);
        Wait_PrevDMA_TransEnd();

        sFlash_Phy_Write((uint8 *) DATABUFFER);
        LBA = LBA + (uint32) sectors_trans_once;

        udisk_rw_sctr_num--;
    }

    //Return: BRECУ���,��������У��: 0:һ��; 1:��һ��.
    ret = Calculate_BREC_Checksum();

    return ret;
}

/*
 *********************************************************************************************************
 *                                   Calculate_BREC_Checksum
 *
 * Description: Calculate the checksum of BREC.
 *
 * Arguments  : None.
 *
 * Returns    : a: if BREC verify success: 0: yes; 1:no; hl: Brec checksum;
 *
 * Note(s)    : ע��phy_block_addr����ֵ,��ʾFlash����block��ַ.
 *********************************************************************************************************
 */
static uint8 Calculate_BREC_Checksum(void)
{
    uint8 ret = 1;
    uint8 brecCount;

    LBA = 0;
    brec_check_sum = 0;

    brecCount = 32; // BREC is  16kb

    //read brec,and add brec checksum
    while (brecCount != 0)
    {
        sFlash_Phy_Read(DATABUFFER);
        brec_check_sum += Caculate_CheckSum(DATABUFFER, 0x200);
        LBA = LBA + (uint32) sectors_trans_once;
        brecCount--;
    }

    //У��Ͷ�����������byte(�������byte��ԭ�е�У���)
    if (*((uint16 *) ((uint8 *) DATABUFFER + 0x1fc)) == 0xaa55)
    {
        if (brec_check_sum == (*((uint16 *) ((uint8 *) DATABUFFER + 0x1fe)) * 2))
        {
            //ret 0,brec check success
            ret = 0;

            //prints("brec check OK-");
        }
    }

    return ret;
}

/*
 *********************************************************************************************************
 *                                           sFlash_Phy_Write
 *
 * Description: ��Buffer��һ����������д��Flash����ռ�.
 *
 * Arguments  : ix: Uram Address.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
static void sFlash_Phy_Write(uint8 *dst_buffer)
{
    SD_UDRWStruct.lba = LBA + (uint32) phy_block_addr;
    SD_UDRWStruct.sectornum = 1;
    SD_UDRWStruct.dstaddr = (uint16) dst_buffer;

    UD_Update();
    SD_Bre_SectorWrite((SD_RW *) &SD_UDRWStruct); // need fix ???
}

/*
 *********************************************************************************************************
 *                                           sFlash_Phy_Read
 *
 * Description: ��һ���������ݴ�Flash����������Bufferȥ.
 *
 * Arguments  : ix: Uram Address.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void sFlash_Phy_Read(uint8 *dst_buffer)
{
    SD_UDRWStruct.lba = LBA + (uint32) phy_block_addr;
    SD_UDRWStruct.sectornum = 1;
    SD_UDRWStruct.dstaddr = (uint16) dst_buffer;

    UD_Update();
    SD_Bre_SectorRead((SD_RW *) &SD_UDRWStruct); // need fix ???
}

/*
 *********************************************************************************************************
 *             Caculate_CheckSum
 *
 * Description : ����zram��ָ����ַ��ʼ��ָ���������ݵ�У���(2byte add)
 *
 * Arguments   : de: data start address; bc: data length
 *
 * Returns     : hl: the checksum
 *
 * Notes       : None.
 *
 *********************************************************************************************************
 */
static uint16 Caculate_CheckSum(uint16 *addr, uint16 length)//length�����ֽ�Ϊһ��λ
{
    uint16 tmp, checksum = 0;

    while (length != 0)
    {
        tmp = *addr;
        addr++;
        length -= 2;
        checksum += tmp;
    }

    ClearWatchdog();

    return checksum;
}
#pragma ROM(HUGE)
