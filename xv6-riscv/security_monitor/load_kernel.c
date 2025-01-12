#include "kernel/types.h"
#include "kernel/memlayout.h"
#include "elf.h"
#include "kernel/riscv.h"
#include "kernel/defs.h"

struct elfhdr *kernel_elfhdr;
struct proghdr *kernel_phdr;
struct secthdr *kernel_secthdr;
struct symtab *kernel_symtab;
struct symtab1 *kernel_symtab1;

const char* getNameFromStringTable (uint64 offset) {
    kernel_elfhdr = (struct elfhdr *)RAMDISK;
    for (int i = 0; i < kernel_elfhdr->shnum; i++)
    {
        kernel_secthdr = (struct secthdr *)((char *)kernel_elfhdr + kernel_elfhdr->shoff + i * kernel_elfhdr->shentsize);
        if (kernel_secthdr->type == 3)
        {
            return (char *)(((char *)kernel_elfhdr + kernel_secthdr->offset) + offset);
        }
    }

    return "Not Found";
}

void* ExtractKernelSectionsInfo(const char* symbolName)
{
    kernel_elfhdr = (struct elfhdr *)RAMDISK;
    for (int i = 0; i < kernel_elfhdr->shnum; i++)
    {
        kernel_secthdr = (struct secthdr *)((char *)kernel_elfhdr + kernel_elfhdr->shoff + i * kernel_elfhdr->shentsize);
        if (kernel_secthdr->type == 2) {
            kernel_symtab1 = (struct symtab1 *)((char *)kernel_elfhdr + kernel_secthdr->offset);
            int symbol_count = kernel_secthdr->size / kernel_secthdr->entsize;
            for (int j = 0; j < symbol_count; j++)
            {
                // printf("Name: %s\n", (char *)kernel_elfhdr + kernel_symtab1->st_name);
                if (strncmp(getNameFromStringTable(kernel_symtab1->st_name), symbolName, strlen(symbolName)) == 0)
                {
                    return (void *)kernel_symtab1->st_value;
                }
                kernel_symtab1++;
            }
        }
    }
    for (;;)
        ;
    return 0;
}

uint64 loadKernel()
{
    // Load the kernel into memory

    kernel_elfhdr = (struct elfhdr *)RAMDISK;
    kernel_phdr = (struct proghdr *)((char *)kernel_elfhdr + kernel_elfhdr->phoff + kernel_elfhdr->phentsize);
    //uint64 kernel_load_addr = kernel_phdr->vaddr;

    for (int i = 0; i < kernel_elfhdr->phnum; i++)
    {
        kernel_phdr = (struct proghdr *)((char *)kernel_elfhdr + kernel_elfhdr->phoff + i * kernel_elfhdr->phentsize);
        if (kernel_phdr->type == ELF_PROG_LOAD)
        {
            void *dest = (void *)(kernel_phdr->vaddr);
            void *src = (void *)(RAMDISK + kernel_phdr->off);

            memmove(dest, src, kernel_phdr->filesz);

            // Zero out the remaining memory if memsz > filesz
            if (kernel_phdr->memsz > kernel_phdr->filesz)
            {
                memset(((char *)dest + kernel_phdr->filesz), 0, kernel_phdr->memsz - kernel_phdr->filesz);
            }

            if (kernel_phdr->flags & ELF_PROG_FLAG_WRITE)
            {
                void* kend = ExtractKernelSectionsInfo("etext");
                printf("Setting PMP for kernel Code Pages %p to %p\n", dest, (char *)kend);
                // Set the PMP to allow write access to the kernel
                w_pmpaddr3((uint64)dest >> 2);
                w_pmpaddr4((uint64)(kend) >> 2);
                w_pmpaddr5(0x3fffffffffffffull);
                w_pmpcfg0(r_pmpcfg0() | (((uint64)0x0f0c0f) << 24));
            }
        }
    }

    return kernel_elfhdr->entry;
}