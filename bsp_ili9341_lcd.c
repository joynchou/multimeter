/**
  ******************************************************************************
  * @file    bsp_ili9341_lcd.c
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ili9341Òº¾§ÆÁÇý¶¯
  ******************************************************************************
  * @attention
  *
  * ÊµÑéÆ½Ì¨:±ü»ð F103-Ö¸ÄÏÕß STM32 ¿ª·¢°å 
  * ÂÛÌ³    :http://www.firebbs.cn
  * ÌÔ±¦    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */

#include "bsp_ili9341_lcd.h"
#include "fonts.h"

//¸ù¾ÝÒº¾§É¨Ãè·½Ïò¶ø±ä»¯µÄXYÏñËØ¿í¶È
//µ÷ÓÃILI9341_GramScanº¯ÊýÉèÖÃ·½ÏòÊ±»á×Ô¶¯¸ü¸Ä
uint16_t LCD_X_LENGTH = ILI9341_LESS_PIXEL;
uint16_t LCD_Y_LENGTH = ILI9341_MORE_PIXEL;

//Òº¾§ÆÁÉ¨ÃèÄ£Ê½£¬±¾±äÁ¿Ö÷ÒªÓÃÓÚ·½±ãÑ¡Ôñ´¥ÃþÆÁµÄ¼ÆËã²ÎÊý
//²ÎÊý¿ÉÑ¡ÖµÎª0-7
//µ÷ÓÃILI9341_GramScanº¯ÊýÉèÖÃ·½ÏòÊ±»á×Ô¶¯¸ü¸Ä
//LCD¸Õ³õÊ¼»¯Íê³ÉÊ±»áÊ¹ÓÃ±¾Ä¬ÈÏÖµ
uint8_t LCD_SCAN_MODE = 6;

static sFONT *LCD_Currentfonts = &Font8x16; //Ó¢ÎÄ×ÖÌå
static uint16_t CurrentTextColor = BLACK;	//Ç°¾°É«
static uint16_t CurrentBackColor = WHITE;	//±³¾°É«

__inline void ILI9341_Write_Cmd(uint16_t usCmd);
__inline void ILI9341_Write_Data(uint16_t usData);
__inline uint16_t ILI9341_Read_Data(void);
static void ILI9341_Delay(__IO uint32_t nCount);
static void ILI9341_GPIO_Config(void);
static void ILI9341_FSMC_Config(void);
static void ILI9341_REG_Config(void);
static void ILI9341_SetCursor(uint16_t usX, uint16_t usY);
static __inline void ILI9341_FillColor(uint32_t ulAmout_Point, uint16_t usColor);
static uint16_t ILI9341_Read_PixelData(void);

/**
  * @brief  ÏòILI9341Ð´ÈëÃüÁî
  * @param  usCmd :ÒªÐ´ÈëµÄÃüÁî£¨±í¼Ä´æÆ÷µØÖ·£©
  * @retval ÎÞ
  */
__inline void ILI9341_Write_Cmd(uint16_t usCmd)
{
	*(__IO uint16_t *)(FSMC_Addr_ILI9341_CMD) = usCmd;
}

/**
  * @brief  ÏòILI9341Ð´ÈëÊý¾Ý
  * @param  usData :ÒªÐ´ÈëµÄÊý¾Ý
  * @retval ÎÞ
  */
__inline void ILI9341_Write_Data(uint16_t usData)
{
	*(__IO uint16_t *)(FSMC_Addr_ILI9341_DATA) = usData;
}

/**
  * @brief  ´ÓILI9341¶ÁÈ¡Êý¾Ý
  * @param  ÎÞ
  * @retval ¶ÁÈ¡µ½µÄÊý¾Ý
  */
__inline uint16_t ILI9341_Read_Data(void)
{
	return (*(__IO uint16_t *)(FSMC_Addr_ILI9341_DATA));
}

/**
  * @brief  ÓÃÓÚ ILI9341 ¼òµ¥ÑÓÊ±º¯Êý
  * @param  nCount £ºÑÓÊ±¼ÆÊýÖµ
  * @retval ÎÞ
  */
static void ILI9341_Delay(__IO uint32_t nCount)
{
	for (; nCount != 0; nCount--)
		;
}

/**
  * @brief  ³õÊ¼»¯ILI9341µÄIOÒý½Å
  * @param  ÎÞ
  * @retval ÎÞ
  */
static void ILI9341_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Ê¹ÄÜFSMC¶ÔÓ¦ÏàÓ¦¹Ü½ÅÊ±ÖÓ*/
	RCC_APB2PeriphClockCmd(
		/*¿ØÖÆÐÅºÅ*/
		ILI9341_CS_CLK | ILI9341_DC_CLK | ILI9341_WR_CLK |
			ILI9341_RD_CLK | ILI9341_BK_CLK | ILI9341_RST_CLK |
			/*Êý¾ÝÐÅºÅ*/
			ILI9341_D0_CLK | ILI9341_D1_CLK | ILI9341_D2_CLK |
			ILI9341_D3_CLK | ILI9341_D4_CLK | ILI9341_D5_CLK |
			ILI9341_D6_CLK | ILI9341_D7_CLK | ILI9341_D8_CLK |
			ILI9341_D9_CLK | ILI9341_D10_CLK | ILI9341_D11_CLK |
			ILI9341_D12_CLK | ILI9341_D13_CLK | ILI9341_D14_CLK |
			ILI9341_D15_CLK,
		ENABLE);

	/* ÅäÖÃFSMCÏà¶ÔÓ¦µÄÊý¾ÝÏß,FSMC-D0~D15 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;

	GPIO_InitStructure.GPIO_Pin = ILI9341_D0_PIN;
	GPIO_Init(ILI9341_D0_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = ILI9341_D1_PIN;
	GPIO_Init(ILI9341_D1_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = ILI9341_D2_PIN;
	GPIO_Init(ILI9341_D2_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = ILI9341_D3_PIN;
	GPIO_Init(ILI9341_D3_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = ILI9341_D4_PIN;
	GPIO_Init(ILI9341_D4_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = ILI9341_D5_PIN;
	GPIO_Init(ILI9341_D5_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = ILI9341_D6_PIN;
	GPIO_Init(ILI9341_D6_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = ILI9341_D7_PIN;
	GPIO_Init(ILI9341_D7_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = ILI9341_D8_PIN;
	GPIO_Init(ILI9341_D8_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = ILI9341_D9_PIN;
	GPIO_Init(ILI9341_D9_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = ILI9341_D10_PIN;
	GPIO_Init(ILI9341_D10_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = ILI9341_D11_PIN;
	GPIO_Init(ILI9341_D11_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = ILI9341_D12_PIN;
	GPIO_Init(ILI9341_D12_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = ILI9341_D13_PIN;
	GPIO_Init(ILI9341_D13_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = ILI9341_D14_PIN;
	GPIO_Init(ILI9341_D14_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = ILI9341_D15_PIN;
	GPIO_Init(ILI9341_D15_PORT, &GPIO_InitStructure);

	/* ÅäÖÃFSMCÏà¶ÔÓ¦µÄ¿ØÖÆÏß
	 * FSMC_NOE   :LCD-RD
	 * FSMC_NWE   :LCD-WR
	 * FSMC_NE1   :LCD-CS
	 * FSMC_A16  	:LCD-DC
	 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;

	GPIO_InitStructure.GPIO_Pin = ILI9341_RD_PIN;
	GPIO_Init(ILI9341_RD_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = ILI9341_WR_PIN;
	GPIO_Init(ILI9341_WR_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = ILI9341_CS_PIN;
	GPIO_Init(ILI9341_CS_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = ILI9341_DC_PIN;
	GPIO_Init(ILI9341_DC_PORT, &GPIO_InitStructure);

	/* ÅäÖÃLCD¸´Î»RST¿ØÖÆ¹Ü½Å*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitStructure.GPIO_Pin = ILI9341_RST_PIN;
	GPIO_Init(ILI9341_RST_PORT, &GPIO_InitStructure);

	/* ÅäÖÃLCD±³¹â¿ØÖÆ¹Ü½ÅBK*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitStructure.GPIO_Pin = ILI9341_BK_PIN;
	GPIO_Init(ILI9341_BK_PORT, &GPIO_InitStructure);
}

/**
  * @brief  LCD  FSMC Ä£Ê½ÅäÖÃ
  * @param  ÎÞ
  * @retval ÎÞ
  */
