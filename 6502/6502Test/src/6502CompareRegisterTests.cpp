#include <gtest/gtest.h>
#include <m6502/System.hpp>

class M6502CompareRegisterTests : public testing::Test
{
public:
    M6502CompareRegisterTests() : cpu(mem) {}
    m6502::Mem mem;
    m6502::CPU cpu;

    virtual void SetUp()
    {
        cpu.Reset();
    }

    virtual void TearDown()
    {
    }



    void ExpectUnaffectedRegisters( m6502::CPU CPUBefore )
    {
        EXPECT_EQ( CPUBefore.Flags.I, cpu.Flags.I );
        EXPECT_EQ( CPUBefore.Flags.D, cpu.Flags.D );
        EXPECT_EQ( CPUBefore.Flags.B, cpu.Flags.B );
        EXPECT_EQ( CPUBefore.Flags.V, cpu.Flags.V );
    }
    
    struct CMPTestData
    {
        bool Carry;
        m6502::Byte A;
        m6502::Byte Operand;

        bool ExpectC;
        bool ExpectZ;
        bool ExpectN;
    };

    CMPTestData CompareTwoIdenticalValues()
    {
        CMPTestData Test;
        Test.A = 26;
        Test.Operand = 26;
        Test.ExpectC = true;
        Test.ExpectZ = true;
        Test.ExpectN = false;
        return Test;
    }

    CMPTestData CompareALargePositiveToSmallPositive()
    {
        CMPTestData Test;
        Test.A = 48;
        Test.Operand = 26;
        Test.ExpectC = true;
        Test.ExpectZ = false;
        Test.ExpectN = false;
        return Test;
    }

    CMPTestData CompareANegativeNumberToPositive()
    {
        CMPTestData Test;
        Test.A = 130;
        Test.Operand = 26;
        Test.ExpectC = true;
        Test.ExpectZ = false;
        Test.ExpectN = false;
        return Test;
    }

    CMPTestData CompareTwoValuesThatResultInNegative()
    {
        CMPTestData Test;
        Test.A = 8;
        Test.Operand = 26;
        Test.ExpectC = false;
        Test.ExpectZ = false;
        Test.ExpectN = true;
        return Test;
    }

    void CMPImmediate (CMPTestData Test)
    {
        // given:
        using namespace m6502;
        cpu.Reset( 0xFF00 );
        cpu.Flags.C = !Test.ExpectC;
        cpu.Flags.Z = !Test.ExpectZ;
        cpu.Flags.N = !Test.ExpectN;
        cpu.A = Test.A;
        mem[0xFF00] = opcode(Ins::CMP);
        mem[0xFF01] = Test.Operand;
        constexpr s64 EXPECTED_CYCLES = 2;
        CPU CPUCopy = cpu;

        // when:
        const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

        // then:
        EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
        EXPECT_EQ( cpu.A, Test.A );
        EXPECT_EQ( cpu.Flags.C , Test.ExpectC );
        EXPECT_EQ( cpu.Flags.Z , Test.ExpectZ );
        EXPECT_EQ( cpu.Flags.N , Test.ExpectN );
        ExpectUnaffectedRegisters(CPUCopy);
    }

    void CMPZeroPage (CMPTestData Test)
    {
        // given:
        using namespace m6502;
        cpu.Reset( 0xFF00 );
        cpu.Flags.C = !Test.ExpectC;
        cpu.Flags.Z = !Test.ExpectZ;
        cpu.Flags.N = !Test.ExpectN;
        cpu.A = Test.A;
        mem[0xFF00] = opcode(Ins::CMP_ZP);
        mem[0xFF01] = 0x42;
        mem[0x0042] = Test.Operand;
        constexpr s64 EXPECTED_CYCLES = 3;
        CPU CPUCopy = cpu;

        // when:
        const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

        // then:
        EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
        EXPECT_EQ( cpu.A, Test.A );
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
        cpu.A = Test.A;
        cpu.X = 0x10;
        mem[0xFF00] = opcode(Ins::CMP_ZPX);
        mem[0xFF01] = 0x42;
        mem[0x0052] = Test.Operand;
        constexpr s64 EXPECTED_CYCLES = 4;
        CPU CPUCopy = cpu;

        // when:
        const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

        // then:
        EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
        EXPECT_EQ( cpu.A, Test.A );
        EXPECT_EQ( cpu.Flags.C , Test.ExpectC );
        EXPECT_EQ( cpu.Flags.Z , Test.ExpectZ );
        EXPECT_EQ( cpu.Flags.N , Test.ExpectN );
        ExpectUnaffectedRegisters(CPUCopy);
    }

