//File created by David Shafer (C) 2007


/** This is a list of constants used in the PE file format **
General File Format

MS-DOS Header (for DOS compatability)
	-0x3c - offset to PE signature

COFF Signature (4 bytes)
COFF Header (20 bytes)
COFF Optional Header (96 bytes PE / 112 bytes PE+) + 128 bytes for data directories
	-Standard Header (28 bytes PE / 24 bytes PE+)
	-Windows specific (68 bytes PE / 88 bytes PE+)
	-Data Directories (8 bytes * 16 directories)
Section Headers (40 bytes)

Section Data (variable size)
	-This is where are the actual code and variable data is
	 stored in file before being loaded into memory.  Actual
	 position and size of each section is defined in the section
	 header.

COFF Symbol Tables (18 bytes + 18 bytes per auxiliary table)

COFF String Tables (first 4 bytes show size of the string table)
	-all strings are null terminated

Other Tables

COFF Relocations [Object Only] (10 bytes each)

COFF Line Numbers (Deprecated)

COFF Symbol Table (MS Visual C++ Debug info)

*************************************************************/
//COFF Offsets
#define clean(x) if (x != NULL) delete[] x

#define COFF_HEADER_OFFSET      0x40

struct COFF_Header
{
	unsigned short Machine;//type of machine file runs on
	unsigned short NumSections;//number of sections that follow header
	unsigned int TimeDateStamp;//seconds since Jan 1 1970 that file was created
	unsigned int SymbolPointer;//obsolete, always 0
	unsigned int NumSymbols;//obsolete, always 0
	unsigned short OptionalHeaderSize;//size of optionsl header
	unsigned short Characteristics;//attribute flags for file
};

//Machine types
/*
#define IMAGE_FILE_MACHINE_UNKNOWN     0x0	//The contents of this field are assumed to be applicable to any machine type
#define IMAGE_FILE_MACHINE_AM33      0x1d3	//Matsushita AM33
#define IMAGE_FILE_MACHINE_AMD64    0x8664	//x64
#define IMAGE_FILE_MACHINE_ARM       0x1c0	//ARM little endian
#define IMAGE_FILE_MACHINE_EBC       0xebc	//EFI byte code
#define IMAGE_FILE_MACHINE_I386      0x14c	//Intel 386 or later processors and compatible processors
#define IMAGE_FILE_MACHINE_IA64      0x200	//Intel Itanium processor family
#define IMAGE_FILE_MACHINE_M32R     0x9041	//Mitsubishi M32R little endian
#define IMAGE_FILE_MACHINE_MIPS16    0x266	//MIPS16
#define IMAGE_FILE_MACHINE_MIPSFPU   0x366	//MIPS with FPU
#define IMAGE_FILE_MACHINE_MIPSFPU16 0x466	//MIPS16 with FPU
#define IMAGE_FILE_MACHINE_POWERPC   0x1f0	//Power PC little endian
#define IMAGE_FILE_MACHINE_POWERPCFP 0x1f1	//Power PC with floating point support
#define IMAGE_FILE_MACHINE_R4000     0x166	//MIPS little endian
#define IMAGE_FILE_MACHINE_SH3       0x1a2	//Hitachi SH3
#define IMAGE_FILE_MACHINE_SH3DSP    0x1a3	//Hitachi SH3 DSP
#define IMAGE_FILE_MACHINE_SH4       0x1a6	//Hitachi SH4
#define IMAGE_FILE_MACHINE_SH5       0x1a8	//Hitachi SH5
#define IMAGE_FILE_MACHINE_THUMB     0x1c2	//Thumb
#define IMAGE_FILE_MACHINE_WCEMIPSV2 0x169	//MIPS little-endian WCE v2

//Characteristics
#define IMAGE_FILE_RELOCS_STRIPPED      0x0001	//Image only, Windows CE, and Microsoft Windows NT® and later. This indicates that the file does not contain base relocations and must therefore be loaded at its preferred base address. If the base address is not available, the loader reports an error. The default behavior of the linker is to strip base relocations from executable (EXE) files.
#define IMAGE_FILE_EXECUTABLE_IMAGE     0x0002	//Image only. This indicates that the image file is valid and can be run. If this flag is not set, it indicates a linker error.
#define IMAGE_FILE_LINE_NUMS_STRIPPED   0x0004	//COFF line numbers have been removed. This flag is deprecated and should be zero.
#define IMAGE_FILE_LOCAL_SYMS_STRIPPED  0x0008	//COFF symbol table entries for local symbols have been removed. This flag is deprecated and should be zero.
#define IMAGE_FILE_AGGRESSIVE_WS_TRIM   0x0010	//Obsolete. Aggressively trim working set. This flag is deprecated for Windows 2000 and later and must be zero.
#define IMAGE_FILE_LARGE_ADDRESS_ AWARE 0x0020	//Application can handle > 2 GB addresses.
//	0x0040	This flag is reserved for future use.
#define IMAGE_FILE_BYTES_REVERSED_LO    0x0080	//Little endian: the least significant bit (LSB) precedes the most significant bit (MSB) in memory. This flag is deprecated and should be zero.
#define IMAGE_FILE_32BIT_MACHINE        0x0100	//Machine is based on a 32-bit-word architecture.
#define IMAGE_FILE_DEBUG_STRIPPED       0x0200	//Debugging information is removed from the image file.
#define IMAGE_FILE_REMOVABLE_RUN_ FROM_SWAP 0x0400	//If the image is on removable media, fully load it and copy it to the swap file.
#define IMAGE_FILE_NET_RUN_FROM_SWAP    0x0800	//If the image is on network media, fully load it and copy it to the swap file.
#define IMAGE_FILE_SYSTEM               0x1000	//The image file is a system file, not a user program.
#define IMAGE_FILE_DLL                  0x2000	//The image file is a dynamic-link library (DLL). Such files are considered executable files for almost all purposes, although they cannot be directly run.
#define IMAGE_FILE_UP_SYSTEM_ONLY       0x4000	//The file should be run only on a uniprocessor machine.
#define IMAGE_FILE_BYTES_REVERSED_HI    0x8000	//Big endian: the MSB precedes the LSB in memory. This flag is deprecated and should be zero.
//*/


