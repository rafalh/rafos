/* The link script
 ( Note: the format should be "pe-i386" instead of
   "pei-i386" to ged rid of the "MZ" header. text
   and data sections should be close to each other. )
*/
OUTPUT_FORMAT("pei-i386")
ENTRY(_start)
SECTIONS {
   . = 0x400000;
   .text : {
      __text_begin__ = . ;
      *(.text)
      . = ALIGN(4096);
      __text_end__ = . ;
   }

   .data ADDR(.text)+SIZEOF(.text) : {
      __data_begin__ = . ;
      *(.data)
      *(.data2)
      *(.rdata)
      . = ALIGN(4096);
      __data_end__ = . ;
   }

   .bss ADDR(.data)+SIZEOF(.data) : {
      __bss_begin__ = . ;
      *(.bss)
      *(COMMON)
      . = ALIGN(4096);
      __bss_end__ = . ;
   }
   __end__ = . ;
}