//#include "AirsimLight/Geofence/VoxelFrameMeshBuilder.h"
//#include "ProceduralMeshComponent.h"
//
//UProceduralMeshComponent* UVoxelFrameMeshBuilder::CreateFrameCube(UObject* Outer, float EdgeThickness)
//{
//    UProceduralMeshComponent* Proc = NewObject<UProceduralMeshComponent>(Outer);
//    Proc->RegisterComponent();
//
//    TArray<FVector> V;
//    TArray<int32> Tri;
//
//    const float S = 50.f;
//    const float T = EdgeThickness;
//
//    FVector P[8] = {
//        FVector(-S,-S,-S), FVector(S,-S,-S),
//        FVector(S,S,-S),   FVector(-S,S,-S),
//        FVector(-S,-S,S),  FVector(S,-S,S),
//        FVector(S,S,S),    FVector(-S,S,S)
//    };
//
//    auto AddEdge = [&](int A, int B)
//        {
//            FVector P0 = P[A];
//            FVector P1 = P[B];
//            FVector Dir = (P1 - P0).GetSafeNormal();
//            FVector Up = FVector::UpVector;
//            FVector Right = FVector::CrossProduct(Up, Dir).GetSafeNormal();
//
//            FVector O = P0;
//            FVector D = P1 - P0;
//
//            FVector X = Right * T;
//            FVector Y = Up * T;
//
//            int Start = V.Num();
//
//            V.Add(O - X - Y);
//            V.Add(O + X - Y);
//            V.Add(O + X + Y);
//            V.Add(O - X + Y);
//
//            V.Add(O + D - X - Y);
//            V.Add(O + D + X - Y);
//            V.Add(O + D + X + Y);
//            V.Add(O + D - X + Y);
//
//            int32 Tris[] = {
//                0,2,1, 0,3,2,
//                4,5,6, 4,6,7,
//                0,7,3, 0,4,7,
//                1,2,6, 1,6,5,
//                3,7,6, 3,6,2,
//                0,1,5, 0,5,4
//            };
//
//            for (int i = 0; i < 36; i++)
//            {
//                Tri.Add(Start + Tris[i]);
//            }
//        };
//
//    AddEdge(0, 1); AddEdge(1, 2); AddEdge(2, 3); AddEdge(3, 0);
//    AddEdge(4, 5); AddEdge(5, 6); AddEdge(6, 7); AddEdge(7, 4);
//    AddEdge(0, 4); AddEdge(1, 5); AddEdge(2, 6); AddEdge(3, 7);
//
//    Proc->CreateMeshSection(0, V, Tri, {}, {}, {}, {}, true);
//
//    return Proc;
//}