    void CMPAbsolute (CMPTestData Test)
    {
        // given:
        using namespace m6502;
        cpu.Reset( 0xFF00 );
        cpu.Flags.C = !Test.ExpectC;
        cpu.Flags.Z = !Test.ExpectZ;
        cpu.Flags.N = !Test.ExpectN;
        cpu.A = Test.A;
        mem[0xFF00] = opcode(Ins::CMP_ABS);
        mem[0xFF01] = 0x00;
        mem[0xFF02] = 0x20;
        mem[0x2000] = Test.Operand;
        constexpr s64 EXPECTED_CYCLES = 4;
        CPU CPUCopy = cpu;

        // when:
        const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

        // then:
        EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
        EXPECT_EQ( cpu.A, Test.A );
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
        cpu.A = Test.A;
        cpu.X = 0x10;
        mem[0xFF00] = opcode(Ins::CMP_ABSX);
        mem[0xFF01] = 0x00;
        mem[0xFF02] = 0x20;
        mem[0x2010] = Test.Operand;
        constexpr s64 EXPECTED_CYCLES = 4;
        CPU CPUCopy = cpu;

        // when:
        const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

        // then:
        EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
        EXPECT_EQ( cpu.A, Test.A );
        EXPECT_EQ( cpu.Flags.C , Test.ExpectC );
        EXPECT_EQ( cpu.Flags.Z , Test.ExpectZ );
        EXPECT_EQ( cpu.Flags.N , Test.ExpectN );
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
        cpu.A = Test.A;
        cpu.Y = 0x10;
        mem[0xFF00] = opcode(Ins::CMP_ABSY);
        mem[0xFF01] = 0x00;
        mem[0xFF02] = 0x20;
        mem[0x2010] = Test.Operand;
        constexpr s64 EXPECTED_CYCLES = 4;
        CPU CPUCopy = cpu;

        // when:
        const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

        // then:
        EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
        EXPECT_EQ( cpu.A, Test.A );
        EXPECT_EQ( cpu.Flags.C , Test.ExpectC );
        EXPECT_EQ( cpu.Flags.Z , Test.ExpectZ );
        EXPECT_EQ( cpu.Flags.N , Test.ExpectN );
        ExpectUnaffectedRegisters(CPUCopy);
    }
};

//-- Immediate

TEST_F( M6502CompareRegisterTests, CMPImmediateCanCompareTwoIdenticalValues  )
{
    CMPImmediate( CompareTwoIdenticalValues() );
}

TEST_F( M6502CompareRegisterTests, CMPImmediateCanCompareALargePositiveToASmallPositive)
{
    CMPImmediate( CompareALargePositiveToSmallPositive() );
}

TEST_F( M6502CompareRegisterTests, CMPImmediateCanCompareANegativeNumberToPositive)
{
    CMPImmediate( CompareANegativeNumberToPositive() );
}

TEST_F( M6502CompareRegisterTests, CMPImmediateCanCompareTwoValuesThatResultInNegative)
{
    CMPImmediate( CompareTwoValuesThatResultInNegative() );
}

//-- Zero Page

TEST_F( M6502CompareRegisterTests, CMP_ZeroPageCanCompareTwoIdenticalValues  )
{
    CMPZeroPage( CompareTwoIdenticalValues() );
}

