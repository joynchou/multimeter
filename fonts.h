#ifndef __FONT_H
#define __FONT_H       

#include "stm32f10x.h"
#include "fonts.h"



/** @defgroup FONTS_Exported_Types
  * @{
  */ 
typedef struct _tFont
{    
  const uint8_t *table;
  uint16_t Width;
  uint16_t Height;
  
} sFONT;

extern sFONT Font24x32;
extern sFONT Font16x24;
extern sFONT Font8x16;

/*******************ÖÐÎÄ********** ÔÚÏÔÊ¾ÆÁÉÏÏÔÊ¾µÄ×Ö·û´óÐ¡ ***************************/
#define      WIDTH_CH_CHAR		                16	    //ÖÐÎÄ×Ö·û¿í¶È 
#define      HEIGHT_CH_CHAR		              	16		  //ÖÐÎÄ×Ö·û¸ß¶È 



#define LINE(x) ((x) * (((sFONT *)LCD_GetFont())->Height))

//LINEYÍ³Ò»Ê¹ÓÃºº×Ö×ÖÄ£µÄ¸ß¶È
#define LINEY(x) ((x) * (WIDTH_CH_CHAR))





//0±íÊ¾Ê¹ÓÃSD¿¨×ÖÄ££¬·ÇÁã±íÊ¾FLASH×ÖÄ£,ÓÉÓÚSD¿¨×ÖÄ£ÓÐÎÄ¼þÏµÍ³£¬ËÙ¶ÈÂýºÜ¶à¡£

#define GBKCODE_FLASH 						1

#if GBKCODE_FLASH
	/*Ê¹ÓÃFLASH×ÖÄ£*/
	/*ÖÐÎÄ×Ö¿â´æ´¢ÔÚFLASHµÄÆðÊ¼µØÖ·*/
	/*FLASH*/
	#define GBKCODE_START_ADDRESS   387*4096


	/*»ñÈ¡×Ö¿âµÄº¯Êý*/
	//¶¨Òå»ñÈ¡ÖÐÎÄ×Ö·û×ÖÄ£Êý×éµÄº¯ÊýÃû£¬ucBufferÎª´æ·Å×ÖÄ£Êý×éÃû£¬usCharÎªÖÐÎÄ×Ö·û£¨¹ú±êÂë£©
	#define      GetGBKCode( ucBuffer, usChar )  GetGBKCode_from_EXFlash( ucBuffer, usChar )  
	int GetGBKCode_from_EXFlash( uint8_t * pBuffer, uint16_t c);

#else
	/*Ê¹ÓÃSD×ÖÄ£*/


	/*SD¿¨×ÖÄ£Â·¾¶*/
	#define GBKCODE_FILE_NAME			"0:/Font/GB2312_H1616.FON"


	/*»ñÈ¡×Ö¿âµÄº¯Êý*/
	//¶¨Òå»ñÈ¡ÖÐÎÄ×Ö·û×ÖÄ£Êý×éµÄº¯ÊýÃû£¬ucBufferÎª´æ·Å×ÖÄ£Êý×éÃû£¬usCharÎªÖÐÎÄ×Ö·û£¨¹ú±êÂë£©

	#define GetGBKCode( ucBuffer, usChar )  GetGBKCode_from_sd( ucBuffer, usChar )
	int GetGBKCode_from_sd ( uint8_t * pBuffer, uint16_t c);

#endif



#endif /*end of __FONT_H    */