static void ILI9341_FSMC_Config(void)
{
	FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef readWriteTiming;

	/* Ê¹ÄÜFSMCÊ±ÖÓ*/
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

	//µØÖ·½¨Á¢Ê±¼ä£¨ADDSET£©Îª1¸öHCLK 2/72M=28ns
	readWriteTiming.FSMC_AddressSetupTime = 0x01; //µØÖ·½¨Á¢Ê±¼ä
	//Êý¾Ý±£³ÖÊ±¼ä£¨DATAST£©+ 1¸öHCLK = 5/72M=70ns
	readWriteTiming.FSMC_DataSetupTime = 0x04; //Êý¾Ý½¨Á¢Ê±¼ä
	//Ñ¡Ôñ¿ØÖÆµÄÄ£Ê½
	//Ä£Ê½B,Òì²½NOR FLASHÄ£Ê½£¬ÓëILI9341µÄ8080Ê±ÐòÆ¥Åä
	readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_B;

	/*ÒÔÏÂÅäÖÃÓëÄ£Ê½BÎÞ¹Ø*/
	//µØÖ·±£³ÖÊ±¼ä£¨ADDHLD£©Ä£Ê½AÎ´ÓÃµ½
	readWriteTiming.FSMC_AddressHoldTime = 0x00; //µØÖ·±£³ÖÊ±¼ä
	//ÉèÖÃ×ÜÏß×ª»»ÖÜÆÚ£¬½öÓÃÓÚ¸´ÓÃÄ£Ê½µÄNOR²Ù×÷
	readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
	//ÉèÖÃÊ±ÖÓ·ÖÆµ£¬½öÓÃÓÚÍ¬²½ÀàÐÍµÄ´æ´¢Æ÷
	readWriteTiming.FSMC_CLKDivision = 0x00;
	//Êý¾Ý±£³ÖÊ±¼ä£¬½öÓÃÓÚÍ¬²½ÐÍµÄNOR
	readWriteTiming.FSMC_DataLatency = 0x00;

	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAMx;
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable;
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &readWriteTiming;

	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

	/* Ê¹ÄÜ FSMC_Bank1_NORSRAM4 */
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAMx, ENABLE);
}

/**
 * @brief  ³õÊ¼»¯ILI9341¼Ä´æÆ÷
 * @param  ÎÞ
 * @retval ÎÞ
 */
static void ILI9341_REG_Config(void)
{
	/*  Power control B (CFh)  */
	DEBUG_DELAY();
	ILI9341_Write_Cmd(0xCF);
	ILI9341_Write_Data(0x00);
	ILI9341_Write_Data(0x81);
	ILI9341_Write_Data(0x30);

	/*  Power on sequence control (EDh) */
	DEBUG_DELAY();
	ILI9341_Write_Cmd(0xED);
	ILI9341_Write_Data(0x64);
	ILI9341_Write_Data(0x03);
	ILI9341_Write_Data(0x12);
	ILI9341_Write_Data(0x81);

	/*  Driver timing control A (E8h) */
	DEBUG_DELAY();
	ILI9341_Write_Cmd(0xE8);
	ILI9341_Write_Data(0x85);
	ILI9341_Write_Data(0x10);
	ILI9341_Write_Data(0x78);

	/*  Power control A (CBh) */
	DEBUG_DELAY();
	ILI9341_Write_Cmd(0xCB);
	ILI9341_Write_Data(0x39);
	ILI9341_Write_Data(0x2C);
	ILI9341_Write_Data(0x00);
	ILI9341_Write_Data(0x34);
	ILI9341_Write_Data(0x02);

	/* Pump ratio control (F7h) */
	DEBUG_DELAY();
	ILI9341_Write_Cmd(0xF7);
	ILI9341_Write_Data(0x20);

	/* Driver timing control B */
	DEBUG_DELAY();
	ILI9341_Write_Cmd(0xEA);
	ILI9341_Write_Data(0x00);
	ILI9341_Write_Data(0x00);

	/* Frame Rate Control (In Normal Mode/Full Colors) (B1h) */
	DEBUG_DELAY();
	ILI9341_Write_Cmd(0xB1);
	ILI9341_Write_Data(0x00);
	ILI9341_Write_Data(0x1B);

	/*  Display Function Control (B6h) */
	DEBUG_DELAY();
	ILI9341_Write_Cmd(0xB6);
	ILI9341_Write_Data(0x0A);
	ILI9341_Write_Data(0xA2);

	/* Power Control 1 (C0h) */
	DEBUG_DELAY();
	ILI9341_Write_Cmd(0xC0);
	ILI9341_Write_Data(0x35);

	/* Power Control 2 (C1h) */
	DEBUG_DELAY();
	ILI9341_Write_Cmd(0xC1);
	ILI9341_Write_Data(0x11);

	/* VCOM Control 1 (C5h) */
	ILI9341_Write_Cmd(0xC5);
	ILI9341_Write_Data(0x45);
	ILI9341_Write_Data(0x45);

	/*  VCOM Control 2 (C7h)  */
	ILI9341_Write_Cmd(0xC7);
	ILI9341_Write_Data(0xA2);

	/* Enable 3G (F2h) */
	ILI9341_Write_Cmd(0xF2);
	ILI9341_Write_Data(0x00);

	/* Gamma Set (26h) */
	ILI9341_Write_Cmd(0x26);
	ILI9341_Write_Data(0x01);
	DEBUG_DELAY();

	/* Positive Gamma Correction */
	ILI9341_Write_Cmd(0xE0); //Set Gamma
	ILI9341_Write_Data(0x0F);
	ILI9341_Write_Data(0x26);
	ILI9341_Write_Data(0x24);
	ILI9341_Write_Data(0x0B);
	ILI9341_Write_Data(0x0E);
	ILI9341_Write_Data(0x09);
	ILI9341_Write_Data(0x54);
	ILI9341_Write_Data(0xA8);
	ILI9341_Write_Data(0x46);
	ILI9341_Write_Data(0x0C);
	ILI9341_Write_Data(0x17);
	ILI9341_Write_Data(0x09);
	ILI9341_Write_Data(0x0F);
	ILI9341_Write_Data(0x07);
	ILI9341_Write_Data(0x00);

	/* Negative Gamma Correction (E1h) */
	ILI9341_Write_Cmd(0XE1); //Set Gamma
	ILI9341_Write_Data(0x00);
	ILI9341_Write_Data(0x19);
	ILI9341_Write_Data(0x1B);
	ILI9341_Write_Data(0x04);
	ILI9341_Write_Data(0x10);
	ILI9341_Write_Data(0x07);
	ILI9341_Write_Data(0x2A);
	ILI9341_Write_Data(0x47);
	ILI9341_Write_Data(0x39);
	ILI9341_Write_Data(0x03);
	ILI9341_Write_Data(0x06);
	ILI9341_Write_Data(0x06);
	ILI9341_Write_Data(0x30);
	ILI9341_Write_Data(0x38);
	ILI9341_Write_Data(0x0F);

	/* memory access control set */
	DEBUG_DELAY();
	ILI9341_Write_Cmd(0x36);
	ILI9341_Write_Data(0xC8); /*ÊúÆÁ  ×óÉÏ½Çµ½ (Æðµã)µ½ÓÒÏÂ½Ç (ÖÕµã)É¨Ãè·½Ê½*/
	DEBUG_DELAY();

	/* column address control set */
	ILI9341_Write_Cmd(CMD_SetCoordinateX);
	ILI9341_Write_Data(0x00);
	ILI9341_Write_Data(0x00);
	ILI9341_Write_Data(0x00);
	ILI9341_Write_Data(0xEF);

	/* page address control set */
	DEBUG_DELAY();
	ILI9341_Write_Cmd(CMD_SetCoordinateY);
	ILI9341_Write_Data(0x00);
	ILI9341_Write_Data(0x00);
	ILI9341_Write_Data(0x01);
	ILI9341_Write_Data(0x3F);

	/*  Pixel Format Set (3Ah)  */
	DEBUG_DELAY();
	ILI9341_Write_Cmd(0x3a);
	ILI9341_Write_Data(0x55);

	/* Sleep Out (11h)  */
	ILI9341_Write_Cmd(0x11);
	ILI9341_Delay(0xAFFf << 2);
	DEBUG_DELAY();

	/* Display ON (29h) */
	ILI9341_Write_Cmd(0x29);
}

