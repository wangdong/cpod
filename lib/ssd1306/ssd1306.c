#include "SSD1306.h"

#include <nordic_common.h>
#include <nrf24le1.h>

#include <hal_delay.h>
#include <hal_spi.h>

// --------------------------------------------------------
sbit DISPLAY_RST = P1 ^ 0;
sbit DISPLAY_DC  = P1 ^ 1;

// --------------------------------------------------------
#define LED_IMAGE_WHITE       1
#define LED_IMAGE_BLACK       0

#define LED_MAX_ROW_NUM      64
#define LED_MAX_COLUMN_NUM  128


//======================================================
// 128X64OLED液晶底层驱动[6X8]字体库
// 描  述: [6X8X8]西文字符的字模数据 (纵向取模,字节倒序)
// !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~
//======================================================
static
const uint8_t code F6x8[][6] =
{
// Font-Face: "Andale Mono"

{0x00,0x00,0x00,0x00,0x00,0x00},/*"\0",0*/

{0x00,0x00,0x2E,0x00,0x00,0x00},/*"!",0*/

{0x00,0x06,0x00,0x06,0x00,0x00},/*""",1*/

{0x0C,0x3C,0x0E,0x3C,0x0E,0x0C},/*"#",2*/

{0x00,0x26,0x2A,0x3E,0x32,0x00},/*"$",3*/

{0x06,0x26,0x1E,0x3C,0x32,0x30},/*"%",4*/

{0x30,0x2E,0x2E,0x36,0x30,0x28},/*"&",5*/

{0x00,0x00,0x06,0x00,0x00,0x00},/*"'",6*/

{0x00,0x00,0x3C,0x44,0x42,0x00},/*"(",7*/

{0x00,0x42,0x24,0x18,0x00,0x00},/*")",8*/

{0x00,0x06,0x0E,0x06,0x00,0x00},/*"*",9*/

{0x00,0x08,0x08,0x1C,0x08,0x08},/*"+",10*/

{0x00,0x40,0x20,0x00,0x00,0x00},/*",",11*/

{0x00,0x00,0x10,0x10,0x10,0x00},/*"-",12*/

{0x00,0x00,0x20,0x00,0x00,0x00},/*".",13*/

{0x00,0x30,0x1C,0x06,0x00,0x00},/*"/",14*/

{0x1C,0x22,0x2A,0x22,0x1C,0x00},/*"0",15*/

{0x00,0x22,0x3E,0x20,0x00,0x00},/*"1",16*/

{0x00,0x22,0x32,0x2A,0x26,0x00},/*"2",17*/

{0x00,0x22,0x2A,0x2A,0x16,0x00},/*"3",18*/

{0x18,0x14,0x12,0x3E,0x10,0x00},/*"4",19*/

{0x00,0x2E,0x2A,0x2A,0x1A,0x00},/*"5",20*/

{0x1C,0x2E,0x2A,0x2A,0x12,0x00},/*"6",21*/

{0x02,0x22,0x12,0x0A,0x06,0x00},/*"7",22*/

{0x34,0x2A,0x2A,0x2A,0x34,0x00},/*"8",23*/

{0x24,0x2A,0x2A,0x3A,0x1C,0x00},/*"9",24*/

{0x00,0x00,0x24,0x00,0x00,0x00},/*":",25*/

{0x00,0x40,0x24,0x00,0x00,0x00},/*";",26*/

{0x00,0x08,0x14,0x14,0x14,0x00},/*"<",27*/

{0x18,0x18,0x18,0x18,0x18,0x00},/*"=",28*/

{0x00,0x14,0x14,0x14,0x08,0x00},/*">",29*/

{0x00,0x04,0x2A,0x0E,0x06,0x00},/*"?",30*/

{0x1C,0x26,0x2A,0x2A,0x2A,0x0C},/*"@",31*/

{0x20,0x1C,0x0A,0x1C,0x20,0x00},/*"A",32*/

{0x00,0x3E,0x2A,0x2A,0x36,0x00},/*"B",33*/

{0x00,0x1C,0x22,0x22,0x36,0x00},/*"C",34*/

{0x00,0x3E,0x22,0x22,0x1C,0x00},/*"D",35*/

{0x00,0x3E,0x2A,0x2A,0x2A,0x00},/*"E",36*/

{0x00,0x3E,0x0A,0x0A,0x0A,0x00},/*"F",37*/

{0x00,0x1C,0x22,0x32,0x32,0x00},/*"G",38*/

{0x00,0x3E,0x08,0x08,0x3E,0x00},/*"H",39*/

{0x00,0x22,0x3E,0x22,0x00,0x00},/*"I",40*/

{0x00,0x20,0x20,0x3E,0x00,0x00},/*"J",41*/

{0x00,0x3E,0x08,0x0C,0x12,0x20},/*"K",42*/

{0x00,0x3E,0x20,0x20,0x20,0x00},/*"L",43*/

{0x3E,0x0C,0x08,0x04,0x3E,0x00},/*"M",44*/

{0x00,0x3E,0x0E,0x30,0x3E,0x00},/*"N",45*/

{0x00,0x1C,0x22,0x22,0x1C,0x00},/*"O",46*/

{0x00,0x3E,0x0A,0x0A,0x0E,0x00},/*"P",47*/

{0x00,0x1C,0x22,0x62,0x5C,0x00},/*"Q",48*/

{0x00,0x3E,0x0A,0x1A,0x26,0x00},/*"R",49*/

{0x00,0x26,0x2A,0x2A,0x16,0x00},/*"S",50*/

{0x02,0x02,0x3E,0x02,0x02,0x00},/*"T",51*/

{0x00,0x3E,0x20,0x20,0x3E,0x00},/*"U",52*/

{0x02,0x1C,0x20,0x1C,0x02,0x00},/*"V",53*/

{0x0E,0x30,0x08,0x30,0x0E,0x00},/*"W",54*/

{0x22,0x14,0x08,0x14,0x22,0x00},/*"X",55*/

{0x02,0x04,0x38,0x04,0x02,0x00},/*"Y",56*/

{0x00,0x22,0x32,0x2E,0x22,0x00},/*"Z",57*/

{0x00,0x00,0x00,0x7E,0x42,0x00},/*"[",58*/

{0x00,0x06,0x18,0x20,0x00,0x00},/*"\",59*/

{0x00,0x42,0x7E,0x00,0x00,0x00},/*"]",60*/

{0x00,0x04,0x02,0x01,0x02,0x04},/*"^",61*/

{0x80,0x80,0x80,0x80,0x80,0x80},/*"_",62*/

{0x00,0x00,0x01,0x02,0x00,0x00},/*"`",63*/

{0x00,0x34,0x34,0x34,0x3C,0x00},/*"a",64*/

{0x00,0x3E,0x24,0x24,0x1C,0x00},/*"b",65*/

{0x00,0x18,0x24,0x24,0x24,0x00},/*"c",66*/

{0x00,0x1C,0x24,0x24,0x3E,0x00},/*"d",67*/

{0x00,0x18,0x2C,0x2C,0x28,0x00},/*"e",68*/

{0x00,0x04,0x3E,0x06,0x06,0x00},/*"f",69*/

{0x00,0x7C,0x6C,0x6C,0x6C,0x00},/*"g",70*/

{0x00,0x3E,0x04,0x04,0x3C,0x00},/*"h",71*/

{0x00,0x00,0x04,0x3E,0x00,0x00},/*"i",72*/

{0x00,0x60,0x44,0x7E,0x00,0x00},/*"j",73*/

{0x00,0x3E,0x08,0x14,0x20,0x00},/*"k",74*/

{0x00,0x00,0x02,0x3E,0x20,0x00},/*"l",75*/

{0x3C,0x04,0x38,0x04,0x38,0x00},/*"m",76*/

{0x00,0x3C,0x04,0x04,0x3C,0x00},/*"n",77*/

{0x00,0x18,0x24,0x24,0x18,0x00},/*"o",78*/

{0x00,0x7C,0x24,0x24,0x1C,0x00},/*"p",79*/

{0x00,0x1C,0x24,0x24,0x7C,0x00},/*"q",80*/

{0x00,0x00,0x3C,0x04,0x04,0x00},/*"r",81*/

{0x00,0x3C,0x2C,0x34,0x3C,0x00},/*"s",82*/

{0x00,0x04,0x3E,0x24,0x24,0x00},/*"t",83*/

{0x00,0x3C,0x20,0x20,0x3C,0x00},/*"u",84*/

{0x04,0x18,0x20,0x18,0x04,0x00},/*"v",85*/

{0x0C,0x38,0x08,0x30,0x0C,0x00},/*"w",86*/

{0x00,0x24,0x18,0x18,0x24,0x00},/*"x",87*/

{0x44,0x58,0x30,0x18,0x04,0x00},/*"y",88*/

{0x00,0x24,0x34,0x2C,0x24,0x00},/*"z",89*/

{0x00,0x00,0x08,0x76,0x42,0x00},/*"{",90*/

{0x00,0x00,0x7E,0x00,0x00,0x00},/*"|",91*/

{0x00,0x42,0x76,0x08,0x00,0x00},/*"}",92*/

{0x10,0x10,0x10,0x10,0x10,0x00},/*"~",93*/

};

