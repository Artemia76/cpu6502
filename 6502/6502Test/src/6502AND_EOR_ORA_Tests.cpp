#include <gtest/gtest.h>
#include <m6502/System.hpp>

class M6502AndEorOraBitTests : public testing::Test
{
public:
    M6502AndEorOraBitTests () : cpu(mem) {}
    m6502::Mem mem;
    m6502::CPU cpu;

    virtual void SetUp()
    {
        cpu.Reset();
    }

    virtual void TearDown()
    {
    }

    static void VerfifyUnmodifiedFlagsFromLogicalOpInstruction(
        const m6502::CPU& cpu,
        const m6502::CPU& CPUCopy )
    {
        EXPECT_EQ( cpu.Flags.C, CPUCopy.Flags.C );
        EXPECT_EQ( cpu.Flags.I, CPUCopy.Flags.I );
        EXPECT_EQ( cpu.Flags.D, CPUCopy.Flags.D );
        EXPECT_EQ( cpu.Flags.B, CPUCopy.Flags.B );
        EXPECT_EQ( cpu.Flags.V, CPUCopy.Flags.V );
    }

    enum class ELogicalOp
    {
        And, Eor, Or
    };

    m6502::Byte DoLogicalOp( 
        m6502::Byte A, 
        m6502::Byte B, 
        ELogicalOp LogicalOp )
    {
        switch ( LogicalOp )
        {
        case ELogicalOp::And:
            return A & B;
            break;
        case ELogicalOp::Or:
            return A | B;
            break;
        case ELogicalOp::Eor:
            return A ^ B;
            break;
        }

        throw - 1; //invalid Logical Op
    }

    void TestLogicalOpImmediate( 
        ELogicalOp LogicalOp )
    {
        // given:
        using namespace m6502;
        cpu.A = 0xCC;
        switch ( LogicalOp )
        {
        case ELogicalOp::And:
            mem[0xFFFC] =  opcode(Ins::AND_IM);
            break;
        case ELogicalOp::Or:
            mem[0xFFFC] =  opcode(Ins::ORA_IM);
            break;
        case ELogicalOp::Eor:
            mem[0xFFFC] =  opcode(Ins::EOR_IM);
            break;
        }
        mem[0xFFFD] = 0x84;

        //when:
        CPU CPUCopy = cpu;
        s64 CyclesUsed = cpu.Execute( 2 );

        //then:
        const Byte ExpectedResult = DoLogicalOp( 0xCC, 0x84, LogicalOp );
        const bool ExpectedNegative = (ExpectedResult & 0b10000000) > 0;
        EXPECT_EQ( cpu.A, ExpectedResult );
        EXPECT_EQ( CyclesUsed, 2 );
        EXPECT_FALSE( cpu.Flags.Z );
        EXPECT_EQ( cpu.Flags.N, ExpectedNegative );
        VerfifyUnmodifiedFlagsFromLogicalOpInstruction( cpu, CPUCopy );
    }

    void TestLogicalOpZeroPage( ELogicalOp LogicalOp )
    {
        // given:
        using namespace m6502;
        cpu.A = 0xCC;
        switch ( LogicalOp )
        {
        case ELogicalOp::And:
            mem[0xFFFC] = opcode(Ins::AND_ZP);
            break;
        case ELogicalOp::Or:
            mem[0xFFFC] = opcode(Ins::ORA_ZP);
            break;
        case ELogicalOp::Eor:
            mem[0xFFFC] = opcode(Ins::EOR_ZP);
            break;
        }
        mem[0xFFFD] = 0x42;
        mem[0x0042] = 0x37;

        //when:
        CPU CPUCopy = cpu;
        s64 CyclesUsed = cpu.Execute( 3 );

        //then:
        const Byte ExpectedResult = DoLogicalOp( 0xCC, 0x37, LogicalOp );
        const bool ExpectedNegative = (ExpectedResult & 0b10000000) > 0;
        EXPECT_EQ( cpu.A, ExpectedResult );
        EXPECT_EQ( CyclesUsed, 3 );
        EXPECT_FALSE( cpu.Flags.Z );
        EXPECT_EQ( cpu.Flags.N, ExpectedNegative );
        VerfifyUnmodifiedFlagsFromLogicalOpInstruction( cpu, CPUCopy );
    }

