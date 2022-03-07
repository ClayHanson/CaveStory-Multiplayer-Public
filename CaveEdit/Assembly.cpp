//File created by David Shafer (C) 2007
#include "stdafx.h"
#include "Assembly.h"

#include <fstream>
using namespace std;


COFF_rsrcSection::COFF_rsrcSection()
{
	header = NULL;
	entry = NULL;
	data = NULL;

	up = NULL;
	down = NULL;
}

COFF_rsrcSection::~COFF_rsrcSection()
{
	clean(down);
}

OpCode::OpCode()
{
	_0F = false;
	name[0] = '\0';//Name of OpCode
	desc[0] = '\0';//Description
	op[0] = '\0';  //Operation
	modRM = false;
	imm = 0;
}

Registers::Registers()
{
	reg8[REG_AL] = &AL;
	reg8[REG_CL] = &CL;
	reg8[REG_DL] = &DL;
	reg8[REG_BL] = &BL;
	reg8[REG_AH] = &AH;
	reg8[REG_CH] = &CH;
	reg8[REG_DH] = &DH;
	reg8[REG_BH] = &BH;

	regX[REG_AX] = &AX;
	regX[REG_CX] = &CX;
	regX[REG_DX] = &DX;
	regX[REG_BX] = &BX;
	regX[REG_SP] = &SP;
	regX[REG_BP] = &BP;
	regX[REG_SI] = &SI;
	regX[REG_DI] = &DI;

	regE[REG_EAX] = &EAX;
	regE[REG_ECX] = &ECX;
	regE[REG_EDX] = &EDX;
	regE[REG_EBX] = &EBX;
	regE[REG_ESP] = &ESP;
	regE[REG_EBP] = &EBP;
	regE[REG_ESI] = &ESI;
	regE[REG_EDI] = &EDI;
};

DisAsm::DisAsm()
{
	char flag;
	char buffer[1024];
	ifstream file;
	int temp;

	file.open("CE_Opcodes.txt");


	file.getline(buffer, 1024);//remove header line
	for (int i = 0; i < OPCODE_COUNT; i++)
	{
		file >> temp;//read opcode
		if (temp >= 0x0F00)
		{
			temp -= 0x0F00;
			opList[i]._0F = true;
		}
		opList[i].code = temp;

		file >> buffer;//ignore HEX representation of code

		file >> opList[i].name;//get name

		file >> opList[i].modRM;//get ModR/M flag

		file >> temp;
		opList[i].regCode = temp;//get reg code for some opcodes

		file >> temp;
		opList[i].reg = temp;//get reg type
		file >> temp;
		opList[i].regSize = temp;//get reg size

		file >> temp;
		opList[i].rm = temp;//get rm type
		file >> temp;
		opList[i].rmSize = temp;//get rm size

		file >> temp;
		opList[i].imm = temp;//get size of any immediate value for opcode

		file >> opList[i].rel;//get relative flag (for jumps)

		file >> temp;
		opList[i].order = temp;//get argument order flag

		file.get();//remove tab
		//get flag assignment
		if (file.get() != '.') opList[i].defFlags.AF = 1; else opList[i].defFlags.AF = 0;
		if (file.get() != '.') opList[i].defFlags.CF = 1; else opList[i].defFlags.CF = 0;
		if (file.get() != '.') opList[i].defFlags.OF = 1; else opList[i].defFlags.OF = 0;
		if (file.get() != '.') opList[i].defFlags.SF = 1; else opList[i].defFlags.SF = 0;
		if (file.get() != '.') opList[i].defFlags.PF = 1; else opList[i].defFlags.PF = 0;
		if (file.get() != '.') opList[i].defFlags.ZF = 1; else opList[i].defFlags.ZF = 0;
		if (file.get() != '.') opList[i].defFlags.TF = 1; else opList[i].defFlags.TF = 0;
		if (file.get() != '.') opList[i].defFlags.IF = 1; else opList[i].defFlags.IF = 0;
		if (file.get() != '.') opList[i].defFlags.DF = 1; else opList[i].defFlags.DF = 0;

		file.get();//remove tab

		file.getline(opList[i].desc, 256, '\t');//get opcode description

		file.getline(opList[i].op, 256);//get opcode operation pseudo code
	};

	file.close();

}


DisAsm::~DisAsm()
{
}

int DisAsm::Decode(BYTE* code, char* decode, int size)
{
	if (code == NULL || decode == NULL)
		return 0;

	int t;
	decode[0] = '\0';
	int s = 0;
	while (s < size)
	{
		t = DecodeOpcode(&code[s], &decode[strlen(decode)]);
		if (t == -1)
			break;
		s += t;
	};

	return 0;
}

