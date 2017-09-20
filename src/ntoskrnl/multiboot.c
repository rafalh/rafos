#include "multiboot.h"

#define FLAGS (MULTIBOOT_PAGE_ALIGN|MULTIBOOT_MEMORY_INFO|MULTIBOOT_AOUT_KLUDGE)

// void *___text_begin__, *___data_end__, *___bss_end__;
// void start(void);

// struct multiboot_header mboot __attribute__ ((section (".mboot"))) = {
// 	MULTIBOOT_HEADER_MAGIC,
// 	FLAGS,
// 	//(~(MULTIBOOT_HEADER_MAGIC + FLAGS)) + 1,
// 	-(MULTIBOOT_HEADER_MAGIC + FLAGS),
// 	(multiboot_uint32_t) &mboot,
// 	(multiboot_uint32_t) &___text_begin__,
// 	(multiboot_uint32_t) &___data_end__,
// 	(multiboot_uint32_t) &___bss_end__,
// 	(multiboot_uint32_t) start,
// 	0
// };