    void TestLogicalOpZeroPageX( ELogicalOp LogicalOp )
    {
        // given:
        using namespace m6502;
        cpu.A = 0xCC;
        cpu.X = 5;
        switch ( LogicalOp )
        {
        case ELogicalOp::And:
            mem[0xFFFC] = opcode(Ins::AND_ZPX);
            break;
        case ELogicalOp::Or:
            mem[0xFFFC] = opcode(Ins::ORA_ZPX);
            break;
        case ELogicalOp::Eor:
            mem[0xFFFC] = opcode(Ins::EOR_ZPX);
            break;
        }
        mem[0xFFFD] = 0x42;
        mem[0x0047] = 0x37;
        CPU CPUCopy = cpu;

        //when:
        s64 CyclesUsed = cpu.Execute( 4 );

        //then:
        const Byte ExpectedResult = DoLogicalOp( 0xCC, 0x37, LogicalOp );
        const bool ExpectedNegative = (ExpectedResult & 0b10000000) > 0;
        EXPECT_EQ( cpu.A, ExpectedResult );
        EXPECT_EQ( CyclesUsed, 4 );
        EXPECT_FALSE( cpu.Flags.Z );
        EXPECT_EQ( cpu.Flags.N, ExpectedNegative );
        VerfifyUnmodifiedFlagsFromLogicalOpInstruction( cpu, CPUCopy );
    }

    void TestLogicalOpAbsolute( ELogicalOp LogicalOp )
    {
        // given:
        using namespace m6502;
        cpu.Flags.Z = cpu.Flags.N = true;
        cpu.A = 0xCC;
        switch ( LogicalOp )
        {
        case ELogicalOp::And:
            mem[0xFFFC] = Ins::AND_ABS;
            //mem[0xFFFC] = opcode(Ins::AND_ABS);
            break;
        case ELogicalOp::Or:
            mem[0xFFFC] = opcode(Ins::ORA_ABS);
            break;
        case ELogicalOp::Eor:
            mem[0xFFFC] = opcode(Ins::EOR_ABS);
            break;
        }
        mem[0xFFFD] = 0x80;
        mem[0xFFFE] = 0x44;	//0x4480
        mem[0x4480] = 0x37;
        constexpr s64 EXPECTED_CYCLES = 4;
        CPU CPUCopy = cpu;

        //when:
        s64 CyclesUsed = cpu.Execute( EXPECTED_CYCLES );

        //then:
        const Byte ExpectedResult = DoLogicalOp( 0xCC, 0x37, LogicalOp );
        const bool ExpectedNegative = (ExpectedResult & 0b10000000) > 0;
        EXPECT_EQ( cpu.A, ExpectedResult );
        EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
        EXPECT_FALSE( cpu.Flags.Z );
        EXPECT_EQ( cpu.Flags.N, ExpectedNegative );
        VerfifyUnmodifiedFlagsFromLogicalOpInstruction( cpu, CPUCopy );
    }

    void TestLogicalOpAbsoluteX( ELogicalOp LogicalOp )
    {
        // given:
        using namespace m6502;
        cpu.Flags.Z = cpu.Flags.N = true;
        cpu.A = 0xCC;
        cpu.X = 1;
        switch ( LogicalOp )
        {
        case ELogicalOp::And:
            mem[0xFFFC] = opcode(Ins::AND_ABSX);
            break;
        case ELogicalOp::Or:
            mem[0xFFFC] = opcode(Ins::ORA_ABSX);
            break;
        case ELogicalOp::Eor:
            mem[0xFFFC] = opcode(Ins::EOR_ABSX);
            break;
        }
        mem[0xFFFD] = 0x80;
        mem[0xFFFE] = 0x44;	//0x4480
        mem[0x4481] = 0x37;
        constexpr s64 EXPECTED_CYCLES = 4;
        CPU CPUCopy = cpu;

        //when:
        s64 CyclesUsed = cpu.Execute( EXPECTED_CYCLES );

        //then:
        const Byte ExpectedResult = DoLogicalOp( 0xCC, 0x37, LogicalOp );
        const bool ExpectedNegative = (ExpectedResult & 0b10000000) > 0;
        EXPECT_EQ( cpu.A, ExpectedResult );
        EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
        EXPECT_FALSE( cpu.Flags.Z );
        EXPECT_EQ( cpu.Flags.N, ExpectedNegative );
        VerfifyUnmodifiedFlagsFromLogicalOpInstruction( cpu, CPUCopy );
    }

