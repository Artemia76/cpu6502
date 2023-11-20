#include <gtest/gtest.h>
#include <m6502/System.hpp>

class M6502JumpsAndCallsTests : public testing::Test
{
public:	
    M6502JumpsAndCallsTests () : cpu(bus), mem(bus,0x0000,0x0000) {}
    m6502::CBus bus;
    m6502::CCPU cpu;
    m6502::CMem mem;

    virtual void SetUp()
    {
        cpu.reset();
    }

    virtual void TearDown()
    {
    }
};

TEST_F( M6502JumpsAndCallsTests, CanJumpToASubroutineAndJumpBackAgain )
{
    // given:
    using namespace m6502;
    cpu.reset( 0xFF00 );
    mem[0xFF00] = opcode(Ins::JSR);
    mem[0xFF01] = 0x00;
    mem[0xFF02] = 0x80;
    mem[0x8000] = opcode(Ins::RTS);
    mem[0xFF03] = opcode(Ins::LDA_IM);
    mem[0xFF04] = 0x42;
    constexpr s64 EXPECTED_CYCLES = 6 + 6 + 2;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.A, 0x42 );
    EXPECT_EQ( cpu.SP, CPUCopy.SP );
}

TEST_F( M6502JumpsAndCallsTests, JSRDoesNotAffectTheProcessorStatus )
{
    // given:
    using namespace m6502;
    cpu.reset( 0xFF00 );
    mem[0xFF00] = opcode(Ins::JSR);
    mem[0xFF01] = 0x00;
    mem[0xFF02] = 0x80;
    constexpr s64 EXPECTED_CYCLES = 6;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.PS, CPUCopy.PS );
    EXPECT_NE( cpu.SP, CPUCopy.SP );
    EXPECT_EQ( cpu.PC, 0x8000 );
}

TEST_F( M6502JumpsAndCallsTests, RTSDoesNotAffectTheProcessorStatus )
{
    // given:
    using namespace m6502;
    cpu.reset( 0xFF00 );
    mem[0xFF00] = opcode(Ins::JSR);
    mem[0xFF01] = 0x00;
    mem[0xFF02] = 0x80;
    mem[0x8000] = opcode(Ins::RTS);
    constexpr s64 EXPECTED_CYCLES = 6 + 6;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.PS, CPUCopy.PS );
    EXPECT_EQ( cpu.PC, 0xFF03 );
}

TEST_F( M6502JumpsAndCallsTests, JumpAbsoluteCanJumpToAnNewLocationInTheProgram )
{
    // given:
    using namespace m6502;
    cpu.reset( 0xFF00 );
    mem[0xFF00] = opcode(Ins::JMP_ABS);
    mem[0xFF01] = 0x00;
    mem[0xFF02] = 0x80;
    constexpr s64 EXPECTED_CYCLES = 3;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.PS, CPUCopy.PS );
    EXPECT_EQ( cpu.SP, CPUCopy.SP );
    EXPECT_EQ( cpu.PC, 0x8000 );
}

TEST_F( M6502JumpsAndCallsTests, JumpIndirectCanJumpToAnNewLocationInTheProgram )
{
    // given:
    using namespace m6502;
    cpu.reset( 0xFF00 );
    mem[0xFF00] = opcode (Ins::JMP_IND);
    mem[0xFF01] = 0x00;
    mem[0xFF02] = 0x80;
    mem[0x8000] = 0x00;
    mem[0x8001] = 0x90;
    constexpr s64 EXPECTED_CYCLES = 5;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.PS, CPUCopy.PS );
    EXPECT_EQ( cpu.SP, CPUCopy.SP );
    EXPECT_EQ( cpu.PC, 0x9000 );
}