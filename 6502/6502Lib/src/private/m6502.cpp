#include "m6502.h"

#include <chrono>
#include <thread>

#define ASSERT( Condition, Text ) { if ( !Condition ) { throw -1; } }

m6502::Mem::Mem ()
{
    Initialise();
}

m6502::Mem::~Mem()
{
}

/** read 1 byte */
m6502::Byte m6502::Mem::operator[]( u32 Address) const
{
    // assert here Address is < MAX_MEM
    return Data[Address];
}

/** write 1 byte */
m6502::Byte& m6502::Mem::operator[]( u32 Address)
{
    // assert here Address is < MAX_MEM
    return Data[Address];
}

m6502::CPU::CPU(Mem& pMem) : m_memory(pMem)
{
    Reset();
}

m6502::CPU::~CPU()
{

}

void m6502::CPU::Reset( )
{
	Reset( 0xFFFC );
}

void m6502::CPU::Reset( Word ResetVector )
{
    PC = ResetVector;
    SP = 0xFF;
    Flags.C = Flags.Z = Flags.I = Flags.D = Flags.B = Flags.V = Flags.N = 0;
    A = X = Y = 0;
    m_memory.Initialise();
}

m6502::Byte m6502::CPU::FetchByte()
{
    Byte Data = m_memory[PC];
    PC++;
    m_cycles--;
    return Data;
}

m6502::Word m6502::CPU::FetchWord()
{
    // 6502 is little endian
    Word Data = m_memory[PC];
    PC++;

    Data |= (m_memory[PC] << 8 );
    PC++;

    m_cycles-=2;
    return Data;
}

m6502::Byte m6502::CPU::ReadByte( Word Address )
{
    Byte Data = m_memory[Address];
    m_cycles--;
    return Data;
}

m6502::Word m6502::CPU::ReadWord( Word Address )
{
    Byte LoByte = ReadByte( Address );
    Byte HiByte = ReadByte( Address + 1 );
    return LoByte | (HiByte << 8);
}

/** write 1 byte to memory */
void m6502::CPU::WriteByte( Byte Value, Word Address )
{
    m_memory[Address] = Value;
    m_cycles--;
}

/** write 2 bytes to memory */
void m6502::CPU::WriteWord(	Word Value, Word Address )
{
    m_memory[Address] = Value & 0xFF;
    m_memory[Address + 1] = (Value >> 8);
    m_cycles -= 2;
}

/** @return the stack pointer as a full 16-bit address (in the 1st page) */
m6502::Word m6502::CPU::SPToAddress() const
{
    return 0x100 | SP;
}

void m6502::CPU::PushWordToStack( Word Value )
{
    WriteByte( Value >> 8, SPToAddress());
    SP--;
    WriteByte( Value & 0xFF, SPToAddress());
    SP--;
}

/** Push the PC-1 onto the stack */
void m6502::CPU::PushPCMinusOneToStack()
{
    PushWordToStack( PC - 1 );
}

/** Push the PC+1 onto the stack */
void m6502::CPU::PushPCPlusOneToStack()
{
    PushWordToStack( PC + 1 );
}

/** Push the PC onto the stack */
void m6502::CPU::PushPCToStack()
{
    PushWordToStack( PC );
}

void m6502::CPU::PushByteOntoStack( Byte Value )
{
    const Word SPWord = SPToAddress();
    m_memory[SPWord] = Value;
    m_cycles--;
    SP--;
    m_cycles--;
}

m6502::Byte m6502::CPU::PopByteFromStack()
{
    SP++;
    m_cycles--;
    const Word SPWord = SPToAddress();
    Byte Value = m_memory[SPWord];
    m_cycles--;
    return Value;
}

/** Pop a 16-bit value from the stack */
m6502::Word m6502::CPU::PopWordFromStack()
{
    Word ValueFromStack = ReadWord( SPToAddress()+1 );
    SP += 2;
    m_cycles--;
    return ValueFromStack;
}

/** Sets the correct Process status after a load register instruction
	*	- LDA, LDX, LDY
	*	@Register The A,X or Y Register */
void m6502::CPU::SetZeroAndNegativeFlags( Byte Register )
{
    Flags.Z = (Register == 0);
    Flags.N = (Register & NegativeFlagBit) > 0;
}

