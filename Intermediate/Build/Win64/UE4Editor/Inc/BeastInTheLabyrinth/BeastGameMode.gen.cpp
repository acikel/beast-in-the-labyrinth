// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "BeastInTheLabyrinth/Public/Core/Game/BeastGameMode.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeBeastGameMode() {}
// Cross Module References
	BEASTINTHELABYRINTH_API UClass* Z_Construct_UClass_ABeastGameMode_NoRegister();
	BEASTINTHELABYRINTH_API UClass* Z_Construct_UClass_ABeastGameMode();
	ENGINE_API UClass* Z_Construct_UClass_AGameModeBase();
	UPackage* Z_Construct_UPackage__Script_BeastInTheLabyrinth();
// End Cross Module References
	void ABeastGameMode::StaticRegisterNativesABeastGameMode()
	{
	}
	UClass* Z_Construct_UClass_ABeastGameMode_NoRegister()
	{
		return ABeastGameMode::StaticClass();
	}
	struct Z_Construct_UClass_ABeastGameMode_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_ABeastGameMode_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_AGameModeBase,
		(UObject* (*)())Z_Construct_UPackage__Script_BeastInTheLabyrinth,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_ABeastGameMode_Statics::Class_MetaDataParams[] = {
		{ "Comment", "/**\n * \n */" },
		{ "HideCategories", "Info Rendering MovementReplication Replication Actor Input Movement Collision Rendering Utilities|Transformation" },
		{ "IncludePath", "Core/Game/BeastGameMode.h" },
		{ "ModuleRelativePath", "Public/Core/Game/BeastGameMode.h" },
		{ "ShowCategories", "Input|MouseInput Input|TouchInput" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_ABeastGameMode_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ABeastGameMode>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_ABeastGameMode_Statics::ClassParams = {
		&ABeastGameMode::StaticClass,
		"Game",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		nullptr,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		0,
		0,
		0x009002ACu,
		METADATA_PARAMS(Z_Construct_UClass_ABeastGameMode_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_ABeastGameMode_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_ABeastGameMode()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_ABeastGameMode_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(ABeastGameMode, 871365295);
	template<> BEASTINTHELABYRINTH_API UClass* StaticClass<ABeastGameMode>()
	{
		return ABeastGameMode::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_ABeastGameMode(Z_Construct_UClass_ABeastGameMode, &ABeastGameMode::StaticClass, TEXT("/Script/BeastInTheLabyrinth"), TEXT("ABeastGameMode"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(ABeastGameMode);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