//======================================================
// 128X64OLED液晶底层驱动[8X16]字体库
// 描  述: [8X16]西文字符的字模数据 (纵向取模,字节倒序)
// !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~
//======================================================
static
const uint8_t code F8X16[][16]=
{
// Font-Face: "Consolas"

{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"\0",0*/

{0x00,0x00,0x00,0xF8,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x11,0x1B,0x00,0x00,0x00},/*"!",0*/

{0x00,0x00,0x78,0x08,0x00,0x78,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*""",1*/

{0x00,0x40,0xE0,0x50,0x40,0xF0,0x50,0x40,0x00,0x04,0x1F,0x04,0x04,0x1F,0x04,0x00},/*"#",2*/

{0x00,0xE0,0xB0,0x10,0xF8,0x10,0x10,0x00,0x00,0x10,0x10,0x7F,0x11,0x13,0x0E,0x00},/*"$",3*/

{0x30,0x58,0x48,0x70,0x80,0x60,0x30,0x08,0x10,0x18,0x04,0x03,0x0C,0x12,0x12,0x0C},/*"%",4*/

{0x00,0x60,0xF0,0x90,0x90,0x70,0x00,0x00,0x04,0x1F,0x11,0x11,0x16,0x1C,0x1F,0x10},/*"&",5*/

{0x00,0x00,0x00,0x78,0x78,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"'",6*/

{0x00,0x00,0x80,0xE0,0x30,0x08,0x00,0x00,0x00,0x00,0x07,0x3F,0x60,0xC0,0x00,0x00},/*"(",7*/

{0x00,0x00,0x08,0x10,0xE0,0x80,0x00,0x00,0x00,0x00,0x80,0x60,0x38,0x0F,0x00,0x00},/*")",8*/

{0x00,0x80,0xA0,0x60,0xD8,0x60,0x90,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00},/*"*",9*/

{0x00,0x00,0x00,0xC0,0xC0,0x00,0x00,0x00,0x00,0x02,0x02,0x1F,0x1F,0x02,0x02,0x02},/*"+",10*/

{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x58,0x38,0x00,0x00,0x00},/*",",11*/

{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x02,0x02,0x02,0x00,0x00},/*"-",12*/

{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00},/*".",13*/

{0x00,0x00,0x00,0x00,0xC0,0x70,0x18,0x00,0x00,0x20,0x18,0x06,0x01,0x00,0x00,0x00},/*"/",14*/

{0x00,0xE0,0x30,0x10,0x10,0xB0,0xE0,0x80,0x00,0x0F,0x1A,0x13,0x11,0x18,0x0F,0x03},/*"0",15*/

{0x00,0x60,0x20,0x30,0xF0,0x00,0x00,0x00,0x00,0x10,0x10,0x10,0x1F,0x10,0x10,0x00},/*"1",16*/

{0x00,0x20,0x10,0x10,0x10,0xF0,0xE0,0x00,0x00,0x10,0x18,0x14,0x12,0x11,0x10,0x00},/*"2",17*/

{0x00,0x10,0x10,0x10,0x10,0xF0,0x60,0x00,0x00,0x10,0x11,0x11,0x11,0x19,0x0E,0x00},/*"3",18*/

{0x00,0x00,0x80,0x60,0x30,0xF0,0x00,0x00,0x04,0x07,0x05,0x04,0x04,0x1F,0x04,0x04},/*"4",19*/

{0x00,0xF0,0xF0,0x10,0x10,0x10,0x10,0x00,0x00,0x11,0x11,0x11,0x11,0x19,0x0F,0x00},/*"5",20*/

{0x00,0xC0,0x60,0x30,0x90,0x10,0x10,0x00,0x00,0x0F,0x19,0x11,0x10,0x11,0x0F,0x00},/*"6",21*/

{0x00,0x10,0x10,0x10,0x10,0xD0,0x70,0x00,0x00,0x00,0x10,0x1C,0x07,0x01,0x00,0x00},/*"7",22*/

{0x00,0x60,0xB0,0x90,0x10,0x90,0xE0,0x00,0x00,0x0E,0x12,0x11,0x11,0x13,0x0E,0x00},/*"8",23*/

{0x00,0xE0,0x30,0x10,0x10,0x30,0xE0,0x00,0x00,0x11,0x11,0x12,0x12,0x09,0x07,0x00},/*"9",24*/

{0x00,0x00,0x00,0xC0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00},/*":",25*/

{0x00,0x00,0x00,0xC0,0xC0,0x00,0x00,0x00,0x00,0x00,0x40,0x58,0x38,0x00,0x00,0x00},/*";",26*/

{0x00,0x00,0x00,0x80,0x80,0x40,0x00,0x00,0x00,0x02,0x03,0x05,0x08,0x18,0x10,0x00},/*"<",27*/

{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x05,0x05,0x05,0x05,0x05,0x00},/*"=",28*/

{0x00,0x00,0x40,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x10,0x08,0x04,0x07,0x02,0x00},/*">",29*/

{0x00,0x00,0x08,0x18,0x10,0xF0,0xE0,0x00,0x00,0x00,0x00,0x1B,0x01,0x00,0x00,0x00},/*"?",30*/

{0x80,0xE0,0x10,0x88,0x88,0x88,0x10,0xE0,0x3F,0x60,0x8F,0x91,0x88,0x9F,0x08,0x0F},/*"@",31*/

{0x00,0x00,0xC0,0x70,0x30,0xE0,0x00,0x00,0x10,0x1E,0x07,0x04,0x04,0x05,0x0F,0x18},/*"A",32*/

{0x00,0xF0,0x10,0x10,0x10,0xB0,0xE0,0x00,0x00,0x1F,0x11,0x11,0x11,0x11,0x0E,0x00},/*"B",33*/

{0x00,0xC0,0x20,0x10,0x10,0x10,0x10,0x00,0x00,0x0F,0x18,0x10,0x10,0x10,0x10,0x00},/*"C",34*/

{0x00,0xF0,0x10,0x10,0x10,0x30,0xE0,0x80,0x00,0x1F,0x10,0x10,0x10,0x08,0x0F,0x03},/*"D",35*/

{0x00,0xF0,0xF0,0x10,0x10,0x10,0x10,0x00,0x00,0x1F,0x1F,0x11,0x11,0x11,0x11,0x00},/*"E",36*/

{0x00,0xF0,0xF0,0x10,0x10,0x10,0x10,0x00,0x00,0x1F,0x1F,0x01,0x01,0x01,0x01,0x00},/*"F",37*/

{0x00,0xE0,0x20,0x10,0x10,0x10,0x10,0x00,0x01,0x0F,0x18,0x10,0x11,0x11,0x1F,0x00},/*"G",38*/

{0x00,0xF0,0x00,0x00,0x00,0x00,0xF0,0x00,0x00,0x1F,0x01,0x01,0x01,0x01,0x1F,0x00},/*"H",39*/

{0x00,0x10,0x10,0xF0,0xF0,0x10,0x10,0x00,0x00,0x10,0x10,0x1F,0x1F,0x10,0x10,0x00},/*"I",40*/

{0x00,0x10,0x10,0x10,0x10,0xF0,0x00,0x00,0x00,0x18,0x10,0x10,0x10,0x0F,0x00,0x00},/*"J",41*/

{0x00,0xF0,0x00,0x80,0x40,0x20,0x10,0x00,0x00,0x1F,0x01,0x03,0x06,0x0C,0x10,0x00},/*"K",42*/

{0x00,0x00,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0x10,0x10,0x10,0x10,0x00},/*"L",43*/

{0x00,0xF0,0x70,0x80,0x80,0xE0,0xF0,0xC0,0x1C,0x1F,0x00,0x03,0x03,0x00,0x03,0x1F},/*"M",44*/

{0x00,0xF0,0x70,0xC0,0x00,0x00,0xF0,0x00,0x00,0x1F,0x00,0x01,0x07,0x1C,0x1F,0x00},/*"N",45*/

{0x80,0xE0,0x30,0x10,0x10,0x10,0xE0,0xC0,0x03,0x0F,0x18,0x10,0x10,0x10,0x0F,0x07},/*"O",46*/

{0x00,0xF0,0x10,0x10,0x10,0x30,0xE0,0x00,0x00,0x1F,0x02,0x02,0x02,0x01,0x01,0x00},/*"P",47*/

{0x80,0xE0,0x30,0x10,0x10,0x10,0xE0,0xC0,0x03,0x0F,0x18,0x30,0x70,0x50,0x4F,0x47},/*"Q",48*/

{0x00,0xF0,0xF0,0x10,0x10,0xB0,0xE0,0x00,0x00,0x1F,0x1F,0x01,0x03,0x0F,0x18,0x00},/*"R",49*/

{0x00,0xE0,0xB0,0x10,0x10,0x10,0x10,0x00,0x00,0x10,0x10,0x11,0x11,0x13,0x0E,0x00},/*"S",50*/

{0x00,0x10,0x10,0xF0,0xF0,0x10,0x10,0x10,0x00,0x00,0x00,0x1F,0x1F,0x00,0x00,0x00},/*"T",51*/

{0x00,0xF0,0x00,0x00,0x00,0x00,0xF0,0x00,0x00,0x0F,0x18,0x10,0x10,0x10,0x0F,0x00},/*"U",52*/

{0x10,0xF0,0x80,0x00,0x00,0x00,0xE0,0x30,0x00,0x00,0x07,0x1C,0x18,0x0F,0x01,0x00},/*"V",53*/

{0x30,0xF0,0x00,0x00,0x80,0x00,0x00,0xF0,0x00,0x1F,0x1C,0x07,0x03,0x1C,0x1F,0x03},/*"W",54*/

{0x00,0x30,0x60,0xC0,0x80,0x60,0x30,0x10,0x10,0x18,0x0C,0x03,0x03,0x0E,0x18,0x10},/*"X",55*/

{0x10,0x70,0xC0,0x80,0x00,0xC0,0x60,0x10,0x00,0x00,0x00,0x1F,0x1F,0x01,0x00,0x00},/*"Y",56*/

{0x00,0x10,0x10,0x10,0x90,0xD0,0x30,0x00,0x00,0x18,0x1C,0x13,0x11,0x10,0x10,0x00},/*"Z",57*/

{0x00,0x00,0x00,0xF8,0x08,0x08,0x00,0x00,0x00,0x00,0x00,0xFF,0x80,0x80,0x00,0x00},/*"[",58*/

{0x00,0x08,0x30,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x1C,0x30,0x00},/*"\",59*/

{0x00,0x00,0x08,0x08,0xF8,0xF8,0x00,0x00,0x00,0x00,0x80,0x80,0xFF,0xFF,0x00,0x00},/*"]",60*/

{0x00,0x80,0xC0,0x30,0x30,0x60,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"^",61*/

{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80},/*"_",62*/

{0x00,0x00,0x08,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"`",63*/

{0x00,0x00,0x40,0x40,0x40,0xC0,0x80,0x00,0x00,0x1C,0x12,0x12,0x12,0x0A,0x1F,0x00},/*"a",64*/

{0x00,0xF8,0x80,0x40,0x40,0x40,0x80,0x00,0x00,0x1F,0x10,0x10,0x10,0x10,0x0F,0x00},/*"b",65*/

{0x00,0x00,0x80,0x40,0x40,0x40,0x40,0x00,0x00,0x07,0x1D,0x10,0x10,0x10,0x10,0x00},/*"c",66*/

{0x00,0x80,0xC0,0x40,0x40,0x40,0xF8,0x00,0x00,0x0F,0x18,0x10,0x10,0x08,0x1F,0x00},/*"d",67*/

{0x00,0x80,0xC0,0x40,0x40,0x40,0x80,0x00,0x00,0x0F,0x1A,0x12,0x12,0x12,0x13,0x00},/*"e",68*/

{0x00,0x80,0x80,0xF0,0x98,0x88,0x88,0x08,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x00},/*"f",69*/

{0x00,0x80,0xC0,0x40,0x40,0xC0,0xC0,0x40,0x00,0x6F,0x92,0x94,0x94,0x92,0x73,0x20},/*"g",70*/

{0x00,0xF8,0x80,0x40,0x40,0x40,0x80,0x00,0x00,0x1F,0x00,0x00,0x00,0x00,0x1F,0x00},/*"h",71*/

{0x00,0x40,0x40,0x58,0xD8,0x00,0x00,0x00,0x00,0x10,0x10,0x10,0x1F,0x10,0x10,0x00},/*"i",72*/

{0x00,0x40,0x40,0x40,0x50,0xD8,0x00,0x00,0x00,0x80,0x80,0x80,0xC0,0x7F,0x00,0x00},/*"j",73*/

{0x00,0xF8,0xF8,0x00,0x80,0x80,0x40,0x00,0x00,0x1F,0x1F,0x03,0x05,0x08,0x10,0x00},/*"k",74*/

{0x00,0x08,0x08,0x08,0xF8,0x00,0x00,0x00,0x00,0x10,0x10,0x10,0x1F,0x10,0x10,0x00},/*"l",75*/

{0x00,0xC0,0x40,0x40,0x80,0x40,0xC0,0x80,0x00,0x1F,0x00,0x00,0x1F,0x00,0x1F,0x1F},/*"m",76*/

{0x00,0xC0,0x80,0x40,0x40,0x40,0x80,0x00,0x00,0x1F,0x00,0x00,0x00,0x00,0x1F,0x00},/*"n",77*/

{0x00,0x80,0xC0,0x40,0x40,0x40,0x80,0x00,0x00,0x0F,0x18,0x10,0x10,0x10,0x0F,0x02},/*"o",78*/

{0x00,0xC0,0x80,0x40,0x40,0x40,0x80,0x00,0x00,0xFF,0x10,0x10,0x10,0x10,0x0F,0x00},/*"p",79*/

{0x00,0x80,0xC0,0x40,0x40,0x40,0xC0,0x00,0x00,0x0F,0x18,0x10,0x10,0x08,0xFF,0x00},/*"q",80*/

{0x00,0xC0,0xC0,0x80,0x40,0x40,0xC0,0x00,0x00,0x1F,0x1F,0x00,0x00,0x00,0x01,0x01},/*"r",81*/

{0x00,0x00,0xC0,0x40,0x40,0x40,0x40,0x00,0x00,0x10,0x13,0x12,0x12,0x16,0x0C,0x00},/*"s",82*/

{0x40,0x40,0x40,0xF0,0x40,0x40,0x40,0x00,0x00,0x00,0x00,0x1F,0x10,0x10,0x10,0x00},/*"t",83*/

{0x00,0xC0,0x00,0x00,0x00,0x00,0xC0,0x00,0x00,0x0F,0x18,0x10,0x10,0x08,0x1F,0x00},/*"u",84*/

{0x00,0xC0,0x00,0x00,0x00,0x00,0xC0,0x40,0x00,0x01,0x07,0x1C,0x18,0x07,0x01,0x00},/*"v",85*/

{0xC0,0xC0,0x00,0x00,0x00,0x00,0x00,0xC0,0x00,0x1F,0x18,0x07,0x07,0x1C,0x1F,0x01},/*"w",86*/

{0x00,0x40,0xC0,0x00,0x00,0x80,0xC0,0x00,0x00,0x10,0x18,0x07,0x07,0x0D,0x18,0x00},/*"x",87*/

{0x00,0xC0,0x00,0x00,0x00,0x00,0xC0,0x40,0x80,0x81,0xC7,0x7C,0x38,0x0F,0x01,0x00},/*"y",88*/

{0x00,0x40,0x40,0x40,0x40,0xC0,0xC0,0x00,0x00,0x10,0x18,0x16,0x13,0x11,0x10,0x00},/*"z",89*/

{0x00,0x00,0x00,0xF0,0x18,0x08,0x08,0x00,0x00,0x02,0x03,0x7F,0xC0,0x80,0x80,0x00},/*"{",90*/

{0x00,0x00,0x00,0x00,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00},/*"|",91*/

{0x00,0x00,0x08,0x18,0xF0,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0x7F,0x03,0x02,0x00},/*"}",92*/

{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x01,0x01,0x02,0x04,0x06,0x03},/*"~",93*/

};


void LED_WrDat(uint8_t ucData);
void LED_WrCmd(uint8_t ucCmd);
void LED_SetPos(uint8_t ucIdxX, uint8_t ucIdxY);
void SetStartColumn(uint8_t ucData);
void SetAddressingMode(uint8_t ucData);
void SetColumnAddress(uint8_t a, uint8_t b);
void SetPageAddress(uint8_t a, uint8_t b);
void SetStartLine(uint8_t d);
void SetContrastControl(uint8_t d);
void SetChargePump(uint8_t d);
void SetSegmentRemap(uint8_t d);
void SetEntireDisplay(uint8_t d);
void SetInverseDisplay(uint8_t d);
void SetMultiplexRatio(uint8_t d);
void SetDisplayOnOff(uint8_t d);
void SetStartPage(uint8_t d);
void SetCommonRemap(uint8_t d);
void SetDisplayOffset(uint8_t d);
void SetDisplayClock(uint8_t d);
void SetPrechargePeriod(uint8_t d);
void SetCommonConfig(uint8_t d);
void SetVCOMH(uint8_t d);
void SetNop(void);


void LED_WrCmd(uint8_t ucCmd) {
	DISPLAY_DC = 0;
	hal_spi_master_read_write(ucCmd);
}

void LED_WrDat(uint8_t ucCmd) {
	DISPLAY_DC = 1;
	hal_spi_master_read_write(ucCmd);
}

/*****************************************************************************
 函 数 名  : LED_SetPos
 功能描述  : 设置坐标
 输入参数  : uint8_t ucIdxX  
             uint8_t ucIdxY  
 输出参数  : NONE
 返 回 值  : NONE
*****************************************************************************/
void LED_SetPos(uint8_t ucIdxX, uint8_t ucIdxY)
{ 
    LED_WrCmd(0xb0 + ucIdxY);
    LED_WrCmd(((ucIdxX & 0xf0) >> 4) | 0x10);
    LED_WrCmd((ucIdxX & 0x0f) | 0x00); 
} 

/*****************************************************************************
 函 数 名  : LED_Fill
 功能描述  : 对全屏写入同一个字符函数 
             如 LED_Fill(0x01);    对于某一位0为不亮 1为亮
            ------------------------------------------------------
                                共128列
                  1           1                1
              共  0           0                0
              8   0           0                0
              组  0       ……0     …………   0
              字  0           0                0
              符  0           0                0
              即  0           0                0
              8   0           0                0
              页  1           1                1
              64  0           0                0
              行  .           .                .
                  .           .    …………    .
                  .           .                . 
                  .           .                .
                  0           0                0
            ------------------------------------------------------
 输入参数  : uint8_t ucData  
 输出参数  : NONE
 返 回 值  : NONE
*****************************************************************************/
void LED_Fill(uint8_t ucData)
{
    uint8_t ucPage,ucColumn;
    
    for(ucPage = 0; ucPage < 8; ucPage++)
    {
        LED_WrCmd(0xb0 + ucPage);  //0xb0+0~7表示页0~7
        LED_WrCmd(0x00);           //0x00+0~16表示将128列分成16组其地址在某组中的第几列
        LED_WrCmd(0x10);           //0x10+0~16表示将128列分成16组其地址所在第几组
        for(ucColumn = 0; ucColumn < 128; ucColumn++)
        {
            LED_WrDat(ucData);
        }
    }
} 

void SetStartColumn(uint8_t ucData)
{
    LED_WrCmd(0x00+ucData % 16);   // Set Lower Column Start Address for Page Addressing Mode
                                   // Default => 0x00
    LED_WrCmd(0x10+ucData / 16);   // Set Higher Column Start Address for Page Addressing Mode
                                   // Default => 0x10
}

void SetAddressingMode(uint8_t ucData)
{
    LED_WrCmd(0x20);        // Set Memory Addressing Mode
    LED_WrCmd(ucData);      // Default => 0x02
                            // 0x00 => Horizontal Addressing Mode
                            // 0x01 => Vertical Addressing Mode
                            // 0x02 => Page Addressing Mode
}

void SetColumnAddress(uint8_t a, uint8_t b)
{
    LED_WrCmd(0x21);        // Set Column Address
    LED_WrCmd(a);           // Default => 0x00 (Column Start Address)
    LED_WrCmd(b);           // Default => 0x7F (Column End Address)
}

void SetPageAddress(uint8_t a, uint8_t b)
{
    LED_WrCmd(0x22);        // Set Page Address
    LED_WrCmd(a);           // Default => 0x00 (Page Start Address)
    LED_WrCmd(b);           // Default => 0x07 (Page End Address)
}

void SetStartLine(uint8_t ucData)
{
    LED_WrCmd(0x40|ucData); // Set Display Start Line
                            // Default => 0x40 (0x00)
}

void SetContrastControl(uint8_t ucData)
{
    LED_WrCmd(0x81);        // Set Contrast Control
    LED_WrCmd(ucData);      // Default => 0x7F
}

void SetChargePump(uint8_t ucData)
{
    LED_WrCmd(0x8D);        // Set Charge Pump
    LED_WrCmd(0x10|ucData); // Default => 0x10
                            // 0x10 (0x00) => Disable Charge Pump
                            // 0x14 (0x04) => Enable Charge Pump
}

void SetSegmentRemap(uint8_t ucData)
{
    LED_WrCmd(0xA0|ucData); // Set Segment Re-Map
                            // Default => 0xA0
                            // 0xA0 (0x00) => Column Address 0 Mapped to SEG0
                            // 0xA1 (0x01) => Column Address 0 Mapped to SEG127
}

void SetEntireDisplay(uint8_t ucData)
{
    LED_WrCmd(0xA4|ucData); // Set Entire Display On / Off
                            // Default => 0xA4
                            // 0xA4 (0x00) => Normal Display
                            // 0xA5 (0x01) => Entire Display On
}

void SetInverseDisplay(uint8_t ucData)
{
    LED_WrCmd(0xA6|ucData); // Set Inverse Display On/Off
                            // Default => 0xA6
                            // 0xA6 (0x00) => Normal Display
                            // 0xA7 (0x01) => Inverse Display On
}

void SetMultiplexRatio(uint8_t ucData)
{
    LED_WrCmd(0xA8);        // Set Multiplex Ratio
    LED_WrCmd(ucData);      // Default => 0x3F (1/64 Duty)
}

void SetDisplayOnOff(uint8_t ucData)
{
    LED_WrCmd(0xAE|ucData); // Set Display On/Off
                            // Default => 0xAE
                            // 0xAE (0x00) => Display Off
                            // 0xAF (0x01) => Display On
}

void SetStartPage(uint8_t ucData)
{
    LED_WrCmd(0xB0|ucData); // Set Page Start Address for Page Addressing Mode
                            // Default => 0xB0 (0x00)
}

void SetCommonRemap(uint8_t ucData)
{
    LED_WrCmd(0xC0|ucData); // Set COM Output Scan Direction
                            // Default => 0xC0
                            // 0xC0 (0x00) => Scan from COM0 to 63
                            // 0xC8 (0x08) => Scan from COM63 to 0
}

void SetDisplayOffset(uint8_t ucData)
{
    LED_WrCmd(0xD3);        // Set Display Offset
    LED_WrCmd(ucData);      // Default => 0x00
}

void SetDisplayClock(uint8_t ucData)
{
    LED_WrCmd(0xD5);        // Set Display Clock Divide Ratio / Oscillator Frequency
    LED_WrCmd(ucData);      // Default => 0x80
                            // D[3:0] => Display Clock Divider
                            // D[7:4] => Oscillator Frequency
}

void SetPrechargePeriod(uint8_t ucData)
{
    LED_WrCmd(0xD9);        // Set Pre-Charge Period
    LED_WrCmd(ucData);      // Default => 0x22 (2 Display Clocks [Phase 2] / 2 Display Clocks [Phase 1])
                            // D[3:0] => Phase 1 Period in 1~15 Display Clocks
                            // D[7:4] => Phase 2 Period in 1~15 Display Clocks
}

void SetCommonConfig(uint8_t ucData)
{
    LED_WrCmd(0xDA);        // Set COM Pins Hardware Configuration
    LED_WrCmd(0x02|ucData); // Default => 0x12 (0x10)
                            // Alternative COM Pin Configuration
                            // Disable COM Left/Right Re-Map
}

void SetVCOMH(uint8_t ucData)
{
    LED_WrCmd(0xDB);        // Set VCOMH Deselect Level
    LED_WrCmd(ucData);      // Default => 0x20 (0.77*VCC)
}

void SetNop(void)
{
    LED_WrCmd(0xE3);        // Command for No Operation
}

void LED_Init() {
	delay_ms(200);

	DISPLAY_RST = 0;
	delay_ms(200);
	DISPLAY_RST = 1;


	hal_spi_master_init(SPI_CLK_DIV2, HAL_SPI_MODE_0, HAL_SPI_MSB_LSB);

    SetDisplayOnOff(0x00);     // Display Off (0x00/0x01)
    SetDisplayClock(0x80);     // Set Clock as 100 Frames/Sec
    SetMultiplexRatio(0x3F);   // 1/64 Duty (0x0F~0x3F)
    SetDisplayOffset(0x00);    // Shift Mapping RAM Counter (0x00~0x3F)
    SetStartLine(0x00);        // Set Mapping RAM Display Start Line (0x00~0x3F)
    SetChargePump(0x04);       // Enable Embedded DC/DC Converter (0x00/0x04)
    SetAddressingMode(0x02);   // Set Page Addressing Mode (0x00/0x01/0x02)
    SetSegmentRemap(0x01);     // Set SEG/Column Mapping     0x00左右反置 0x01正常
    SetCommonRemap(0x08);      // Set COM/Row Scan Direction 0x00上下反置 0x08正常
    SetCommonConfig(0x10);     // Set Sequential Configuration (0x00/0x10)
    SetContrastControl(0xCF);  // Set SEG Output Current
    SetPrechargePeriod(0xF1);  // Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    SetVCOMH(0x40);            // Set VCOM Deselect Level
    SetEntireDisplay(0x00);    // Disable Entire Display On (0x00/0x01)
    SetInverseDisplay(0x00);   // Disable Inverse Display On (0x00/0x01)  
    SetDisplayOnOff(0x01);     // Display On (0x00/0x01)
    LED_Fill(0x00);            // 初始清屏
    LED_SetPos(0,0);	
}

/*****************************************************************************
 函 数 名  : LED_P6x8Char
 功能描述  : 显示一个6x8标准ASCII字符
 输入参数  : uint8_t ucIdxX  显示的横坐标0~122
             uint8_t ucIdxY  页范围0～7
             uint8_t ucData  显示的字符
 输出参数  : NONE
 返 回 值  : NONE
*****************************************************************************/
void LED_P6x8Char(uint8_t ucIdxX, uint8_t ucIdxY, uint8_t ucData)
{
    uint8_t i, ucDataTmp;     
       
    ucDataTmp = ucData-32;
    if(ucIdxX > 122)
    {
        ucIdxX = 0;
        ucIdxY++;
    }
    
    LED_SetPos(ucIdxX, ucIdxY);
    
    for(i = 0; i < 6; i++)
    {     
        LED_WrDat(F6x8[ucDataTmp][i]);  
    }
}

/*****************************************************************************
 函 数 名  : LED_P6x8Str
 功能描述  : 写入一组6x8标准ASCII字符串
 输入参数  : uint8_t ucIdxX       显示的横坐标0~122
             uint8_t ucIdxY       页范围0～7
             uint8_t ucDataStr[]  显示的字符串
 输出参数  : NONE
 返 回 值  : NONE
*****************************************************************************/
void LED_P6x8Str(uint8_t ucIdxX, uint8_t ucIdxY, uint8_t ucDataStr[])
{
    uint8_t i, j, ucDataTmp; 

    for (j = 0; ucDataStr[j] != '\0'; j++)
    {    
        ucDataTmp = ucDataStr[j] - 32;
        if(ucIdxX > 122)
        {
            ucIdxX = 0;
            ucIdxY++;
        }
        
        LED_SetPos(ucIdxX,ucIdxY); 
        
        for(i = 0; i < 6; i++)
        {     
            LED_WrDat(F6x8[ucDataTmp][i]);  
        }
        ucIdxX += 6;
    }

    return;
}

/*****************************************************************************
 函 数 名  : LED_P8x16Str
 功能描述  : 写入一组8x16标准ASCII字符串
 输入参数  : uint8_t ucIdxX       为显示的横坐标0~120
             uint8_t ucIdxY       为页范围0～3
             uint8_t ucDataStr[]  要显示的字符串
 输出参数  : NONE
 返 回 值  : NONE
*****************************************************************************/
void LED_P8x16Str(uint8_t ucIdxX, uint8_t ucIdxY, uint8_t ucDataStr[])
{
    uint8_t i, j, ucDataTmp;

    ucIdxY <<= 1;
    
    for (j = 0; ucDataStr[j] != '\0'; j++)
    {    
        ucDataTmp = ucDataStr[j] - 32;
        if(ucIdxX > 120)
        {
            ucIdxX = 0;
            ucIdxY += 2;
        }
        LED_SetPos(ucIdxX, ucIdxY);   
        
        for(i = 0; i < 8; i++) 
        {
            LED_WrDat(F8X16[ucDataTmp][i]);
        }
        
        LED_SetPos(ucIdxX, ucIdxY + 1);   
        
        for(i = 0; i < 8; i++) 
        {
            LED_WrDat(F8X16[ucDataTmp][i + 8]);
        }
        ucIdxX += 8;
        
    }

    return;
}

/*****************************************************************************
 函 数 名  : LED_PrintChar
 功能描述  : 将一个Char型数转换成3位数进行显示
 输入参数  : uint8_t ucIdxX    ucIdxX的范围为0～122
            uint8_t ucIdxY    ucIdxY的范围为0～7
            int8_t cData      cData为需要转化显示的数值 -128~127
 输出参数  : none
 返 回 值  : none
*****************************************************************************/
void LED_PrintChar(uint8_t ucIdxX, uint8_t ucIdxY, int8_t cData)
{
    uint16_t i, j, k, usData;

    if(cData < 0)
    {
        LED_P6x8Char(ucIdxX, ucIdxY, '-');
        usData = (uint16_t)(-cData);  
    }
    else
    {
        LED_P6x8Char(ucIdxX, ucIdxY, '+');
        usData = (uint16_t)cData;
    }
    i = usData / 100;
    j = (usData % 100) / 10;
    k = usData % 10;
    
    LED_P6x8Char(ucIdxX+6, ucIdxY, i+48);
    LED_P6x8Char(ucIdxX+12, ucIdxY, j+48);
    LED_P6x8Char(ucIdxX+18, ucIdxY, k+48);   

    return;
}

/*****************************************************************************
 函 数 名  : LED_PrintShort
 功能描述  : 将一个Short型数转换成5位数进行显示
 输入参数  : uint8_t ucIdxX ucIdxX的范围为0～120
             uint8_t ucIdxY ucIdxY为页的范围0～7
             int16_t sData  sData为需要转化显示的数值
 输出参数  : none
 返 回 值  : none
*****************************************************************************/
void LED_PrintShort(uint8_t ucIdxX, uint8_t ucIdxY, int16_t sData)
{
    uint16_t i, j, k, l, m, usData;  
    if(sData < 0)
    {
        LED_P6x8Char(ucIdxX,ucIdxY,'-');
        usData = (uint16_t)(-sData);  
    }
    else
    {
        LED_P6x8Char(ucIdxX,ucIdxY,'+');
        usData = (uint16_t)sData;
    }
    
    l = usData / 10000;
    m = (usData % 10000) /1000;
    i = (usData % 1000) / 100;
    j = (usData % 100) / 10;
    k = usData % 10;
    LED_P6x8Char(ucIdxX+6, ucIdxY, l+48);
    LED_P6x8Char(ucIdxX+12, ucIdxY, m+48);
    LED_P6x8Char(ucIdxX+18, ucIdxY, i+48);
    LED_P6x8Char(ucIdxX+24, ucIdxY, j+48);
    LED_P6x8Char(ucIdxX+30, ucIdxY, k+48);  

    return;
}

/*****************************************************************************
 函 数 名  : LED_PrintImage
 功能描述  : 将图像显示出来
 输入参数  : uint8_t *pucTable     二维图像数组的地址
             uint16_t usRowNum    二维图像的行数1~64
             uint16_t usColumnNum 二维图像的列数1~128
 输出参数  : none
 返 回 值  : none
*****************************************************************************/
void LED_PrintImage(uint8_t *pucTable, uint16_t usRowNum, uint16_t usColumnNum)
{
    uint8_t ucData;
    uint16_t i,j,k,m,n;
    uint16_t usRowTmp;

    m = usRowNum >> 3;   //计算图片行数以8位为一组完整的组数
    n = usRowNum % 8;    //计算分完组后剩下的行数
    
    for(i = 0; i < m; i++) //完整组行扫描
    {
        LED_SetPos(0,(uint8_t)i);
        usRowTmp = i << 3;    //计算当前所在行的下标                  
        for(j = 0; j < usColumnNum; j++) //列扫描        
        {
            ucData = 0;
            for(k = 0; k < 8; k++) //在i组中对这8行扫描
            {
                ucData = ucData >> 1;
                if((pucTable + (usRowTmp + k) * usColumnNum)[j] == LED_IMAGE_WHITE)
                {
                    ucData = ucData | 0x80;
                }
                
            }
            LED_WrDat(ucData);
        }
    }
    
    LED_SetPos(0,(uint8_t)i); //设置剩下的行显示的起始坐标
    usRowTmp = i << 3;       //计算当前所在行的下标                  
    for(j = 0; j < usColumnNum; j++) //列扫描        
    {
        ucData = 0;
        for(k = 0; k < n; k++) //对剩下的行扫描
        {
            ucData = ucData >> 1;
            if((pucTable + (usRowTmp + k) * usColumnNum)[j] == LED_IMAGE_WHITE)
            {
                ucData = ucData | 0x80;
            }
            
        }
        ucData = ucData >> (8 - n);
        LED_WrDat(ucData);
    }

    return;
}


void ssd1306_init() {
    LED_Init();
}
void ssd1306_puts_6x8(int row, int col, char* s) {
    LED_P6x8Str(col, row, s);
}
void ssd1306_puts_8x16(int row, int col, char* s) {
    LED_P8x16Str(col, row, s);
}

void ssd1306_clear() {
	LED_Fill(0);
}