m6502::s64 m6502::CPU::Execute( s64 Cycles )
{
    /** Load a Register with the value from the memory address */
    auto LoadRegister = 
        [this]
        ( Word Address, Byte& Register )
    {
        Register = ReadByte ( Address );
        SetZeroAndNegativeFlags( Register );
    };
    s64 CyclesRequested = Cycles;
	m_cycles = Cycles;
    while ( m_cycles > 0)
    {
        Byte Instr = FetchByte();
        switch (static_cast<Ins>(Instr))
        {
            case Ins::LDA_IM:
            {
                A = FetchByte ();
                SetZeroAndNegativeFlags(A);
            } break;
            case Ins::LDX_IM:
            {
                X = FetchByte ();
                SetZeroAndNegativeFlags(X);
            } break;
            case Ins::LDY_IM:
            {
                Y = FetchByte ();
                SetZeroAndNegativeFlags(Y);
            } break;
            case Ins::LDA_ZP:
            {
                Word Address = AddrZeroPage();
                LoadRegister ( Address, A );
            } break;
            case Ins::LDX_ZP:
            {
                Word Address = AddrZeroPage();
                LoadRegister ( Address, X );
            } break;
            case Ins::LDY_ZP:
            {
                Word Address = AddrZeroPage();
                LoadRegister ( Address, Y );
            } break;
            case Ins::LDA_ZPX:
            {
                Word Address = AddrZeroPageX();
                LoadRegister ( Address, A );
            } break;
            case Ins::LDX_ZPY:
            {
                Word Address = AddrZeroPageY();
                LoadRegister ( Address, X );
            } break;
            case Ins::LDY_ZPX:
            {
                Word Address = AddrZeroPageX();
                LoadRegister ( Address, Y );
            } break;
            case Ins::LDA_ABS:
            {
                Word Address = AddrAbsolute();
                LoadRegister ( Address, A );
            } break;
            case Ins::LDX_ABS:
            {
                Word Address = AddrAbsolute();
                LoadRegister ( Address, X );
            } break;
            case Ins::LDY_ABS:
            {
                Word Address = AddrAbsolute();
                LoadRegister ( Address, Y );
            } break;
            case Ins::LDA_ABSX:
            {
                Word Address = AddrAbsoluteX();
                LoadRegister ( Address, A );
            } break;
            case Ins::LDA_ABSY:
            {
                Word Address = AddrAbsoluteY();
                LoadRegister ( Address, A );
            } break;
            case Ins::LDX_ABSY:
            {
                Word Address = AddrAbsoluteY();
                LoadRegister ( Address, X );
            } break;
            case Ins::LDY_ABSX:
            {
                Word Address = AddrAbsoluteX();
                LoadRegister ( Address, Y );
            } break;
            case Ins::LDA_INDX:
            {
                Word Address = AddrIndirectX();
                LoadRegister ( Address, A );
            } break;
            case Ins::LDA_INDY:
            {
                Word Address = AddrIndirectY();
                LoadRegister ( Address, A );
            } break;
            case Ins::STA_ZP:
            {
                Word Address = AddrZeroPage();
                WriteByte ( A , Address );
            } break;
            case Ins::STX_ZP:
            {
                Word Address = AddrZeroPage();
                WriteByte ( X , Address );
            } break;
            case Ins::STY_ZP:
            {
                Word Address = AddrZeroPage();
                WriteByte ( Y , Address );
            } break;
            case Ins::STA_ABS:
            {
                Word Address = AddrAbsolute();
                WriteByte ( A , Address );
            } break;
            case Ins::STX_ABS:
            {
                Word Address = AddrAbsolute();
                WriteByte ( X , Address );
            } break;
            case Ins::STY_ABS:
            {
                Word Address = AddrAbsolute();
                WriteByte ( Y , Address );
            } break;
            case Ins::STA_ZPX:
            {
                Word Address = AddrZeroPageX();
                WriteByte ( A , Address );
            } break;
            case Ins::STY_ZPX:
            {
                Word Address = AddrZeroPageX();
                WriteByte ( Y , Address );
            } break;
            case Ins::STA_ABSX:
            {
                Word Address = AddrAbsoluteX_5();
                WriteByte ( A , Address );
            } break;
            case Ins::STA_ABSY:
            {
                Word Address = AddrAbsoluteY_5();
                WriteByte ( A , Address );
            } break;
            case Ins::STX_ZPY:
            {
                Word Address = AddrZeroPageY();
                WriteByte ( X , Address );
            } break;
            case Ins::STA_INDX:
            {
                Word Address = AddrIndirectX_6();
                WriteByte ( A , Address );
            } break;
            case Ins::STA_INDY:
            {
                Word Address = AddrIndirectY_6();
                WriteByte ( A , Address );
            } break;
            case Ins::JSR:
            {
                Word SubAddr = FetchWord();
                PushPCMinusOneToStack();
                PC = SubAddr;
                m_cycles--;
            } break;
            case Ins::RTS:
            {
                Word ReturnAddress = PopWordFromStack();
                PC = ReturnAddress + 1;	
                m_cycles -= 2;
            } break;
            //TODO:
            //An original 6502 has does not correctly fetch the target 
            //address if the indirect vector falls on a page boundary
            //( e.g.$xxFF where xx is any value from $00 to $FF ).
            //In this case fetches the LSB from $xxFF as expected but 
            //takes the MSB from $xx00.This is fixed in some later chips 
            //like the 65SC02 so for compatibility always ensure the 
            //indirect vector is not at the end of the page.
            case Ins::JMP_ABS:
            {
                Word Address = AddrAbsolute();
                PC = Address;
            } break;
            case Ins::JMP_IND:
            {
                Word Address = AddrAbsolute();
                Address = ReadWord( Address );
                PC = Address;
            } break;
            case Ins::INX:
            {
                X++;
                m_cycles--;
                SetZeroAndNegativeFlags( X );
            } break;
            case Ins::INY:
            {
                Y++;
                m_cycles--;
                SetZeroAndNegativeFlags( Y );
            } break;
            case Ins::DEX:
            {
                X--;
                m_cycles--;
                SetZeroAndNegativeFlags( X );
            } break;
            case Ins::DEY:
            {
                Y--;
                m_cycles--;
                SetZeroAndNegativeFlags( Y );
            } break;
            case Ins::DEC_ZP:
            {
                Word Address = AddrZeroPage();
                Byte Value = ReadByte( Address );
                Value--;
                m_cycles--;
                WriteByte( Value, Address );
                SetZeroAndNegativeFlags( Value );
            } break;
            case Ins::DEC_ZPX:
            {
                Word Address = AddrZeroPageX();
                Byte Value = ReadByte( Address );
                Value--;
                m_cycles--;
                WriteByte( Value, Address );
                SetZeroAndNegativeFlags( Value );
            } break;
            case Ins::DEC_ABS:
            {
                Word Address = AddrAbsolute();
                Byte Value = ReadByte( Address );
                Value--;
                m_cycles--;
                WriteByte( Value, Address );
                SetZeroAndNegativeFlags( Value );
            } break;
            case Ins::DEC_ABSX:
            {
                Word Address = AddrAbsoluteX_5();
                Byte Value = ReadByte( Address );
                Value--;
                m_cycles--;
                WriteByte( Value, Address );
                SetZeroAndNegativeFlags( Value );
            } break;
            case Ins::INC_ZP:
            {
                Word Address = AddrZeroPage();
                Byte Value = ReadByte( Address );
                Value++;
                m_cycles--;
                WriteByte( Value, Address );
                SetZeroAndNegativeFlags( Value );
            } break;
            case Ins::INC_ZPX:
            {
                Word Address = AddrZeroPageX();
                Byte Value = ReadByte( Address );
                Value++;
                m_cycles--;
                WriteByte( Value, Address );
                SetZeroAndNegativeFlags( Value );
            } break;
            case Ins::INC_ABS:
            {
                Word Address = AddrAbsolute();
                Byte Value = ReadByte( Address );
                Value++;
                m_cycles--;
                WriteByte( Value, Address );
                SetZeroAndNegativeFlags( Value );
            } break;
            case Ins::INC_ABSX:
            {
                Word Address = AddrAbsoluteX_5();
                Byte Value = ReadByte( Address );
                Value++;
                m_cycles--;
                WriteByte( Value, Address );
                SetZeroAndNegativeFlags( Value );
            } break;
            default:
            {
                printf("Instruction %02X not handled\n", Instr);
                throw - 1;
            } break;
        }
    }
    const s64 NumCyclesUsed = CyclesRequested - m_cycles;
	return NumCyclesUsed;
}

