// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TriangleSphere.generated.h"

class UProceduralMeshComponent;

UCLASS()
class ICOSPHERE_API ATriangleSphere : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATriangleSphere();

	UPROPERTY(EditAnywhere, Category = "Planet")
	TObjectPtr<UMaterialInterface> Material;

	UPROPERTY(EditAnywhere, Category = "Planet")
	int CraterNum = 10;

	UPROPERTY(EditAnywhere, Category = "Planet")
	float PlanetRadius = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "Planet")
	int SubDivisions = 2;

	UPROPERTY(EditAnywhere, Category = "Crater")
	float CraterRadius = 250.0f;

	UPROPERTY(EditAnywhere, Category = "Crater")
	float CraterFloor = -0.2f;

	UPROPERTY(EditAnywhere, Category = "Crater")
	float RimSteepness = 0.23f;

	UPROPERTY(EditAnywhere, Category = "Crater")
	float RimHeight = 0.81f;

	UPROPERTY(EditAnywhere, Category = "Crater")
	float Smoothfactor = 0.2f;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:

	TArray<FVector> Points;
	TArray<FVector> Vertices;
	TArray<int> Triangles;
	TArray<FVector> Craters;
	


	TObjectPtr<UProceduralMeshComponent> Mesh;

	void CreateOctahedron(TArray<FVector>& vertices, TArray<int>& triangles, int resolution);
	void CreateVertexLine(FVector from, FVector to, int steps, TArray<FVector>& vertices);
	void CreateLowerStrip(int steps, int vTop, int vBottom, TArray<int>& triangles);
	void CreateUpperStrip(int steps, int vTop, int vBottom, TArray<int>& triangles);
	float SmoothMin(float a, float b, float k);
	float SmoothMax(float a, float b, float k);

	void CreatePlanet();
	void setPoints();

	float GetCraterVal(float Distance);
	
};
