OUTPUT_ARCH( "riscv" )
ENTRY( _entry )

MEMORY {
  SM_MEM : ORIGIN = 0x80000000, LENGTH = 14M
  SM_TRAMPOLINE : ORIGIN = 0x80EFE000, LENGTH = 4096
}

SECTIONS
{
  /*
   * ensure that entry.S / _entry is at 0x80000000,
   * where qemu's -kernel jumps.
   */
  . = 0x80000000;

  .text :
  {
    *(.text .text.*)
    PROVIDE(smtext = .);
  } > SM_MEM

  .trampoline :
  {
    _trampoline = .;
    *(trampsec)
    . = ALIGN(0x1000);
    ASSERT(. - _trampoline == 0x1000, "error: trampoline larger than one page");
    . = ALIGN(0x1000);
    PROVIDE(trapframe_begin = .);
  } > SM_TRAMPOLINE

  .rodata : {
    . = ALIGN(16);
    *(.srodata .srodata.*) /* do not need to distinguish this from .rodata */
    . = ALIGN(16);
    *(.rodata .rodata.*)
  } > SM_MEM

  .data : {
    . = ALIGN(16);
    *(.sdata .sdata.*) /* do not need to distinguish this from .data */
    . = ALIGN(16);
    *(.data .data.*)
  } > SM_MEM

  .bss : {
    . = ALIGN(16);
    *(.sbss .sbss.*) /* do not need to distinguish this from .bss */
    . = ALIGN(16);
    *(.bss .bss.*)
  } > SM_MEM

  PROVIDE(smend = .);
}