/**
 * @brief  ILI9341³õÊ¼»¯º¯Êý£¬Èç¹ûÒªÓÃµ½lcd£¬Ò»¶¨Òªµ÷ÓÃÕâ¸öº¯Êý
 * @param  ÎÞ
 * @retval ÎÞ
 */
void ILI9341_Init(void)
{
	ILI9341_GPIO_Config();
	ILI9341_FSMC_Config();

	ILI9341_BackLed_Control(ENABLE); //µãÁÁLCD±³¹âµÆ
	ILI9341_Rst();
	ILI9341_REG_Config();

	//ÉèÖÃÄ¬ÈÏÉ¨Ãè·½Ïò£¬ÆäÖÐ 6 Ä£Ê½Îª´ó²¿·ÖÒº¾§Àý³ÌµÄÄ¬ÈÏÏÔÊ¾·½Ïò
	ILI9341_GramScan(LCD_SCAN_MODE);
}

/**
 * @brief  ILI9341±³¹âLED¿ØÖÆ
 * @param  enumState £º¾ö¶¨ÊÇ·ñÊ¹ÄÜ±³¹âLED
  *   ¸Ã²ÎÊýÎªÒÔÏÂÖµÖ®Ò»£º
  *     @arg ENABLE :Ê¹ÄÜ±³¹âLED
  *     @arg DISABLE :½ûÓÃ±³¹âLED
 * @retval ÎÞ
 */
void ILI9341_BackLed_Control(FunctionalState enumState)
{
	if (enumState)
		GPIO_ResetBits(ILI9341_BK_PORT, ILI9341_BK_PIN);
	else
		GPIO_SetBits(ILI9341_BK_PORT, ILI9341_BK_PIN);
}

/**
 * @brief  ILI9341 Èí¼þ¸´Î»
 * @param  ÎÞ
 * @retval ÎÞ
 */
void ILI9341_Rst(void)
{
	GPIO_ResetBits(ILI9341_RST_PORT, ILI9341_RST_PIN); //µÍµçÆ½¸´Î»

	ILI9341_Delay(0xAFF);

	GPIO_SetBits(ILI9341_RST_PORT, ILI9341_RST_PIN);

	ILI9341_Delay(0xAFF);
}

/**
 * @brief  ÉèÖÃILI9341µÄGRAMµÄÉ¨Ãè·½Ïò 
 * @param  ucOption £ºÑ¡ÔñGRAMµÄÉ¨Ãè·½Ïò 
 *     @arg 0-7 :²ÎÊý¿ÉÑ¡ÖµÎª0-7Õâ°Ë¸ö·½Ïò
 *
 *	£¡£¡£¡ÆäÖÐ0¡¢3¡¢5¡¢6 Ä£Ê½ÊÊºÏ´Ó×óÖÁÓÒÏÔÊ¾ÎÄ×Ö£¬
 *				²»ÍÆ¼öÊ¹ÓÃÆäËüÄ£Ê½ÏÔÊ¾ÎÄ×Ö	ÆäËüÄ£Ê½ÏÔÊ¾ÎÄ×Ö»áÓÐ¾µÏñÐ§¹û			
 *		
 *	ÆäÖÐ0¡¢2¡¢4¡¢6 Ä£Ê½µÄX·½ÏòÏñËØÎª240£¬Y·½ÏòÏñËØÎª320
 *	ÆäÖÐ1¡¢3¡¢5¡¢7 Ä£Ê½ÏÂX·½ÏòÏñËØÎª320£¬Y·½ÏòÏñËØÎª240
 *
 *	ÆäÖÐ 6 Ä£Ê½Îª´ó²¿·ÖÒº¾§Àý³ÌµÄÄ¬ÈÏÏÔÊ¾·½Ïò
 *	ÆäÖÐ 3 Ä£Ê½ÎªÉãÏñÍ·Àý³ÌÊ¹ÓÃµÄ·½Ïò
 *	ÆäÖÐ 0 Ä£Ê½ÎªBMPÍ¼Æ¬ÏÔÊ¾Àý³ÌÊ¹ÓÃµÄ·½Ïò
 *
 * @retval ÎÞ
 * @note  ×ø±êÍ¼Àý£ºA±íÊ¾ÏòÉÏ£¬V±íÊ¾ÏòÏÂ£¬<±íÊ¾Ïò×ó£¬>±íÊ¾ÏòÓÒ
					X±íÊ¾XÖá£¬Y±íÊ¾YÖá

------------------------------------------------------------
Ä£Ê½0£º				.		Ä£Ê½1£º		.	Ä£Ê½2£º			.	Ä£Ê½3£º					
					A		.					A		.		A					.		A									
					|		.					|		.		|					.		|							
					Y		.					X		.		Y					.		X					
					0		.					1		.		2					.		3					
	<--- X0 o		.	<----Y1	o		.		o 2X--->  .		o 3Y--->	
------------------------------------------------------------	
Ä£Ê½4£º				.	Ä£Ê½5£º			.	Ä£Ê½6£º			.	Ä£Ê½7£º					
	<--- X4 o		.	<--- Y5 o		.		o 6X--->  .		o 7Y--->	
					4		.					5		.		6					.		7	
					Y		.					X		.		Y					.		X						
					|		.					|		.		|					.		|							
					V		.					V		.		V					.		V		
---------------------------------------------------------				
											 LCDÆÁÊ¾Àý
								|-----------------|
								|			±ü»ðLogo		|
								|									|
								|									|
								|									|
								|									|
								|									|
								|									|
								|									|
								|									|
								|-----------------|
								ÆÁÄ»ÕýÃæ£¨¿í240£¬¸ß320£©

 *******************************************************/
void ILI9341_GramScan(uint8_t ucOption)
{
	//²ÎÊý¼ì²é£¬Ö»¿ÉÊäÈë0-7
	if (ucOption > 7)
		return;

	//¸ù¾ÝÄ£Ê½¸üÐÂLCD_SCAN_MODEµÄÖµ£¬Ö÷ÒªÓÃÓÚ´¥ÃþÆÁÑ¡Ôñ¼ÆËã²ÎÊý
	LCD_SCAN_MODE = ucOption;

	//¸ù¾ÝÄ£Ê½¸üÐÂXY·½ÏòµÄÏñËØ¿í¶È
	if (ucOption % 2 == 0)
	{
		//0 2 4 6Ä£Ê½ÏÂX·½ÏòÏñËØ¿í¶ÈÎª240£¬Y·½ÏòÎª320
		LCD_X_LENGTH = ILI9341_LESS_PIXEL;
		LCD_Y_LENGTH = ILI9341_MORE_PIXEL;
	}
	else
	{
		//1 3 5 7Ä£Ê½ÏÂX·½ÏòÏñËØ¿í¶ÈÎª320£¬Y·½ÏòÎª240
		LCD_X_LENGTH = ILI9341_MORE_PIXEL;
		LCD_Y_LENGTH = ILI9341_LESS_PIXEL;
	}

	//0x36ÃüÁî²ÎÊýµÄ¸ß3Î»¿ÉÓÃÓÚÉèÖÃGRAMÉ¨Ãè·½Ïò
	ILI9341_Write_Cmd(0x36);
	ILI9341_Write_Data(0x08 | (ucOption << 5)); //¸ù¾ÝucOptionµÄÖµÉèÖÃLCD²ÎÊý£¬¹²0-7ÖÖÄ£Ê½
	ILI9341_Write_Cmd(CMD_SetCoordinateX);
	ILI9341_Write_Data(0x00);							  /* x ÆðÊ¼×ø±ê¸ß8Î» */
	ILI9341_Write_Data(0x00);							  /* x ÆðÊ¼×ø±êµÍ8Î» */
	ILI9341_Write_Data(((LCD_X_LENGTH - 1) >> 8) & 0xFF); /* x ½áÊø×ø±ê¸ß8Î» */
	ILI9341_Write_Data((LCD_X_LENGTH - 1) & 0xFF);		  /* x ½áÊø×ø±êµÍ8Î» */

	ILI9341_Write_Cmd(CMD_SetCoordinateY);
	ILI9341_Write_Data(0x00);							  /* y ÆðÊ¼×ø±ê¸ß8Î» */
	ILI9341_Write_Data(0x00);							  /* y ÆðÊ¼×ø±êµÍ8Î» */
	ILI9341_Write_Data(((LCD_Y_LENGTH - 1) >> 8) & 0xFF); /* y ½áÊø×ø±ê¸ß8Î» */
	ILI9341_Write_Data((LCD_Y_LENGTH - 1) & 0xFF);		  /* y ½áÊø×ø±êµÍ8Î» */

	/* write gram start */
	ILI9341_Write_Cmd(CMD_SetPixel);
}