    void TestLogicalOpAbsoluteY( ELogicalOp LogicalOp )
    {
        // given:
        using namespace m6502;
        cpu.Flags.Z = cpu.Flags.N = true;
        cpu.A = 0xCC;
        cpu.Y = 1;
        switch ( LogicalOp )
        {
        case ELogicalOp::And:
            mem[0xFFFC] = opcode(Ins::AND_ABSY);
            break;
        case ELogicalOp::Or:
            mem[0xFFFC] = opcode(Ins::ORA_ABSY);
            break;
        case ELogicalOp::Eor:
            mem[0xFFFC] = opcode(Ins::EOR_ABSY);
            break;
        }
        mem[0xFFFD] = 0x80;
        mem[0xFFFE] = 0x44;	//0x4480
        mem[0x4481] = 0x37;
        constexpr s64 EXPECTED_CYCLES = 4;
        CPU CPUCopy = cpu;

        //when:
        s64 CyclesUsed = cpu.Execute( EXPECTED_CYCLES );

        //then:
        const Byte ExpectedResult = DoLogicalOp( 0xCC, 0x37, LogicalOp );
        const bool ExpectedNegative = (ExpectedResult & 0b10000000) > 0;
        EXPECT_EQ( cpu.A, ExpectedResult );
        EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
        EXPECT_FALSE( cpu.Flags.Z );
        EXPECT_EQ( cpu.Flags.N, ExpectedNegative );
        VerfifyUnmodifiedFlagsFromLogicalOpInstruction( cpu, CPUCopy );
    }

    void TestLoadRegisterAbsoluteYWhenCrossingPage( ELogicalOp LogicalOp )
    {
        // given:
        using namespace m6502;
        cpu.A = 0xCC;
        cpu.Y = 0xFF;
        switch ( LogicalOp )
        {
        case ELogicalOp::And:
            mem[0xFFFC] = opcode(Ins::AND_ABSY);
            break;
        case ELogicalOp::Or:
            mem[0xFFFC] = opcode(Ins::ORA_ABSY);
            break;
        case ELogicalOp::Eor:
            mem[0xFFFC] = opcode(Ins::EOR_ABSY);
            break;
        }
        mem[0xFFFD] = 0x02;
        mem[0xFFFE] = 0x44;	//0x4402
        mem[0x4501] = 0x37;	//0x4402+0xFF crosses page boundary!
        constexpr s64 EXPECTED_CYCLES = 5;
        CPU CPUCopy = cpu;

        //when:
        s64 CyclesUsed = cpu.Execute( EXPECTED_CYCLES );

        //then:
        const Byte ExpectedResult = DoLogicalOp( 0xCC, 0x37, LogicalOp );
        const bool ExpectedNegative = (ExpectedResult & 0b10000000) > 0;
        EXPECT_EQ( cpu.A, ExpectedResult );
        EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
        EXPECT_FALSE( cpu.Flags.Z );
        EXPECT_EQ( cpu.Flags.N, ExpectedNegative );
        VerfifyUnmodifiedFlagsFromLogicalOpInstruction( cpu, CPUCopy );
    }

    void TestLoadRegisterAbsoluteXWhenCrossingPage( ELogicalOp LogicalOp )
    {
        // given:
        using namespace m6502;
        cpu.A = 0xCC;
        cpu.X = 0xFF;
        switch ( LogicalOp )
        {
        case ELogicalOp::And:
            mem[0xFFFC] = opcode(Ins::AND_ABSX);
            break;
        case ELogicalOp::Or:
            mem[0xFFFC] = opcode(Ins::ORA_ABSX);
            break;
        case ELogicalOp::Eor:
            mem[0xFFFC] = opcode(Ins::EOR_ABSX);
            break;
        }
        mem[0xFFFD] = 0x02;
        mem[0xFFFE] = 0x44;	//0x4402
        mem[0x4501] = 0x37;	//0x4402+0xFF crosses page boundary!
        constexpr s64 EXPECTED_CYCLES = 5;
        CPU CPUCopy = cpu;

        //when:
        s64 CyclesUsed = cpu.Execute( EXPECTED_CYCLES );

        //then:
        const Byte ExpectedResult = DoLogicalOp( 0xCC, 0x37, LogicalOp );
        const bool ExpectedNegative = (ExpectedResult & 0b10000000) > 0;
        EXPECT_EQ( cpu.A, ExpectedResult );
        EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
        EXPECT_FALSE( cpu.Flags.Z );
        EXPECT_EQ( cpu.Flags.N, ExpectedNegative );
        VerfifyUnmodifiedFlagsFromLogicalOpInstruction( cpu, CPUCopy );
    }

