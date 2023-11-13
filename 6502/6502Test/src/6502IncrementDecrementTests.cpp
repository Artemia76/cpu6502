#include <gtest/gtest.h>
#include <m6502/System.hpp>

class M6502IncrementDecrementTests : public testing::Test
{
public:
    M6502IncrementDecrementTests() : cpu(bus), mem(bus,0x0000,0x0000) {}
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

    void ExpectUnaffectedFlags( m6502::CCPU CPUBefore )
    {
        EXPECT_EQ( CPUBefore.Flags.C, cpu.Flags.C );
        EXPECT_EQ( CPUBefore.Flags.I, cpu.Flags.I );
        EXPECT_EQ( CPUBefore.Flags.D, cpu.Flags.D );
        EXPECT_EQ( CPUBefore.Flags.B, cpu.Flags.B );
        EXPECT_EQ( CPUBefore.Flags.V, cpu.Flags.V );
    }
};

TEST_F( M6502IncrementDecrementTests, INXCanIncrementAZeroValue )
{
    // given:
    using namespace m6502;
    cpu.Reset( 0xFF00 );
    cpu.X = 0x0;
    cpu.Flags.Z = true;
    cpu.Flags.N = true;
    mem[0xFF00] = opcode(Ins::INX);
    constexpr s64 EXPECTED_CYCLES = 2;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.X, 0x1 );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_FALSE( cpu.Flags.N );
    ExpectUnaffectedFlags( CPUCopy );
}

TEST_F( M6502IncrementDecrementTests, INXCanIncrement255 )
{
    // given:
    using namespace m6502;
    cpu.Reset( 0xFF00 );
    cpu.X = 0xFF;
    cpu.Flags.Z = false;
    cpu.Flags.N = true;
    mem[0xFF00] = opcode(Ins::INX);
    constexpr s64 EXPECTED_CYCLES = 2;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.X, 0x0 );	//NOTE: does this instruction actually wrap?
    EXPECT_TRUE( cpu.Flags.Z );
    EXPECT_FALSE( cpu.Flags.N );
    ExpectUnaffectedFlags( CPUCopy );
}

TEST_F( M6502IncrementDecrementTests, INXCanIncrementANegativeValue )
{
    // given:
    using namespace m6502;
    cpu.Reset( 0xFF00 );
    cpu.X = 0b10001000;
    cpu.Flags.Z = true;
    cpu.Flags.N = false;
    mem[0xFF00] = opcode (Ins::INX);
    constexpr s64 EXPECTED_CYCLES = 2;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.X, 0b10001001 );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_TRUE( cpu.Flags.N );
    ExpectUnaffectedFlags( CPUCopy );
}


TEST_F( M6502IncrementDecrementTests, INYCanIncrementAZeroValue )
{
    // given:
    using namespace m6502;
    cpu.Reset( 0xFF00 );
    cpu.Y = 0x0;
    cpu.Flags.Z = true;
    cpu.Flags.N = true;
    mem[0xFF00] = opcode(Ins::INY);
    constexpr s64 EXPECTED_CYCLES = 2;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.Y, 0x1 );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_FALSE( cpu.Flags.N );
    ExpectUnaffectedFlags( CPUCopy );
}

TEST_F( M6502IncrementDecrementTests, INYCanIncrement255 )
{
    // given:
    using namespace m6502;
    cpu.Reset( 0xFF00 );
    cpu.Y = 0xFF;
    cpu.Flags.Z = false;
    cpu.Flags.N = true;
    mem[0xFF00] = opcode(Ins::INY);
    constexpr s64 EXPECTED_CYCLES = 2;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.Y, 0x0 );	//NOTE: does this instruction actually wrap?
    EXPECT_TRUE( cpu.Flags.Z );
    EXPECT_FALSE( cpu.Flags.N );
    ExpectUnaffectedFlags( CPUCopy );
}

TEST_F( M6502IncrementDecrementTests, INYCanIncrementANegativeValue )
{
    // given:
    using namespace m6502;
    cpu.Reset( 0xFF00 );
    cpu.Y = 0b10001000;
    cpu.Flags.Z = true;
    cpu.Flags.N = false;
    mem[0xFF00] = opcode(Ins::INY);
    constexpr s64 EXPECTED_CYCLES = 2;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.Y, 0b10001001 );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_TRUE( cpu.Flags.N );
    ExpectUnaffectedFlags( CPUCopy );
}