/**
 * @brief  ÔÚILI9341ÏÔÊ¾Æ÷ÉÏ¿ª±ÙÒ»¸ö´°¿Ú
 * @param  usX £ºÔÚÌØ¶¨É¨Ãè·½ÏòÏÂ´°¿ÚµÄÆðµãX×ø±ê
 * @param  usY £ºÔÚÌØ¶¨É¨Ãè·½ÏòÏÂ´°¿ÚµÄÆðµãY×ø±ê
 * @param  usWidth £º´°¿ÚµÄ¿í¶È
 * @param  usHeight £º´°¿ÚµÄ¸ß¶È
 * @retval ÎÞ
 */
void ILI9341_OpenWindow(uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight)
{
	ILI9341_Write_Cmd(CMD_SetCoordinateX); /* ÉèÖÃX×ø±ê */
	ILI9341_Write_Data(usX >> 8);		   /* ÏÈ¸ß8Î»£¬È»ºóµÍ8Î» */
	ILI9341_Write_Data(usX & 0xff);		   /* ÉèÖÃÆðÊ¼µãºÍ½áÊøµã*/
	ILI9341_Write_Data((usX + usWidth - 1) >> 8);
	ILI9341_Write_Data((usX + usWidth - 1) & 0xff);

	ILI9341_Write_Cmd(CMD_SetCoordinateY); /* ÉèÖÃY×ø±ê*/
	ILI9341_Write_Data(usY >> 8);
	ILI9341_Write_Data(usY & 0xff);
	ILI9341_Write_Data((usY + usHeight - 1) >> 8);
	ILI9341_Write_Data((usY + usHeight - 1) & 0xff);
}

/**
 * @brief  Éè¶¨ILI9341µÄ¹â±ê×ø±ê
 * @param  usX £ºÔÚÌØ¶¨É¨Ãè·½ÏòÏÂ¹â±êµÄX×ø±ê
 * @param  usY £ºÔÚÌØ¶¨É¨Ãè·½ÏòÏÂ¹â±êµÄY×ø±ê
 * @retval ÎÞ
 */
static void ILI9341_SetCursor(uint16_t usX, uint16_t usY)
{
	ILI9341_OpenWindow(usX, usY, 1, 1);
}

/**
 * @brief  ÔÚILI9341ÏÔÊ¾Æ÷ÉÏÒÔÄ³Ò»ÑÕÉ«Ìî³äÏñËØµã
 * @param  ulAmout_Point £ºÒªÌî³äÑÕÉ«µÄÏñËØµãµÄ×ÜÊýÄ¿
 * @param  usColor £ºÑÕÉ«
 * @retval ÎÞ
 */
static __inline void ILI9341_FillColor(uint32_t ulAmout_Point, uint16_t usColor)
{
	uint32_t i = 0;

	/* memory write */
	ILI9341_Write_Cmd(CMD_SetPixel);

	for (i = 0; i < ulAmout_Point; i++)
		ILI9341_Write_Data(usColor);
}

/**
 * @brief  ¶ÔILI9341ÏÔÊ¾Æ÷µÄÄ³Ò»´°¿ÚÒÔÄ³ÖÖÑÕÉ«½øÐÐÇåÆÁ
 * @param  usX £ºÔÚÌØ¶¨É¨Ãè·½ÏòÏÂ´°¿ÚµÄÆðµãX×ø±ê
 * @param  usY £ºÔÚÌØ¶¨É¨Ãè·½ÏòÏÂ´°¿ÚµÄÆðµãY×ø±ê
 * @param  usWidth £º´°¿ÚµÄ¿í¶È
 * @param  usHeight £º´°¿ÚµÄ¸ß¶È
 * @note ¿ÉÊ¹ÓÃLCD_SetBackColor¡¢LCD_SetTextColor¡¢LCD_SetColorsº¯ÊýÉèÖÃÑÕÉ«
 * @retval ÎÞ
 */
void ILI9341_Clear(uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight)
{
	ILI9341_OpenWindow(usX, usY, usWidth, usHeight);

	ILI9341_FillColor(usWidth * usHeight, CurrentBackColor);
}

/**
 * @brief  ¶ÔILI9341ÏÔÊ¾Æ÷µÄÄ³Ò»µãÒÔÄ³ÖÖÑÕÉ«½øÐÐÌî³ä
 * @param  usX £ºÔÚÌØ¶¨É¨Ãè·½ÏòÏÂ¸ÃµãµÄX×ø±ê
 * @param  usY £ºÔÚÌØ¶¨É¨Ãè·½ÏòÏÂ¸ÃµãµÄY×ø±ê
 * @note ¿ÉÊ¹ÓÃLCD_SetBackColor¡¢LCD_SetTextColor¡¢LCD_SetColorsº¯ÊýÉèÖÃÑÕÉ«
 * @retval ÎÞ
 */
void ILI9341_SetPointPixel(uint16_t usX, uint16_t usY)
{
	if ((usX < LCD_X_LENGTH) && (usY < LCD_Y_LENGTH))
	{
		ILI9341_SetCursor(usX, usY);

		ILI9341_FillColor(1, CurrentTextColor);
	}
}

/**
 * @brief  ¶ÁÈ¡ILI9341 GRAN µÄÒ»¸öÏñËØÊý¾Ý
 * @param  ÎÞ
 * @retval ÏñËØÊý¾Ý
 */
static uint16_t ILI9341_Read_PixelData(void)
{
	uint16_t usR = 0, usG = 0, usB = 0;

	ILI9341_Write_Cmd(0x2E); /* ¶ÁÊý¾Ý */

	usR = ILI9341_Read_Data(); /*FIRST READ OUT DUMMY DATA*/

	usR = ILI9341_Read_Data(); /*READ OUT RED DATA  */
	usB = ILI9341_Read_Data(); /*READ OUT BLUE DATA*/
	usG = ILI9341_Read_Data(); /*READ OUT GREEN DATA*/

	return (((usR >> 11) << 11) | ((usG >> 10) << 5) | (usB >> 11));
}

/**
 * @brief  »ñÈ¡ ILI9341 ÏÔÊ¾Æ÷ÉÏÄ³Ò»¸ö×ø±êµãµÄÏñËØÊý¾Ý
 * @param  usX £ºÔÚÌØ¶¨É¨Ãè·½ÏòÏÂ¸ÃµãµÄX×ø±ê
 * @param  usY £ºÔÚÌØ¶¨É¨Ãè·½ÏòÏÂ¸ÃµãµÄY×ø±ê
 * @retval ÏñËØÊý¾Ý
 */
uint16_t ILI9341_GetPointPixel(uint16_t usX, uint16_t usY)
{
	uint16_t usPixelData;

	ILI9341_SetCursor(usX, usY);

	usPixelData = ILI9341_Read_PixelData();

	return usPixelData;
}

/**
 * @brief  ÔÚ ILI9341 ÏÔÊ¾Æ÷ÉÏÊ¹ÓÃ Bresenham Ëã·¨»­Ïß¶Î 
 * @param  usX1 £ºÔÚÌØ¶¨É¨Ãè·½ÏòÏÂÏß¶ÎµÄÒ»¸ö¶ËµãX×ø±ê
 * @param  usY1 £ºÔÚÌØ¶¨É¨Ãè·½ÏòÏÂÏß¶ÎµÄÒ»¸ö¶ËµãY×ø±ê
 * @param  usX2 £ºÔÚÌØ¶¨É¨Ãè·½ÏòÏÂÏß¶ÎµÄÁíÒ»¸ö¶ËµãX×ø±ê
 * @param  usY2 £ºÔÚÌØ¶¨É¨Ãè·½ÏòÏÂÏß¶ÎµÄÁíÒ»¸ö¶ËµãY×ø±ê
 * @note ¿ÉÊ¹ÓÃLCD_SetBackColor¡¢LCD_SetTextColor¡¢LCD_SetColorsº¯ÊýÉèÖÃÑÕÉ«
 * @retval ÎÞ
 */
