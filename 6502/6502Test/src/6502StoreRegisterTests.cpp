#include <gtest/gtest.h>
#include <m6502/System.hpp>

static void VerfifyUnmodifiedFlagsFromStoreRegister(
	const m6502::CPU& cpu,
	const m6502::CPU& CPUCopy )
{
	EXPECT_EQ( cpu.Flags.C, CPUCopy.Flags.C );
	EXPECT_EQ( cpu.Flags.I, CPUCopy.Flags.I );
	EXPECT_EQ( cpu.Flags.D, CPUCopy.Flags.D );
	EXPECT_EQ( cpu.Flags.B, CPUCopy.Flags.B );
	EXPECT_EQ( cpu.Flags.V, CPUCopy.Flags.V );
	EXPECT_EQ( cpu.Flags.Z, CPUCopy.Flags.Z );
	EXPECT_EQ( cpu.Flags.N, CPUCopy.Flags.N );
}

class M6502StoreRegisterTests : public testing::Test
{
public:	
	m6502::Mem mem;
	m6502::CPU cpu;
	M6502StoreRegisterTests() : cpu(mem)
	{
	}
	virtual void SetUp()
	{
		cpu.Reset();
	}

	virtual void TearDown()
	{
	}

	void TestStoreRegisterZeroPage(
		m6502::Ins OpcodeToTest,
		m6502::Byte m6502::Registers::*Register )
	{
		// given:
		using namespace m6502;
		cpu.*Register = 0x2F;
		mem[0xFFFC] = opcode(OpcodeToTest);
		mem[0xFFFD] = 0x80;
		mem[0x0080] = 0x00;
		constexpr s64 EXPECTED_CYCLES = 3;
		CPU CPUCopy = cpu;

		// when:
		const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

		// then:
		EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
		EXPECT_EQ( mem[0x0080], 0x2F );
		VerfifyUnmodifiedFlagsFromStoreRegister( cpu, CPUCopy );
	}

	void TestStoreRegisterAbsolute(
		m6502::Ins OpcodeToTest,
		m6502::Byte m6502::Registers::*Register )
	{
		// given:
		using namespace m6502;
		cpu.*Register = 0x2F;
		mem[0xFFFC] = opcode(OpcodeToTest);
		mem[0xFFFD] = 0x00;
		mem[0xFFFE] = 0x80;
		mem[0x8000] = 0x00;
		constexpr s64 EXPECTED_CYCLES = 4;
		CPU CPUCopy = cpu;

		// when:
		const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES);

		// then:
		EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
		EXPECT_EQ( mem[0x8000], 0x2F );
		VerfifyUnmodifiedFlagsFromStoreRegister( cpu, CPUCopy );
	}

	void TestStoreRegisterZeroPageX(
		m6502::Ins OpcodeToTest,
		m6502::Byte m6502::Registers::*Register )
	{
		// given:
		using namespace m6502;
		cpu.*Register = 0x42;
		cpu.X = 0x0F;
		mem[0xFFFC] = opcode(OpcodeToTest);
		mem[0xFFFD] = 0x80;
		mem[0x008F] = 0x00;
		constexpr s64 EXPECTED_CYCLES = 4;
		CPU CPUCopy = cpu;

		// when:
		const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

		// then:
		EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
		EXPECT_EQ( mem[0x008F], 0x42 );
		VerfifyUnmodifiedFlagsFromStoreRegister( cpu, CPUCopy );
	}

	void TestStoreRegisterZeroPageY(
		m6502::Ins OpcodeToTest,
		m6502::Byte m6502::Registers::*Register )
	{
		// given:
		using namespace m6502;
		cpu.*Register = 0x42;
		cpu.Y = 0x0F;
		mem[0xFFFC] = opcode(OpcodeToTest);
		mem[0xFFFD] = 0x80;
		mem[0x008F] = 0x00;
		constexpr s64 EXPECTED_CYCLES = 4;
		CPU CPUCopy = cpu;

		// when:
		const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

		// then:
		EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
		EXPECT_EQ( mem[0x008F], 0x42 );
		VerfifyUnmodifiedFlagsFromStoreRegister( cpu, CPUCopy );
	}
};

TEST_F( M6502StoreRegisterTests, STAZeroPageCanStoreTheARegisterIntoMemory )
{
	using namespace m6502;
	TestStoreRegisterZeroPage( Ins::STA_ZP, &Registers::A );
}

TEST_F( M6502StoreRegisterTests, STXZeroPageCanStoreTheXRegisterIntoMemory )
{
	using namespace m6502;
	TestStoreRegisterZeroPage( Ins::STX_ZP, &Registers::X );
}

