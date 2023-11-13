#include <gtest/gtest.h>
#include <m6502/System.hpp>

class M6502LoadRegisterTests : public testing::Test
{
public:	
    M6502LoadRegisterTests() : cpu(bus), mem(bus,0x0000,0x0000) {}
    m6502::CBus bus;
    m6502::CMem mem;
    m6502::CCPU cpu;;

    virtual void SetUp()
    {
        cpu.Reset();
    }

    virtual void TearDown()
    {
    }

    void TestLoadRegisterImmediate( 
        m6502::Ins Opcode, 
        m6502::Byte m6502::CRegisters::*RegisterToTest );

    void TestLoadRegisterZeroPage(
        m6502::Ins OpcodeToTest,
        m6502::Byte m6502::CRegisters::*RegisterToTest );

    void TestLoadRegisterZeroPageX(
        m6502::Ins OpcodeToTest,
        m6502::Byte m6502::CRegisters::*RegisterToTest );

    void TestLoadRegisterZeroPageY(
        m6502::Ins OpcodeToTest,
        m6502::Byte m6502::CRegisters::*RegisterToTest );

    void TestLoadRegisterAbsolute(
        m6502::Ins OpcodeToTest,
        m6502::Byte m6502::CRegisters::*RegisterToTest );

    void TestLoadRegisterAbsoluteX(
        m6502::Ins OpcodeToTest,
        m6502::Byte m6502::CRegisters::*RegisterToTest );

    void TestLoadRegisterAbsoluteY(
        m6502::Ins OpcodeToTest,
        m6502::Byte m6502::CRegisters::*RegisterToTest );

    void TestLoadRegisterAbsoluteYWhenCrossingPage(
        m6502::Ins OpcodeToTest,
        m6502::Byte m6502::CRegisters::*RegisterToTest );

    void TestLoadRegisterAbsoluteXWhenCrossingPage(
        m6502::Ins OpcodeToTest,
        m6502::Byte m6502::CRegisters::*RegisterToTest );
};

static void VerfifyUnmodifiedFlagsFromLoadRegister( 
    const m6502::CCPU& cpu, 
    const m6502::CCPU& CPUCopy )
{
    EXPECT_EQ( cpu.Flags.C, CPUCopy.Flags.C );
    EXPECT_EQ( cpu.Flags.I, CPUCopy.Flags.I );
    EXPECT_EQ( cpu.Flags.D, CPUCopy.Flags.D );
    EXPECT_EQ( cpu.Flags.B, CPUCopy.Flags.B );
    EXPECT_EQ( cpu.Flags.V, CPUCopy.Flags.V );
}

TEST_F( M6502LoadRegisterTests, TheCPUDoesNothingWhenWeExecuteZeroCycles )
{
    //given:
    using namespace m6502;
    constexpr s64 NUM_CYCLES = 0;

    //when:
    s64 CyclesUsed = cpu.Execute( NUM_CYCLES );

    //then:
    EXPECT_EQ( CyclesUsed, 0 );
}

TEST_F( M6502LoadRegisterTests, CPUCanExecuteMoreCyclesThanRequestedIfRequiredByTheInstruction )
{
    // given:
    using namespace m6502;
    mem[0xFFFC] = opcode(Ins::LDA_IM);
    mem[0xFFFD] = 0x84;
    //CPU CPUCopy = cpu;
    constexpr s32 NUM_CYCLES = 1;

    //when:
    s64 CyclesUsed = cpu.Execute( NUM_CYCLES );

    //then:
    EXPECT_EQ( CyclesUsed, 2 );
}

void M6502LoadRegisterTests::TestLoadRegisterImmediate( 
    m6502::Ins OpcodeToTest,  
    m6502::Byte m6502::CRegisters::*RegisterToTest )
{
    // given:
    using namespace m6502;
    mem[0xFFFC] = opcode(OpcodeToTest);
    mem[0xFFFD] = 0x84;

    //when:
    CCPU CPUCopy = cpu;
    s64 CyclesUsed = cpu.Execute( 2 );

    //then:
    EXPECT_EQ( cpu.*RegisterToTest, 0x84 );
    EXPECT_EQ( CyclesUsed, 2 );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_TRUE( cpu.Flags.N );
    VerfifyUnmodifiedFlagsFromLoadRegister( cpu, CPUCopy );
}