void ILI9341_DrawLine(uint16_t usX1, uint16_t usY1, uint16_t usX2, uint16_t usY2)
{
	uint16_t us;
	uint16_t usX_Current, usY_Current;

	int32_t lError_X = 0, lError_Y = 0, lDelta_X, lDelta_Y, lDistance;
	int32_t lIncrease_X, lIncrease_Y;

	lDelta_X = usX2 - usX1; //¼ÆËã×ø±êÔöÁ¿
	lDelta_Y = usY2 - usY1;

	usX_Current = usX1;
	usY_Current = usY1;

	if (lDelta_X > 0)
		lIncrease_X = 1; //ÉèÖÃµ¥²½·½Ïò

	else if (lDelta_X == 0)
		lIncrease_X = 0; //´¹Ö±Ïß

	else
	{
		lIncrease_X = -1;
		lDelta_X = -lDelta_X;
	}

	if (lDelta_Y > 0)
		lIncrease_Y = 1;

	else if (lDelta_Y == 0)
		lIncrease_Y = 0; //Ë®Æ½Ïß

	else
	{
		lIncrease_Y = -1;
		lDelta_Y = -lDelta_Y;
	}

	if (lDelta_X > lDelta_Y)
		lDistance = lDelta_X; //Ñ¡È¡»ù±¾ÔöÁ¿×ø±êÖá

	else
		lDistance = lDelta_Y;

	for (us = 0; us <= lDistance + 1; us++) //»­ÏßÊä³ö
	{
		ILI9341_SetPointPixel(usX_Current, usY_Current); //»­µã

		lError_X += lDelta_X;
		lError_Y += lDelta_Y;

		if (lError_X > lDistance)
		{
			lError_X -= lDistance;
			usX_Current += lIncrease_X;
		}

		if (lError_Y > lDistance)
		{
			lError_Y -= lDistance;
			usY_Current += lIncrease_Y;
		}
	}
}

/**
 * @brief  ÔÚ ILI9341 ÏÔÊ¾Æ÷ÉÏ»­Ò»¸ö¾ØÐÎ
 * @param  usX_Start £ºÔÚÌØ¶¨É¨Ãè·½ÏòÏÂ¾ØÐÎµÄÆðÊ¼µãX×ø±ê
 * @param  usY_Start £ºÔÚÌØ¶¨É¨Ãè·½ÏòÏÂ¾ØÐÎµÄÆðÊ¼µãY×ø±ê
 * @param  usWidth£º¾ØÐÎµÄ¿í¶È£¨µ¥Î»£ºÏñËØ£©
 * @param  usHeight£º¾ØÐÎµÄ¸ß¶È£¨µ¥Î»£ºÏñËØ£©
 * @param  ucFilled £ºÑ¡ÔñÊÇ·ñÌî³ä¸Ã¾ØÐÎ
  *   ¸Ã²ÎÊýÎªÒÔÏÂÖµÖ®Ò»£º
  *     @arg 0 :¿ÕÐÄ¾ØÐÎ
  *     @arg 1 :ÊµÐÄ¾ØÐÎ 
 * @note ¿ÉÊ¹ÓÃLCD_SetBackColor¡¢LCD_SetTextColor¡¢LCD_SetColorsº¯ÊýÉèÖÃÑÕÉ«
 * @retval ÎÞ
 */
void ILI9341_DrawRectangle(uint16_t usX_Start, uint16_t usY_Start, uint16_t usWidth, uint16_t usHeight, uint8_t ucFilled)
{
	if (ucFilled)
	{
		ILI9341_OpenWindow(usX_Start, usY_Start, usWidth, usHeight);
		ILI9341_FillColor(usWidth * usHeight, CurrentTextColor);
	}
	else
	{
		ILI9341_DrawLine(usX_Start, usY_Start, usX_Start + usWidth - 1, usY_Start);
		ILI9341_DrawLine(usX_Start, usY_Start + usHeight - 1, usX_Start + usWidth - 1, usY_Start + usHeight - 1);
		ILI9341_DrawLine(usX_Start, usY_Start, usX_Start, usY_Start + usHeight - 1);
		ILI9341_DrawLine(usX_Start + usWidth - 1, usY_Start, usX_Start + usWidth - 1, usY_Start + usHeight - 1);
	}
}

/**
 * @brief  ÔÚ ILI9341 ÏÔÊ¾Æ÷ÉÏÊ¹ÓÃ Bresenham Ëã·¨»­Ô²
 * @param  usX_Center £ºÔÚÌØ¶¨É¨Ãè·½ÏòÏÂÔ²ÐÄµÄX×ø±ê
 * @param  usY_Center £ºÔÚÌØ¶¨É¨Ãè·½ÏòÏÂÔ²ÐÄµÄY×ø±ê
 * @param  usRadius£ºÔ²µÄ°ë¾¶£¨µ¥Î»£ºÏñËØ£©
 * @param  ucFilled £ºÑ¡ÔñÊÇ·ñÌî³ä¸ÃÔ²
  *   ¸Ã²ÎÊýÎªÒÔÏÂÖµÖ®Ò»£º
  *     @arg 0 :¿ÕÐÄÔ²
  *     @arg 1 :ÊµÐÄÔ²
 * @note ¿ÉÊ¹ÓÃLCD_SetBackColor¡¢LCD_SetTextColor¡¢LCD_SetColorsº¯ÊýÉèÖÃÑÕÉ«
 * @retval ÎÞ
 */
void ILI9341_DrawCircle(uint16_t usX_Center, uint16_t usY_Center, uint16_t usRadius, uint8_t ucFilled)
{
	int16_t sCurrentX, sCurrentY;
	int16_t sError;

	sCurrentX = 0;
	sCurrentY = usRadius;

	sError = 3 - (usRadius << 1); //ÅÐ¶ÏÏÂ¸öµãÎ»ÖÃµÄ±êÖ¾

	while (sCurrentX <= sCurrentY)
	{
		int16_t sCountY;

		if (ucFilled)
			for (sCountY = sCurrentX; sCountY <= sCurrentY; sCountY++)
			{
				ILI9341_SetPointPixel(usX_Center + sCurrentX, usY_Center + sCountY); //1£¬ÑÐ¾¿¶ÔÏó
				ILI9341_SetPointPixel(usX_Center - sCurrentX, usY_Center + sCountY); //2
				ILI9341_SetPointPixel(usX_Center - sCountY, usY_Center + sCurrentX); //3
				ILI9341_SetPointPixel(usX_Center - sCountY, usY_Center - sCurrentX); //4
				ILI9341_SetPointPixel(usX_Center - sCurrentX, usY_Center - sCountY); //5
				ILI9341_SetPointPixel(usX_Center + sCurrentX, usY_Center - sCountY); //6
				ILI9341_SetPointPixel(usX_Center + sCountY, usY_Center - sCurrentX); //7
				ILI9341_SetPointPixel(usX_Center + sCountY, usY_Center + sCurrentX); //0
			}

		else
		{
			ILI9341_SetPointPixel(usX_Center + sCurrentX, usY_Center + sCurrentY); //1£¬ÑÐ¾¿¶ÔÏó
			ILI9341_SetPointPixel(usX_Center - sCurrentX, usY_Center + sCurrentY); //2
			ILI9341_SetPointPixel(usX_Center - sCurrentY, usY_Center + sCurrentX); //3
			ILI9341_SetPointPixel(usX_Center - sCurrentY, usY_Center - sCurrentX); //4
			ILI9341_SetPointPixel(usX_Center - sCurrentX, usY_Center - sCurrentY); //5
			ILI9341_SetPointPixel(usX_Center + sCurrentX, usY_Center - sCurrentY); //6
			ILI9341_SetPointPixel(usX_Center + sCurrentY, usY_Center - sCurrentX); //7
			ILI9341_SetPointPixel(usX_Center + sCurrentY, usY_Center + sCurrentX); //0
		}

		sCurrentX++;

		if (sError < 0)
			sError += 4 * sCurrentX + 6;

		else
		{
			sError += 10 + 4 * (sCurrentX - sCurrentY);
			sCurrentY--;
		}
	}
}

