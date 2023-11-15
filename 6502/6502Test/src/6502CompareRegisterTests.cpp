#include <gtest/gtest.h>
#include <m6502/System.hpp>

class M6502CompareRegisterTests : public testing::Test
{
public:
    M6502CompareRegisterTests() : cpu(bus), mem(bus,0x0000,0x0000) {}
    m6502::CBus bus;
    m6502::CMem mem;
    m6502::CCPU cpu;

    virtual void SetUp()
    {
        cpu.Reset();
    }

    virtual void TearDown()
    {
    }



    void ExpectUnaffectedRegisters( m6502::CCPU CPUBefore )
    {
        EXPECT_EQ( CPUBefore.Flags.I, cpu.Flags.I );
        EXPECT_EQ( CPUBefore.Flags.D, cpu.Flags.D );
        EXPECT_EQ( CPUBefore.Flags.B, cpu.Flags.B );
        EXPECT_EQ( CPUBefore.Flags.V, cpu.Flags.V );
    }
    
    struct CMPTestData
    {
        bool Carry;
        m6502::Byte RegisterValue;
        m6502::Byte Operand;

        bool ExpectC;
        bool ExpectZ;
        bool ExpectN;
    };

    CMPTestData CompareTwoIdenticalValues()
    {
        CMPTestData Test;
        Test.RegisterValue = 26;
        Test.Operand = 26;
        Test.ExpectC = true;
        Test.ExpectN = false;
        Test.ExpectZ = true;
        return Test;
    }

    CMPTestData CompareALargePositiveToASmallPositive()
    {
        CMPTestData Test;
        Test.RegisterValue = 48;
        Test.Operand = 26;
        Test.ExpectZ = false;
        Test.ExpectN = false;
        Test.ExpectC = true;
        return Test;
    }

    CMPTestData CompareANegativeNumberToAPositive()
    {
        CMPTestData Test;
        Test.RegisterValue = 130; // Negative number!
        Test.Operand = 26;
        Test.ExpectZ = false;
        Test.ExpectN = false;
        Test.ExpectC = true;
        return Test;
    }

    CMPTestData CompareTwoValuesThatResultInANegativeFlagSet()
    {
        CMPTestData Test;
        Test.RegisterValue = 8;
        Test.Operand = 26;
        Test.ExpectZ = false;
        Test.ExpectN = true;
        Test.ExpectC = false;
        return Test;
    }

    enum class ERegister
    {
        A, X, Y
    };

    void CompareImmediate (CMPTestData Test, ERegister RegisterToCompare )
    {
        // given:
        using namespace m6502;
        cpu.Reset( 0xFF00 );
        cpu.Flags.C = !Test.ExpectC;
        cpu.Flags.Z = !Test.ExpectZ;
        cpu.Flags.N = !Test.ExpectN;
        Byte* Register = &cpu.A;
        Byte OpCode = opcode(Ins::CMP);
        switch ( RegisterToCompare )
        {
        case ERegister::X:
            Register = &cpu.X;
            OpCode = opcode(Ins::CPX);
            break;
        case ERegister::Y:
            Register = &cpu.Y,
            OpCode = opcode(Ins::CPY);
            break;
        };
        *Register = Test.RegisterValue;

        mem[0xFF00] = OpCode;
        mem[0xFF01] = Test.Operand;
        constexpr s64 EXPECTED_CYCLES = 2;
        CCPU CPUCopy = cpu;

        // when:
        const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

        // then:
        EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
        EXPECT_EQ( *Register, Test.RegisterValue );
        EXPECT_EQ( cpu.Flags.Z , Test.ExpectZ );
        EXPECT_EQ( cpu.Flags.N , Test.ExpectN );
        EXPECT_EQ( cpu.Flags.C , Test.ExpectC );
        ExpectUnaffectedRegisters(CPUCopy);
    }