struct COFF_OptionalHeaderPE32Plus
{
	unsigned short Magic;//magic number which determines type of image file
	unsigned char MajorLinkerVersion;
	unsigned char MinorLinkerVersion;
	unsigned int SizeOfCode;//size of sode section
	unsigned int SizeOfInitData;//size of initialized data section
	unsigned int SizeOfUninitData;//size of un-initialized data section
	unsigned int AddressOfEntryPoint;//Address of Point relative to image base, code starting address
	unsigned int BaseOfCode;//base address of code, all absolute addresses are based on this address
};

struct COFF_OptionalHeader: public COFF_OptionalHeaderPE32Plus
{
	unsigned int BaseOfData;//base address of data, base of data section when loaded into memory
};

struct COFF_DataDirectories
{
	_IMAGE_DATA_DIRECTORY ExportTable;//see section 6.3
	_IMAGE_DATA_DIRECTORY ImportTable;//see section 6.4
	_IMAGE_DATA_DIRECTORY ResourceTable;//see section 6.9
	_IMAGE_DATA_DIRECTORY ExceptionTable;//see section 6.5
	_IMAGE_DATA_DIRECTORY CertificateTable;//see section 5.7
	_IMAGE_DATA_DIRECTORY BaseRelocationTable;//see section 6.6
	_IMAGE_DATA_DIRECTORY Debug;//see section 6.1
	_IMAGE_DATA_DIRECTORY Architecture;//must be zero
	_IMAGE_DATA_DIRECTORY GlobalPtr;//VirtualAddress of value to store in global pointer register, Size 0
	_IMAGE_DATA_DIRECTORY TLSTable;//(Thread Local Storage) see section 6.7
	_IMAGE_DATA_DIRECTORY LoadConfigTable;//see section 6.8
	_IMAGE_DATA_DIRECTORY BoundImport;//
	_IMAGE_DATA_DIRECTORY IAT;//(Import Address Table) see section 6.4.4
	_IMAGE_DATA_DIRECTORY DelayImportTable;//see section 5.8
	_IMAGE_DATA_DIRECTORY CLRRuntimeTable;//see section 6.10
	_IMAGE_DATA_DIRECTORY reserved;
};


