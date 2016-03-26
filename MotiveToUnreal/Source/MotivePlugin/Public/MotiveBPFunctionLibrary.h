#pragma once

#if PLATFORM_WINDOWS
#include "AllowWindowsPlatformTypes.h"
#endif
#include <stdio.h>
#include <tchar.h>
#include <conio.h>
#include <winsock2.h>
#include <string>
#include <sstream>
#include <map>
#if PLATFORM_WINDOWS
#include "HideWindowsPlatformTypes.h"
#endif

#include "NatNetTypes.h"
#include "NatNetClient.h"

#include "MotiveBPFunctionLibrary.generated.h"

USTRUCT(BlueprintType)
struct FMoCapDataStruct
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MoCapData Struct")
		int32 ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MoCapData Struct")
		float x;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MoCapData Struct")
		float y;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MoCapData Struct")
		float z;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MoCapData Struct")
		float qx;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MoCapData Struct")
		float qy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MoCapData Struct")
		float qz;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MoCapData Struct")
		float qw;
};

UENUM(BlueprintType)
enum class EVectorOrder : uint8
{
	M_XYZ UMETA(DisplayName = "XYZ"),
	M_XZY UMETA(DisplayName = "XZY"),
	M_YXZ UMETA(DisplayName = "YXZ"),
	M_YZX UMETA(DisplayName = "YZX"),
	M_ZXY UMETA(DisplayName = "ZXY"),
	M_ZYX UMETA(DisplayName = "ZYX")
};

UCLASS(ClassGroup = Motive, Blueprintable)
class UMotiveBPFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Motive", meta = (AdvancedDisplay = "2"))
		static TArray<FMoCapDataStruct> ReceiveMotive();

	UFUNCTION(BlueprintCallable, Category = "Motive", meta = (AdvancedDisplay = "2"))
		static void CloseMotiveClient();

	UFUNCTION(BlueprintCallable, Category = "Motive", meta = (AdvancedDisplay = "2"))
		static FMoCapDataStruct getRigidbodyData(TArray<FMoCapDataStruct> MoCapDataArray, int32 ID);

	UFUNCTION(BlueprintCallable, Category = "Motive", meta = (AdvancedDisplay = "2"))
		static FVector FQuatToEuler(FQuat quaternion, EVectorOrder vectorOrder, bool reverseX, bool reverseY, bool reverseZ);
};
