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

	UPROPERTY(EditInstanceOnly, Category = "Chunk World")
	TObjectPtr<UMaterialInterface> Material;

	UPROPERTY(EditInstanceOnly, Category = "Chunk World")
	int CraterNum;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	TObjectPtr<UProceduralMeshComponent> Mesh;

	void CreateOctahedron(TArray<FVector>& vertices, TArray<int>& triangles, int resolution);
	void CreateVertexLine(FVector from, FVector to, int steps, TArray<FVector>& vertices);
	void CreateLowerStrip(int steps, int vTop, int vBottom, TArray<int>& triangles);
	void CreateUpperStrip(int steps, int vTop, int vBottom, TArray<int>& triangles);
};
