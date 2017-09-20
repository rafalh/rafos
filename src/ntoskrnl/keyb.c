#include "keyb.h"
#include "intr.h"
#include "irq.h"
#include "stdio.h"
#include "io.h"
#include "main.h"
#include "ctype.h"

void irq1();

unsigned char ScanCodeToVK[0x100] = {
//  0      1      2      3      4      5      6      7      8      9      A      B      C      D      E      F
	0,     VK_ESCAPE,'1','2',   '3',   '4',   '5',   '6',   '7',   '8',   '9',   '0',   VK_OEM_MINUS,VK_OEM_PLUS,VK_BACK,VK_TAB,               /* 0 */
	'Q',   'W',   'E',   'R',   'T',   'Y',   'U',   'I',   'O',   'P',   VK_OEM_4,VK_OEM_6,VK_RETURN,VK_LCONTROL,'A','S',                     /* 1 */
	'D',   'F',   'G',   'H',   'J',   'K',   'L',   VK_OEM_1,VK_OEM_7,VK_OEM_3,VK_LSHIFT,VK_OEM_5,'Z','X','C','V',                            /* 2 */
	'B',   'N',   'M',   VK_OEM_COMMA,VK_OEM_PERIOD,VK_OEM_2,VK_RSHIFT,VK_MULTIPLY,VK_LMENU,VK_SPACE,VK_CAPITAL,VK_F1,VK_F2,VK_F3,VK_F4,VK_F5, /* 3 */
	VK_F6, VK_F7, VK_F8, VK_F9, VK_F10,VK_NUMLOCK,VK_SCROLL,VK_NUMPAD7,VK_NUMPAD8,VK_NUMPAD9,VK_SUBTRACT,VK_NUMPAD4,VK_NUMPAD5,VK_NUMPAD6,VK_ADD,VK_NUMPAD1, /* 4 */
	VK_NUMPAD2,VK_NUMPAD3,VK_NUMPAD0,VK_DECIMAL,0,0,0,VK_F11,VK_F12,0,    0,     0,     0,     0,     0,     0,                                /* 5 */
	0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,                                /* 6 */
	0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,                                /* 7 */
/********************************************************************* Extended *********************************************************************/
	0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,                                /* 0 */
	0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     VK_RETURN,VK_RCONTROL,0,0,                             /* 1 */
	0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,                                /* 2 */
	0,     0,     0,     0,     0,     VK_DIVIDE,0,  0,     VK_RMENU,0,   0,     0,     0,     0,     0,     0,                                /* 3 */
	0,     0,     0,     0,     0,     0,     0,     VK_HOME,VK_UP,VK_PRIOR,0,   VK_LEFT,0,    VK_RIGHT,0,   VK_END,                           /* 4 */
	VK_DOWN,VK_NEXT,VK_INSERT,VK_DELETE,0,0,  0,     0,     0,     0,     0,     VK_LWIN,VK_RWIN,VK_APPS,0,  VK_SLEEP,                         /* 5 */
	0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,                                /* 6 */
	0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,                                /* 7 */
};

unsigned char NumpadUnlockedVK[11] = {
	VK_INSERT,
	VK_END, VK_DOWN, VK_NEXT,
	VK_LEFT, 0, VK_RIGHT,
	VK_HOME, VK_UP, VK_PRIOR,
	VK_DELETE
};

//  0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
unsigned VKToChar[0x100] = {
	0,   0,   0,   0,   0,   0,   0,   0,   '\b','\t',0,   0,   0,   '\n',0,   0,   /* 0 */
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   /* 1 */
	' ', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   /* 2 */
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 0,   0,   0,   0,   0,   0,   /* 3 */
	0,   'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', /* 4 */
	'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 0,   0,   0,   0,   0,   /* 5 */
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '*', '+', 0,   '-', '.', '/', /* 6 */
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   /* 7 */
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   /* 8 */
	0,   0,   '=', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   /* 9 */
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   /* A */
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   ';', '=', ',', '-', '.', '/', /* B */
	'`', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   /* C */
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   '[', '\\',']', '\'',0    /* D */
};