    void CompareZeroPage (CMPTestData Test, ERegister RegisterToCompare )
    {
        // given:
        using namespace m6502;
        cpu.Reset( 0xFF00 );
        cpu.Flags.C = !Test.ExpectC;
        cpu.Flags.Z = !Test.ExpectZ;
        cpu.Flags.N = !Test.ExpectN;

        Byte* Register = &cpu.A;
        Byte OpCode = opcode(Ins::CMP_ZP);
        switch ( RegisterToCompare )
        {
        case ERegister::X:
            Register = &cpu.X;
            OpCode = opcode(Ins::CPX_ZP);
            break;
        case ERegister::Y:
            Register = &cpu.Y,
            OpCode = opcode(Ins::CPY_ZP);
            break;
        };
        *Register = Test.RegisterValue;
        mem[0xFF00] = OpCode;
        mem[0xFF01] = 0x42;
        mem[0x0042] = Test.Operand;
        constexpr s64 EXPECTED_CYCLES = 3;
        CCPU CPUCopy = cpu;

        // when:
        const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

        // then:
        EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
		EXPECT_EQ( *Register, Test.RegisterValue );
        EXPECT_EQ( cpu.Flags.C , Test.ExpectC );
        EXPECT_EQ( cpu.Flags.Z , Test.ExpectZ );
        EXPECT_EQ( cpu.Flags.N , Test.ExpectN );
        ExpectUnaffectedRegisters(CPUCopy);
    }

    void CMPZeroPageX (CMPTestData Test)
    {
        // given:
        using namespace m6502;
        cpu.Reset( 0xFF00 );
        cpu.Flags.C = !Test.ExpectC;
        cpu.Flags.Z = !Test.ExpectZ;
        cpu.Flags.N = !Test.ExpectN;
        cpu.A = Test.RegisterValue;
        cpu.X = 4;
        mem[0xFF00] = opcode(Ins::CMP_ZPX);
        mem[0xFF01] = 0x42;
        mem[0x0042 + 4] = Test.Operand;
        constexpr s64 EXPECTED_CYCLES = 4;
        CCPU CPUCopy = cpu;

        // when:
        const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

        // then:
        EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
        EXPECT_EQ( cpu.A, Test.RegisterValue );
        EXPECT_EQ( cpu.X, 4 );
        EXPECT_EQ( cpu.Flags.Z , Test.ExpectZ );
        EXPECT_EQ( cpu.Flags.N , Test.ExpectN );
        EXPECT_EQ( cpu.Flags.C , Test.ExpectC );
        ExpectUnaffectedRegisters(CPUCopy);
    }

    void CompareAbsolute (CMPTestData Test, ERegister RegisterToCompare )
    {
        // given:
        using namespace m6502;
        cpu.Reset( 0xFF00 );
        cpu.Flags.C = !Test.ExpectC;
        cpu.Flags.Z = !Test.ExpectZ;
        cpu.Flags.N = !Test.ExpectN;
        
        Byte* Register = &cpu.A;
        Byte OpCode = opcode(Ins::CMP_ABS);
        switch ( RegisterToCompare )
        {
        case ERegister::X:
            Register = &cpu.X;
            OpCode = opcode(Ins::CPX_ABS);
            break;
        case ERegister::Y:
            Register = &cpu.Y,
            OpCode = opcode(Ins::CPY_ABS);
            break;
        };
        *Register = Test.RegisterValue;

        mem[0xFF00] = OpCode;
        mem[0xFF01] = 0x00;
        mem[0xFF02] = 0x80;
        mem[0x8000] = Test.Operand;
        constexpr s64 EXPECTED_CYCLES = 4;
        CCPU CPUCopy = cpu;

        // when:
        const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

        // then:
        EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
	    EXPECT_EQ( *Register, Test.RegisterValue );
        EXPECT_EQ( cpu.Flags.C , Test.ExpectC );
        EXPECT_EQ( cpu.Flags.Z , Test.ExpectZ );
        EXPECT_EQ( cpu.Flags.N , Test.ExpectN );
        ExpectUnaffectedRegisters(CPUCopy);
    }

    void CMPAbsoluteX (CMPTestData Test)
    {
        // given:
        using namespace m6502;
        cpu.Reset( 0xFF00 );
        cpu.Flags.C = !Test.ExpectC;
        cpu.Flags.Z = !Test.ExpectZ;
        cpu.Flags.N = !Test.ExpectN;
		cpu.A = Test.RegisterValue;
        cpu.X = 4;
        mem[0xFF00] = opcode(Ins::CMP_ABSX);
        mem[0xFF01] = 0x00;
        mem[0xFF02] = 0x80;
        mem[0x8000+4] = Test.Operand;
        constexpr s64 EXPECTED_CYCLES = 4;
        CCPU CPUCopy = cpu;

        // when:
        const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

        // then:
        EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
        EXPECT_EQ( cpu.A, Test.RegisterValue );
        EXPECT_EQ( cpu.X, 4 );
        EXPECT_EQ( cpu.Flags.Z , Test.ExpectZ );
        EXPECT_EQ( cpu.Flags.N , Test.ExpectN );
        EXPECT_EQ( cpu.Flags.C , Test.ExpectC );
        ExpectUnaffectedRegisters(CPUCopy);
    }