TEST_F( M6502IncrementDecrementTests, DEYCanDecementAZeroValue )
{
    // given:
    using namespace m6502;
    cpu.Reset( 0xFF00 );
    cpu.Y = 0x0;
    cpu.Flags.Z = true;
    cpu.Flags.N = false;
    mem[0xFF00] = opcode(Ins::DEY);
    constexpr s64 EXPECTED_CYCLES = 2;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.Y, 0xFF );	//NOTE: Does this wrap?
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_TRUE( cpu.Flags.N );
    ExpectUnaffectedFlags( CPUCopy );
}

TEST_F( M6502IncrementDecrementTests, DEYCanDecrement255 )
{
    // given:
    using namespace m6502;
    cpu.Reset( 0xFF00 );
    cpu.Y = 0xFF;
    cpu.Flags.Z = true;
    cpu.Flags.N = false;
    mem[0xFF00] = opcode(Ins::DEY);
    constexpr s64 EXPECTED_CYCLES = 2;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.Y, 0xFE );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_TRUE( cpu.Flags.N );
    ExpectUnaffectedFlags( CPUCopy );
}

TEST_F( M6502IncrementDecrementTests, DEYCanDecrementANegativeValue )
{
    // given:
    using namespace m6502;
    cpu.Reset( 0xFF00 );
    cpu.Y = 0b10001001;
    cpu.Flags.Z = true;
    cpu.Flags.N = false;
    mem[0xFF00] = opcode(Ins::DEY);
    constexpr s64 EXPECTED_CYCLES = 2;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.Y, 0b10001000 );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_TRUE( cpu.Flags.N );
    ExpectUnaffectedFlags( CPUCopy );
}

TEST_F( M6502IncrementDecrementTests, DEXCanDecementAZeroValue )
{
    // given:
    using namespace m6502;
    cpu.Reset( 0xFF00 );
    cpu.X = 0x0;
    cpu.Flags.Z = true;
    cpu.Flags.N = false;
    mem[0xFF00] = opcode(Ins::DEX);
    constexpr s64 EXPECTED_CYCLES = 2;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.X, 0xFF );	//NOTE: Does this wrap?
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_TRUE( cpu.Flags.N );
    ExpectUnaffectedFlags( CPUCopy );
}

TEST_F( M6502IncrementDecrementTests, DEXCanDecrement255 )
{
    // given:
    using namespace m6502;
    cpu.Reset( 0xFF00 );
    cpu.X = 0xFF;
    cpu.Flags.Z = true;
    cpu.Flags.N = false;
    mem[0xFF00] = opcode(Ins::DEX);
    constexpr s64 EXPECTED_CYCLES = 2;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.X, 0xFE );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_TRUE( cpu.Flags.N );
    ExpectUnaffectedFlags( CPUCopy );
}

TEST_F( M6502IncrementDecrementTests, DEXCanDecrementANegativeValue )
{
    // given:
    using namespace m6502;
    cpu.Reset( 0xFF00 );
    cpu.X = 0b10001001;
    cpu.Flags.Z = true;
    cpu.Flags.N = false;
    mem[0xFF00] = opcode(Ins::DEX);
    constexpr s64 EXPECTED_CYCLES = 2;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.X, 0b10001000 );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_TRUE( cpu.Flags.N );
    ExpectUnaffectedFlags( CPUCopy );
}

TEST_F( M6502IncrementDecrementTests, DECCanDecrementAValueInTheZeroPage )
{
    // given:
    using namespace m6502;
    cpu.Reset( 0xFF00 );
    cpu.Flags.Z = true;
    cpu.Flags.N = true;
    mem[0xFF00] = opcode(Ins::DEC_ZP);
    mem[0xFF01] = 0x42;
    mem[0x0042] = 0x57;
    constexpr s64 EXPECTED_CYCLES = 5;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( mem[0x0042], 0x56 );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_FALSE( cpu.Flags.N );
    ExpectUnaffectedFlags( CPUCopy );
}

TEST_F( M6502IncrementDecrementTests, DECCanDecrementAValueInTheZeroPageX )
{
    // given:
    using namespace m6502;
    cpu.Reset( 0xFF00 );
    cpu.Flags.Z = true;
    cpu.Flags.N = true;
    cpu.X = 0x10;
    mem[0xFF00] = opcode(Ins::DEC_ZPX);
    mem[0xFF01] = 0x42;
    mem[0x0042 + 0x10] = 0x57;
    constexpr s64 EXPECTED_CYCLES = 6;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( mem[0x0042 + 0x10], 0x56 );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_FALSE( cpu.Flags.N );
    ExpectUnaffectedFlags( CPUCopy );
}