m6502::Word m6502::CPU::AddrZeroPage()
{
	Byte ZeroPageAddr = FetchByte();
	return ZeroPageAddr;
}

m6502::Word m6502::CPU::AddrZeroPageX()
{
	Byte ZeroPageAddr = FetchByte();
	ZeroPageAddr += X;
	m_cycles--;
	return ZeroPageAddr;
}

m6502::Word m6502::CPU::AddrZeroPageY()
{
	Byte ZeroPageAddr = FetchByte();
	ZeroPageAddr += Y;
	m_cycles--;
	return ZeroPageAddr;
}

m6502::Word m6502::CPU::AddrAbsolute()
{
	Word AbsAddress = FetchWord();
	return AbsAddress;
}

m6502::Word m6502::CPU::AddrAbsoluteX()
{
	Word AbsAddress = FetchWord();
	Word AbsAddressX = AbsAddress + X;
	const bool CrossedPageBoundary = (AbsAddress ^ AbsAddressX) >> 8;
	if ( CrossedPageBoundary )
	{
		m_cycles--;
	}

	return AbsAddressX;
}

m6502::Word m6502::CPU::AddrAbsoluteX_5()
{
	Word AbsAddress = FetchWord();
	Word AbsAddressX = AbsAddress + X;
	m_cycles--;
	return AbsAddressX;
}