    void CMPAbsoluteY (CMPTestData Test)
    {
        // given:
        using namespace m6502;
        cpu.Reset( 0xFF00 );
        cpu.Flags.C = !Test.ExpectC;
        cpu.Flags.Z = !Test.ExpectZ;
        cpu.Flags.N = !Test.ExpectN;
        cpu.A = Test.RegisterValue;
        cpu.Y = 4;
        mem[0xFF00] = opcode(Ins::CMP_ABSY);
        mem[0xFF01] = 0x00;
        mem[0xFF02] = 0x80;
        mem[0x8000 + 4] = Test.Operand;
        constexpr s64 EXPECTED_CYCLES = 4;
        CCPU CPUCopy = cpu;

        // when:
        const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

        // then:
        EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
		EXPECT_EQ( cpu.A, Test.RegisterValue );
		EXPECT_EQ( cpu.Y, 4 );
        EXPECT_EQ( cpu.Flags.C , Test.ExpectC );
        EXPECT_EQ( cpu.Flags.Z , Test.ExpectZ );
        EXPECT_EQ( cpu.Flags.N , Test.ExpectN );
        ExpectUnaffectedRegisters(CPUCopy);
    }

    void CMPIndirectX( CMPTestData Test )
	{
		// given:
		using namespace m6502;
		cpu.Reset( 0xFF00 );
		cpu.Flags.C = !Test.ExpectC;
		cpu.Flags.Z = !Test.ExpectZ;
		cpu.Flags.N = !Test.ExpectN;
		cpu.A = Test.RegisterValue;
		cpu.X = 4;
		mem[0xFF00] = opcode(Ins::CMP_INDX);
		mem[0xFF01] = 0x42;
		mem[0x42 + 4] = 0x00;
		mem[0x42 + 5] = 0x80;
		mem[0x8000] = Test.Operand;
		constexpr s64 EXPECTED_CYCLES = 6;
		CCPU CPUCopy = cpu;

		// when:
		const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

		// then:
		EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
		EXPECT_EQ( cpu.A, Test.RegisterValue );
		EXPECT_EQ( cpu.X, 4 );
		EXPECT_EQ( cpu.Flags.Z, Test.ExpectZ );
		EXPECT_EQ( cpu.Flags.N, Test.ExpectN );
		EXPECT_EQ( cpu.Flags.C, Test.ExpectC );
		ExpectUnaffectedRegisters( CPUCopy );
	}

	void CMPIndirectY( CMPTestData Test )
	{
		// given:
		using namespace m6502;
		cpu.Reset( 0xFF00 );
		cpu.Flags.C = !Test.ExpectC;
		cpu.Flags.Z = !Test.ExpectZ;
		cpu.Flags.N = !Test.ExpectN;
		cpu.A = Test.RegisterValue;
		cpu.Y = 4;
		mem[0xFF00] = opcode(Ins::CMP_INDY);
		mem[0xFF01] = 0x42;
		mem[0x42] = 0x00;
		mem[0x43] = 0x80;
		mem[0x8000+4] = Test.Operand;
		constexpr s64 EXPECTED_CYCLES = 5;
		CCPU CPUCopy = cpu;

		// when:
		const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

		// then:
		EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
		EXPECT_EQ( cpu.A, Test.RegisterValue );
		EXPECT_EQ( cpu.Y, 4 );
		EXPECT_EQ( cpu.Flags.Z, Test.ExpectZ );
		EXPECT_EQ( cpu.Flags.N, Test.ExpectN );
		EXPECT_EQ( cpu.Flags.C, Test.ExpectC );
		ExpectUnaffectedRegisters( CPUCopy );
	}
};

//-- Immediate

TEST_F( M6502CompareRegisterTests, CMPImmediateCanCompareTwoIdenticalValues  )
{
    CompareImmediate( CompareTwoIdenticalValues(), ERegister::A );
}

TEST_F( M6502CompareRegisterTests, CMPImmediateCanCompareALargePositiveToASmallPositive)
{
    CompareImmediate( CompareALargePositiveToASmallPositive(), ERegister::A );
}

