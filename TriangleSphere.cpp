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

	int subdivisions = 8;
	int resolution = 1 << subdivisions;
	TArray<FVector> Vertices;
	//Vertices.SetNum((resolution + 1) * (resolution + 1) * 4 - (resolution * 2 - 1) * 3);
	TArray<int> Triangles;
	//Triangles.SetNum((1 << (subdivisions * 2 + 3)) * 3);
	TArray<FVector> Normals;
	CreateOctahedron(Vertices, Triangles, resolution);

	TArray<FVector> Points;

	for (FVector vertice : Vertices)
	{
		Points.Add(vertice.GetSafeNormal() * 8000);
	}

	TArray<FVector> Craters;

	for (int i = 0; i < CraterNum; i++)
	{
		int CraterIndex = FMath::RandRange(0, Points.Num() - 1);
		Craters.Add(Points[CraterIndex]);
	}


	for (int i = 0; i < Vertices.Num(); i++)
	{
		float offset = 0;

		for (FVector Crater : Craters)
		{
			float distance = FVector::Distance(Crater, Points[i]) / 1000.0f; //radius

			float MainShape = distance * distance - 1;
			float CraterFloor = -0.7f; //percentage of crater before floor out of 1
			float RimSteepness = 0.33f; //how steep is transition from top of rim to outside
			float RimHeight = 1.61f; //how farm away should the outside of rim be drawn from (+ steep = how high)
			float smoothfactor = 0.4f;// how rounded are corners
			float Cutoff = SmoothMin(distance - 1 - RimHeight, 0, smoothfactor);
			float CraterRim = Cutoff * Cutoff * RimSteepness;
			

			float val = SmoothMax(SmoothMin(MainShape, CraterRim, smoothfactor), CraterFloor, smoothfactor);

			if (std::fabs(val) > 1e-6)
			{
				if (std::fabs(offset) < 1e-6)
				{
					offset = val * 1000.0f; //radius
				}
				else
				{
					offset += val * 1000.0f; //radius
				}
			}

			//offset += val * 100;
			


			/*if (FVector::Distance(Crater * 8000, Vertices[i].GetSafeNormal() * 8000) < 1000)
				offset = -500;*/
		}


		
		Points[i] += Vertices[i].GetSafeNormal() * offset;
		Normals.Add(Points[i].GetSafeNormal());
	}
	

	
	Mesh->SetMaterial(0, Material);
	Mesh->CreateMeshSection(0, Points, Triangles, Normals , TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);

	
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
			//vertices.Add(direction3[ud]);
			vBottom = vertices.Num();
			for (int i = 0; i <= resolution; i++)
			{
				float progress = (float)i / resolution;
				FVector from, to;
				from = FMath::Lerp(direction3[ud], direction1[d], progress);
				vertices.Add(from);
				to = FMath::Lerp(direction3[ud], direction2[d], progress);
				if (direction3[ud] == FVector::DownVector)
				{
					CreateLowerStrip(i, vertices.Num(), vBottom, triangles); //builds from the top down
				}
				else
				{
					CreateUpperStrip(i, vertices.Num(), vBottom, triangles); //builds from the bottom up
				}
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

float ATriangleSphere::SmoothMin(float a, float b, float k)
{
	float h = FMath::Clamp((b - a + k) / (2 * k), 0, 1);

	return a * h + b * (1 - h) - k * h * (1 - h);
}

float ATriangleSphere::SmoothMax(float a, float b, float k)
{
	return SmoothMin(a, b, -k);
}

