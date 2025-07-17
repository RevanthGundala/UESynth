// Copyright (c) 2025 UESynth Project
// SPDX-License-Identifier: MIT

#include "../UESynthTestBase.h"
#include "pb/uesynth.grpc.pb.h"

/**
 * Unit tests for transform utility functions
 * Tests conversion between UE and protobuf transform representations
 */

// Test transform conversion utilities
class FUESynthTransformConversionTest : public FAutomationTestBase, public UESynthTestBase
{
public:
    FUESynthTransformConversionTest(const FString& InName, const bool bInComplexTask)
        : FAutomationTestBase(InName, bInComplexTask)
    {
        CurrentTest = this;
    }

    virtual bool RunTest(const FString& Parameters) override;
    bool RunTestImpl();

private:
    // Helper functions to test transform conversions
    uesynth::Transform UETransformToProtobuf(const FTransform& UETransform);
    FTransform ProtobufToUETransform(const uesynth::Transform& ProtoTransform);
};

IMPLEMENT_UESYNTH_UNIT_TEST(FUESynthTransformConversionTest,
    "UESynth.Unit.TransformUtils.Conversion",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
{
    // Test UE to Protobuf conversion
    {
        FVector TestLocation(100.0f, 200.0f, 300.0f);
        FRotator TestRotation(45.0f, 90.0f, 180.0f);
        FVector TestScale(1.5f, 2.0f, 0.5f);
        FTransform TestTransform(TestRotation, TestLocation, TestScale);

        uesynth::Transform ProtoTransform = UETransformToProtobuf(TestTransform);

        // Verify location conversion
        UESYNTH_TEST_TRUE(FMath::IsNearlyEqual(ProtoTransform.location().x(), TestLocation.X, 0.001f),
            "Location X should match");
        UESYNTH_TEST_TRUE(FMath::IsNearlyEqual(ProtoTransform.location().y(), TestLocation.Y, 0.001f),
            "Location Y should match");
        UESYNTH_TEST_TRUE(FMath::IsNearlyEqual(ProtoTransform.location().z(), TestLocation.Z, 0.001f),
            "Location Z should match");

        // Verify rotation conversion
        UESYNTH_TEST_TRUE(FMath::IsNearlyEqual(ProtoTransform.rotation().pitch(), TestRotation.Pitch, 0.001f),
            "Rotation Pitch should match");
        UESYNTH_TEST_TRUE(FMath::IsNearlyEqual(ProtoTransform.rotation().yaw(), TestRotation.Yaw, 0.001f),
            "Rotation Yaw should match");
        UESYNTH_TEST_TRUE(FMath::IsNearlyEqual(ProtoTransform.rotation().roll(), TestRotation.Roll, 0.001f),
            "Rotation Roll should match");
    }

    // Test Protobuf to UE conversion
    {
        uesynth::Transform ProtoTransform;
        ProtoTransform.mutable_location()->set_x(50.0f);
        ProtoTransform.mutable_location()->set_y(75.0f);
        ProtoTransform.mutable_location()->set_z(125.0f);
        ProtoTransform.mutable_rotation()->set_pitch(30.0f);
        ProtoTransform.mutable_rotation()->set_yaw(60.0f);
        ProtoTransform.mutable_rotation()->set_roll(90.0f);

        FTransform UETransform = ProtobufToUETransform(ProtoTransform);

        // Verify location conversion
        UESYNTH_TEST_TRUE(FMath::IsNearlyEqual(UETransform.GetLocation().X, 50.0f, 0.001f),
            "UE Location X should match");
        UESYNTH_TEST_TRUE(FMath::IsNearlyEqual(UETransform.GetLocation().Y, 75.0f, 0.001f),
            "UE Location Y should match");
        UESYNTH_TEST_TRUE(FMath::IsNearlyEqual(UETransform.GetLocation().Z, 125.0f, 0.001f),
            "UE Location Z should match");

        // Verify rotation conversion
        FRotator UERotation = UETransform.GetRotation().Rotator();
        UESYNTH_TEST_TRUE(FMath::IsNearlyEqual(UERotation.Pitch, 30.0f, 0.001f),
            "UE Rotation Pitch should match");
        UESYNTH_TEST_TRUE(FMath::IsNearlyEqual(UERotation.Yaw, 60.0f, 0.001f),
            "UE Rotation Yaw should match");
        UESYNTH_TEST_TRUE(FMath::IsNearlyEqual(UERotation.Roll, 90.0f, 0.001f),
            "UE Rotation Roll should match");
    }

    // Test round-trip conversion
    {
        FVector OriginalLocation(123.456f, 789.012f, 345.678f);
        FRotator OriginalRotation(12.34f, 56.78f, 90.12f);
        FTransform OriginalTransform(OriginalRotation, OriginalLocation);

        // Convert to protobuf and back
        uesynth::Transform ProtoTransform = UETransformToProtobuf(OriginalTransform);
        FTransform RoundTripTransform = ProtobufToUETransform(ProtoTransform);

        // Verify round-trip accuracy
        AssertVectorEquals(OriginalLocation, RoundTripTransform.GetLocation(), 0.01f);
        AssertRotatorEquals(OriginalRotation, RoundTripTransform.GetRotation().Rotator(), 0.01f);
    }

    return true;
}

// Helper function implementations
uesynth::Transform FUESynthTransformConversionTest::UETransformToProtobuf(const FTransform& UETransform)
{
    uesynth::Transform ProtoTransform;
    
    // Convert location
    ProtoTransform.mutable_location()->set_x(UETransform.GetLocation().X);
    ProtoTransform.mutable_location()->set_y(UETransform.GetLocation().Y);
    ProtoTransform.mutable_location()->set_z(UETransform.GetLocation().Z);
    
    // Convert rotation
    FRotator Rotation = UETransform.GetRotation().Rotator();
    ProtoTransform.mutable_rotation()->set_pitch(Rotation.Pitch);
    ProtoTransform.mutable_rotation()->set_yaw(Rotation.Yaw);
    ProtoTransform.mutable_rotation()->set_roll(Rotation.Roll);
    
    return ProtoTransform;
}

FTransform FUESynthTransformConversionTest::ProtobufToUETransform(const uesynth::Transform& ProtoTransform)
{
    FVector Location(
        ProtoTransform.location().x(),
        ProtoTransform.location().y(),
        ProtoTransform.location().z()
    );
    
    FRotator Rotation(
        ProtoTransform.rotation().pitch(),
        ProtoTransform.rotation().yaw(),
        ProtoTransform.rotation().roll()
    );
    
    return FTransform(Rotation, Location);
}

// Test edge cases and boundary conditions
class FUESynthTransformEdgeCasesTest : public FAutomationTestBase, public UESynthTestBase
{
public:
    FUESynthTransformEdgeCasesTest(const FString& InName, const bool bInComplexTask)
        : FAutomationTestBase(InName, bInComplexTask)
    {
        CurrentTest = this;
    }

    virtual bool RunTest(const FString& Parameters) override;
    bool RunTestImpl();
};

IMPLEMENT_UESYNTH_UNIT_TEST(FUESynthTransformEdgeCasesTest,
    "UESynth.Unit.TransformUtils.EdgeCases",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
{
    // Test zero transform
    {
        uesynth::Transform ZeroTransform;
        ZeroTransform.mutable_location()->set_x(0.0f);
        ZeroTransform.mutable_location()->set_y(0.0f);
        ZeroTransform.mutable_location()->set_z(0.0f);
        ZeroTransform.mutable_rotation()->set_pitch(0.0f);
        ZeroTransform.mutable_rotation()->set_yaw(0.0f);
        ZeroTransform.mutable_rotation()->set_roll(0.0f);

        UESYNTH_TEST_TRUE(ZeroTransform.location().x() == 0.0f, "Zero location X");
        UESYNTH_TEST_TRUE(ZeroTransform.location().y() == 0.0f, "Zero location Y");
        UESYNTH_TEST_TRUE(ZeroTransform.location().z() == 0.0f, "Zero location Z");
        UESYNTH_TEST_TRUE(ZeroTransform.rotation().pitch() == 0.0f, "Zero rotation Pitch");
        UESYNTH_TEST_TRUE(ZeroTransform.rotation().yaw() == 0.0f, "Zero rotation Yaw");
        UESYNTH_TEST_TRUE(ZeroTransform.rotation().roll() == 0.0f, "Zero rotation Roll");
    }

    // Test extreme values
    {
        uesynth::Transform ExtremeTransform;
        ExtremeTransform.mutable_location()->set_x(FLT_MAX);
        ExtremeTransform.mutable_location()->set_y(-FLT_MAX);
        ExtremeTransform.mutable_location()->set_z(0.0f);
        ExtremeTransform.mutable_rotation()->set_pitch(360.0f);
        ExtremeTransform.mutable_rotation()->set_yaw(-360.0f);
        ExtremeTransform.mutable_rotation()->set_roll(720.0f);

        UESYNTH_TEST_TRUE(ExtremeTransform.location().x() == FLT_MAX, "Extreme location X");
        UESYNTH_TEST_TRUE(ExtremeTransform.location().y() == -FLT_MAX, "Extreme location Y");
        UESYNTH_TEST_TRUE(ExtremeTransform.rotation().pitch() == 360.0f, "Extreme rotation Pitch");
        UESYNTH_TEST_TRUE(ExtremeTransform.rotation().yaw() == -360.0f, "Extreme rotation Yaw");
        UESYNTH_TEST_TRUE(ExtremeTransform.rotation().roll() == 720.0f, "Extreme rotation Roll");
    }

    // Test NaN and infinity handling
    {
        uesynth::Transform InvalidTransform;
        InvalidTransform.mutable_location()->set_x(std::numeric_limits<float>::quiet_NaN());
        InvalidTransform.mutable_location()->set_y(std::numeric_limits<float>::infinity());
        InvalidTransform.mutable_location()->set_z(-std::numeric_limits<float>::infinity());

        // These should be handled gracefully by the protobuf system
        UESYNTH_TEST_TRUE(std::isnan(InvalidTransform.location().x()), "NaN should be preserved");
        UESYNTH_TEST_TRUE(std::isinf(InvalidTransform.location().y()), "Infinity should be preserved");
        UESYNTH_TEST_TRUE(std::isinf(InvalidTransform.location().z()), "Negative infinity should be preserved");
    }

    return true;
}