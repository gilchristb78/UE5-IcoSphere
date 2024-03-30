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

	//TArray<FVector> Vertices = {
	//	FVector::DownVector,
	//	FVector::ForwardVector,
	//	FVector::LeftVector,
	//	FVector::BackwardVector,
	//	FVector::RightVector,
	//	FVector::UpVector
	//};
	//
	//TArray<int> Triangles = {
	//	2, 1, 0,
	//	3, 2, 0,
	//	4, 3, 0,
	//	1, 4, 0,
	//	

	//	1, 2, 5,
	//	2, 3, 5,
	//	3, 4, 5,
	//	4, 1, 5
	//};

	//TArray<FVector> Normals = TArray<FVector>();

	//for (int i = 0; i < Vertices.Num(); i++)
	//{
	//	Vertices[i] *= 800;
	//	Normals.Add(Vertices[i].GetSafeNormal());
	//	//Normals.Add(FVector::UpVector);
	//}

	int subdivisions = 5;
	int resolution = 1 << subdivisions;
	TArray<FVector> Vertices;
	//Vertices.SetNum((resolution + 1) * (resolution + 1) * 4 - (resolution * 2 - 1) * 3);
	TArray<int> Triangles;
	//Triangles.SetNum((1 << (subdivisions * 2 + 3)) * 3);
	CreateOctahedron(Vertices, Triangles, resolution);

	for (int i = 0; i < Vertices.Num(); i++)
	{
		Vertices[i] = Vertices[i].GetSafeNormal() * 8000;
		//	Normals.Add(Vertices[i].GetSafeNormal());
		//	//Normals.Add(FVector::UpVector);
	}

	
	Mesh->SetMaterial(0, Material);
	Mesh->CreateMeshSection(0, Vertices, Triangles, TArray<FVector>() , TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);

	
}

// Called every frame
void ATriangleSphere::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATriangleSphere::CreateOctahedron(TArray<FVector>& vertices, TArray<int>& triangles, int resolution)
{
	int v = 0, vBottom = 0, t = 0;

	for (int i = 0; i < 4; i++)
	{
		vertices.Add(FVector::DownVector);
		v++;
	}

	for (int i = 0; i <= resolution; i++)
	{
		float progress = (float)i / resolution;
		FVector from, to;
		from = FMath::Lerp(FVector::DownVector, FVector::ForwardVector, progress);
		vertices.Add(from);
		v++;
		to = FMath::Lerp(FVector::DownVector, FVector::LeftVector, progress);
		CreateLowerStrip(i, v, vBottom, triangles, t);
		CreateVertexLine(from, to, i, vertices, v);
		vBottom = v - 1 - i;
	}
}

void ATriangleSphere::CreateVertexLine(FVector from, FVector to, int steps, TArray<FVector>& vertices, int& v)
{
	for (int i = 1; i <= steps; i++)
	{
		vertices.Add(FMath::LerpStable(from, to, (float)i / steps));
		v++;
	}
}

void ATriangleSphere::CreateLowerStrip(int steps, int vTop, int vBottom, TArray<int>& triangles, int& t)
{
	for (int i = 1; i < steps; i++)
	{
		
		triangles.Add(vTop);
		triangles.Add(vTop - 1);
		triangles.Add(vBottom);

		triangles.Add(vBottom + 1);
		triangles.Add(vTop++);
		triangles.Add(vBottom++);
		t += 6;
	}

	triangles.Add(vTop);
	triangles.Add(vTop - 1);
	triangles.Add(vBottom);
	t += 3;
}