    void TestLogicalOpIndirectX( ELogicalOp LogicalOp )
    {
        // given:
        using namespace m6502;
        cpu.Flags.Z = cpu.Flags.N = true;
        cpu.A = 0xCC;
        cpu.X = 0x04;
        switch ( LogicalOp )
        {
        case ELogicalOp::And:
            mem[0xFFFC] = opcode(Ins::AND_INDX);
            break;
        case ELogicalOp::Or:
            mem[0xFFFC] = opcode(Ins::ORA_INDX);
            break;
        case ELogicalOp::Eor:
            mem[0xFFFC] = opcode(Ins::EOR_INDX);
            break;
        }
        mem[0xFFFD] = 0x02;
        mem[0x0006] = 0x00;	//0x2 + 0x4
        mem[0x0007] = 0x80;
        mem[0x8000] = 0x37;
        constexpr s64 EXPECTED_CYCLES = 6;
        CPU CPUCopy = cpu;

        //when:
        s64 CyclesUsed = cpu.Execute( EXPECTED_CYCLES );

        //then:
        const Byte ExpectedResult = DoLogicalOp( 0xCC, 0x37, LogicalOp );
        const bool ExpectedNegative = (ExpectedResult & 0b10000000) > 0;
        EXPECT_EQ( cpu.A, ExpectedResult );
        EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
        EXPECT_FALSE( cpu.Flags.Z );
        EXPECT_EQ( cpu.Flags.N, ExpectedNegative );
        VerfifyUnmodifiedFlagsFromLogicalOpInstruction( cpu, CPUCopy );
    }

    void TestLogicalOpIndirectY( ELogicalOp LogicalOp )
    {
        // given:
        using namespace m6502;
        cpu.Flags.Z = cpu.Flags.N = true;
        cpu.A = 0xCC;
        cpu.Y = 0x04;
        switch ( LogicalOp )
        {
        case ELogicalOp::And:
            mem[0xFFFC] = opcode(Ins::AND_INDY);
            break;
        case ELogicalOp::Or:
            mem[0xFFFC] = opcode(Ins::ORA_INDY);
            break;
        case ELogicalOp::Eor:
            mem[0xFFFC] = opcode(Ins::EOR_INDY);
            break;
        }
        mem[0xFFFD] = 0x02;
        mem[0x0002] = 0x00;
        mem[0x0003] = 0x80;
        mem[0x8004] = 0x37;	//0x8000 + 0x4
        constexpr s64 EXPECTED_CYCLES = 5;
        CPU CPUCopy = cpu;

        //when:
        s64 CyclesUsed = cpu.Execute( EXPECTED_CYCLES );

        //then:
        const Byte ExpectedResult = DoLogicalOp( 0xCC, 0x37, LogicalOp );
        const bool ExpectedNegative = (ExpectedResult & 0b10000000) > 0;
        EXPECT_EQ( cpu.A, ExpectedResult );
        EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
        EXPECT_FALSE( cpu.Flags.Z );
        EXPECT_EQ( cpu.Flags.N, ExpectedNegative );
        VerfifyUnmodifiedFlagsFromLogicalOpInstruction( cpu, CPUCopy );
    }

