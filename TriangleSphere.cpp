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

	CreatePlanet();
	
}

// Called every frame
void ATriangleSphere::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATriangleSphere::CreateOctahedron(TArray<FVector>& vertices, TArray<int>& triangles, int resolution)
{
	int vBottom = 0;

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

void ATriangleSphere::CreatePlanet()
{
	
	setPoints();

	Craters = TArray<FVector>();

	for (int i = 0; i < CraterNum; i++)
	{
		int CraterIndex = FMath::RandRange(0, Vertices.Num() - 1);
		Craters.Add(Vertices[CraterIndex]);
	}

	for (int i = 0; i < Vertices.Num(); i++)
	{
		float offset = 0;

		for (FVector Crater : Craters)
		{
			float distance = FVector::Distance(Crater, Vertices[i]) / CraterRadius; //radius
			float val = GetCraterVal(distance);

			if (std::fabs(val) > 0.001f)
			{
				offset += val * CraterRadius;
			}
		}
		Vertices[i] += Points[i].GetSafeNormal() * offset;
	}

	Mesh->SetMaterial(0, Material);
	Mesh->CreateMeshSection(0, Vertices, Triangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);
}

void ATriangleSphere::setPoints()
{
	int resolution = 1 << SubDivisions;

	Points = TArray<FVector>();
	Triangles = TArray<int>();
	CreateOctahedron(Points, Triangles, resolution);

	Vertices = TArray<FVector>();
	for (FVector point : Points)
	{
		Vertices.Add(point.GetSafeNormal() * PlanetRadius);
	}
}

float ATriangleSphere::GetCraterVal(float Distance)
{

	//float CraterFloor = -0.2f; //percentage of crater before floor out of 1
	//float RimSteepness = 0.23f; //how steep is transition from top of rim to outside
	//float RimHeight = 0.81f; //how farm away should the outside of rim be drawn from (+ steep = how high)
	//float smoothfactor = 0.2f;// how rounded are corners

	float MainShape = Distance * Distance - 1;
	float Cutoff = SmoothMin(Distance - 1 - RimHeight, 0, Smoothfactor);
	float CraterRim = Cutoff * Cutoff * RimSteepness;

	return FMath::Max(SmoothMin(MainShape, CraterRim, Smoothfactor), CraterFloor);
}

#if WITH_EDITOR
void ATriangleSphere::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ATriangleSphere, SubDivisions) ||
		PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ATriangleSphere, CraterNum) ||
		PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ATriangleSphere, PlanetRadius) ||
		PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ATriangleSphere, CraterRadius) ||
		PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ATriangleSphere, CraterFloor) ||
		PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ATriangleSphere, RimSteepness) ||
		PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ATriangleSphere, RimHeight) ||
		PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ATriangleSphere, Smoothfactor))
	{
		CreatePlanet(); // Call CreatePlanet() whenever SubDivisions changes
	}
}
#endif