TEST_F( M6502IncrementDecrementTests, DECCanDecrementAValueAbsolute )
{
    // given:
    using namespace m6502;
    cpu.Reset( 0xFF00 );
    cpu.Flags.Z = true;
    cpu.Flags.N = true;
    mem[0xFF00] = opcode(Ins::DEC_ABS);
    mem[0xFF01] = 0x00;
    mem[0xFF02] = 0x80;
    mem[0x8000] = 0x57;
    constexpr s64 EXPECTED_CYCLES = 6;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( mem[0x8000], 0x56 );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_FALSE( cpu.Flags.N );
    ExpectUnaffectedFlags( CPUCopy );
}

TEST_F( M6502IncrementDecrementTests, DECCanDecrementAValueAbsoluteX )
{
    // given:
    using namespace m6502;
    cpu.Reset( 0xFF00 );
    cpu.Flags.Z = true;
    cpu.Flags.N = true;
    cpu.X = 0x10;
    mem[0xFF00] = opcode(Ins::DEC_ABSX);
    mem[0xFF01] = 0x00;
    mem[0xFF02] = 0x80;
    mem[0x8000 + 0x10] = 0x57;
    constexpr s64 EXPECTED_CYCLES = 7;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( mem[0x8000 + 0x10], 0x56 );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_FALSE( cpu.Flags.N );
    ExpectUnaffectedFlags( CPUCopy );
}

TEST_F( M6502IncrementDecrementTests, INCCanIncrementAValueInTheZeroPage )
{
    // given:
    using namespace m6502;
    cpu.Reset( 0xFF00 );
    cpu.Flags.Z = true;
    cpu.Flags.N = true;
    mem[0xFF00] = opcode(Ins::INC_ZP);
    mem[0xFF01] = 0x42;
    mem[0x0042] = 0x57;
    constexpr s64 EXPECTED_CYCLES = 5;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( mem[0x0042], 0x58 );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_FALSE( cpu.Flags.N );
    ExpectUnaffectedFlags( CPUCopy );
}

TEST_F( M6502IncrementDecrementTests, INCCanIncrementAValueInTheZeroPageX )
{
    // given:
    using namespace m6502;
    cpu.Reset( 0xFF00 );
    cpu.Flags.Z = true;
    cpu.Flags.N = true;
    cpu.X = 0x10;
    mem[0xFF00] = opcode(Ins::INC_ZPX);
    mem[0xFF01] = 0x42;
    mem[0x0042 + 0x10] = 0x57;
    constexpr s64 EXPECTED_CYCLES = 6;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( mem[0x0042 + 0x10], 0x58 );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_FALSE( cpu.Flags.N );
    ExpectUnaffectedFlags( CPUCopy );
}

TEST_F( M6502IncrementDecrementTests, INCCanIncrementAValueAbsolute )
{
    // given:
    using namespace m6502;
    cpu.Reset( 0xFF00 );
    cpu.Flags.Z = true;
    cpu.Flags.N = true;
    mem[0xFF00] = opcode(Ins::INC_ABS);
    mem[0xFF01] = 0x00;
    mem[0xFF02] = 0x80;
    mem[0x8000] = 0x57;
    constexpr s64 EXPECTED_CYCLES = 6;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( mem[0x8000], 0x58 );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_FALSE( cpu.Flags.N );
    ExpectUnaffectedFlags( CPUCopy );
}

TEST_F( M6502IncrementDecrementTests, INCCanIncrementAValueAbsoluteX )
{
    // given:
    using namespace m6502;
    cpu.Reset( 0xFF00 );
    cpu.Flags.Z = true;
    cpu.Flags.N = true;
    cpu.X = 0x10;
    mem[0xFF00] = opcode(Ins::INC_ABSX);
    mem[0xFF01] = 0x00;
    mem[0xFF02] = 0x80;
    mem[0x8000 + 0x10] = 0x57;
    constexpr s64 EXPECTED_CYCLES = 7;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( mem[0x8000 + 0x10], 0x58 );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_FALSE( cpu.Flags.N );
    ExpectUnaffectedFlags( CPUCopy );
}

TEST_F( M6502IncrementDecrementTests, TestLoadAProgramThatCanIncMemory )
{
    // given:
    using namespace m6502;

    // when:
    /*
    * = $1000

    lda #00
    sta $42

    start
    inc $42
    ldx $42
    inx
    jmp start
    */
    Byte TestPrg[] = 
        { 0x0,0x10,0xA9,0x00,0x85,0x42,0xE6,0x42,
        0xA6,0x42,0xE8,0x4C,0x04,0x10 };

    Word StartAddress = cpu.LoadPrg( TestPrg, sizeof(TestPrg) );
    cpu.PC = StartAddress;

    //then:
    for ( m6502::s64 Clock = 1000; Clock > 0; )
    {
        Clock -= cpu.Execute( 1 );
    }
}