    void TestLogicalOpIndirectYWhenItCrossesAPage( ELogicalOp LogicalOp )
    {
        // given:
        using namespace m6502;
        cpu.A = 0xCC;
        cpu.Y = 0xFF;
        switch ( LogicalOp )
        {
        case ELogicalOp::And:
            mem[0xFFFC] = opcode(Ins::AND_INDY);
            break;
        case ELogicalOp::Or:
            mem[0xFFFC] = opcode(Ins::ORA_INDY);
            break;
        case ELogicalOp::Eor:
            mem[0xFFFC] = opcode(Ins::EOR_INDY);
            break;
        }
        mem[0xFFFD] = 0x02;
        mem[0x0002] = 0x02;
        mem[0x0003] = 0x80;
        mem[0x8101] = 0x37;	//0x8002 + 0xFF
        constexpr s64 EXPECTED_CYCLES = 6;
        CPU CPUCopy = cpu;

        //when:
        s64 CyclesUsed = cpu.Execute( EXPECTED_CYCLES );

        //then:
        const Byte ExpectedResult = DoLogicalOp( 0xCC, 0x37, LogicalOp );
        const bool ExpectedNegative = (ExpectedResult & 0b10000000) > 0;
        EXPECT_EQ( cpu.A, ExpectedResult );
        EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
        EXPECT_FALSE( cpu.Flags.Z );
        EXPECT_EQ( cpu.Flags.N, ExpectedNegative );
        VerfifyUnmodifiedFlagsFromLogicalOpInstruction( cpu, CPUCopy );
    }

    void TestLogicalOpZeroPageXWhenItWraps
        ( ELogicalOp LogicalOp )
    {
        // given:
        using namespace m6502;
        cpu.A = 0xCC;
        cpu.X = 0xFF;
        switch ( LogicalOp )
        {
        case ELogicalOp::And:
            mem[0xFFFC] = opcode(Ins::AND_ZPX);
            break;
        case ELogicalOp::Or:
            mem[0xFFFC] = opcode(Ins::ORA_ZPX);
            break;
        case ELogicalOp::Eor:
            mem[0xFFFC] = opcode(Ins::EOR_ZPX);
            break;
        }
        mem[0xFFFD] = 0x80;
        mem[0x007F] = 0x37;

        //when:
        CPU CPUCopy = cpu;
        s64 CyclesUsed = cpu.Execute( 4 );

        //then:
        const Byte ExpectedResult = DoLogicalOp( 0xCC, 0x37, LogicalOp );
        const bool ExpectedNegative = (ExpectedResult & 0b10000000) > 0;
        EXPECT_EQ( cpu.A, ExpectedResult );
        EXPECT_EQ( CyclesUsed, 4 );
        EXPECT_FALSE( cpu.Flags.Z );
        EXPECT_EQ( cpu.Flags.N, ExpectedNegative );
        VerfifyUnmodifiedFlagsFromLogicalOpInstruction( cpu, CPUCopy );
    }
};

TEST_F( M6502AndEorOraBitTests, TestLogicalOpANDOnARegisterImmediate )
{
    using namespace m6502;
    TestLogicalOpImmediate( ELogicalOp::And );
}

TEST_F( M6502AndEorOraBitTests, TestLogicalOpOROnARegisterImmediate )
{
    using namespace m6502;
    TestLogicalOpImmediate( ELogicalOp::Or );
}

TEST_F( M6502AndEorOraBitTests, TestLogicalOpEOROnARegisterImmediate )
{
    using namespace m6502;
    TestLogicalOpImmediate( ELogicalOp::Eor );
}

TEST_F( M6502AndEorOraBitTests, TestLogicalOpAndOnARegisterZeroPage )
{
    using namespace m6502;
    TestLogicalOpZeroPage( ELogicalOp::And );
}

TEST_F( M6502AndEorOraBitTests, TestLogicalOpOrOnARegisterZeroPage )
{
    using namespace m6502;
    TestLogicalOpZeroPage( ELogicalOp::Or );
}

TEST_F( M6502AndEorOraBitTests, TestLogicalOpEorOnARegisterZeroPage )
{
    using namespace m6502;
    TestLogicalOpZeroPage( ELogicalOp::Eor );
}

TEST_F( M6502AndEorOraBitTests, TestLogicalOpEorImmediateCanAffectZeroFlag )
{
    // given:
    using namespace m6502;
    cpu.A = 0xCC;
    mem[0xFFFC] = opcode(Ins::EOR_IM);
    mem[0xFFFD] = cpu.A;
    CPU CPUCopy = cpu;

    //when:
    cpu.Execute( 2 );

    //then:
    EXPECT_TRUE( cpu.Flags.Z );
    EXPECT_FALSE( cpu.Flags.N );
    VerfifyUnmodifiedFlagsFromLogicalOpInstruction( cpu, CPUCopy );
}