struct COFF_OptionalHeaderWindows: public COFF_OptionalHeader
{
	unsigned int ImageBase;//preferred address of first byte of image when loaded into memory
	unsigned int SectionAlignment;//the alignement or sectinos when loaded into memory
	unsigned int FileAlignment;//the alignment factor (in bytes) must be a power of 2, min 512
	unsigned short MajorOperatingSystemVersion;
	unsigned short MinorOperatingSystemVersion;
	unsigned short MajorImageVersion;
	unsigned short MinorImageVersion;
	unsigned short MajorSubsystemVersion;
	unsigned short MinorSubsystemVersion;
	unsigned int Win32VersionValue;//must be zero
	unsigned int SizeOfImage;//size of miage including all headers once loaded into memory, multiple of SectionAlignment
	unsigned int SizeOfHeaders;//The combined size of all headers (DOS, PE, and section) multiple of FileAlignment
	unsigned int CheckSum;//checked for dlls
	unsigned short Subsystem;//subsystem required to run image
	unsigned short DllCharacteristics;
	unsigned int SizeOfStackReserve;//maximum stack
	unsigned int SizeOfStackCommit;//size of stack to commit from start
	unsigned int SizeOfHeapReserve;
	unsigned int SizeOfHeapCommit;
	unsigned int LoaderFlags;//must be zero
	unsigned int NumberOfRvaAndSizes;//number of data-directory entries in optional header
	COFF_DataDirectories dataDir;       //data directories
};

//Magic numbers
#define MAGIC_NUMBER_PE32                0x10b
#define MAGIC_NUMBER_PE32PLUS            0x20b
#define MAGIC_NUMBER_ROM                 0x107

/*
IMAGE_SUBSYSTEM_UNKNOWN	  0	An unknown subsystem
IMAGE_SUBSYSTEM_NATIVE	  1	Device drivers and native Windows processes
IMAGE_SUBSYSTEM_WINDOWS_GUI	  2	The Windows graphical user interface (GUI) subsystem
IMAGE_SUBSYSTEM_WINDOWS_CUI	  3	The Windows character subsystem
IMAGE_SUBSYSTEM_POSIX_CUI	  7	The Posix character subsystem
IMAGE_SUBSYSTEM_WINDOWS_CE_GUI	  9	Windows CE
IMAGE_SUBSYSTEM_EFI_APPLICATION	10	An Extensible Firmware Interface (EFI) application
IMAGE_SUBSYSTEM_EFI_BOOT_ SERVICE_DRIVER	11	An EFI driver with boot services
IMAGE_SUBSYSTEM_EFI_RUNTIME_ DRIVER	12	An EFI driver with run-time services
IMAGE_SUBSYSTEM_EFI_ROM	13	An EFI ROM image
IMAGE_SUBSYSTEM_XBOX	14	XBOX
//*/

/*********** declared by Windows *************
struct _IMAGE_DATA_DIRECTORY
{
	DWORD VirtualAddress;
	DWORD Size;
} IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;
*********************************************/


struct COFF_SectionHeader
{
	char Name[8];
	unsigned int VirtualSize;//total size of section when loaded into memory
	unsigned int VirtualAddress;//address of first byte in section when loaded into memory, normally be zero
	unsigned int SizeOfRawData;//size of raw data on disk, multiple of FileAlignment
	unsigned int PointerToRawData;//file pointer to first page of section with section data, multiple of FileAlignment
	unsigned int PointerToRelocations;//zero for executables
	unsigned int PointerToLineNumbers;//file pointer to line numbers for section or zero if no COFF line numbers
	unsigned short NumberOfRelocations;//zero for executables
	unsigned short NumberOfLineNumbers;//number of line numbers for section or zero
	unsigned int Characteristics;//flags for section
};