unsigned VKToCharShift[0x100] = {
	0,   0,   0,   0,   0,   0,   0,   0,   '\b','\t',0,   0,   0,   '\n',0,   0,   /* 0 */
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   /* 1 */
	' ', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   /* 2 */
	')', '!', '@', '#', '$', '%', '^', '&', '*', '(', 0,   0,   0,   0,   0,   0,   /* 3 */
	0,   'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', /* 4 */
	'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 0,   0,   0,   0,   0,   /* 5 */
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '*', '+', 0,   '-', '.', '/', /* 6 */
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   /* 7 */
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   /* 8 */
	0,   0,   '=', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   /* 9 */
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   /* A */
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   ':', '+', '<', '_', '>', '?', /* B */
	'~', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   /* C */
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   '{', '|', '}', '"', 0    /* D */
};

struct iobuf_t InBuffer = {0, 0};
static unsigned kbd_status = 0;
static BOOL kbd_extkey = FALSE;

void input_add(char c)
{
	if(InBuffer.i+1==InBuffer.o)
		return;
	InBuffer.buf[InBuffer.i] = c;
	InBuffer.i = (InBuffer.i+1)%64;
}

char input_get()
{
	char c;
	if(InBuffer.i==InBuffer.o)
		return '\0';
	c = InBuffer.buf[InBuffer.o];
	InBuffer.o = (InBuffer.o+1)%64;
	return c;
}

void kbd_leds()
{
	unsigned char b;
	do { inportb(0x64, b); } while((b&2)!=0); //loop until zero
	outportb(0x60, 0xED);
	do { inportb(0x64, b); } while((b&2)!=0); //loop until zero

	b = ((kbd_status&MK_CAPS)?4:0)|((kbd_status&MK_NUMLOCK)?2:0)|((kbd_status&MK_SCROLL)?1:0);
	outportb(0x60, b);
}

void keyb_irq()
{
	unsigned char key, vk;
	char c;
	BOOL bBreak = FALSE;

	inportb(0x60, key);
	if(key==0xe0)
	{
		kbd_extkey = TRUE;
		return;
	}
	if(key>0x80)
	{
		bBreak = TRUE;
		key &= 0x7f;
	}
	if(!kbd_extkey) vk = ScanCodeToVK[key];
	else vk = ScanCodeToVK[0x80|key];

	kbd_extkey = FALSE;

	if(!(kbd_status&MK_NUMLOCK) && ((vk>=VK_NUMPAD0 && vk<=VK_NUMPAD9) || vk==VK_DECIMAL))
	{
		if(vk==VK_DECIMAL)
			vk = NumpadUnlockedVK[10];
		else vk = NumpadUnlockedVK[vk-VK_NUMPAD0];
	}

	if(!vk)
		return;

	if(bBreak) /* Button Up */
	{
		if(vk == VK_MENU) {
			kbd_status &= ~MK_ALT;
			return;
		} else if(vk == VK_LCONTROL || vk == VK_RCONTROL) {
			kbd_status &= ~MK_CONTROL;
			return;
		} else if(vk == VK_LSHIFT || vk == VK_RSHIFT) {
			kbd_status &= ~MK_SHIFT;
			return;
		}
		return;
	} else { /* Button Down */
		if(vk == VK_MENU) {
			kbd_status |= MK_ALT;
			return;
		} else if(vk == VK_LCONTROL || vk == VK_RCONTROL) {
			kbd_status |= MK_CONTROL;
			return;
		} else if(vk == VK_LSHIFT || vk == VK_RSHIFT) {
			kbd_status |= MK_SHIFT;
			return;
		} else if(vk == VK_CAPITAL) {
			kbd_status ^= MK_CAPS;
			kbd_leds();
			return;
		} else if(vk == VK_NUMLOCK) {
			kbd_status ^= MK_NUMLOCK;
			kbd_leds();
			return;
		} else if(vk == VK_SCROLL) {
			kbd_status ^= MK_SCROLL;
			kbd_leds();
			return;
		}
	}

	if(kbd_status&MK_SHIFT) c = (kbd_status&MK_CAPS)?tolower(VKToCharShift[vk]):(VKToCharShift[vk]);
	else c = (kbd_status&MK_CAPS)?toupper(VKToChar[vk]):(VKToChar[vk]);
	if(c)
		input_add(c);
}

void keyb_init()
{
	set_intr_gate(0x21, &irq1);
	enable_irq(1);
}