/**
 * @brief  ÔÚ ILI9341 ÏÔÊ¾Æ÷ÉÏÏÔÊ¾Ò»¸öÓ¢ÎÄ×Ö·û
 * @param  usX £ºÔÚÌØ¶¨É¨Ãè·½ÏòÏÂ×Ö·ûµÄÆðÊ¼X×ø±ê
 * @param  usY £ºÔÚÌØ¶¨É¨Ãè·½ÏòÏÂ¸ÃµãµÄÆðÊ¼Y×ø±ê
 * @param  cChar £ºÒªÏÔÊ¾µÄÓ¢ÎÄ×Ö·û
 * @note ¿ÉÊ¹ÓÃLCD_SetBackColor¡¢LCD_SetTextColor¡¢LCD_SetColorsº¯ÊýÉèÖÃÑÕÉ«
 * @retval ÎÞ
 */
void ILI9341_DispChar_EN(uint16_t usX, uint16_t usY, const char cChar)
{
	uint8_t byteCount, bitCount, fontLength;
	uint16_t ucRelativePositon;
	uint8_t *Pfont;

	//¶ÔasciiÂë±íÆ«ÒÆ£¨×ÖÄ£±í²»°üº¬ASCII±íµÄÇ°32¸ö·ÇÍ¼ÐÎ·ûºÅ£©
	ucRelativePositon = cChar - ' ';

	//Ã¿¸ö×ÖÄ£µÄ×Ö½ÚÊý
	fontLength = (LCD_Currentfonts->Width * LCD_Currentfonts->Height) / 8;

	//×ÖÄ£Ê×µØÖ·
	/*asciiÂë±íÆ«ÒÆÖµ³ËÒÔÃ¿¸ö×ÖÄ£µÄ×Ö½ÚÊý£¬Çó³ö×ÖÄ£µÄÆ«ÒÆÎ»ÖÃ*/
	Pfont = (uint8_t *)&LCD_Currentfonts->table[ucRelativePositon * fontLength];

	//ÉèÖÃÏÔÊ¾´°¿Ú
	ILI9341_OpenWindow(usX, usY, LCD_Currentfonts->Width, LCD_Currentfonts->Height);

	ILI9341_Write_Cmd(CMD_SetPixel);

	//°´×Ö½Ú¶ÁÈ¡×ÖÄ£Êý¾Ý
	//ÓÉÓÚÇ°ÃæÖ±½ÓÉèÖÃÁËÏÔÊ¾´°¿Ú£¬ÏÔÊ¾Êý¾Ý»á×Ô¶¯»»ÐÐ
	for (byteCount = 0; byteCount < fontLength; byteCount++)
	{
		//Ò»Î»Ò»Î»´¦ÀíÒªÏÔÊ¾µÄÑÕÉ«
		for (bitCount = 0; bitCount < 8; bitCount++)
		{
			if (Pfont[byteCount] & (0x80 >> bitCount))
				ILI9341_Write_Data(CurrentTextColor);
			else
				ILI9341_Write_Data(CurrentBackColor);
		}
	}
}

/**
 * @brief  ÔÚ ILI9341 ÏÔÊ¾Æ÷ÉÏÏÔÊ¾Ó¢ÎÄ×Ö·û´®
 * @param  line £ºÔÚÌØ¶¨É¨Ãè·½ÏòÏÂ×Ö·û´®µÄÆðÊ¼Y×ø±ê
  *   ±¾²ÎÊý¿ÉÊ¹ÓÃºêLINE(0)¡¢LINE(1)µÈ·½Ê½Ö¸¶¨ÎÄ×Ö×ø±ê£¬
  *   ºêLINE(x)»á¸ù¾Ýµ±Ç°Ñ¡ÔñµÄ×ÖÌåÀ´¼ÆËãY×ø±êÖµ¡£
	*		ÏÔÊ¾ÖÐÎÄÇÒÊ¹ÓÃLINEºêÊ±£¬ÐèÒª°ÑÓ¢ÎÄ×ÖÌåÉèÖÃ³ÉFont8x16
 * @param  pStr £ºÒªÏÔÊ¾µÄÓ¢ÎÄ×Ö·û´®µÄÊ×µØÖ·
 * @note ¿ÉÊ¹ÓÃLCD_SetBackColor¡¢LCD_SetTextColor¡¢LCD_SetColorsº¯ÊýÉèÖÃÑÕÉ«
 * @retval ÎÞ
 */
void ILI9341_DispStringLine_EN(uint16_t line, char *pStr)
{
	uint16_t usX = 0;

	while (*pStr != '\0')
	{
		if ((usX - ILI9341_DispWindow_X_Star + LCD_Currentfonts->Width) > LCD_X_LENGTH)
		{
			usX = ILI9341_DispWindow_X_Star;
			line += LCD_Currentfonts->Height;
		}

		if ((line - ILI9341_DispWindow_Y_Star + LCD_Currentfonts->Height) > LCD_Y_LENGTH)
		{
			usX = ILI9341_DispWindow_X_Star;
			line = ILI9341_DispWindow_Y_Star;
		}

		ILI9341_DispChar_EN(usX, line, *pStr);

		pStr++;

		usX += LCD_Currentfonts->Width;
	}
}

/**
 * @brief  ÔÚ ILI9341 ÏÔÊ¾Æ÷ÉÏÏÔÊ¾Ó¢ÎÄ×Ö·û´®
 * @param  usX £ºÔÚÌØ¶¨É¨Ãè·½ÏòÏÂ×Ö·ûµÄÆðÊ¼X×ø±ê
 * @param  usY £ºÔÚÌØ¶¨É¨Ãè·½ÏòÏÂ×Ö·ûµÄÆðÊ¼Y×ø±ê
 * @param  pStr £ºÒªÏÔÊ¾µÄÓ¢ÎÄ×Ö·û´®µÄÊ×µØÖ·
 * @note ¿ÉÊ¹ÓÃLCD_SetBackColor¡¢LCD_SetTextColor¡¢LCD_SetColorsº¯ÊýÉèÖÃÑÕÉ«
 * @retval ÎÞ
 */
void ILI9341_DispString_EN(uint16_t usX, uint16_t usY, char *pStr)
{
	while (*pStr != '\0')
	{
		if ((usX - ILI9341_DispWindow_X_Star + LCD_Currentfonts->Width) > LCD_X_LENGTH)
		{
			usX = ILI9341_DispWindow_X_Star;
			usY += LCD_Currentfonts->Height;
		}

		if ((usY - ILI9341_DispWindow_Y_Star + LCD_Currentfonts->Height) > LCD_Y_LENGTH)
		{
			usX = ILI9341_DispWindow_X_Star;
			usY = ILI9341_DispWindow_Y_Star;
		}

		ILI9341_DispChar_EN(usX, usY, *pStr);

		pStr++;

		usX += LCD_Currentfonts->Width;
	}
}

/**
 * @brief  ÔÚ ILI9341 ÏÔÊ¾Æ÷ÉÏÏÔÊ¾Ó¢ÎÄ×Ö·û´®(ÑØYÖá·½Ïò)
 * @param  usX £ºÔÚÌØ¶¨É¨Ãè·½ÏòÏÂ×Ö·ûµÄÆðÊ¼X×ø±ê
 * @param  usY £ºÔÚÌØ¶¨É¨Ãè·½ÏòÏÂ×Ö·ûµÄÆðÊ¼Y×ø±ê
 * @param  pStr £ºÒªÏÔÊ¾µÄÓ¢ÎÄ×Ö·û´®µÄÊ×µØÖ·
 * @note ¿ÉÊ¹ÓÃLCD_SetBackColor¡¢LCD_SetTextColor¡¢LCD_SetColorsº¯ÊýÉèÖÃÑÕÉ«
 * @retval ÎÞ
 */
void ILI9341_DispString_EN_YDir(uint16_t usX, uint16_t usY, char *pStr)
{
	while (*pStr != '\0')
	{
		if ((usY - ILI9341_DispWindow_Y_Star + LCD_Currentfonts->Height) > LCD_Y_LENGTH)
		{
			usY = ILI9341_DispWindow_Y_Star;
			usX += LCD_Currentfonts->Width;
		}

		if ((usX - ILI9341_DispWindow_X_Star + LCD_Currentfonts->Width) > LCD_X_LENGTH)
		{
			usX = ILI9341_DispWindow_X_Star;
			usY = ILI9341_DispWindow_Y_Star;
		}

		ILI9341_DispChar_EN(usX, usY, *pStr);

		pStr++;

		usY += LCD_Currentfonts->Height;
	}
}

