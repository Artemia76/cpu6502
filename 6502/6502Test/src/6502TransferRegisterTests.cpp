#include <gtest/gtest.h>
#include <m6502/System.hpp>

class M6502TransferRegisterTests : public testing::Test
{
public:
    M6502TransferRegisterTests () : cpu(bus), mem(bus,0x0000,0x0000) {}
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

    void ExpectUnaffectedRegister( m6502::CCPU CPUBefore)
    {
        EXPECT_EQ( CPUBefore.Flags.C, cpu.Flags.C );
        EXPECT_EQ( CPUBefore.Flags.I, cpu.Flags.I );
        EXPECT_EQ( CPUBefore.Flags.D, cpu.Flags.D );
        EXPECT_EQ( CPUBefore.Flags.B, cpu.Flags.B );
        EXPECT_EQ( CPUBefore.Flags.V, cpu.Flags.V );
    }
};

TEST_F( M6502TransferRegisterTests, TAXCanTransferANonNegativeNonZeroValue )
{
    // given:
    using namespace m6502;
    cpu.reset( 0xFF00 );
    cpu.A = 0x42;
    cpu.X = 0x32;
    cpu.Flags.Z = true;
    cpu.Flags.N = true;
    mem[0xFF00] = opcode(Ins::TAX);
    constexpr s64 EXPECTED_CYCLES = 2;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.A, 0x42 );
    EXPECT_EQ( cpu.X, 0x42 );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_FALSE( cpu.Flags.N );
    ExpectUnaffectedRegister( CPUCopy );
}

TEST_F( M6502TransferRegisterTests, TAXCanTransferANonNegativeZeroValue )
{
    // given:
    using namespace m6502;
    cpu.reset( 0xFF00 );
    cpu.A = 0x00;
    cpu.X = 0x32;
    cpu.Flags.Z = false;
    cpu.Flags.N = true;
    mem[0xFF00] = opcode(Ins::TAX);
    constexpr s64 EXPECTED_CYCLES = 2;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.A, 0x00 );
    EXPECT_EQ( cpu.X, 0x00 );
    EXPECT_TRUE( cpu.Flags.Z );
    EXPECT_FALSE( cpu.Flags.N );
    ExpectUnaffectedRegister( CPUCopy );
}

TEST_F( M6502TransferRegisterTests, TAXCanTransferANegativeValue )
{
    // given:
    using namespace m6502;
    cpu.reset( 0xFF00 );
    cpu.A = 0b10001011;
    cpu.X = 0x32;
    cpu.Flags.Z = true;
    cpu.Flags.N = false;
    mem[0xFF00] = opcode(Ins::TAX);
    constexpr s64 EXPECTED_CYCLES = 2;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.A, 0b10001011 );
    EXPECT_EQ( cpu.X, 0b10001011 );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_TRUE( cpu.Flags.N );
    ExpectUnaffectedRegister( CPUCopy );
}

TEST_F( M6502TransferRegisterTests, TAYCanTransferANonNegativeNonZeroValue )
{
    // given:
    using namespace m6502;
    cpu.reset( 0xFF00 );
    cpu.A = 0x42;
    cpu.Y = 0x32;
    cpu.Flags.Z = true;
    cpu.Flags.N = true;
    mem[0xFF00] = opcode(Ins::TAY);
    constexpr s64 EXPECTED_CYCLES = 2;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.A, 0x42 );
    EXPECT_EQ( cpu.Y, 0x42 );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_FALSE( cpu.Flags.N );
    ExpectUnaffectedRegister( CPUCopy );
}

TEST_F( M6502TransferRegisterTests, TAYCanTransferANonNegativeZeroValue )
{
    // given:
    using namespace m6502;
    cpu.reset( 0xFF00 );
    cpu.A = 0x00;
    cpu.Y = 0x32;
    cpu.Flags.Z = false;
    cpu.Flags.N = true;
    mem[0xFF00] = opcode(Ins::TAY);
    constexpr s64 EXPECTED_CYCLES = 2;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.A, 0x00 );
    EXPECT_EQ( cpu.Y, 0x00 );
    EXPECT_TRUE( cpu.Flags.Z );
    EXPECT_FALSE( cpu.Flags.N );
    ExpectUnaffectedRegister( CPUCopy );
}

