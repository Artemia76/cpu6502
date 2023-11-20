#include <gtest/gtest.h>
#include <m6502/System.hpp>

class M6502StatusFlagChangeTests : public testing::Test
{
public:
    M6502StatusFlagChangeTests() : cpu(bus), mem(bus,0x0000,0x0000) {}
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

TEST_F( M6502StatusFlagChangeTests, CLCWillClearTheCarryFlag )
{
    // given:
    using namespace m6502;
    cpu.reset( 0xFF00);
    cpu.Flags.C = true;
    mem[0xFF00] = opcode(Ins::CLC);
    constexpr s64 EXPECTED_CYCLES = 2;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_FALSE( cpu.Flags.C );
    EXPECT_EQ( cpu.Flags.Z, CPUCopy.Flags.Z);
    EXPECT_EQ( cpu.Flags.I, CPUCopy.Flags.I);
    EXPECT_EQ( cpu.Flags.D, CPUCopy.Flags.D);
    EXPECT_EQ( cpu.Flags.B, CPUCopy.Flags.B);
    EXPECT_EQ( cpu.Flags.V, CPUCopy.Flags.V);
    EXPECT_EQ( cpu.Flags.N, CPUCopy.Flags.N);
}

TEST_F( M6502StatusFlagChangeTests, SECWillSetTheCarryFlag )
{
    // given:
    using namespace m6502;
    cpu.reset( 0xFF00);
    cpu.Flags.C = false;
    mem[0xFF00] = opcode(Ins::SEC);
    constexpr s64 EXPECTED_CYCLES = 2;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_TRUE( cpu.Flags.C );
    EXPECT_EQ( cpu.Flags.Z, CPUCopy.Flags.Z);
    EXPECT_EQ( cpu.Flags.I, CPUCopy.Flags.I);
    EXPECT_EQ( cpu.Flags.D, CPUCopy.Flags.D);
    EXPECT_EQ( cpu.Flags.B, CPUCopy.Flags.B);
    EXPECT_EQ( cpu.Flags.V, CPUCopy.Flags.V);
    EXPECT_EQ( cpu.Flags.N, CPUCopy.Flags.N);
}

TEST_F( M6502StatusFlagChangeTests, CLDWillClearTheDecimalFlag )
{
    // given:
    using namespace m6502;
    cpu.reset( 0xFF00);
    cpu.Flags.D = true;
    mem[0xFF00] = opcode(Ins::CLD);
    constexpr s64 EXPECTED_CYCLES = 2;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_FALSE( cpu.Flags.D );
    EXPECT_EQ( cpu.Flags.Z, CPUCopy.Flags.Z);
    EXPECT_EQ( cpu.Flags.I, CPUCopy.Flags.I);
    EXPECT_EQ( cpu.Flags.C, CPUCopy.Flags.C);
    EXPECT_EQ( cpu.Flags.B, CPUCopy.Flags.B);
    EXPECT_EQ( cpu.Flags.V, CPUCopy.Flags.V);
    EXPECT_EQ( cpu.Flags.N, CPUCopy.Flags.N);
}

TEST_F( M6502StatusFlagChangeTests, SEDWillSetTheDecimalFlag )
{
    // given:
    using namespace m6502;
    cpu.reset( 0xFF00);
    cpu.Flags.D = false;
    mem[0xFF00] = opcode(Ins::SED);
    constexpr s64 EXPECTED_CYCLES = 2;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_TRUE( cpu.Flags.D );
    EXPECT_EQ( cpu.Flags.Z, CPUCopy.Flags.Z);
    EXPECT_EQ( cpu.Flags.I, CPUCopy.Flags.I);
    EXPECT_EQ( cpu.Flags.C, CPUCopy.Flags.C);
    EXPECT_EQ( cpu.Flags.B, CPUCopy.Flags.B);
    EXPECT_EQ( cpu.Flags.V, CPUCopy.Flags.V);
    EXPECT_EQ( cpu.Flags.N, CPUCopy.Flags.N);
}

TEST_F( M6502StatusFlagChangeTests, CLIWillClearTheInterruptFlag )
{
    // given:
    using namespace m6502;
    cpu.reset( 0xFF00);
    cpu.Flags.I = true;
    mem[0xFF00] = opcode(Ins::CLI);
    constexpr s64 EXPECTED_CYCLES = 2;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_FALSE( cpu.Flags.I );
    EXPECT_EQ( cpu.Flags.Z, CPUCopy.Flags.Z);
    EXPECT_EQ( cpu.Flags.D, CPUCopy.Flags.D);
    EXPECT_EQ( cpu.Flags.C, CPUCopy.Flags.C);
    EXPECT_EQ( cpu.Flags.B, CPUCopy.Flags.B);
    EXPECT_EQ( cpu.Flags.V, CPUCopy.Flags.V);
    EXPECT_EQ( cpu.Flags.N, CPUCopy.Flags.N);
}

TEST_F( M6502StatusFlagChangeTests, SEIWillSetTheInterruptFlag )
{
    // given:
    using namespace m6502;
    cpu.reset( 0xFF00);
    cpu.Flags.I = false;
    mem[0xFF00] = opcode(Ins::SEI);
    constexpr s64 EXPECTED_CYCLES = 2;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_TRUE( cpu.Flags.I );
    EXPECT_EQ( cpu.Flags.Z, CPUCopy.Flags.Z);
    EXPECT_EQ( cpu.Flags.D, CPUCopy.Flags.D);
    EXPECT_EQ( cpu.Flags.C, CPUCopy.Flags.C);
    EXPECT_EQ( cpu.Flags.B, CPUCopy.Flags.B);
    EXPECT_EQ( cpu.Flags.V, CPUCopy.Flags.V);
    EXPECT_EQ( cpu.Flags.N, CPUCopy.Flags.N);
}

TEST_F( M6502StatusFlagChangeTests, CLVWillClearTheOverflowFlag )
{
    // given:
    using namespace m6502;
    cpu.reset( 0xFF00);
    cpu.Flags.V = true;
    mem[0xFF00] = opcode(Ins::CLV);
    constexpr s64 EXPECTED_CYCLES = 2;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_FALSE( cpu.Flags.V );
    EXPECT_EQ( cpu.Flags.Z, CPUCopy.Flags.Z);
    EXPECT_EQ( cpu.Flags.D, CPUCopy.Flags.D);
    EXPECT_EQ( cpu.Flags.C, CPUCopy.Flags.C);
    EXPECT_EQ( cpu.Flags.B, CPUCopy.Flags.B);
    EXPECT_EQ( cpu.Flags.I, CPUCopy.Flags.I);
    EXPECT_EQ( cpu.Flags.N, CPUCopy.Flags.N);
}

TEST_F( M6502StatusFlagChangeTests, NOPWillDoNothingButConsumeACycle )
{
    // given:
    using namespace m6502;
    cpu.reset( 0xFF00);
    mem[0xFF00] = opcode(Ins::NOP);
    constexpr s64 EXPECTED_CYCLES = 2;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.PS, CPUCopy.PS );
    EXPECT_EQ( cpu.PC, 0xFF01 );
    EXPECT_EQ( cpu.A, CPUCopy.A );
    EXPECT_EQ( cpu.X, CPUCopy.X );
    EXPECT_EQ( cpu.Y, CPUCopy.Y );
    EXPECT_EQ( cpu.SP, CPUCopy.SP );
}
