#ifndef __SD_PRIVATE_H__
#define __SD_PRIVATE_H__

#define CMD_FLAG        0x40
#define CMD_NUM(x)      (x)

#define CMD_GO_IDLE_STATE           CMD_FLAG | CMD_NUM(0)
#define CMD_SEND_OP_COND            CMD_FLAG | CMD_NUM(1)
#define CMD_SWITCH_FUNC             CMD_FLAG | CMD_NUM(6)
#define CMD_SEND_IF_COND            CMD_FLAG | CMD_NUM(8)
#define CMD_SEND_CSD                CMD_FLAG | CMD_NUM(9)
#define CMD_SEND_CID                CMD_FLAG | CMD_NUM(10)
#define CMD_STOP_TRANSMISSION       CMD_FLAG | CMD_NUM(12)
#define CMD_SEND_STATUS             CMD_FLAG | CMD_NUM(13)
#define CMD_SET_BLOCKLEN            CMD_FLAG | CMD_NUM(16)
#define CMD_READ_SINGLE_BLOCK       CMD_FLAG | CMD_NUM(17)
#define CMD_READ_MULTIPLE_BLOCK     CMD_FLAG | CMD_NUM(18)
#define CMD_WRITE_BLOCK             CMD_FLAG | CMD_NUM(24)
#define CMD_WRITE_MULTIPLE_BLOCK    CMD_FLAG | CMD_NUM(25)
#define CMD_PROGRAM_CSD             CMD_FLAG | CMD_NUM(27)
#define CMD_SET_WRITE_PROT          CMD_FLAG | CMD_NUM(28)
#define CMD_CLR_WRITE_PROT          CMD_FLAG | CMD_NUM(29)
#define CMD_SEND_WRITE_PROT         CMD_FLAG | CMD_NUM(30)
#define CMD_ERASE_WR_BLK_START_ADDR CMD_FLAG | CMD_NUM(32)
#define CMD_ERASE_WR_BLK_END_ADDR   CMD_FLAG | CMD_NUM(33)
#define CMD_ERASE                   CMD_FLAG | CMD_NUM(38)
#define CMD_LOCK_UNLOCK             CMD_FLAG | CMD_NUM(42)
#define CMD_APP_CMD                 CMD_FLAG | CMD_NUM(55)
#define CMD_GEN_CMD                 CMD_FLAG | CMD_NUM(56)
#define CMD_READ_OCR                CMD_FLAG | CMD_NUM(58)
#define CMD_CRC_ON_OFF              CMD_FLAG | CMD_NUM(59)

#define ACMD_SD_STATUS              CMD_FLAG | CMD_NUM(13)
#define ACMD_SEND_NUM_WR_BLOCKS     CMD_FLAG | CMD_NUM(22)
#define ACMD_SET_WR_BLK_ERASE_COUNT CMD_FLAG | CMD_NUM(23)
#define ACMD_SD_SEND_OP_COND        CMD_FLAG | CMD_NUM(41)
#define ACMD_SET_CLR_CARD_DETECT    CMD_FLAG | CMD_NUM(42)
#define ACMD_SEND_SCR               CMD_FLAG | CMD_NUM(51)

#define SD_READY                0
#define IN_IDLE_STATE           (1 << 0)
#define ERASE_RESET             (1 << 1)
#define ILLEGAL_COMMAND         (1 << 2)
#define COM_CRC_ERROR           (1 << 3)
#define ERASE_SEQUENCE_ERROR    (1 << 4)
#define ADDRESS_ERROR           (1 << 5)
#define PARAMETER_ERROR         (1 << 6)
#define ERROR_MASK              0x7E

typedef struct __attribute__((packed)) _csd_v2_t 
{
    unsigned always1            :   1;
    unsigned crc                :   7;
    unsigned reserved5          :   2;
    unsigned file_format        :   2;
    unsigned tmp_write_protect  :   1;
    unsigned perm_write_protect :   1;
    unsigned copy               :   1;
    unsigned file_format_grp    :   1;
    unsigned reserved4          :   5;
    unsigned write_bl_partial   :   1;
    unsigned write_bl_len       :   4;
    unsigned r2w_factor         :   3;
    unsigned reserved3          :   2;
    unsigned wp_grp_enable      :   1;
    unsigned wp_grp_size        :   7;
    unsigned sector_size        :   7;
    unsigned erase_blk_len      :   1;
    unsigned reserved2          :   1;
    unsigned c_size             :   22;
    unsigned reserved1          :   6;
    unsigned dsr_imp            :   1;
    unsigned read_blk_misalign  :   1;
    unsigned write_blk_misalign :   1;
    unsigned read_bl_partial    :   1;
    unsigned read_bl_len        :   4;
    unsigned ccc                :   12;
    unsigned tran_speed         :   8;
    unsigned nsac               :   8;
    unsigned taac               :   8;
    unsigned reserved0          :   6;
	unsigned csd_structure      :   2;
} csd_v2_t;


#endif