TEST_F( M6502LoadRegisterTests, LDAImmediateCanLoadAValueIntoTheARegister )
{
    using namespace m6502;
    TestLoadRegisterImmediate( Ins::LDA_IM, &CRegisters::A );
}

TEST_F( M6502LoadRegisterTests, LDXImmediateCanLoadAValueIntoTheXRegister )
{
    using namespace m6502;
    TestLoadRegisterImmediate( Ins::LDX_IM, &CRegisters::X );
}

TEST_F( M6502LoadRegisterTests, LDYImmediateCanLoadAValueIntoTheYRegister )
{
    using namespace m6502;
    TestLoadRegisterImmediate( Ins::LDY_IM, &CRegisters::Y );
}

void M6502LoadRegisterTests::TestLoadRegisterZeroPage(
    m6502::Ins OpcodeToTest,
    m6502::Byte m6502::CRegisters::*RegisterToTest )
{
    // given:
    using namespace m6502;
    mem[0xFFFC] = opcode(OpcodeToTest);
    mem[0xFFFD] = 0x42;
    mem[0x0042] = 0x37;

    //when:
    CCPU CPUCopy = cpu;
    s64 CyclesUsed = cpu.Execute( 3 );

    //then:
    EXPECT_EQ( cpu.*RegisterToTest, 0x37 );
    EXPECT_EQ( CyclesUsed, 3 );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_FALSE( cpu.Flags.N );
    VerfifyUnmodifiedFlagsFromLoadRegister( cpu, CPUCopy );
}

TEST_F( M6502LoadRegisterTests, LDAZeroPageCanLoadAValueIntoTheARegister )
{
    using namespace m6502;
    TestLoadRegisterZeroPage( Ins::LDA_ZP, &CRegisters::A );
}

TEST_F( M6502LoadRegisterTests, LDXZeroPageCanLoadAValueIntoTheXRegister )
{
    using namespace m6502;
    TestLoadRegisterZeroPage( Ins::LDX_ZP, &CRegisters::X );
}

TEST_F( M6502LoadRegisterTests, LDYZeroPageCanLoadAValueIntoTheYRegister )
{
    using namespace m6502;
    TestLoadRegisterZeroPage( Ins::LDY_ZP, &CRegisters::Y );
}

TEST_F( M6502LoadRegisterTests, LDAImmediateCanAffectTheZeroFlag )
{
    // given:
    using namespace m6502;
    cpu.A = 0x44;
    mem[0xFFFC] = opcode(Ins::LDA_IM);
    mem[0xFFFD] = 0x0;
    CCPU CPUCopy = cpu;

    //when:
    cpu.Execute( 2 );

    //then:
    EXPECT_TRUE( cpu.Flags.Z );
    EXPECT_FALSE( cpu.Flags.N );
    VerfifyUnmodifiedFlagsFromLoadRegister( cpu, CPUCopy );
}

void M6502LoadRegisterTests::TestLoadRegisterZeroPageX(
    m6502::Ins OpcodeToTest,
    m6502::Byte m6502::CRegisters::*RegisterToTest )
{
    // given:
    using namespace m6502;
    cpu.X = 5;
    mem[0xFFFC] = opcode(OpcodeToTest);
    mem[0xFFFD] = 0x42;
    mem[0x0047] = 0x37;
    CCPU CPUCopy = cpu;

    //when:
    s64 CyclesUsed = cpu.Execute( 4 );

    //then:
    EXPECT_EQ( cpu.*RegisterToTest, 0x37 );
    EXPECT_EQ( CyclesUsed, 4 );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_FALSE( cpu.Flags.N );
    VerfifyUnmodifiedFlagsFromLoadRegister( cpu, CPUCopy );
}

void M6502LoadRegisterTests::TestLoadRegisterZeroPageY(
    m6502::Ins OpcodeToTest,
    m6502::Byte m6502::CRegisters::*RegisterToTest )
{
    // given:
    using namespace m6502;
    cpu.Y = 5;
    mem[0xFFFC] = opcode(OpcodeToTest);
    mem[0xFFFD] = 0x42;
    mem[0x0047] = 0x37;
    CCPU CPUCopy = cpu;

    //when:
    s64 CyclesUsed = cpu.Execute( 4 );

    //then:
    EXPECT_EQ( cpu.*RegisterToTest, 0x37 );
    EXPECT_EQ( CyclesUsed, 4 );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_FALSE( cpu.Flags.N );
    VerfifyUnmodifiedFlagsFromLoadRegister( cpu, CPUCopy );
}