TEST_F( M6502TransferRegisterTests, TAYCanTransferANegativeValue )
{
    // given:
    using namespace m6502;
    cpu.reset( 0xFF00 );
    cpu.A = 0b10001011;
    cpu.Y = 0x32;
    cpu.Flags.Z = true;
    cpu.Flags.N = false;
    mem[0xFF00] = opcode(Ins::TAY);
    constexpr s64 EXPECTED_CYCLES = 2;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.A, 0b10001011 );
    EXPECT_EQ( cpu.Y, 0b10001011 );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_TRUE( cpu.Flags.N );
    ExpectUnaffectedRegister( CPUCopy );
}

TEST_F( M6502TransferRegisterTests, TXACanTransferANonNegativeNonZeroValue )
{
    // given:
    using namespace m6502;
    cpu.reset( 0xFF00 );
    cpu.X = 0x42;
    cpu.A = 0x32;
    cpu.Flags.Z = true;
    cpu.Flags.N = true;
    mem[0xFF00] = opcode(Ins::TXA);
    constexpr s64 EXPECTED_CYCLES = 2;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.A, 0x42 );
    EXPECT_EQ( cpu.X, 0x42 );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_FALSE( cpu.Flags.N );
    ExpectUnaffectedRegister( CPUCopy );
}

TEST_F( M6502TransferRegisterTests, TXACanTransferANonNegativeZeroValue )
{
    // given:
    using namespace m6502;
    cpu.reset( 0xFF00 );
    cpu.X = 0x00;
    cpu.A = 0x32;
    cpu.Flags.Z = false;
    cpu.Flags.N = true;
    mem[0xFF00] = opcode(Ins::TXA);
    constexpr s64 EXPECTED_CYCLES = 2;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.A, 0x00 );
    EXPECT_EQ( cpu.X, 0x00 );
    EXPECT_TRUE( cpu.Flags.Z );
    EXPECT_FALSE( cpu.Flags.N );
    ExpectUnaffectedRegister( CPUCopy );
}

TEST_F( M6502TransferRegisterTests, TXACanTransferANegativeValue )
{
    // given:
    using namespace m6502;
    cpu.reset( 0xFF00 );
    cpu.X = 0b10001011;
    cpu.A = 0x32;
    cpu.Flags.Z = true;
    cpu.Flags.N = false;
    mem[0xFF00] = opcode(Ins::TXA);
    constexpr s64 EXPECTED_CYCLES = 2;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.A, 0b10001011 );
    EXPECT_EQ( cpu.X, 0b10001011 );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_TRUE( cpu.Flags.N );
    ExpectUnaffectedRegister( CPUCopy );
}

TEST_F( M6502TransferRegisterTests, TYACanTransferANonNegativeNonZeroValue )
{
    // given:
    using namespace m6502;
    cpu.reset( 0xFF00 );
    cpu.Y = 0x42;
    cpu.A = 0x32;
    cpu.Flags.Z = true;
    cpu.Flags.N = true;
    mem[0xFF00] = opcode(Ins::TYA);
    constexpr s64 EXPECTED_CYCLES = 2;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.A, 0x42 );
    EXPECT_EQ( cpu.Y, 0x42 );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_FALSE( cpu.Flags.N );
    ExpectUnaffectedRegister( CPUCopy );
}

TEST_F( M6502TransferRegisterTests, TYACanTransferANonNegativeZeroValue )
{
    // given:
    using namespace m6502;
    cpu.reset( 0xFF00 );
    cpu.Y = 0x00;
    cpu.A = 0x32;
    cpu.Flags.Z = false;
    cpu.Flags.N = true;
    mem[0xFF00] = opcode(Ins::TYA);
    constexpr s64 EXPECTED_CYCLES = 2;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.A, 0x00 );
    EXPECT_EQ( cpu.Y, 0x00 );
    EXPECT_TRUE( cpu.Flags.Z );
    EXPECT_FALSE( cpu.Flags.N );
    ExpectUnaffectedRegister( CPUCopy );
}

TEST_F( M6502TransferRegisterTests, TYACanTransferANegativeValue )
{
    // given:
    using namespace m6502;
    cpu.reset( 0xFF00 );
    cpu.Y = 0b10001011;
    cpu.A = 0x32;
    cpu.Flags.Z = true;
    cpu.Flags.N = false;
    mem[0xFF00] = opcode(Ins::TYA);
    constexpr s64 EXPECTED_CYCLES = 2;
    CCPU CPUCopy = cpu;

    // when:
    const s64 ActualCycles = cpu.execute( EXPECTED_CYCLES );

    // then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.A, 0b10001011 );
    EXPECT_EQ( cpu.Y, 0b10001011 );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_TRUE( cpu.Flags.N );
    ExpectUnaffectedRegister( CPUCopy );
}