m6502::Word m6502::CPU::AddrAbsoluteY()
{
	Word AbsAddress = FetchWord();
	Word AbsAddressY = AbsAddress + Y;
	const bool CrossedPageBoundary = (AbsAddress ^ AbsAddressY) >> 8;
	if ( CrossedPageBoundary )
	{
		m_cycles--;
	}

	return AbsAddressY;
}

m6502::Word m6502::CPU::AddrIndirectX()
{
	Byte ZPAddress = FetchByte();
	ZPAddress += X;
	m_cycles--;
	Word EffectiveAddr = ReadWord(ZPAddress);
	return EffectiveAddr;
}

m6502::Word m6502::CPU::AddrIndirectY()
{
	Byte ZPAddress = FetchByte();
	Word EffectiveAddr = ReadWord( ZPAddress );
	Word EffectiveAddrY = EffectiveAddr + Y;
	const bool CrossedPageBoundary = (EffectiveAddr ^ EffectiveAddrY) >> 8;
	if ( CrossedPageBoundary )
	{
		m_cycles--;
	}
	return EffectiveAddrY;
}

m6502::Word m6502::CPU::AddrAbsoluteY_5()
{
	Word AbsAddress = FetchWord();
	Word AbsAddressY = AbsAddress + Y;	
	m_cycles--;
	return AbsAddressY;
}

m6502::Word m6502::CPU::AddrIndirectX_6()
{
	Byte ZPAddress = FetchByte();
	Word EffectiveAddr = ReadWord( ZPAddress );
	Word EffectiveAddrY = EffectiveAddr + X;
	m_cycles--;
	return EffectiveAddrY;
}

m6502::Word m6502::CPU::AddrIndirectY_6()
{
	Byte ZPAddress = FetchByte();
	Word EffectiveAddr = ReadWord( ZPAddress );
	Word EffectiveAddrY = EffectiveAddr + Y;
	m_cycles--;
	return EffectiveAddrY;
}

m6502::Word m6502::CPU::LoadPrg( const Byte* Program, u32 NumBytes ) const
{
	Word LoadAddress = 0;
	if ( Program && NumBytes > 2 )
	{
		u32 At = 0;
		const Word Lo = Program[At++];
		const Word Hi = Program[At++] << 8;
		LoadAddress = Lo | Hi;
		for ( Word i = LoadAddress; i < LoadAddress+NumBytes-2; i++ )
		{
			//TODO: mem copy?
		m_memory[i] = Program[At++];
		}
	}

	return LoadAddress;
}