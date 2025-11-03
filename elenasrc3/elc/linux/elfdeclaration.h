//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains Nt Linker declaration for cross-compile
//                                              (C)2025, by Aleksey Rakov
//---------------------------------------------------------------------------

/* This file defines standard ELF types, structures, and macros.
   Copyright (C) 1995-2018 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#ifndef ELFDECLARATION_H
#define ELFDECLARATION_H

namespace elena_lang
{

#if (defined(_WIN32) || defined(__WIN32__))

   #define DT_NULL		0		/* Marks end of dynamic section */
   #define DT_NEEDED	1		/* Name of needed library */
   #define DT_PLTRELSZ	2		/* Size in bytes of PLT relocs */
   #define DT_PLTGOT	3		/* Processor defined value */
   #define DT_HASH		4		/* Address of symbol hash table */
   #define DT_STRTAB	5		/* Address of string table */
   #define DT_SYMTAB	6		/* Address of symbol table */
   #define DT_RELA		7		/* Address of Rela relocs */
   #define DT_RELASZ	8		/* Total size of Rela relocs */
   #define DT_RELAENT	9		/* Size of one Rela reloc */
   #define DT_STRSZ	10		/* Size of string table */
   #define DT_SYMENT	11		/* Size of one symbol table entry */
   #define DT_INIT		12		/* Address of init function */
   #define DT_FINI		13		/* Address of termination function */
   #define DT_SONAME	14		/* Name of shared object */
   #define DT_RPATH	15		/* Library search path (deprecated) */
   #define DT_SYMBOLIC	16		/* Start symbol search here */
   #define DT_REL		17		/* Address of Rel relocs */
   #define DT_RELSZ	18		/* Total size of Rel relocs */
   #define DT_RELENT	19		/* Size of one Rel reloc */
   #define DT_PLTREL	20		/* Type of reloc in PLT */
   #define DT_DEBUG	21		/* For debugging; unspecified */
   #define DT_TEXTREL	22		/* Reloc might modify .text */
   #define DT_JMPREL	23		/* Address of PLT relocs */
   #define	DT_BIND_NOW	24		/* Process relocations of object */
   #define	DT_INIT_ARRAY	25		/* Array with addresses of init fct */
   #define	DT_FINI_ARRAY	26		/* Array with addresses of fini fct */
   #define	DT_INIT_ARRAYSZ	27		/* Size in bytes of DT_INIT_ARRAY */
   #define	DT_FINI_ARRAYSZ	28		/* Size in bytes of DT_FINI_ARRAY */
   #define DT_RUNPATH	29		/* Library search path */
   #define DT_FLAGS	30		/* Flags for the object being loaded */
   #define DT_ENCODING	32		/* Start of encoded range */
   #define DT_PREINIT_ARRAY 32		/* Array with addresses of preinit fct*/
   #define DT_PREINIT_ARRAYSZ 33		/* size in bytes of DT_PREINIT_ARRAY */
   #define DT_SYMTAB_SHNDX	34		/* Address of SYMTAB_SHNDX section */
   #define	DT_NUM		35		/* Number used */
   #define DT_LOOS		0x6000000d	/* Start of OS-specific */
   #define DT_HIOS		0x6ffff000	/* End of OS-specific */
   #define DT_LOPROC	0x70000000	/* Start of processor-specific */
   #define DT_HIPROC	0x7fffffff	/* End of processor-specific */
   #define	DT_PROCNUM	DT_MIPS_NUM	/* Most used by any processor */

   #define	PT_NULL		0		/* Program header table entry unused */
   #define PT_LOAD		1		/* Loadable program segment */
   #define PT_DYNAMIC	2		/* Dynamic linking information */
   #define PT_INTERP	3		/* Program interpreter */
   #define PT_NOTE		4		/* Auxiliary information */
   #define PT_SHLIB	5		/* Reserved */
   #define PT_PHDR		6		/* Entry for header table itself */
   #define PT_TLS		7		/* Thread-local storage segment */
   #define	PT_NUM		8		/* Number of defined types */
   #define PT_LOOS		0x60000000	/* Start of OS-specific */
   #define PT_GNU_EH_FRAME	0x6474e550	/* GCC .eh_frame_hdr segment */
   #define PT_GNU_STACK	0x6474e551	/* Indicates stack executability */
   #define PT_GNU_RELRO	0x6474e552	/* Read-only after relocation */
   #define PT_LOSUNW	0x6ffffffa
   #define PT_SUNWBSS	0x6ffffffa	/* Sun Specific segment */
   #define PT_SUNWSTACK	0x6ffffffb	/* Stack segment */
   #define PT_HISUNW	0x6fffffff
   #define PT_HIOS		0x6fffffff	/* End of OS-specific */
   #define PT_LOPROC	0x70000000	/* Start of processor-specific */
   #define PT_HIPROC	0x7fffffff	/* End of processor-specific */

   #define R_386_JMP_SLOT	   7		/* Create PLT entry */

   #define R_X86_64_JUMP_SLOT	7	/* Create PLT entry */

   #define STN_UNDEF	0		/* End of a chain.  */
   #define SHN_UNDEF	0		/* Undefined section */

   #define EI_NIDENT (16)

   #define PF_X		(1 << 0)	/* Segment is executable */
   #define PF_W		(1 << 1)	/* Segment is writable */
   #define PF_R		(1 << 2)	/* Segment is readable */
   #define PF_MASKOS	0x0ff00000	/* OS-specific */
   #define PF_MASKPROC	0xf0000000	/* Processor-specific */

   #define EI_CLASS	4		/* File class byte index */
   #define ELFCLASSNONE	0		/* Invalid class */
   #define ELFCLASS32	1		/* 32-bit objects */
   #define ELFCLASS64	2		/* 64-bit objects */
   #define ELFCLASSNUM	3

   #define EI_DATA		5		/* Data encoding byte index */
   #define ELFDATANONE	0		/* Invalid data encoding */
   #define ELFDATA2LSB	1		/* 2's complement, little endian */
   #define ELFDATA2MSB	2		/* 2's complement, big endian */
   #define ELFDATANUM	3

   #define EI_VERSION	6		/* File version byte index */
      /* Value must be EV_CURRENT */

   #define EI_OSABI	7		/* OS ABI identification */
   #define ELFOSABI_NONE		0	/* UNIX System V ABI */
   #define ELFOSABI_SYSV		0	/* Alias.  */
   #define ELFOSABI_HPUX		1	/* HP-UX */
   #define ELFOSABI_NETBSD		2	/* NetBSD.  */
   #define ELFOSABI_GNU		3	/* Object uses GNU ELF extensions.  */
   #define ELFOSABI_LINUX		ELFOSABI_GNU /* Compatibility alias.  */
   #define ELFOSABI_SOLARIS	6	/* Sun Solaris.  */
   #define ELFOSABI_AIX		7	/* IBM AIX.  */
   #define ELFOSABI_IRIX		8	/* SGI Irix.  */
   #define ELFOSABI_FREEBSD	9	/* FreeBSD.  */
   #define ELFOSABI_TRU64		10	/* Compaq TRU64 UNIX.  */
   #define ELFOSABI_MODESTO	11	/* Novell Modesto.  */
   #define ELFOSABI_OPENBSD	12	/* OpenBSD.  */
   #define ELFOSABI_ARM_AEABI	64	/* ARM EABI */
   #define ELFOSABI_ARM		97	/* ARM */
   #define ELFOSABI_STANDALONE	255	/* Standalone (embedded) application */

   #define EI_ABIVERSION	8		/* ABI version */

   #define EI_PAD		9		/* Byte index of padding bytes */

   #define EV_CURRENT	1		/* Current version */

   #define EI_VERSION	6		/* File version byte index */
                              /* Value must be EV_CURRENT */

   #define ET_EXEC		2		/* Executable file */

   #define EM_386		 3	/* Intel 80386 */
   #define EM_X86_64	62	/* AMD x86-64 architecture */

   /* Types for signed and unsigned 32-bit quantities.  */
   typedef uint32_t Elf32_Word;
   typedef	int32_t  Elf32_Sword;
   typedef uint32_t Elf64_Word;
   typedef	int32_t  Elf64_Sword;

   typedef uint16_t Elf32_Half;
   typedef uint16_t Elf64_Half;

   typedef uint32_t Elf32_Addr;
   typedef uint64_t Elf64_Addr;

   typedef uint32_t Elf32_Off;
   typedef uint64_t Elf64_Off;

   typedef uint64_t Elf32_Xword;
   typedef	int64_t  Elf32_Sxword;
   typedef uint64_t Elf64_Xword;
   typedef	int64_t  Elf64_Sxword;