TEST_F( M6502CompareRegisterTests, CMPImmediateCanCompareANegativeNumberToPositive)
{
    CompareImmediate( CompareANegativeNumberToAPositive(), ERegister::A );
}

TEST_F( M6502CompareRegisterTests, CMPImmediateCanCompareTwoValuesThatResultInANegativeFlagSet)
{
    CompareImmediate( CompareTwoValuesThatResultInANegativeFlagSet(), ERegister::A );
}

//-- Zero Page

TEST_F( M6502CompareRegisterTests, CMPZeroPageCanCompareTwoIdenticalValues  )
{
	CompareZeroPage( CompareTwoIdenticalValues(), ERegister::A );
}

TEST_F( M6502CompareRegisterTests, CMPZeroPageCanCompareALargePositiveToASmallPositive)
{
    CompareZeroPage( CompareALargePositiveToASmallPositive(), ERegister::A );
}

TEST_F( M6502CompareRegisterTests, CMPZeroPageCanCompareANegativeNumberToPositive)
{
	CompareZeroPage( CompareANegativeNumberToAPositive(), ERegister::A );
}

TEST_F( M6502CompareRegisterTests, CMPZeroPageCanCompareTwoValuesThatResultInNegative)
{
    CompareZeroPage( CompareTwoValuesThatResultInANegativeFlagSet(), ERegister::A );
}

//-- Zero Page X

TEST_F( M6502CompareRegisterTests, CMPZeroPageXCanCompareTwoIdenticalValues  )
{
    CMPZeroPageX( CompareTwoIdenticalValues() );
}

TEST_F( M6502CompareRegisterTests, CMPZeroPageXCanCompareALargePositiveToASmallPositive)
{
    CMPZeroPageX( CompareALargePositiveToASmallPositive() );
}

TEST_F( M6502CompareRegisterTests, CMPZeroPageXCanCompareANegativeNumberToPositive)
{
    CMPZeroPageX( CompareANegativeNumberToAPositive() );
}

TEST_F( M6502CompareRegisterTests, CMPZeroPageXCanCompareTwoValuesThatResultInNegative)
{
    CMPZeroPageX( CompareTwoValuesThatResultInANegativeFlagSet() );
}

//-- Absolute


TEST_F( M6502CompareRegisterTests, CMPAbsoluteCanCompareTwoIdenticalValues  )
{
    CompareAbsolute( CompareTwoIdenticalValues(), ERegister::A );
}

TEST_F( M6502CompareRegisterTests, CMPAbsoluteCanCompareALargePositiveToASmallPositive)
{
    CompareAbsolute( CompareALargePositiveToASmallPositive(), ERegister::A );
}

TEST_F( M6502CompareRegisterTests, CMPAbsoluteCanCompareANegativeNumberToPositive)
{
    CompareAbsolute( CompareANegativeNumberToAPositive(), ERegister::A );
}

TEST_F( M6502CompareRegisterTests, CMPAbsoluteCanCompareTwoValuesThatResultInNegative)
{
    CompareAbsolute( CompareTwoValuesThatResultInANegativeFlagSet(), ERegister::A );
}

//- Absolute X

TEST_F( M6502CompareRegisterTests, CMPAbsoluteXCanCompareTwoIdenticalValues  )
{
    CMPAbsoluteX( CompareTwoIdenticalValues() );
}

TEST_F( M6502CompareRegisterTests, CMPAbsoluteXCanCompareALargePositiveToASmallPositive)
{
    CMPAbsoluteX( CompareALargePositiveToASmallPositive() );
}

TEST_F( M6502CompareRegisterTests, CMPAbsoluteXCanCompareANegativeNumberToPositive)
{
    CMPAbsoluteX( CompareANegativeNumberToAPositive() );
}

TEST_F( M6502CompareRegisterTests, CMPAbsoluteXCanCompareTwoValuesThatResultInNegative)
{
    CMPAbsoluteX( CompareTwoValuesThatResultInANegativeFlagSet() );
}

//- Absolute Y

TEST_F( M6502CompareRegisterTests, CMPAbsoluteYCanCompareTwoIdenticalValues  )
{
    CMPAbsoluteY( CompareTwoIdenticalValues() );
}

TEST_F( M6502CompareRegisterTests, CMPAbsoluteYCanCompareALargePositiveToASmallPositive)
{
    CMPAbsoluteY( CompareALargePositiveToASmallPositive() );
}

