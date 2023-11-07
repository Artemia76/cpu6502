#include <gtest/gtest.h>
#include <m6502/System.hpp>

class M6502BranchTests : public testing::Test
{
public:	
	m6502::Mem mem;
	m6502::CPU cpu;
	M6502BranchTests () : cpu(mem){}
	virtual void SetUp()
	{
		cpu.Reset();
	}

	virtual void TearDown()
	{
	}
};

TEST_F( M6502BranchTests, BEQCanBranchForwardWhenZeroIsSet )
{
	// given:
	using namespace m6502;
	cpu.Reset( 0xFF00 );
    cpu.Flags.Z = true;
	mem[0xFF00] = opcode(Ins::BEQ);
    mem[0xFF01] = 0x01;
	constexpr s64 EXPECTED_CYCLES = 3;
	CPU CPUCopy = cpu;

	// when:
	const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

	// then:
	EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
	EXPECT_EQ( cpu.PC, 0xFF03 );
	EXPECT_EQ( cpu.PS, CPUCopy.PS );
}

TEST_F( M6502BranchTests, BEQDoesNotBranchForwardWhenZeroIsNotSet )
{
	// given:
	using namespace m6502;
	cpu.Reset( 0xFF00 );
    cpu.Flags.Z = false;
	mem[0xFF00] = opcode(Ins::BEQ);
    mem[0xFF01] = 0x01;
	constexpr s64 EXPECTED_CYCLES = 2;
	CPU CPUCopy = cpu;

	// when:
	const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

	// then:
	EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
	EXPECT_EQ( cpu.PC, 0xFF02 );
	EXPECT_EQ( cpu.PS, CPUCopy.PS );
}

TEST_F( M6502BranchTests, BEQCanBranchForwardIntoANewPageWhenZeroIsSet )
{
	// given:
	using namespace m6502;
	cpu.Reset( 0xFEFD );
    cpu.Flags.Z = true;
	mem[0xFEFD] = opcode(Ins::BEQ);
    mem[0xFEFE] = 0x01;
	constexpr s64 EXPECTED_CYCLES = 4;
	CPU CPUCopy = cpu;

	// when:
	const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

	// then:
	EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
	EXPECT_EQ( cpu.PC, 0xFF00 );
	EXPECT_EQ( cpu.PS, CPUCopy.PS );
}

TEST_F( M6502BranchTests, BEQCanBranchBackwardWhenZeroIsSet )
{
	// given:
	using namespace m6502;
	cpu.Reset( 0xFFCC );
    cpu.Flags.Z = true;
	mem[0xFFCC] = opcode(Ins::BEQ);
    mem[0xFFCD] = static_cast<Byte>( -0x2 );
	constexpr s64 EXPECTED_CYCLES = 3;
	CPU CPUCopy = cpu;

	// when:
	const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

	// then:
	EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
	EXPECT_EQ( cpu.PC, 0xFFCC );
	EXPECT_EQ( cpu.PS, CPUCopy.PS );
}

TEST_F( M6502BranchTests, BEQCanBranchBackwardWhenZeroIsSetFromAssemble )
{
	// given:
	using namespace m6502;
	cpu.Reset( 0xFFCC );
    cpu.Flags.Z = true;
    /*
    loop
    lda #0
    beq loop
    */
	mem[0xFFCC] = 0xA9;
    mem[0xFFCC+1] = 0x00;
    mem[0xFFCC+2] = 0xF0;
    mem[0xFFCC+3] = 0xFC;
	constexpr s64 EXPECTED_CYCLES = 2 + 3;
	CPU CPUCopy = cpu;

	// when:
	const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

	// then:
	EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
	EXPECT_EQ( cpu.PC, 0xFFCC );
	EXPECT_EQ( cpu.PS, CPUCopy.PS );
}