TEST_F( M6502AndEorOraBitTests, TestLogicalOpAndOnARegisterZeroPageX )
{
    TestLogicalOpZeroPageX( ELogicalOp::And );
}

TEST_F( M6502AndEorOraBitTests, TestLogicalOpOrOnARegisterZeroPageX )
{
    TestLogicalOpZeroPageX( ELogicalOp::Or );
}

TEST_F( M6502AndEorOraBitTests, TestLogicalOpEorOnARegisterZeroPageX )
{
    TestLogicalOpZeroPageX( ELogicalOp::Eor );
}

TEST_F( M6502AndEorOraBitTests, LogicalOpEorCanLoadAValueIntoTheARegisterWhenItWrapsZeroPageX )
{
    TestLogicalOpZeroPageXWhenItWraps( ELogicalOp::Eor );
}

TEST_F( M6502AndEorOraBitTests, LogicalOpOrCanLoadAValueIntoTheARegisterWhenItWrapsZeroPageX )
{
    TestLogicalOpZeroPageXWhenItWraps( ELogicalOp::Or );
}
TEST_F( M6502AndEorOraBitTests, LogicalOpAndCanLoadAValueIntoTheARegisterWhenItWrapsZeroPageX )
{
    TestLogicalOpZeroPageXWhenItWraps( ELogicalOp::And );
}

TEST_F( M6502AndEorOraBitTests, TestLogicalOpEorOnARegisterAbsolute )
{
    TestLogicalOpAbsolute( ELogicalOp::Eor );
}

TEST_F( M6502AndEorOraBitTests, TestLogicalOpOrOnARegisterAbsolute )
{
    TestLogicalOpAbsolute( ELogicalOp::Or );
}

TEST_F( M6502AndEorOraBitTests, TestLogicalOpAndOnARegisterAbsolute )
{
    TestLogicalOpAbsolute( ELogicalOp::And );
}

TEST_F( M6502AndEorOraBitTests, TestLogicalOpAndOnARegisterAbsoluteX )
{
    TestLogicalOpAbsoluteX( ELogicalOp::And );
}

TEST_F( M6502AndEorOraBitTests, TestLogicalOpOrOnARegisterAbsoluteX )
{
    TestLogicalOpAbsoluteX( ELogicalOp::Or );
}

TEST_F( M6502AndEorOraBitTests, TestLogicalOpEorOnARegisterAbsoluteX )
{
    TestLogicalOpAbsoluteX( ELogicalOp::Eor );
}

TEST_F( M6502AndEorOraBitTests, TestLogicalOpAndWhenItCrossesAPageBoundaryAbsoluteX )
{
    TestLoadRegisterAbsoluteXWhenCrossingPage( ELogicalOp::And );
}

TEST_F( M6502AndEorOraBitTests, TestLogicalOpOrWhenItCrossesAPageBoundaryAbsoluteX )
{
    TestLoadRegisterAbsoluteXWhenCrossingPage( ELogicalOp::Or );
}

TEST_F( M6502AndEorOraBitTests, TestLogicalOpEorWhenItCrossesAPageBoundaryAbsoluteX )
{
    TestLoadRegisterAbsoluteXWhenCrossingPage( ELogicalOp::Eor );
}

TEST_F( M6502AndEorOraBitTests, TestLogicalOpAndAbsoluteY )
{
    TestLogicalOpAbsoluteY( ELogicalOp::And );
}

TEST_F( M6502AndEorOraBitTests, TestLogicalOpOrAbsoluteY )
{
    TestLogicalOpAbsoluteY( ELogicalOp::Or );
}

TEST_F( M6502AndEorOraBitTests, TestLogicalOpEorAbsoluteY )
{
    TestLogicalOpAbsoluteY( ELogicalOp::Eor );
}

TEST_F( M6502AndEorOraBitTests, TestLogicalOpAndWhenItCrossesAPageBoundaryAbsoluteY )
{
    TestLoadRegisterAbsoluteYWhenCrossingPage( ELogicalOp::And );
}