/* Section Flags
	0x00000000	//Reserved for future use.
	0x00000001	//Reserved for future use.
	0x00000002	//Reserved for future use.
	0x00000004	//Reserved for future use.
IMAGE_SCN_TYPE_NO_PAD	0x00000008	//The section should not be padded to the next boundary. This flag is obsolete and is replaced by IMAGE_SCN_ALIGN_1BYTES. This is valid only for object files.
	0x00000010	//Reserved for future use.
IMAGE_SCN_CNT_CODE	0x00000020	//The section contains executable code.
IMAGE_SCN_CNT_INITIALIZED_DATA	0x00000040	//The section contains initialized data.
IMAGE_SCN_CNT_UNINITIALIZED_ DATA	0x00000080	//The section contains uninitialized data.
IMAGE_SCN_LNK_OTHER	0x00000100	//Reserved for future use.
IMAGE_SCN_LNK_INFO	0x00000200	//The section contains comments or other information. The .drectve section has this type. This is valid for object files only.
	0x00000400	//Reserved for future use.
IMAGE_SCN_LNK_REMOVE	0x00000800	//The section will not become part of the image. This is valid only for object files.
IMAGE_SCN_LNK_COMDAT	0x00001000	//The section contains COMDAT data. For more information, see section 5.5.6, “COMDAT Sections (Object Only).” This is valid only for object files.
IMAGE_SCN_GPREL	0x00008000	//The section contains data referenced through the global pointer (GP).
IMAGE_SCN_MEM_PURGEABLE	0x00020000	//Reserved for future use.
IMAGE_SCN_MEM_16BIT	0x00020000	//Reserved for future use.
IMAGE_SCN_MEM_LOCKED	0x00040000	//Reserved for future use.
IMAGE_SCN_MEM_PRELOAD	0x00080000	//Reserved for future use.
IMAGE_SCN_ALIGN_1BYTES	0x00100000	//Align data on a 1-byte boundary. Valid only for object files.
IMAGE_SCN_ALIGN_2BYTES	0x00200000	//Align data on a 2-byte boundary. Valid only for object files.
IMAGE_SCN_ALIGN_4BYTES	0x00300000	//Align data on a 4-byte boundary. Valid only for object files.
IMAGE_SCN_ALIGN_8BYTES	0x00400000	//Align data on an 8-byte boundary. Valid only for object files.
IMAGE_SCN_ALIGN_16BYTES	0x00500000	//Align data on a 16-byte boundary. Valid only for object files.
IMAGE_SCN_ALIGN_32BYTES	0x00600000	//Align data on a 32-byte boundary. Valid only for object files.
IMAGE_SCN_ALIGN_64BYTES	0x00700000	//Align data on a 64-byte boundary. Valid only for object files.
IMAGE_SCN_ALIGN_128BYTES	0x00800000	//Align data on a 128-byte boundary. Valid only for object files.
IMAGE_SCN_ALIGN_256BYTES	0x00900000	//Align data on a 256-byte boundary. Valid only for object files.
IMAGE_SCN_ALIGN_512BYTES	0x00A00000	//Align data on a 512-byte boundary. Valid only for object files.
IMAGE_SCN_ALIGN_1024BYTES	0x00B00000	//Align data on a 1024-byte boundary. Valid only for object files.
IMAGE_SCN_ALIGN_2048BYTES	0x00C00000	//Align data on a 2048-byte boundary. Valid only for object files.
IMAGE_SCN_ALIGN_4096BYTES	0x00D00000	//Align data on a 4096-byte boundary. Valid only for object files.
IMAGE_SCN_ALIGN_8192BYTES	0x00E00000	//Align data on an 8192-byte boundary. Valid only for object files.
IMAGE_SCN_LNK_NRELOC_OVFL	0x01000000	//The section contains extended relocations.
IMAGE_SCN_MEM_DISCARDABLE	0x02000000	//The section can be discarded as needed.
IMAGE_SCN_MEM_NOT_CACHED	0x04000000	//The section cannot be cached.
IMAGE_SCN_MEM_NOT_PAGED	0x08000000	//The section is not pageable.
IMAGE_SCN_MEM_SHARED	0x10000000	//The section can be shared in memory.
IMAGE_SCN_MEM_EXECUTE	0x20000000	//The section can be executed as code.
IMAGE_SCN_MEM_READ	0x40000000	//The section can be read.
IMAGE_SCN_MEM_WRITE	0x80000000	//The section can be written to.
*/