TEST_F( M6502CompareRegisterTests, CMPAbsoluteYCanCompareANegativeNumberToPositive)
{
    CMPAbsoluteY( CompareANegativeNumberToAPositive() );
}

TEST_F( M6502CompareRegisterTests, CMPAbsoluteYCanCompareTwoValuesThatResultInNegative)
{
    CMPAbsoluteY( CompareTwoValuesThatResultInANegativeFlagSet() );
}

//-- Indirect X

TEST_F( M6502CompareRegisterTests, CMPIndirectXCanCompareTwoIdenticalValues )
{
	CMPTestData Test = CompareTwoIdenticalValues();
	CMPIndirectX( Test );
}

TEST_F( M6502CompareRegisterTests, CMPIndirectXCanCompareALargePositiveToASmallPositive )
{
	CMPTestData Test = CompareALargePositiveToASmallPositive();
	CMPIndirectX( Test );
}

TEST_F( M6502CompareRegisterTests, CMPIndirectXCanCompareANegativeNumberToAPositive )
{
	CMPTestData Test = CompareANegativeNumberToAPositive();
	CMPIndirectX( Test );
}

TEST_F( M6502CompareRegisterTests, CMPIndirectXCanCompareTwoValuesThatResultInANegativeFlagSet )
{
	CMPTestData Test = CompareTwoValuesThatResultInANegativeFlagSet();
	CMPIndirectX( Test );
}

//-- Indirect Y

TEST_F( M6502CompareRegisterTests, CMPIndirectYCanCompareTwoIdenticalValues )
{
	CMPTestData Test = CompareTwoIdenticalValues();
	CMPIndirectY( Test );
}

TEST_F( M6502CompareRegisterTests, CMPIndirectYCanCompareALargePositiveToASmallPositive )
{
	CMPTestData Test = CompareALargePositiveToASmallPositive();
	CMPIndirectY( Test );
}

TEST_F( M6502CompareRegisterTests, CMPIndirectYCanCompareANegativeNumberToAPositive )
{
	CMPTestData Test = CompareANegativeNumberToAPositive();
	CMPIndirectY( Test );
}

TEST_F( M6502CompareRegisterTests, CMPIndirectYCanCompareTwoValuesThatResultInANegativeFlagSet )
{
	CMPTestData Test = CompareTwoValuesThatResultInANegativeFlagSet();
	CMPIndirectY( Test );
}

//-- CPX Immediate

TEST_F( M6502CompareRegisterTests, CPXImmediateCanCompareTwoIdenticalValues )
{
	CMPTestData Test = CompareTwoIdenticalValues();
	CompareImmediate( Test, ERegister::X );
}

TEST_F( M6502CompareRegisterTests, CPXImmediateCanCompareALargePositiveToASmallPositive )
{
	CMPTestData Test = CompareALargePositiveToASmallPositive();
	CompareImmediate( Test, ERegister::X );
}

TEST_F( M6502CompareRegisterTests, CPXImmediateCanCompareANegativeNumberToAPositive )
{
	CMPTestData Test = CompareANegativeNumberToAPositive();
	CompareImmediate( Test, ERegister::X );
}

TEST_F( M6502CompareRegisterTests, CPXImmediateCanCompareTwoValuesThatResultInANegativeFlagSet )
{
	CMPTestData Test = CompareTwoValuesThatResultInANegativeFlagSet();
	CompareImmediate( Test, ERegister::X );
}

//-- CPY Immediate

TEST_F( M6502CompareRegisterTests, CPYImmediateCanCompareTwoIdenticalValues )
{
	CMPTestData Test = CompareTwoIdenticalValues();
	CompareImmediate( Test, ERegister::Y );
}

TEST_F( M6502CompareRegisterTests, CPYImmediateCanCompareALargePositiveToASmallPositive )
{
	CMPTestData Test = CompareALargePositiveToASmallPositive();
	CompareImmediate( Test, ERegister::Y );
}

TEST_F( M6502CompareRegisterTests, CPYImmediateCanCompareANegativeNumberToAPositive )
{
	CMPTestData Test = CompareANegativeNumberToAPositive();
	CompareImmediate( Test, ERegister::Y );
}

TEST_F( M6502CompareRegisterTests, CPYImmediateCanCompareTwoValuesThatResultInANegativeFlagSet )
{
	CMPTestData Test = CompareTwoValuesThatResultInANegativeFlagSet();
	CompareImmediate( Test, ERegister::Y );
}