#pragma pack(push, 1)

   typedef struct
   {
      unsigned char	e_ident[EI_NIDENT];	/* Magic number and other info */
      Elf32_Half	e_type;			/* Object file type */
      Elf32_Half	e_machine;		/* Architecture */
      Elf32_Word	e_version;		/* Object file version */
      Elf32_Addr	e_entry;		/* Entry point virtual address */
      Elf32_Off	e_phoff;		/* Program header table file offset */
      Elf32_Off	e_shoff;		/* Section header table file offset */
      Elf32_Word	e_flags;		/* Processor-specific flags */
      Elf32_Half	e_ehsize;		/* ELF header size in bytes */
      Elf32_Half	e_phentsize;		/* Program header table entry size */
      Elf32_Half	e_phnum;		/* Program header table entry count */
      Elf32_Half	e_shentsize;		/* Section header table entry size */
      Elf32_Half	e_shnum;		/* Section header table entry count */
      Elf32_Half	e_shstrndx;		/* Section header string table index */
   } Elf32_Ehdr;

   typedef struct
   {
      unsigned char	e_ident[EI_NIDENT];	/* Magic number and other info */
      Elf64_Half	e_type;			/* Object file type */
      Elf64_Half	e_machine;		/* Architecture */
      Elf64_Word	e_version;		/* Object file version */
      Elf64_Addr	e_entry;		/* Entry point virtual address */
      Elf64_Off	e_phoff;		/* Program header table file offset */
      Elf64_Off	e_shoff;		/* Section header table file offset */
      Elf64_Word	e_flags;		/* Processor-specific flags */
      Elf64_Half	e_ehsize;		/* ELF header size in bytes */
      Elf64_Half	e_phentsize;		/* Program header table entry size */
      Elf64_Half	e_phnum;		/* Program header table entry count */
      Elf64_Half	e_shentsize;		/* Section header table entry size */
      Elf64_Half	e_shnum;		/* Section header table entry count */
      Elf64_Half	e_shstrndx;		/* Section header string table index */
   } Elf64_Ehdr;

   typedef struct
   {
      Elf32_Word	p_type;			/* Segment type */
      Elf32_Off	p_offset;		/* Segment file offset */
      Elf32_Addr	p_vaddr;		/* Segment virtual address */
      Elf32_Addr	p_paddr;		/* Segment physical address */
      Elf32_Word	p_filesz;		/* Segment size in file */
      Elf32_Word	p_memsz;		/* Segment size in memory */
      Elf32_Word	p_flags;		/* Segment flags */
      Elf32_Word	p_align;		/* Segment alignment */
   } Elf32_Phdr;

   typedef struct
   {
      Elf64_Word	p_type;			/* Segment type */
      Elf64_Word	p_flags;		/* Segment flags */
      Elf64_Off	p_offset;		/* Segment file offset */
      Elf64_Addr	p_vaddr;		/* Segment virtual address */
      Elf64_Addr	p_paddr;		/* Segment physical address */
      Elf64_Xword	p_filesz;		/* Segment size in file */
      Elf64_Xword	p_memsz;		/* Segment size in memory */
      Elf64_Xword	p_align;		/* Segment alignment */
   } Elf64_Phdr;

#pragma pack(pop)

#endif

} // elena_lang

#endif // ELFDECLARATION_H