union SymbolName//used to store either the 8 byte name or the offset to the real name if longer
{
	struct
	{
		unsigned short Zeros;
		unsigned short Offset;
	};
	char Name[8];
};

struct COFF_SymbolTable
{
	SymbolName Name;//name of symbol
	unsigned int Value;//value of symbol as defined by SectionNumber and StorageClass
	signed short SectionNumber;//1 based index into section
	unsigned char TypeLO;//represents type of data stored
	unsigned char TypeHI;//represents type of data stored
	unsigned char StorageClass;//represents storage class type
	unsigned char NumberOfAuxSymbols;//number of auxiliary symbols that follow this table
};

struct COFF_rsrcSectionHeader
{
	unsigned int Characteristics;//characteristics of .rsrc section (always zero)
	unsigned int TimeStamp;
	unsigned short VersionMajor;
	unsigned short VersionMinor;
	unsigned short NumNamedEntries;//number of named entries after header
	unsigned short NumIDEntries;//number of ID entries after named entries
};



struct COFF_rsrcEntry
{
	union
	{
		unsigned int NameRVA;
		unsigned int ID;
	};
	struct
	{
		unsigned int RVA:31;
		unsigned int Dir:1;
	};
};

struct COFF_rsrcStringEntry
{
	unsigned short Length;
	char NameString[1];
};

struct COFF_rsrcDataEntry
{
	unsigned int OffsetToData;
	unsigned int Size;
	unsigned int CodePage;
	unsigned int Reserved;
};

class COFF_rsrcSection
{
public:
	COFF_rsrcSection();
	~COFF_rsrcSection();

	COFF_rsrcSectionHeader* header;
	COFF_rsrcEntry* entry;
	COFF_rsrcDataEntry* data;
	COFF_rsrcStringEntry* strings[1024];

	COFF_rsrcSection* up;//next level up
	COFF_rsrcSection* down;//next level down
};


/*  SectionNumber values
IMAGE_SYM_UNDEFINED	  0	The symbol record is not yet assigned a section. A value of zero indicates that a reference to an external symbol is defined elsewhere. A value of non-zero is a common symbol with a size that is specified by the value.
IMAGE_SYM_ABSOLUTE	-1	The symbol has an absolute (non-relocatable) value and is not an address.
IMAGE_SYM_DEBUG	-2	The symbol provides general type or debugging information but does not correspond to a section. Microsoft tools use this setting along with .file records (storage class FILE).

/* Type Low byte
IMAGE_SYM_TYPE_NULL	  0	No type information or unknown base type. Microsoft tools use this setting
IMAGE_SYM_TYPE_VOID	  1	No valid type; used with void pointers and functions
IMAGE_SYM_TYPE_CHAR	  2	A character (signed byte)
IMAGE_SYM_TYPE_SHORT	  3	A 2-byte signed integer
IMAGE_SYM_TYPE_INT	  4	A natural integer type (normally 4 bytes in Windows)
IMAGE_SYM_TYPE_LONG	  5	A 4-byte signed integer
IMAGE_SYM_TYPE_FLOAT	  6	A 4-byte floating-point number
IMAGE_SYM_TYPE_DOUBLE	  7	An 8-byte floating-point number
IMAGE_SYM_TYPE_STRUCT	  8	A structure
IMAGE_SYM_TYPE_UNION	  9	A union
IMAGE_SYM_TYPE_ENUM	10	An enumerated type
IMAGE_SYM_TYPE_MOE	11	A member of enumeration (a specific value)
IMAGE_SYM_TYPE_BYTE	12	A byte; unsigned 1-byte integer
IMAGE_SYM_TYPE_WORD	13	A word; unsigned 2-byte integer
IMAGE_SYM_TYPE_UINT	14	An unsigned integer of natural size (normally, 4 bytes)
IMAGE_SYM_TYPE_DWORD	15	An unsigned 4-byte integer
/*Type High byte
IMAGE_SYM_DTYPE_NULL	0	No derived type; the symbol is a simple scalar variable. 
IMAGE_SYM_DTYPE_POINTER	1	The symbol is a pointer to base type.
IMAGE_SYM_DTYPE_FUNCTION	2	The symbol is a function that returns a base type.
IMAGE_SYM_DTYPE_ARRAY	3	The symbol is an array of base type.
*/