//-- CPX Zero Page

TEST_F( M6502CompareRegisterTests, CPXZeroPageCanCompareTwoIdenticalValues )
{
	CMPTestData Test = CompareTwoIdenticalValues();
	CompareZeroPage( Test, ERegister::X );
}

TEST_F( M6502CompareRegisterTests, CPXZeroPageCanCompareALargePositiveToASmallPositive )
{
	CMPTestData Test = CompareALargePositiveToASmallPositive();
	CompareZeroPage( Test, ERegister::X );
}

TEST_F( M6502CompareRegisterTests, CPXZeroPageCanCompareANegativeNumberToAPositive )
{
	CMPTestData Test = CompareANegativeNumberToAPositive();
	CompareZeroPage( Test, ERegister::X );
}

TEST_F( M6502CompareRegisterTests, CPXZeroPageCanCompareTwoValuesThatResultInANegativeFlagSet )
{
	CMPTestData Test = CompareTwoValuesThatResultInANegativeFlagSet();
	CompareZeroPage( Test, ERegister::X );
}

//-- CPY Zero Page

TEST_F( M6502CompareRegisterTests, CPYZeroPageCanCompareTwoIdenticalValues )
{
	CMPTestData Test = CompareTwoIdenticalValues();
	CompareZeroPage( Test, ERegister::Y );
}

TEST_F( M6502CompareRegisterTests, CPYZeroPageCanCompareALargePositiveToASmallPositive )
{
	CMPTestData Test = CompareALargePositiveToASmallPositive();
	CompareZeroPage( Test, ERegister::Y );
}

TEST_F( M6502CompareRegisterTests, CPYZeroPageCanCompareANegativeNumberToAPositive )
{
	CMPTestData Test = CompareANegativeNumberToAPositive();
	CompareZeroPage( Test, ERegister::Y );
}

TEST_F( M6502CompareRegisterTests, CPYZeroPageCanCompareTwoValuesThatResultInANegativeFlagSet )
{
	CMPTestData Test = CompareTwoValuesThatResultInANegativeFlagSet();
	CompareZeroPage( Test, ERegister::Y );
}

//-- CPX Absolute

TEST_F( M6502CompareRegisterTests, CPXAbsoluteCanCompareTwoIdenticalValues )
{
	CMPTestData Test = CompareTwoIdenticalValues();
	CompareAbsolute( Test, ERegister::X );
}

TEST_F( M6502CompareRegisterTests, CPXAbsoluteCanCompareALargePositiveToASmallPositive )
{
	CMPTestData Test = CompareALargePositiveToASmallPositive();
	CompareAbsolute( Test, ERegister::X );
}

TEST_F( M6502CompareRegisterTests, CPXAbsoluteCanCompareANegativeNumberToAPositive )
{
	CMPTestData Test = CompareANegativeNumberToAPositive();
	CompareAbsolute( Test, ERegister::X );
}

TEST_F( M6502CompareRegisterTests, CPXAbsoluteCanCompareTwoValuesThatResultInANegativeFlagSet )
{
	CMPTestData Test = CompareTwoValuesThatResultInANegativeFlagSet();
	CompareAbsolute( Test, ERegister::X );
}

//-- CPY Absolute

TEST_F( M6502CompareRegisterTests, CPYAbsoluteCanCompareTwoIdenticalValues )
{
	CMPTestData Test = CompareTwoIdenticalValues();
	CompareAbsolute( Test, ERegister::Y );
}

TEST_F( M6502CompareRegisterTests, CPYAbsoluteCanCompareALargePositiveToASmallPositive )
{
	CMPTestData Test = CompareALargePositiveToASmallPositive();
	CompareAbsolute( Test, ERegister::Y );
}

TEST_F( M6502CompareRegisterTests, CPYAbsoluteCanCompareANegativeNumberToAPositive )
{
	CMPTestData Test = CompareANegativeNumberToAPositive();
	CompareAbsolute( Test, ERegister::Y );
}

TEST_F( M6502CompareRegisterTests, CPYAbsoluteCanCompareTwoValuesThatResultInANegativeFlagSet )
{
	CMPTestData Test = CompareTwoValuesThatResultInANegativeFlagSet();
	CompareAbsolute( Test, ERegister::Y );
}