TEST_F( M6502LoadRegisterTests, LDAZeroPageXCanLoadAValueIntoTheARegister )
{
    using namespace m6502;
    TestLoadRegisterZeroPageX( Ins::LDA_ZPX, &CRegisters::A );
}

TEST_F( M6502LoadRegisterTests, LDXZeroPageYCanLoadAValueIntoTheXRegister )
{
    using namespace m6502;
    TestLoadRegisterZeroPageY( Ins::LDX_ZPY, &CRegisters::X );
}

TEST_F( M6502LoadRegisterTests, LDYZeroPageXCanLoadAValueIntoTheYRegister )
{
    using namespace m6502;
    TestLoadRegisterZeroPageX( Ins::LDY_ZPX, &CRegisters::Y );
}

TEST_F( M6502LoadRegisterTests, LDAZeroPageXCanLoadAValueIntoTheARegisterWhenItWraps )
{
    // given:
    using namespace m6502;
    cpu.X = 0xFF;
    mem[0xFFFC] = opcode(Ins::LDA_ZPX);
    mem[0xFFFD] = 0x80;
    mem[0x007F] = 0x37;

    //when:
    CCPU CPUCopy = cpu;
    s64 CyclesUsed = cpu.Execute( 4 );

    //then:
    EXPECT_EQ( cpu.A, 0x37 );
    EXPECT_EQ( CyclesUsed, 4 );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_FALSE( cpu.Flags.N );
    VerfifyUnmodifiedFlagsFromLoadRegister( cpu, CPUCopy );
}

void M6502LoadRegisterTests::TestLoadRegisterAbsolute(
    m6502::Ins OpcodeToTest,
    m6502::Byte m6502::CRegisters::*RegisterToTest )
{
    // given:
    cpu.Flags.Z = cpu.Flags.N = true;
    using namespace m6502;
    mem[0xFFFC] = opcode(OpcodeToTest);
    mem[0xFFFD] = 0x80;
    mem[0xFFFE] = 0x44;	//0x4480
    mem[0x4480] = 0x37;
    constexpr s32 EXPECTED_CYCLES = 4;
    CCPU CPUCopy = cpu;

    //when:
    s64 CyclesUsed = cpu.Execute( EXPECTED_CYCLES );

    //then:
    EXPECT_EQ( cpu.*RegisterToTest, 0x37 );
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_FALSE( cpu.Flags.N );
    VerfifyUnmodifiedFlagsFromLoadRegister( cpu, CPUCopy );
}

TEST_F( M6502LoadRegisterTests, LDAAbsoluteCanLoadAValueIntoTheARegister )
{
    using namespace m6502;
    TestLoadRegisterAbsolute( Ins::LDA_ABS, &CRegisters::A );
}

TEST_F( M6502LoadRegisterTests, LDXAbsoluteCanLoadAValueIntoTheXRegister )
{
    using namespace m6502;
    TestLoadRegisterAbsolute( Ins::LDX_ABS, &CRegisters::X );
}

TEST_F( M6502LoadRegisterTests, LDYAbsoluteCanLoadAValueIntoTheYRegister )
{
    using namespace m6502;
    TestLoadRegisterAbsolute( Ins::LDY_ABS, &CRegisters::Y );
}

void M6502LoadRegisterTests::TestLoadRegisterAbsoluteX(
    m6502::Ins OpcodeToTest,
    m6502::Byte m6502::CRegisters::*RegisterToTest )
{
    // given:
    cpu.Flags.Z = cpu.Flags.N = true;
    using namespace m6502;
    cpu.X = 1;
    mem[0xFFFC] = opcode(OpcodeToTest);
    mem[0xFFFD] = 0x80;
    mem[0xFFFE] = 0x44;	//0x4480
    mem[0x4481] = 0x37;
    constexpr s64 EXPECTED_CYCLES = 4;
    CCPU CPUCopy = cpu;

    //when:
    s64 CyclesUsed = cpu.Execute( EXPECTED_CYCLES );

    //then:
    EXPECT_EQ( cpu.*RegisterToTest, 0x37 );
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_FALSE( cpu.Flags.N );
    VerfifyUnmodifiedFlagsFromLoadRegister( cpu, CPUCopy );
}

