/* The link script
 ( Note: the format should be "pe-i386" instead of
   "pei-i386" to ged rid of the "MZ" header. text
   and data sections should be close to each other. )
*/
OUTPUT_FORMAT("pe-i386")
ENTRY(_DllMain)
SECTIONS {

   . = 0x400000;

   ___text_begin__ = . ;
   .mboot : AT(0) {
      *(.mboot)
      . = ALIGN(4096);
   }
   .text : {
      *(.text*)
      . = ALIGN(4096);
   }
   ___text_end__ = . ;
   
   ___data_begin__ = . ;
   .data : {
      *(.data*)
      *(.rdata*)
      *(.drectve)
      *(.reloc*)
      *(.edata)
      . = ALIGN(4096);
   }
   ___data_end__ = . ;

   .bss : {
      ___bss_begin__ = . ;
      *(.bss*)
      *(COMMON)
      . = ALIGN(4096);
      ___bss_end__ = . ;
   }
   ___end__ = . ;
}