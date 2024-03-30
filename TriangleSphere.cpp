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

	int subdivisions = 7;
	int resolution = 1 << subdivisions;
	TArray<FVector> Vertices;
	//Vertices.SetNum((resolution + 1) * (resolution + 1) * 4 - (resolution * 2 - 1) * 3);
	TArray<int> Triangles;
	//Triangles.SetNum((1 << (subdivisions * 2 + 3)) * 3);
	TArray<FVector> Normals;
	CreateOctahedron(Vertices, Triangles, resolution);

	for (int i = 0; i < Vertices.Num(); i++)
	{
		Vertices[i] = Vertices[i].GetSafeNormal() * 80000;
		Normals.Add(Vertices[i].GetSafeNormal());
		//	//Normals.Add(FVector::UpVector);
	}

	
	Mesh->SetMaterial(0, Material);
	Mesh->CreateMeshSection(0, Vertices, Triangles, Normals , TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);

	
}

// Called every frame
void ATriangleSphere::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATriangleSphere::CreateOctahedron(TArray<FVector>& vertices, TArray<int>& triangles, int resolution)
{
	int vBottom = 0;

	/*for (int i = 0; i < 4; i++)
	{
		vertices.Add(FVector::DownVector);
	}*/

	//vertices.Add(FVector::DownVector);

	TArray<FVector> direction1 = { FVector::ForwardVector, FVector::LeftVector, FVector::BackwardVector, FVector::RightVector };
	TArray<FVector> direction2 = { FVector::LeftVector, FVector::BackwardVector, FVector::RightVector, FVector::ForwardVector };
	TArray<FVector> direction3 = { FVector::DownVector, FVector::UpVector};

	for (int ud = 0; ud < direction3.Num(); ud++)
	{
		for (int d = 0; d < direction1.Num(); d++)
		{
			vertices.Add(direction3[ud]);
			vBottom = vertices.Num() - 1;
			for (int i = 0; i <= resolution; i++)
			{
				float progress = (float)i / resolution;
				FVector from, to;
				from = FMath::Lerp(direction3[ud], direction1[d], progress);
				vertices.Add(from);
				to = FMath::Lerp(direction3[ud], direction2[d], progress);
				CreateLowerStrip(i, vertices.Num(), vBottom, triangles);
				CreateUpperStrip(i, vertices.Num(), vBottom, triangles);
				CreateVertexLine(from, to, i, vertices);
				vBottom = vertices.Num() - 1 - i;
			}
		}
	}
	

	
}

void ATriangleSphere::CreateVertexLine(FVector from, FVector to, int steps, TArray<FVector>& vertices)
{
	for (int i = 1; i <= steps; i++)
	{
		vertices.Add(FMath::LerpStable(from, to, (float)i / steps));
	}
}

void ATriangleSphere::CreateLowerStrip(int steps, int vTop, int vBottom, TArray<int>& triangles)
{
	for (int i = 1; i < steps; i++)
	{
		
		triangles.Add(vTop);
		triangles.Add(vTop - 1);
		triangles.Add(vBottom);

		triangles.Add(vBottom + 1);
		triangles.Add(vTop++);
		triangles.Add(vBottom++);
	}

	triangles.Add(vTop);
	triangles.Add(vTop - 1);
	triangles.Add(vBottom);
}

void ATriangleSphere::CreateUpperStrip(int steps, int vTop, int vBottom, TArray<int>& triangles)
{
	for (int i = 1; i < steps; i++)
	{

		triangles.Add(vBottom);
		triangles.Add(vTop - 1);
		triangles.Add(vTop);

		triangles.Add(vBottom++);
		triangles.Add(vTop++);
		triangles.Add(vBottom);
	}

	triangles.Add(vBottom);
	triangles.Add(vTop - 1);
	triangles.Add(vTop);
}