void M6502LoadRegisterTests::TestLoadRegisterAbsoluteY(
    m6502::Ins OpcodeToTest,
    m6502::Byte m6502::CRegisters::*RegisterToTest )
{
    // given:
    using namespace m6502;
    cpu.Flags.Z = cpu.Flags.N = true;
    cpu.Y = 1;
    mem[0xFFFC] = opcode(OpcodeToTest);
    mem[0xFFFD] = 0x80;
    mem[0xFFFE] = 0x44;	//0x4480
    mem[0x4481] = 0x37;
    constexpr s64 EXPECTED_CYCLES = 4;
    CCPU CPUCopy = cpu;

    //when:
    s64 CyclesUsed = cpu.Execute( EXPECTED_CYCLES );

    //then:
    EXPECT_EQ( cpu.*RegisterToTest, 0x37 );
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_FALSE( cpu.Flags.N );
    VerfifyUnmodifiedFlagsFromLoadRegister( cpu, CPUCopy );
}

TEST_F( M6502LoadRegisterTests, LDAAbsoluteXCanLoadAValueIntoTheARegister )
{
    using namespace m6502;
    TestLoadRegisterAbsoluteX( Ins::LDA_ABSX, &CRegisters::A );
}

TEST_F( M6502LoadRegisterTests, LDXAbsoluteYCanLoadAValueIntoTheXRegister )
{
    using namespace m6502;
    TestLoadRegisterAbsoluteY( Ins::LDX_ABSY, &CRegisters::X );
}

TEST_F( M6502LoadRegisterTests, LDYAbsoluteXCanLoadAValueIntoTheYRegister )
{
    using namespace m6502;
    TestLoadRegisterAbsoluteX( Ins::LDY_ABSX, &CRegisters::Y );
}

void M6502LoadRegisterTests::TestLoadRegisterAbsoluteXWhenCrossingPage(
    m6502::Ins OpcodeToTest,
    m6502::Byte m6502::CRegisters::*RegisterToTest )
{
    // given:
    using namespace m6502;
    cpu.X = 0x1;
    mem[0xFFFC] = opcode(OpcodeToTest);
    mem[0xFFFD] = 0xFF;
    mem[0xFFFE] = 0x44;	//0x44FF
    mem[0x4500] = 0x37;	//0x44FF+0x1 crosses page boundary!
    constexpr s64 EXPECTED_CYCLES = 5;
    CCPU CPUCopy = cpu;

    //when:
    s64 CyclesUsed = cpu.Execute( EXPECTED_CYCLES );

    //then:
    EXPECT_EQ( cpu.*RegisterToTest, 0x37 );
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_FALSE( cpu.Flags.N );
    VerfifyUnmodifiedFlagsFromLoadRegister( cpu, CPUCopy );
}

TEST_F( M6502LoadRegisterTests, LDAAbsoluteXCanLoadAValueIntoTheARegisterWhenItCrossesAPageBoundary )
{
    using namespace m6502;
    TestLoadRegisterAbsoluteXWhenCrossingPage( Ins::LDA_ABSX, &CRegisters::A );
}

TEST_F( M6502LoadRegisterTests, LDYAbsoluteXCanLoadAValueIntoTheYRegisterWhenItCrossesAPageBoundary )
{
    using namespace m6502;
    TestLoadRegisterAbsoluteXWhenCrossingPage( Ins::LDY_ABSX, &CRegisters::Y );
}

TEST_F( M6502LoadRegisterTests, LDAAbsoluteYCanLoadAValueIntoTheARegister )
{
    using namespace m6502;
    TestLoadRegisterAbsoluteY( Ins::LDA_ABSY, &CRegisters::A );
}