TEST_F( M6502AndEorOraBitTests, TestLogicalOpOrWhenItCrossesAPageBoundaryAbsoluteY )
{
    TestLoadRegisterAbsoluteYWhenCrossingPage( ELogicalOp::Or );
}

TEST_F( M6502AndEorOraBitTests, TestLogicalOpEorWhenItCrossesAPageBoundaryAbsoluteY )
{
    TestLoadRegisterAbsoluteYWhenCrossingPage( ELogicalOp::Eor );
}

TEST_F( M6502AndEorOraBitTests, TestLogicalOpAndIndirectX )
{
    TestLogicalOpIndirectX( ELogicalOp::And );
}

TEST_F( M6502AndEorOraBitTests, TestLogicalOpEorIndirectX )
{
    TestLogicalOpIndirectX( ELogicalOp::Eor );
}

TEST_F( M6502AndEorOraBitTests, TestLogicalOpOrIndirectX )
{
    TestLogicalOpIndirectX( ELogicalOp::Or );
}

TEST_F( M6502AndEorOraBitTests, TestLogicalOpAndIndirectY )
{
    TestLogicalOpIndirectY( ELogicalOp::And );
}

TEST_F( M6502AndEorOraBitTests, TestLogicalOpOrIndirectY )
{
    TestLogicalOpIndirectY( ELogicalOp::Or );
}

TEST_F( M6502AndEorOraBitTests, TestLogicalOpEorIndirectY )
{
    TestLogicalOpIndirectY( ELogicalOp::Eor );
}

TEST_F( M6502AndEorOraBitTests, TestLogicalOpAndWhenItCrossesAPageIndirectY )
{
    TestLogicalOpIndirectYWhenItCrossesAPage( ELogicalOp::And );
}

TEST_F( M6502AndEorOraBitTests, TestLogicalOpOrWhenItCrossesAPageIndirectY )
{
    TestLogicalOpIndirectYWhenItCrossesAPage( ELogicalOp::Or );
}

TEST_F( M6502AndEorOraBitTests, TestLogicalOpEorWhenItCrossesAPageIndirectY )
{
    TestLogicalOpIndirectYWhenItCrossesAPage( ELogicalOp::Eor );
}

TEST_F( M6502AndEorOraBitTests, TestBitZeroPage )
{
    // given:
    using namespace m6502;
    cpu.Flags.V = cpu.Flags.N = false;
    cpu.A = 0xCC;
    mem[0xFFFC] = opcode(Ins::BIT_ZP);	
    mem[0xFFFD] = 0x42;
    mem[0x0042] = 0xCC;
    CPU CPUCopy = cpu;
    constexpr s64 EXPECTED_CYCLES = 3;

    //when:
    s64 CyclesUsed = cpu.Execute( EXPECTED_CYCLES );

    //then:
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.A, 0xCC );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_TRUE( cpu.Flags.V );
    EXPECT_TRUE( cpu.Flags.N );
}

TEST_F( M6502AndEorOraBitTests, TestBitZeroPageResultZero )
{
    // given:
    using namespace m6502;
    cpu.Flags.V = cpu.Flags.N = true;
    cpu.A = 0xCC;
    mem[0xFFFC] = opcode(Ins::BIT_ZP);
    mem[0xFFFD] = 0x42;
    mem[0x0042] = 0x33;
    CPU CPUCopy = cpu;
    constexpr s64 EXPECTED_CYCLES = 3;

    //when:
    s64 CyclesUsed = cpu.Execute( EXPECTED_CYCLES );

    //then:
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.A, 0xCC );
    EXPECT_TRUE( cpu.Flags.Z );
    EXPECT_FALSE( cpu.Flags.V );
    EXPECT_FALSE( cpu.Flags.N );
}

TEST_F( M6502AndEorOraBitTests, TestBitZeroPageResultZeroBits6And7Zero )
{
    // given:
    using namespace m6502;
    cpu.Flags.V = cpu.Flags.N = false;
    cpu.A = 0x33;
    mem[0xFFFC] = opcode(Ins::BIT_ZP);
    mem[0xFFFD] = 0x42;
    mem[0x0042] = 0xCC;
    CPU CPUCopy = cpu;
    constexpr s64 EXPECTED_CYCLES = 3;

    //when:
    s64 CyclesUsed = cpu.Execute( EXPECTED_CYCLES );

    //then:
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.A, 0x33 );
    EXPECT_TRUE( cpu.Flags.Z );
    EXPECT_TRUE( cpu.Flags.V );
    EXPECT_TRUE( cpu.Flags.N );
}