/**
  * @brief  ÉèÖÃÓ¢ÎÄ×ÖÌåÀàÐÍ
  * @param  fonts: Ö¸¶¨ÒªÑ¡ÔñµÄ×ÖÌå
	*		²ÎÊýÎªÒÔÏÂÖµÖ®Ò»
  * 	@arg£ºFont24x32;
  * 	@arg£ºFont16x24;
  * 	@arg£ºFont8x16;
  * @retval None
  */
void LCD_SetFont(sFONT *fonts)
{
	LCD_Currentfonts = fonts;
}

/**
  * @brief  »ñÈ¡µ±Ç°×ÖÌåÀàÐÍ
  * @param  None.
  * @retval ·µ»Øµ±Ç°×ÖÌåÀàÐÍ
  */
sFONT *LCD_GetFont(void)
{
	return LCD_Currentfonts;
}

/**
  * @brief  ÉèÖÃLCDµÄÇ°¾°(×ÖÌå)¼°±³¾°ÑÕÉ«,RGB565
  * @param  TextColor: Ö¸¶¨Ç°¾°(×ÖÌå)ÑÕÉ«
  * @param  BackColor: Ö¸¶¨±³¾°ÑÕÉ«
  * @retval None
  */
void LCD_SetColors(uint16_t TextColor, uint16_t BackColor)
{
	CurrentTextColor = TextColor;
	CurrentBackColor = BackColor;
}

/**
  * @brief  »ñÈ¡LCDµÄÇ°¾°(×ÖÌå)¼°±³¾°ÑÕÉ«,RGB565
  * @param  TextColor: ÓÃÀ´´æ´¢Ç°¾°(×ÖÌå)ÑÕÉ«µÄÖ¸Õë±äÁ¿
  * @param  BackColor: ÓÃÀ´´æ´¢±³¾°ÑÕÉ«µÄÖ¸Õë±äÁ¿
  * @retval None
  */
void LCD_GetColors(uint16_t *TextColor, uint16_t *BackColor)
{
	*TextColor = CurrentTextColor;
	*BackColor = CurrentBackColor;
}

/**
  * @brief  ÉèÖÃLCDµÄÇ°¾°(×ÖÌå)ÑÕÉ«,RGB565
  * @param  Color: Ö¸¶¨Ç°¾°(×ÖÌå)ÑÕÉ« 
  * @retval None
  */
void LCD_SetTextColor(uint16_t Color)
{
	CurrentTextColor = Color;
}

/**
  * @brief  ÉèÖÃLCDµÄ±³¾°ÑÕÉ«,RGB565
  * @param  Color: Ö¸¶¨±³¾°ÑÕÉ« 
  * @retval None
  */
void LCD_SetBackColor(uint16_t Color)
{
	CurrentBackColor = Color;
}

/**
  * @brief  Çå³ýÄ³ÐÐÎÄ×Ö
  * @param  Line: Ö¸¶¨ÒªÉ¾³ýµÄÐÐ
  *   ±¾²ÎÊý¿ÉÊ¹ÓÃºêLINE(0)¡¢LINE(1)µÈ·½Ê½Ö¸¶¨ÒªÉ¾³ýµÄÐÐ£¬
  *   ºêLINE(x)»á¸ù¾Ýµ±Ç°Ñ¡ÔñµÄ×ÖÌåÀ´¼ÆËãY×ø±êÖµ£¬²¢É¾³ýµ±Ç°×ÖÌå¸ß¶ÈµÄµÚxÐÐ¡£
  * @retval None
  */
void LCD_ClearLine(uint16_t Line)
{
	ILI9341_Clear(0, Line, LCD_X_LENGTH, ((sFONT *)LCD_GetFont())->Height); /* ÇåÆÁ£¬ÏÔÊ¾È«ºÚ */
}
/**
 * @brief  ÔÚ ILI9341 ÏÔÊ¾Æ÷ÉÏÏÔÊ¾Ò»¸öÖÐÎÄ×Ö·û
 * @param  usX £ºÔÚÌØ¶¨É¨Ãè·½ÏòÏÂ×Ö·ûµÄÆðÊ¼X×ø±ê
 * @param  usY £ºÔÚÌØ¶¨É¨Ãè·½ÏòÏÂ×Ö·ûµÄÆðÊ¼Y×ø±ê
 * @param  usChar £ºÒªÏÔÊ¾µÄÖÐÎÄ×Ö·û£¨¹ú±êÂë£©
 * @note ¿ÉÊ¹ÓÃLCD_SetBackColor¡¢LCD_SetTextColor¡¢LCD_SetColorsº¯ÊýÉèÖÃÑÕÉ«
 * @retval ÎÞ
 */
void ILI9341_DispChar_CH(uint16_t usX, uint16_t usY, uint16_t usChar)
{
	uint8_t rowCount, bitCount;
	uint8_t ucBuffer[WIDTH_CH_CHAR * HEIGHT_CH_CHAR / 8];
	uint16_t usTemp;

	//ÉèÖÃÏÔÊ¾´°¿Ú
	ILI9341_OpenWindow(usX, usY, WIDTH_CH_CHAR, HEIGHT_CH_CHAR);

	ILI9341_Write_Cmd(CMD_SetPixel);

	//È¡×ÖÄ£Êý¾Ý
	GetGBKCode(ucBuffer, usChar);

	for (rowCount = 0; rowCount < HEIGHT_CH_CHAR; rowCount++)
	{
		/* È¡³öÁ½¸ö×Ö½ÚµÄÊý¾Ý£¬ÔÚlcdÉÏ¼´ÊÇÒ»¸öºº×ÖµÄÒ»ÐÐ */
		usTemp = ucBuffer[rowCount * 2];
		usTemp = (usTemp << 8);
		usTemp |= ucBuffer[rowCount * 2 + 1];

		for (bitCount = 0; bitCount < WIDTH_CH_CHAR; bitCount++)
		{
			if (usTemp & (0x8000 >> bitCount)) //¸ßÎ»ÔÚÇ°
				ILI9341_Write_Data(CurrentTextColor);
			else
				ILI9341_Write_Data(CurrentBackColor);
		}
	}
}

/**
 * @brief  ÔÚ ILI9341 ÏÔÊ¾Æ÷ÉÏÏÔÊ¾ÖÐÎÄ×Ö·û´®
 * @param  line £ºÔÚÌØ¶¨É¨Ãè·½ÏòÏÂ×Ö·û´®µÄÆðÊ¼Y×ø±ê
  *   ±¾²ÎÊý¿ÉÊ¹ÓÃºêLINE(0)¡¢LINE(1)µÈ·½Ê½Ö¸¶¨ÎÄ×Ö×ø±ê£¬
  *   ºêLINE(x)»á¸ù¾Ýµ±Ç°Ñ¡ÔñµÄ×ÖÌåÀ´¼ÆËãY×ø±êÖµ¡£
	*		ÏÔÊ¾ÖÐÎÄÇÒÊ¹ÓÃLINEºêÊ±£¬ÐèÒª°ÑÓ¢ÎÄ×ÖÌåÉèÖÃ³ÉFont8x16
 * @param  pStr £ºÒªÏÔÊ¾µÄÓ¢ÎÄ×Ö·û´®µÄÊ×µØÖ·
 * @note ¿ÉÊ¹ÓÃLCD_SetBackColor¡¢LCD_SetTextColor¡¢LCD_SetColorsº¯ÊýÉèÖÃÑÕÉ«
 * @retval ÎÞ
 */
void ILI9341_DispString_CH(uint16_t usX, uint16_t usY, char *pStr)
{
	uint16_t usCh;

	while (*pStr != '\0')
	{
		if ((usX - ILI9341_DispWindow_X_Star + WIDTH_CH_CHAR) > LCD_X_LENGTH)
		{
			usX = ILI9341_DispWindow_X_Star;
			usY += HEIGHT_CH_CHAR;
		}

		if ((usY - ILI9341_DispWindow_Y_Star + HEIGHT_CH_CHAR) > LCD_Y_LENGTH)
		{
			usX = ILI9341_DispWindow_X_Star;
			usY = ILI9341_DispWindow_Y_Star;
		}

		usCh = *(uint16_t *)pStr;
		usCh = (usCh << 8) + (usCh >> 8);

		ILI9341_DispChar_CH(usX, usY, usCh);

		usX += WIDTH_CH_CHAR;

		pStr += 2; //Ò»¸öºº×ÖÁ½¸ö×Ö½Ú
	}
}