TEST_F( M6502StoreRegisterTests, STXZeroPageYCanStoreTheXRegisterIntoMemory )
{
	using namespace m6502;
	TestStoreRegisterZeroPageY( Ins::STX_ZPY, &Registers::X );
}

TEST_F( M6502StoreRegisterTests, STYZeroPageCanStoreTheYRegisterIntoMemory )
{
	using namespace m6502;
	TestStoreRegisterZeroPage( Ins::STY_ZP, &Registers::Y );
}

TEST_F( M6502StoreRegisterTests, STAAbsoluteCanStoreTheARegisterIntoMemory )
{
	using namespace m6502;
	TestStoreRegisterAbsolute( Ins::STA_ABS, &Registers::A );
}

TEST_F( M6502StoreRegisterTests, STXAbsoluteCanStoreTheXRegisterIntoMemory )
{
	using namespace m6502;
	TestStoreRegisterAbsolute( Ins::STX_ABS, &Registers::X );
}

TEST_F( M6502StoreRegisterTests, STYAbsoluteCanStoreTheYRegisterIntoMemory )
{
	using namespace m6502;
	TestStoreRegisterAbsolute( Ins::STY_ABS, &Registers::Y );
}

TEST_F( M6502StoreRegisterTests, STAZeroPageXCanStoreTheARegisterIntoMemory )
{
	using namespace m6502;
	TestStoreRegisterZeroPageX( Ins::STA_ZPX, &Registers::A );
}

TEST_F( M6502StoreRegisterTests, STYZeroPageXCanStoreTheYRegisterIntoMemory )
{
	using namespace m6502;
	TestStoreRegisterZeroPageX( Ins::STY_ZPX, &Registers::Y );
}

TEST_F( M6502StoreRegisterTests, STAAbsoluteXCanStoreTheARegisterIntoMemory )
{
	// given:
	using namespace m6502;
	cpu.A = 0x42;
	cpu.X = 0x0F;
	mem[0xFFFC] = opcode (Ins::STA_ABSX);
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x80;
	constexpr s64 EXPECTED_CYCLES = 5;
	CPU CPUCopy = cpu;

	// when:
	const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

	// then:
	EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
	EXPECT_EQ( mem[0x800F], 0x42 );
	VerfifyUnmodifiedFlagsFromStoreRegister( cpu, CPUCopy );
}

TEST_F( M6502StoreRegisterTests, STAAbsoluteYCanStoreTheARegisterIntoMemory )
{
	// given:
	using namespace m6502;
	cpu.A = 0x42;
	cpu.Y = 0x0F;
	mem[0xFFFC] = opcode (Ins::STA_ABSY);
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x80;
	constexpr s64 EXPECTED_CYCLES = 5;
	CPU CPUCopy = cpu;

	// when:
	const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

	// then:
	EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
	EXPECT_EQ( mem[0x800F], 0x42 );
	VerfifyUnmodifiedFlagsFromStoreRegister( cpu, CPUCopy );
}

TEST_F( M6502StoreRegisterTests, STAIndirectXCanStoreTheARegisterIntoMemory )
{
	// given:
	using namespace m6502;
	cpu.A = 0x42;
	cpu.X = 0x0F;
	mem[0xFFFC] = opcode(Ins::STA_INDX);
	mem[0xFFFD] = 0x20;
	mem[0x0020] = 0x00;
	mem[0x0021] = 0x80;
	mem[0x8000 + 0x0F] = 0x00;
	constexpr s64 EXPECTED_CYCLES = 6;
	CPU CPUCopy = cpu;

	// when:
	const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

	// then:
	EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
	EXPECT_EQ( mem[0x8000 + 0x0F], 0x42 );
	VerfifyUnmodifiedFlagsFromStoreRegister( cpu, CPUCopy );
}

TEST_F( M6502StoreRegisterTests, STAIndirectYCanStoreTheARegisterIntoMemory )
{
	// given:
	using namespace m6502;
	cpu.A = 0x42;
	cpu.Y = 0x0F;
	mem[0xFFFC] = opcode(Ins::STA_INDY);
	mem[0xFFFD] = 0x20;
	mem[0x0020] = 0x00;
	mem[0x0021] = 0x80;
	mem[0x8000 + 0x0F] = 0x00;
	constexpr s64 EXPECTED_CYCLES = 6;
	CPU CPUCopy = cpu;

	// when:
	const s64 ActualCycles = cpu.Execute( EXPECTED_CYCLES );

	// then:
	EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
	EXPECT_EQ( mem[0x8000 + 0x0F], 0x42 );
	VerfifyUnmodifiedFlagsFromStoreRegister( cpu, CPUCopy );
}
