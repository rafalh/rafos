#include "multiboot.h"

#define FLAGS (MULTIBOOT_PAGE_ALIGN|MULTIBOOT_MEMORY_INFO|MULTIBOOT_AOUT_KLUDGE)

extern char __text_begin__, __data_end__, __bss_end__;
void start(void);

struct multiboot_header mboot __attribute__ ((section (".mboot"))) = {
	MULTIBOOT_HEADER_MAGIC,
	FLAGS,
	-(MULTIBOOT_HEADER_MAGIC + FLAGS),
	(multiboot_uint32_t) &mboot,
	(multiboot_uint32_t) &__text_begin__,
	(multiboot_uint32_t) &__data_end__,
	(multiboot_uint32_t) &__bss_end__,
	(multiboot_uint32_t) start,
	0
};