int DisAsm::DecodeOpcode(BYTE* code, char* decode)
{
	int ptr = 0;
	int i = 0;
	int j = 0;

	BYTE prefix[4];
	char sign;
	char arg[2][32];//1-reg  2-R/M
	int reg = RM_UNDEF;//register
	int rm = RM_UNDEF;//register

	bool sib = false;//SIB byte?
	int sBase = RM_UNDEF;//register
	int sIndex = RM_UNDEF;//register
	int sScale;

	bool disp = false;//displacement byte(s)?
	int dSize;//size of displacement in bytes
	int dVal; //value of displacement

	bool of = false;//begins with 0x0F?

	//first check for up to 4 prefixes
	for (i = 0; i < 4; i++)
	{
		switch (code[ptr])
		{
			//LOCK and Repeat Prefixes
		case 0xF0://LOCK prefix
			prefix[i] = code[ptr++];
			break;
		case 0xF2://REPNE prefix
			prefix[i] = code[ptr++];
			ptr++;//skip byte for later use in prefix code
			break;
		case 0xF3://REP and REPE prefix
			prefix[i] = code[ptr++];
			ptr++;//skip byte for later use in prefix code
			break;

			//Segment Override Prefixes
		case 0x2E://CS override prefix
			prefix[i] = code[ptr++];
			break;
		case 0x36://SS override prefix
			prefix[i] = code[ptr++];
			break;
		case 0x3E://DS override prefix
			prefix[i] = code[ptr++];
			break;
		case 0x26://ES override prefix
			prefix[i] = code[ptr++];
			break;
		case 0x64://FS override prefix
			prefix[i] = code[ptr++];
			break;
		case 0x65://GS override prefix
			prefix[i] = code[ptr++];
			break;

		case 0x66://Opperand size override prefix
			prefix[i] = code[ptr++];
			break;
		
		case 0x67://Address size override prefix
			prefix[i] = code[ptr++];
			break;
/*/Not used
		case 0x0F://SIMD Extension prefix
			prefix[i] = code[ptr++];
			break;
//*/
		};
	}
	//End prefix check

	//check 0x0F since all 2byte opcodes start with it we'll remove it and check a flag
	if (code[ptr] == 0x0F)
	{
		of = true;
		ptr++;
	}

	//Find opcode
	for (i = 0; i < OPCODE_COUNT; i++)
	{
		if (of == opList[i]._0F && code[ptr] == opList[i].code)
		{
			if (opList[i].modRM == 0
			 || (opList[i].modRM && (opList[i].regCode == RM_UNDEF
			                      || opList[i].regCode == ((ModRM*)(&code[ptr+1]))->reg)
			    )
			   )
				break;
		}
	}
	//opcode error.  Return -1 to indicate.
	if (i == OPCODE_COUNT)
		return -1;
	ptr++;
	//End Find opcode


	//read optional ModR/M Byte
	if (opList[i].modRM)
	{
		reg = opList[i].reg;//default values
		rm = opList[i].rm;

		//read reg
		if (opList[i].reg == RM_UNDEF)
			reg = ((ModRM*)(&code[ptr]))->reg;
		else
			reg = opList[i].reg;
      
		rm = ((ModRM*)(&code[ptr]))->RM;

		//read mod and R/M

		//first check for other special bytes
		if (((ModRM*)(&code[ptr]))->mod != 3 && ((ModRM*)(&code[ptr]))->RM == 4)
		{
			sib = true;
		}
		if (((ModRM*)(&code[ptr]))->mod == 1)
		{
			disp = true;
			dSize = 1;
		}
		if ( ((ModRM*)(&code[ptr]))->mod == 3
		  || ( ((ModRM*)(&code[ptr]))->mod == 0 && ((ModRM*)(&code[ptr]))->RM == 5) )
		{
			disp = true;
			dSize = 4;
		}
	}
	//End read optional ModR/M Byte

	//Read optional SIB byte
	if (sib)
	{
		ptr++;
		sBase = ((SIB*)(&code[ptr]))->base;
		if (sBase == 5)
		{
			switch (((ModRM*)(&code[ptr]))->mod)
			{
			case 0:
				sBase = RM_NONE;
				disp = true;
				dSize = 4;
				break;
			case 1:
				sBase = REG_EBP;
				break;
			case 2:
				sBase = REG_EBP;
				break;
			};
		}
		sIndex = ((SIB*)(&code[ptr]))->index;
		if (sIndex == 4)
			sIndex = RM_NONE;

		sScale = 1 << (((SIB*)(&code[ptr]))->scale-1);
	}
	//End read optional SIB byte

	//Read optional displacement byte(s)
	if (disp)
	{
		ptr++;
		if (dSize == 1)
			dVal = code[ptr];
		if (dSize == 2)
			dVal = *((short*)(&code[ptr]));
		if (dSize == 4)
			dVal = *((int*)(&code[ptr]));

		ptr += dSize-1;
	}
	//End read optional displacement byte(s)

	if (sib && disp && opList[i].imm > 0)
	{
		sprintf(arg[0], "%s, PTR [%s]");
	}



	return ptr;
}

void DisAsm::InitializeRegisters(int entryPoint)
{
}

void DisAsm::Step()
{
	PreformOp(&data[r.EIP]);
}

void DisAsm::PreformOp(BYTE* code)
{
}

