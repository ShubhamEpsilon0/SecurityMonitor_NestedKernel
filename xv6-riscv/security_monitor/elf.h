#include "kernel/types.h"

// File header
struct elfhdr {
  uint magic;  // must equal ELF_MAGIC
  uchar elf[12];
  ushort type;
  ushort machine;
  uint version;
  uint64 entry;
  uint64 phoff;
  uint64 shoff;
  uint flags;
  ushort ehsize;
  ushort phentsize;
  ushort phnum;
  ushort shentsize;
  ushort shnum;
  ushort shstrndx;
};

// Program section header
struct proghdr {
  uint32 type;
  uint32 flags;
  uint64 off;
  uint64 vaddr;
  uint64 paddr;
  uint64 filesz;
  uint64 memsz;
  uint64 align;
};

struct secthdr {
  uint32 name;
  uint32 type;
  uint64 flags;
  uint64 addr;
  uint64 offset;
  uint64 size;
  uint32 link;
  uint32 info;
  uint64 addralign;
  uint64 entsize;
};

struct symtab {
  uint32 name;
  uint32 value;
  uint32 size;
  uchar info;
  uchar other;
  uint16 shndx;
};

struct symtab1
{
  uint32 st_name;     // Offset into string table
  unsigned char st_info;  // Symbol type and binding attributes
  unsigned char st_other; // Symbol visibility
  uint16 st_shndx;    // Section header index
  uint64 st_value;    // Symbol value (address)
  uint64 st_size;    // Symbol size
};


// Values for Proghdr type
#define ELF_PROG_LOAD           1

// Flag bits for Proghdr flags
#define ELF_PROG_FLAG_EXEC      1
#define ELF_PROG_FLAG_WRITE     2
#define ELF_PROG_FLAG_READ      4
