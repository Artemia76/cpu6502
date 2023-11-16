#include <gtest/gtest.h>
#include <m6502/System.hpp>

class M6502SystemFunctionsTests : public testing::Test
{
public:
    M6502SystemFunctionsTests() :cpu(bus), mem(bus,0x0000,0x0000) {}
    m6502::CBus bus;
    m6502::CCPU cpu;
    m6502::CMem mem;

	virtual void SetUp()
	{
		cpu.Reset();
	}

	virtual void TearDown()
	{
	}
};

TEST_F( M6502SystemFunctionsTests, NOPWillDoNothingButConsumeACycle )
{
	// given:
	using namespace m6502;
	cpu.Reset( 0xFF00 );
	mem[0xFF00] = opcode(Ins::NOP);
	constexpr s64 EXPECTED_CYCLES = 2;
	CCPU CPUCopy = cpu;

	// when:
	const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

	// then:
	EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
	EXPECT_EQ( cpu.PS, CPUCopy.PS );
	EXPECT_EQ( cpu.PC, 0xFF01 );
	EXPECT_EQ( cpu.A, CPUCopy.A );
	EXPECT_EQ( cpu.X, CPUCopy.X );
	EXPECT_EQ( cpu.Y, CPUCopy.Y );
	EXPECT_EQ( cpu.SP, CPUCopy.SP );
}

TEST_F( M6502SystemFunctionsTests, BRKWillLoadTheProgramCounterFromTheInterruptVector )
{
	// given:
	using namespace m6502;
	cpu.Reset( 0xFF00 );
	mem[0xFF00] = opcode(Ins::BRK);
	mem[0xFFFE] = 0x00;
	mem[0xFFFF] = 0x80;
	constexpr s32 EXPECTED_CYCLES = 7;
	CCPU CPUCopy = cpu;

	// when:
	const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

	// then:
	EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
	EXPECT_EQ( cpu.PC, 0x8000 );
}

TEST_F( M6502SystemFunctionsTests, BRKWillLoadTheProgramCounterFromTheInterruptVector2 )
{
	// given:
	using namespace m6502;
	cpu.Reset( 0xFF00 );
	mem[0xFF00] = opcode(Ins::BRK);
	mem[0xFFFE] = 0x00;
	mem[0xFFFF] = 0x90;
	constexpr s64 EXPECTED_CYCLES = 7;
	CCPU CPUCopy = cpu;

	// when:
	const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

	// then:
	EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
	EXPECT_EQ( cpu.PC, 0x9000 );
}

TEST_F( M6502SystemFunctionsTests, BRKWillSetTheBreakFlag )
{
	// given:
	using namespace m6502;
	cpu.Reset( 0xFF00 );
	cpu.Flags.B = false;
	mem[0xFF00] = opcode(Ins::BRK);
	constexpr s64 EXPECTED_CYCLES = 7;
	CCPU CPUCopy = cpu;

	// when:
	const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

	// then:
	EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
	EXPECT_TRUE( cpu.Flags.B );
}

TEST_F( M6502SystemFunctionsTests, BRKWillPush3BytesOntoTheStack )
{
	// given:
	using namespace m6502;
	cpu.Reset( 0xFF00 );
	mem[0xFF00] = opcode(Ins::BRK);
	constexpr s64 EXPECTED_CYCLES = 7;
	CCPU CPUCopy = cpu;

	// when:
	const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

	// then:
	EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
	EXPECT_EQ( cpu.SP, CPUCopy.SP - 3 );
}

TEST_F( M6502SystemFunctionsTests, BRKWillPushPCandPSOntoTheStack )
{
	// given:
	using namespace m6502;
	cpu.Reset( 0xFF00 );
	mem[0xFF00] = opcode(Ins::BRK);
	constexpr s64 EXPECTED_CYCLES = 7;
	CCPU CPUCopy = cpu;
	Byte OldSP = CPUCopy.SP;

	// when:
	const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

	// then:
	EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
	EXPECT_EQ( mem[(0x100 | OldSP)-0], 0xFF );
	// https://www.c64-wiki.com/wiki/BRK
	// Note that since BRK increments the program counter by 
	// 2 instead of 1, it is advisable to use a NOP after it 
	// to avoid issues
	EXPECT_EQ( mem[(0x100 | OldSP)-1], 0x02 );
	EXPECT_EQ( mem[(0x100 | OldSP)-2], 
		CPUCopy.PS 
		| CCPU::UnusedFlagBit 
		| CCPU::BreakFlagBit );

	// https://wiki.nesdev.com/w/index.php/Status_flags
	// Instruction	|Bits 5 and 4	| Side effects after pushing 
	// BRK			|	11			| I is set to 1 
	EXPECT_EQ( cpu.Flags.I, true );
}

TEST_F( M6502SystemFunctionsTests, RTICanReturnFromAnInterruptLeavingTheCPUInTheStateWhenItEntered )
{
	// given:
	using namespace m6502;
	cpu.Reset( 0xFF00 );
	mem[0xFF00] = opcode(Ins::BRK);
	mem[0xFFFE] = 0x00;
	mem[0xFFFF] = 0x80;
	mem[0x8000] = opcode(Ins::RTI);
	constexpr s64 EXPECTED_CYCLES_BRK = 7;
	constexpr s64 EXPECTED_CYCLES_RTI = 6;
	CCPU CPUCopy = cpu;

	// when:
	const s64 ActualCyclesBRK = cpu.Execute( EXPECTED_CYCLES_BRK );
	const s64 ActualCyclesRTI = cpu.Execute( EXPECTED_CYCLES_RTI );

	// then:
	EXPECT_EQ( ActualCyclesBRK, EXPECTED_CYCLES_BRK );
	EXPECT_EQ( ActualCyclesRTI, EXPECTED_CYCLES_RTI );
	EXPECT_EQ( CPUCopy.SP, cpu.SP );
	EXPECT_EQ( 0xFF02, cpu.PC );
	EXPECT_EQ( CPUCopy.PS, cpu.PS );
}