TEST_F( M6502CompareRegisterTests, CMP_ZeroPageCanCompareALargePositiveToASmallPositive)
{
    CMPZeroPage( CompareALargePositiveToSmallPositive() );
}

TEST_F( M6502CompareRegisterTests, CMP_ZeroPageCanCompareANegativeNumberToPositive)
{
    CMPZeroPage( CompareANegativeNumberToPositive() );
}

TEST_F( M6502CompareRegisterTests, CMP_ZeroPageCanCompareTwoValuesThatResultInNegative)
{
    CMPZeroPage( CompareTwoValuesThatResultInNegative() );
}

//-- Zero Page X

TEST_F( M6502CompareRegisterTests, CMP_ZeroPageXCanCompareTwoIdenticalValues  )
{
    CMPZeroPageX( CompareTwoIdenticalValues() );
}

TEST_F( M6502CompareRegisterTests, CMP_ZeroPageXCanCompareALargePositiveToASmallPositive)
{
    CMPZeroPageX( CompareALargePositiveToSmallPositive() );
}

TEST_F( M6502CompareRegisterTests, CMP_ZeroPageXCanCompareANegativeNumberToPositive)
{
    CMPZeroPageX( CompareANegativeNumberToPositive() );
}

TEST_F( M6502CompareRegisterTests, CMP_ZeroPageXCanCompareTwoValuesThatResultInNegative)
{
    CMPZeroPageX( CompareTwoValuesThatResultInNegative() );
}

//-- Absolute


TEST_F( M6502CompareRegisterTests, CMP_AbsoluteCanCompareTwoIdenticalValues  )
{
    CMPAbsolute( CompareTwoIdenticalValues() );
}

TEST_F( M6502CompareRegisterTests, CMP_AbsoluteCanCompareALargePositiveToASmallPositive)
{
    CMPAbsolute( CompareALargePositiveToSmallPositive() );
}

TEST_F( M6502CompareRegisterTests, CMP_AbsoluteCanCompareANegativeNumberToPositive)
{
    CMPAbsolute( CompareANegativeNumberToPositive() );
}

TEST_F( M6502CompareRegisterTests, CMP_AbsoluteCanCompareTwoValuesThatResultInNegative)
{
    CMPAbsolute( CompareTwoValuesThatResultInNegative() );
}

//- Absolute X

TEST_F( M6502CompareRegisterTests, CMP_AbsoluteXCanCompareTwoIdenticalValues  )
{
    CMPAbsoluteX( CompareTwoIdenticalValues() );
}

TEST_F( M6502CompareRegisterTests, CMP_AbsoluteXCanCompareALargePositiveToASmallPositive)
{
    CMPAbsoluteX( CompareALargePositiveToSmallPositive() );
}

TEST_F( M6502CompareRegisterTests, CMP_AbsoluteXCanCompareANegativeNumberToPositive)
{
    CMPAbsoluteX( CompareANegativeNumberToPositive() );
}

TEST_F( M6502CompareRegisterTests, CMP_AbsoluteXCanCompareTwoValuesThatResultInNegative)
{
    CMPAbsoluteX( CompareTwoValuesThatResultInNegative() );
}

//- Absolute Y

TEST_F( M6502CompareRegisterTests, CMP_AbsoluteYCanCompareTwoIdenticalValues  )
{
    CMPAbsoluteY( CompareTwoIdenticalValues() );
}

TEST_F( M6502CompareRegisterTests, CMP_AbsoluteYCanCompareALargePositiveToASmallPositive)
{
    CMPAbsoluteY( CompareALargePositiveToSmallPositive() );
}

TEST_F( M6502CompareRegisterTests, CMP_AbsoluteYCanCompareANegativeNumberToPositive)
{
    CMPAbsoluteY( CompareANegativeNumberToPositive() );
}

TEST_F( M6502CompareRegisterTests, CMP_AbsoluteYCanCompareTwoValuesThatResultInNegative)
{
    CMPAbsoluteY( CompareTwoValuesThatResultInNegative() );
}