/**
 * @brief  ÔÚ ILI9341 ÏÔÊ¾Æ÷ÉÏÏÔÊ¾ÖÐÓ¢ÎÄ×Ö·û´®
 * @param  line £ºÔÚÌØ¶¨É¨Ãè·½ÏòÏÂ×Ö·û´®µÄÆðÊ¼Y×ø±ê
  *   ±¾²ÎÊý¿ÉÊ¹ÓÃºêLINE(0)¡¢LINE(1)µÈ·½Ê½Ö¸¶¨ÎÄ×Ö×ø±ê£¬
  *   ºêLINE(x)»á¸ù¾Ýµ±Ç°Ñ¡ÔñµÄ×ÖÌåÀ´¼ÆËãY×ø±êÖµ¡£
	*		ÏÔÊ¾ÖÐÎÄÇÒÊ¹ÓÃLINEºêÊ±£¬ÐèÒª°ÑÓ¢ÎÄ×ÖÌåÉèÖÃ³ÉFont8x16
 * @param  pStr £ºÒªÏÔÊ¾µÄ×Ö·û´®µÄÊ×µØÖ·
 * @note ¿ÉÊ¹ÓÃLCD_SetBackColor¡¢LCD_SetTextColor¡¢LCD_SetColorsº¯ÊýÉèÖÃÑÕÉ«
 * @retval ÎÞ
 */
void ILI9341_DispStringLine_EN_CH(uint16_t line, char *pStr)
{
	uint16_t usCh;
	uint16_t usX = 0;

	while (*pStr != '\0')
	{
		if (*pStr <= 126) //Ó¢ÎÄ×Ö·û
		{
			if ((usX - ILI9341_DispWindow_X_Star + LCD_Currentfonts->Width) > LCD_X_LENGTH)
			{
				usX = ILI9341_DispWindow_X_Star;
				line += LCD_Currentfonts->Height;
			}

			if ((line - ILI9341_DispWindow_Y_Star + LCD_Currentfonts->Height) > LCD_Y_LENGTH)
			{
				usX = ILI9341_DispWindow_X_Star;
				line = ILI9341_DispWindow_Y_Star;
			}

			ILI9341_DispChar_EN(usX, line, *pStr);

			usX += LCD_Currentfonts->Width;

			pStr++;
		}

		else //ºº×Ö×Ö·û
		{
			if ((usX - ILI9341_DispWindow_X_Star + WIDTH_CH_CHAR) > LCD_X_LENGTH)
			{
				usX = ILI9341_DispWindow_X_Star;
				line += HEIGHT_CH_CHAR;
			}

			if ((line - ILI9341_DispWindow_Y_Star + HEIGHT_CH_CHAR) > LCD_Y_LENGTH)
			{
				usX = ILI9341_DispWindow_X_Star;
				line = ILI9341_DispWindow_Y_Star;
			}

			usCh = *(uint16_t *)pStr;

			usCh = (usCh << 8) + (usCh >> 8);

			ILI9341_DispChar_CH(usX, line, usCh);

			usX += WIDTH_CH_CHAR;

			pStr += 2; //Ò»¸öºº×ÖÁ½¸ö×Ö½Ú
		}
	}
}

/**
 * @brief  ÔÚ ILI9341 ÏÔÊ¾Æ÷ÉÏÏÔÊ¾ÖÐÓ¢ÎÄ×Ö·û´®
 * @param  usX £ºÔÚÌØ¶¨É¨Ãè·½ÏòÏÂ×Ö·ûµÄÆðÊ¼X×ø±ê
 * @param  usY £ºÔÚÌØ¶¨É¨Ãè·½ÏòÏÂ×Ö·ûµÄÆðÊ¼Y×ø±ê
 * @param  pStr £ºÒªÏÔÊ¾µÄ×Ö·û´®µÄÊ×µØÖ·
 * @note ¿ÉÊ¹ÓÃLCD_SetBackColor¡¢LCD_SetTextColor¡¢LCD_SetColorsº¯ÊýÉèÖÃÑÕÉ«
 * @retval ÎÞ
 */
void ILI9341_DispString_EN_CH(uint16_t usX, uint16_t usY, char *pStr)
{
	uint16_t usCh;

	while (*pStr != '\0')
	{
		if (*pStr <= 126) //Ó¢ÎÄ×Ö·û
		{
			if ((usX - ILI9341_DispWindow_X_Star + LCD_Currentfonts->Width) > LCD_X_LENGTH)
			{
				usX = ILI9341_DispWindow_X_Star;
				usY += LCD_Currentfonts->Height;
			}

			if ((usY - ILI9341_DispWindow_Y_Star + LCD_Currentfonts->Height) > LCD_Y_LENGTH)
			{
				usX = ILI9341_DispWindow_X_Star;
				usY = ILI9341_DispWindow_Y_Star;
			}

			ILI9341_DispChar_EN(usX, usY, *pStr);

			usX += LCD_Currentfonts->Width;

			pStr++;
		}

		else //ºº×Ö×Ö·û
		{
			if ((usX - ILI9341_DispWindow_X_Star + WIDTH_CH_CHAR) > LCD_X_LENGTH)
			{
				usX = ILI9341_DispWindow_X_Star;
				usY += HEIGHT_CH_CHAR;
			}

			if ((usY - ILI9341_DispWindow_Y_Star + HEIGHT_CH_CHAR) > LCD_Y_LENGTH)
			{
				usX = ILI9341_DispWindow_X_Star;
				usY = ILI9341_DispWindow_Y_Star;
			}

			usCh = *(uint16_t *)pStr;

			usCh = (usCh << 8) + (usCh >> 8);

			ILI9341_DispChar_CH(usX, usY, usCh);

			usX += WIDTH_CH_CHAR;

			pStr += 2; //Ò»¸öºº×ÖÁ½¸ö×Ö½Ú
		}
	}
}

/**
 * @brief  ÔÚ ILI9341 ÏÔÊ¾Æ÷ÉÏÏÔÊ¾ÖÐÓ¢ÎÄ×Ö·û´®(ÑØYÖá·½Ïò)
 * @param  usX £ºÔÚÌØ¶¨É¨Ãè·½ÏòÏÂ×Ö·ûµÄÆðÊ¼X×ø±ê
 * @param  usY £ºÔÚÌØ¶¨É¨Ãè·½ÏòÏÂ×Ö·ûµÄÆðÊ¼Y×ø±ê
 * @param  pStr £ºÒªÏÔÊ¾µÄÖÐÓ¢ÎÄ×Ö·û´®µÄÊ×µØÖ·
 * @note ¿ÉÊ¹ÓÃLCD_SetBackColor¡¢LCD_SetTextColor¡¢LCD_SetColorsº¯ÊýÉèÖÃÑÕÉ«
 * @retval ÎÞ
 */
void ILI9341_DispString_EN_CH_YDir(uint16_t usX, uint16_t usY, char *pStr)
{
	uint16_t usCh;

	while (*pStr != '\0')
	{
		//Í³Ò»Ê¹ÓÃºº×ÖµÄ¿í¸ßÀ´¼ÆËã»»ÐÐ
		if ((usY - ILI9341_DispWindow_Y_Star + HEIGHT_CH_CHAR) > LCD_Y_LENGTH)
		{
			usY = ILI9341_DispWindow_Y_Star;
			usX += WIDTH_CH_CHAR;
		}
		if ((usX - ILI9341_DispWindow_X_Star + WIDTH_CH_CHAR) > LCD_X_LENGTH)
		{
			usX = ILI9341_DispWindow_X_Star;
			usY = ILI9341_DispWindow_Y_Star;
		}

		//ÏÔÊ¾
		if (*pStr <= 126) //Ó¢ÎÄ×Ö·û
		{
			ILI9341_DispChar_EN(usX, usY, *pStr);

			pStr++;

			usY += HEIGHT_CH_CHAR;
		}
		else //ºº×Ö×Ö·û
		{
			usCh = *(uint16_t *)pStr;

			usCh = (usCh << 8) + (usCh >> 8);

			ILI9341_DispChar_CH(usX, usY, usCh);

			usY += HEIGHT_CH_CHAR;

			pStr += 2; //Ò»¸öºº×ÖÁ½¸ö×Ö½Ú
		}
	}
}

/*********************end of file*************************/
