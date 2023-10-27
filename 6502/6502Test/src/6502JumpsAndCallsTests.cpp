#include <gtest/gtest.h>
#include "m6502.h"

class M6502JumpsAndCallsTests : public testing::Test
{
public:	
	m6502::Mem mem;
	m6502::CPU cpu;

	virtual void SetUp()
	{
		cpu.Reset( mem );
	}

	virtual void TearDown()
	{
	}
};

TEST_F( M6502JumpsAndCallsTests, JSRDoesNotAffectTheProcessorStatus )
{
	// given:
	using namespace m6502;
	cpu.Reset( 0xFF00, mem );
	mem[0xFF00] = CPU::INS_JSR;
	mem[0xFF01] = 0x00;
	mem[0xFF02] = 0x80;
	constexpr s32 EXPECTED_CYCLES = 6;
	CPU CPUCopy = cpu;

	// when:
	const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

	// then:
	EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
	EXPECT_EQ( cpu.PS, CPUCopy.PS );
	EXPECT_NE( cpu.SP, CPUCopy.SP );
	EXPECT_EQ( cpu.PC, 0x8000 );
}