/* Storage Class symbols
IMAGE_SYM_CLASS_END_OF_FUNCTION	-1 (0xFF)	A special symbol that represents the end of function, for debugging purposes.
IMAGE_SYM_CLASS_NULL	    0	No assigned storage class.
IMAGE_SYM_CLASS_AUTOMATIC	    1	The automatic (stack) variable. The Value field specifies the stack frame offset.
IMAGE_SYM_CLASS_EXTERNAL	    2	A value that Microsoft tools use for external symbols. The Value field indicates the size if the section number is IMAGE_SYM_UNDEFINED (0). If the section number is not zero, then the Value field specifies the offset within the section.
IMAGE_SYM_CLASS_STATIC	    3	The offset of the symbol within the section. If the Value field is zero, then the symbol represents a section name.
IMAGE_SYM_CLASS_REGISTER	    4	A register variable. The Value field specifies the register number.
IMAGE_SYM_CLASS_EXTERNAL_DEF	    5	A symbol that is defined externally.
IMAGE_SYM_CLASS_LABEL	    6	A code label that is defined within the module. The Value field specifies the offset of the symbol within the section.
IMAGE_SYM_CLASS_UNDEFINED_LABEL	    7	A reference to a code label that is not defined.
IMAGE_SYM_CLASS_MEMBER_OF_STRUCT	    8	The structure member. The Value field specifies the nth member.
IMAGE_SYM_CLASS_ARGUMENT	    9	A formal argument (parameter) of a function. The Value field specifies the nth argument.
IMAGE_SYM_CLASS_STRUCT_TAG	  10	The structure tag-name entry.
IMAGE_SYM_CLASS_MEMBER_OF_UNION	  11	A union member. The Value field specifies the nth member.
IMAGE_SYM_CLASS_UNION_TAG	  12	The Union tag-name entry.
IMAGE_SYM_CLASS_TYPE_DEFINITION	  13	A Typedef entry.
IMAGE_SYM_CLASS_UNDEFINED_STATIC	  14	A static data declaration.
IMAGE_SYM_CLASS_ENUM_TAG	  15	An enumerated type tagname entry.
IMAGE_SYM_CLASS_MEMBER_OF_ENUM	  16	A member of an enumeration. The Value field specifies the nth member.
IMAGE_SYM_CLASS_REGISTER_PARAM	  17	A register parameter.
IMAGE_SYM_CLASS_BIT_FIELD	  18	A bit-field reference. The Value field specifies the nth bit in the bit field.
IMAGE_SYM_CLASS_BLOCK	 100	A .bb (beginning of block) or .eb (end of block) record. The Value field is the relocatable address of the code location.
IMAGE_SYM_CLASS_FUNCTION	101	A value that Microsoft tools use for symbol records that define the extent of a function: begin function (.bf), end function (.ef), and lines in function (.lf). For .lf records, the Value field gives the number of source lines in the function. For .ef records, the Value field gives the size of the function code.
IMAGE_SYM_CLASS_END_OF_STRUCT	102	An end-of-structure entry.
IMAGE_SYM_CLASS_FILE	103	A value that Microsoft tools, as well as traditional COFF format, use for the source-file symbol record. The symbol is followed by auxiliary records that name the file.
IMAGE_SYM_CLASS_SECTION	104	A definition of a section (Microsoft tools use STATIC storage class instead).
IMAGE_SYM_CLASS_WEAK_EXTERNAL	105	A weak external. For more information, see section 5.5.3, “Auxiliary Format 3: Weak Externals.”
IMAGE_SYM_CLASS_CLR_TOKEN	107	A CLR token symbol. The name is an ASCII string that consists of the hexadecimal value of the token. For more information, see section 5.5.7, “CLR Token Definition (Object Only).”
*/