TEST_F( M6502AndEorOraBitTests, TestBitZeroPageResultZeroBits6And7Mixed )
{
    // given:
    using namespace m6502;
    cpu.Flags.V = false;
    cpu.Flags.N = true;
    mem[0xFFFC] = opcode(Ins::BIT_ZP);
    mem[0xFFFD] = 0x42;
    mem[0x0042] = 0b01000000;
    constexpr s64 EXPECTED_CYCLES = 3;

    //when:
    cpu.Execute( EXPECTED_CYCLES );

    //then:
    EXPECT_TRUE( cpu.Flags.V );
    EXPECT_FALSE( cpu.Flags.N );
}

TEST_F( M6502AndEorOraBitTests, TestBitAbsolute )
{
    // given:
    using namespace m6502;
    cpu.Flags.V = cpu.Flags.N = false;
    cpu.A = 0xCC;
    mem[0xFFFC] = opcode(Ins::BIT_ABS);
    mem[0xFFFD] = 0x00;
    mem[0xFFFE] = 0x80;
    mem[0x8000] = 0xCC;
    CPU CPUCopy = cpu;
    constexpr s64 EXPECTED_CYCLES = 4;

    //when:
    s64 CyclesUsed = cpu.Execute( EXPECTED_CYCLES );

    //then:
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.A, 0xCC );
    EXPECT_FALSE( cpu.Flags.Z );
    EXPECT_TRUE( cpu.Flags.V );
    EXPECT_TRUE( cpu.Flags.N );
}

TEST_F( M6502AndEorOraBitTests, TestBitAbsoluteResultZero )
{
    // given:
    using namespace m6502;
    cpu.Flags.V = cpu.Flags.N = true;
    cpu.A = 0xCC;
    mem[0xFFFC] = opcode(Ins::BIT_ABS);
    mem[0xFFFD] = 0x00;
    mem[0xFFFE] = 0x80;
    mem[0x8000] = 0x33;
    CPU CPUCopy = cpu;
    constexpr s64 EXPECTED_CYCLES = 4;

    //when:
    s64 CyclesUsed = cpu.Execute( EXPECTED_CYCLES );

    //then:
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.A, 0xCC );
    EXPECT_TRUE( cpu.Flags.Z );
    EXPECT_FALSE( cpu.Flags.V );
    EXPECT_FALSE( cpu.Flags.N );
}

TEST_F( M6502AndEorOraBitTests, TestBitAbsoluteResultZeroBit6And7Zero )
{
    // given:
    using namespace m6502;
    cpu.Flags.V = cpu.Flags.N = false;
    cpu.A = 0x33;
    mem[0xFFFC] = opcode(Ins::BIT_ABS);
    mem[0xFFFD] = 0x00;
    mem[0xFFFE] = 0x80;
    mem[0x8000] = 0xCC;
    CPU CPUCopy = cpu;
    constexpr s64 EXPECTED_CYCLES = 4;

    //when:
    s64 CyclesUsed = cpu.Execute( EXPECTED_CYCLES );

    //then:
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.A, 0x33 );
    EXPECT_TRUE( cpu.Flags.Z );
    EXPECT_TRUE( cpu.Flags.V );
    EXPECT_TRUE( cpu.Flags.N );
}

TEST_F( M6502AndEorOraBitTests, TestBitAbsoluteResultZeroBit6And7Mixed )
{
    // given:
    using namespace m6502;
    cpu.Flags.V = true;
    cpu.Flags.N = false;
    mem[0xFFFC] = opcode(Ins::BIT_ABS);
    mem[0xFFFD] = 0x00;
    mem[0xFFFE] = 0x80;
    mem[0x8000] = 0b10000000;
    CPU CPUCopy = cpu;
    constexpr s64 EXPECTED_CYCLES = 4;

    //when:
    cpu.Execute( EXPECTED_CYCLES );

    //then:
    EXPECT_FALSE( cpu.Flags.V );
    EXPECT_TRUE( cpu.Flags.N );
}