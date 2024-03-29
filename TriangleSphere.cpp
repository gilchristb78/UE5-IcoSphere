// Fill out your copyright notice in the Description page of Project Settings.


#include "TriangleSphere.h"
#include "ProceduralMeshComponent.h"

// Sets default values
ATriangleSphere::ATriangleSphere()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UProceduralMeshComponent>("Mesh");
	Mesh->SetCastShadow(false);
	RootComponent = Mesh;

}

// Called when the game starts or when spawned
void ATriangleSphere::BeginPlay()
{
	Super::BeginPlay();

	TArray<FVector> Vertices = {
		FVector::DownVector,
		FVector::ForwardVector,
		FVector::LeftVector,
		FVector::BackwardVector,
		FVector::RightVector,
		FVector::UpVector
	};
	
	TArray<int> Triangles = {
		2, 1, 0,
		3, 2, 0,
		4, 3, 0,
		1, 4, 0,
		

		1, 2, 5,
		2, 3, 5,
		3, 4, 5,
		4, 1, 5
	};

	TArray<FVector> Normals = TArray<FVector>();

	for (int i = 0; i < Vertices.Num(); i++)
	{
		Vertices[i] *= 800;
		Normals.Add(Vertices[i].GetSafeNormal());
		//Normals.Add(FVector::UpVector);
	}

	
	Mesh->SetMaterial(0, Material);
	Mesh->CreateMeshSection(0, Vertices, Triangles, Normals, TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);
}

// Called every frame
void ATriangleSphere::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