#define OPCODE_COUNT             343

#define REG_AL                     0
#define REG_CL                     1
#define REG_DL                     2
#define REG_BL                     3
#define REG_AH                     4
#define REG_CH                     5
#define REG_DH                     6
#define REG_BH                     7

#define REG_AX                     0
#define REG_CX                     1
#define REG_DX                     2
#define REG_BX                     3
#define REG_SP                     4
#define REG_BP                     5
#define REG_SI                     6
#define REG_DI                     7

#define REG_EAX                    0
#define REG_ECX                    1
#define REG_EDX                    2
#define REG_EBX                    3
#define REG_ESP                    4
#define REG_EBP                    5
#define REG_ESI                    6
#define REG_EDI                    7

#define RM_UNDEF                 100
#define RM_NONE                  101
#define RM_MEM                   102
#define RM_IMM                   103

#define REG_FIRST                  0
#define RM_FIRST                   1

#define ARG_REG                    0
#define ARG_RM                     1
#define ARG_IMM                    2


struct Flags
{
	union
	{
		int all;
		struct
		{
			int CF:1;   //flag 0
			int flag1:1;//flag 1 always 1
			int PF:1;   //flag 2
			int flag3:1;//flag 3 always 0
			int AF:1;   //flag 4
			int flag5:1;//flag 5 always 0
			int ZF:1;   //flag 6
			int SF:1;   //flag 7
			int TF:1;   //flag 8
			int IF:1;   //flag 9
			int DF:1;   //flag A
			int OF:1;   //flag B
			int IO:1;   //flag C
			int PL:1;   //flag D
			int NT:1;   //flag E
			int flagF:1;//flag F always 0
		};
	};
};

struct ModRM
{
	BYTE RM:3;
	BYTE reg:3;
	BYTE mod:2;
};

struct SIB
{
	BYTE base:3;
	BYTE index:3;
	BYTE scale:2;
};

struct OpCode
{
	OpCode();

	bool _0F;
	BYTE code;
	bool modRM;

	BYTE regCode;

	char reg;
	BYTE regSize;  //size of REG register to use

	char rm;
	BYTE rmSize;   //size of R/M register to use

	BYTE imm;      //size of immediate value

	bool rel;      //used for jump codes to show relative or absolute addresses

	char name[10]; //Name of OpCode
	char op[256];  //Operation

	char desc[128];//Description

	BYTE order;     //1-RM,R2    2-R,RM

	Flags setFlags;
	Flags clrFlags;
	Flags defFlags;

};

struct Registers
{
	Registers();

	union
	{
		int EAX;
		short AX;
		struct
		{
			char AL;
			char AH;
		};
	};
	union
	{
		int ECX;
		short CX;
		struct
		{
			char CL;
			char CH;
		};
	};
	union
	{
		int EDX;
		short DX;
		struct
		{
			char DL;
			char DH;
		};
	};
	union
	{
		int EBX;
		short BX;
		struct
		{
			char BL;
			char BH;
		};
	};
	union
	{
		int ESP;
		short SP;
	};
	union
	{
		int EBP;
		short BP;
	};
	union
	{
		int ESI;
		short SI;
	};
	union
	{
		int EDI;
		short DI;
	};
	union
	{
		int EIP;
		short IP;
	};
	Flags EFlags;

	char* reg8[8];
	short* regX[8];
	int* regE[8];
};

class DisAsm
{
public:
	DisAsm();
	~DisAsm();

	OpCode opList[OPCODE_COUNT];

	Registers r;

	int stack[1024];//the stack

	BYTE* data;

	void InitializeRegisters(int entryPoint);
	void Step();//emulate next instruction
	void PreformOp(BYTE* code);//emulate an instruction

	int Decode(BYTE* code, char* decode, int size = 0xFF0000);//decode a data block
	int DecodeOpcode(BYTE* code, char* decode);//decode a single opcode
};