void M6502LoadRegisterTests::TestLoadRegisterAbsoluteYWhenCrossingPage(
    m6502::Ins OpcodeToTest,
    m6502::Byte m6502::CRegisters::*RegisterToTest )
{
    // given:
    using namespace m6502;
    cpu.Y = 0x1;
    mem[0xFFFC] = opcode(OpcodeToTest);
    mem[0xFFFD] = 0xFF;
    mem[0xFFFE] = 0x44;	//0x44FF
    mem[0x4500] = 0x37;	//0x44FF+0x1 crosses page boundary!
    constexpr s64 EXPECTED_CYCLES = 5;
    CCPU CPUCopy = cpu;

    //when:
    s64 CyclesUsed = cpu.Execute( EXPECTED_CYCLES );

    //then:
    EXPECT_EQ( cpu.*RegisterToTest, 0x37 );
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_FALSE( cpu.Flags.N );
    VerfifyUnmodifiedFlagsFromLoadRegister( cpu, CPUCopy );
}

TEST_F( M6502LoadRegisterTests, LDAAbsoluteYCanLoadAValueIntoTheARegisterWhenItCrossesAPageBoundary )
{
    using namespace m6502;
    TestLoadRegisterAbsoluteYWhenCrossingPage( Ins::LDA_ABSY, &CRegisters::A );
}

TEST_F( M6502LoadRegisterTests, LDXAbsoluteYCanLoadAValueIntoTheXRegisterWhenItCrossesAPageBoundary )
{
    using namespace m6502;
    TestLoadRegisterAbsoluteYWhenCrossingPage( Ins::LDX_ABSY, &CRegisters::X );
}

TEST_F( M6502LoadRegisterTests, LDAIndirectXCanLoadAValueIntoTheARegister )
{
    // given:
    using namespace m6502;
    cpu.Flags.Z = cpu.Flags.N = true;
    cpu.X = 0x04;
    mem[0xFFFC] = opcode(Ins::LDA_INDX);
    mem[0xFFFD] = 0x02;
    mem[0x0006] = 0x00;	//0x2 + 0x4
    mem[0x0007] = 0x80;	
    mem[0x8000] = 0x37;
    constexpr s64 EXPECTED_CYCLES = 6;
    CCPU CPUCopy = cpu;

    //when:
    s64 CyclesUsed = cpu.Execute( EXPECTED_CYCLES );

    //then:
    EXPECT_EQ( cpu.A, 0x37 );
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_FALSE( cpu.Flags.N );
    VerfifyUnmodifiedFlagsFromLoadRegister( cpu, CPUCopy );
}

TEST_F( M6502LoadRegisterTests, LDAIndirectYCanLoadAValueIntoTheARegister )
{
    // given:
    using namespace m6502;
    cpu.Flags.Z = cpu.Flags.N = true;
    cpu.Y = 0x04;
    mem[0xFFFC] = opcode(Ins::LDA_INDY);
    mem[0xFFFD] = 0x02;
    mem[0x0002] = 0x00;	
    mem[0x0003] = 0x80;
    mem[0x8004] = 0x37;	//0x8000 + 0x4
    constexpr s64 EXPECTED_CYCLES = 5;
    CCPU CPUCopy = cpu;

    //when:
    s64 CyclesUsed = cpu.Execute( EXPECTED_CYCLES );

    //then:
    EXPECT_EQ( cpu.A, 0x37 );
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_FALSE( cpu.Flags.N );
    VerfifyUnmodifiedFlagsFromLoadRegister( cpu, CPUCopy );
}

TEST_F( M6502LoadRegisterTests, LDAIndirectYCanLoadAValueIntoTheARegisterWhenItCrossesAPage )
{
    // given:
    using namespace m6502;
    cpu.Y = 0x1;
    mem[0xFFFC] = opcode(Ins::LDA_INDY);
    mem[0xFFFD] = 0x05;
    mem[0x0005] = 0xFF;
    mem[0x0006] = 0x80;
    mem[0x8100] = 0x37;	//0x80FF + 0x1
    constexpr s64 EXPECTED_CYCLES = 6;
    CCPU CPUCopy = cpu;

    //when:
    s64 CyclesUsed = cpu.Execute( EXPECTED_CYCLES );

    //then:
    EXPECT_EQ( cpu.A, 0x37 );
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_FALSE( cpu.Flags.N );
    VerfifyUnmodifiedFlagsFromLoadRegister( cpu, CPUCopy );
}