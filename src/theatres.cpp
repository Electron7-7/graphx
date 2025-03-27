#include <string>
#include <map>
std::map<int, std::string> embedded_theatres =
{{0, std::string{R"~(@HelloWorldTheatre
Material (Doom_Shiny)
{
	DiffuseTexture    [DOOM_TEXTURE_DIFF]
	SpecularTexture   [DOOM_TEXTURE_SPEC]
	SpecularSharpness (64)
	SpecularStrength  (0.8)
}
Material (Doom_Dull)
{
	DiffuseTexture    [DOOM_TEXTURE_DIFF]
	SpecularTexture   [DOOM_TEXTURE_SPEC]
	SpecularSharpness (8)
	SpecularStrength  (0.4)
}
Material (Source_Mat_1)
{
	DiffuseTexture [SOURCE_ORANGE]
	SpecularTexture [NO_TEXTURE]
	SpecularStrength (0)
}
Material (Suzanne_Mat)
{
	DiffuseTexture [SOURCE_LIGHT_GREY]
	SpecularTexture [NO_TEXTURE]
	Color (1.0, 0.05, 0.1)
	SpecularSharpness (128)
	SpecularStrength (0.5)
}
Material (Pyramid_Mat)
{
	DiffuseTexture [SOURCE_LIGHT_GREY]
	SpecularTexture [NO_TEXTURE]
	Color (0.1, 0.7, 0.9)
	SpecularSharpness (128)
	SpecularStrength (0.5)
}
Mesh (Cube)
{
	MeshData [GRAPHX_CUBE]
}
Mesh (Pyramid)
{
	MeshData [GRAPHX_PYRAMID]
}
Mesh (OBJ_Mesh)
{
	MeshData [OBJ_SUZANNE]
}
Actor (Suzanne_Tester)
{
	Mesh:Material <OBJ_Mesh>:<Suzanne_Mat>
	Scale (1.0, 1.0, 1.0)
	Position (10.0, 4.0, 0.0)
}
Actor (Pyramid_Tester)
{
	Mesh:Material <Pyramid>:<Source_Mat_1>
	Scale (1.5, 1.5, 1.5)
	Position (8.0, 4.0, -4.0)
}
Actor (Pyramid_Tester_2)
{
	Mesh:Material <Pyramid>:<Source_Mat_1>
	Scale (1.5, 1.5, 1.5)
	Position (8.0, 1.0, -8.0)
}
StaticBodyActor (Floor)
{
	Mesh:MeshData:Material <OBJ_Mesh>:[GRAPHX_CUBE]:<Doom_Dull>
	Scale		  (50.0, 1.0, 50.0)
	Position      (0.0, -1.0, 0.0)
}
StaticBodyActor (Wall)
{
	Mesh:Material <Cube>:<Doom_Dull>
	Scale		  (50.0, 50.0, 1.0)
	Position      (0.0, 50.0, -51.0)
}
Environment (environment)
{
	AmbientLightingColor (1.0, 1.0, 1.0)
	AmbientLightingStrength (0.2)
}
LightDirectional (Sun)
{
	Direction (-0.2, -1.0, -0.3)
	Strength  (0.2)
	Color     (0.8, 0.9, 1.0)
}
LightTesterMover (spinny_light)
{
	PivotPosition (5.3, 1.0, -3.8)
	PivotRadius   (1.2)
	PivotSpeed    (1.8)
	Color         (1.0, 0.15, 0.3)
	Strength      (1.0)
}
LightTesterMover (spinny_light_2)
{
	PivotPosition (-11.0, 1.7, -4.3)
	PivotRadius   (2.4)
	PivotSpeed    (0.87)
	Color         (0.10, 1.0, 0.4)
}
RigidBodyActor (Falling_Cube_1)
{
	Mesh:Material <Cube>:<Doom_Shiny>
	Scale		  (1.0, 1.0, 1.0)
	Position      (-2.0, 9.0, -6.0)
}
RigidBodyActor (Falling_Cube_2)
{
	Mesh:Material   <Cube>:<Doom_Shiny>
	Scale		    (3.0, 0.8, 1.0)
	Position        (-2.3, 11.5, -5.2)
	Rotation        (5.0, -2.0, 37.0)
}
RigidBodyActor (Falling_Cube_3)
{
	Mesh:Material   <Cube>:<Doom_Dull>
	Scale		    (4.2, 0.9, 2.7)
	Position        (-3.5, 6.7, -4.0)
	Rotation        (0.0, 60.0, 25.0)
}
RigidBodyActor (Falling_Cube_4)
{
	Mesh:Material   <Cube>:<Doom_Dull>
	Scale		    (0.78, 0.5, 0.8)
	Position        (-3.3, 7.2, -5.2)
	Rotation        (5.0, -2.0, 37.0)
}
RigidBodyActor (Falling_Cube_5)
{
	Mesh:Material   <Cube>:<Source_Mat_1>
	Scale		    (5.0, 3.5, 1.2)
	Position        (-5.3, 12.2, -6.2)
	Rotation        (15.0, -25.0, 37.0)
}
RigidBodyActor (Falling_Cube_6)
{
	Mesh:Material   <Cube>:<Source_Mat_1>
	Scale		    (0.3, 0.5, 0.3)
	Position        (-5.3, 9.2, -6.2)
	Rotation        (-45.0, 25.0, 8.0)
}
GraphXPlayer (main_player)
{
	Position             (0.0, 3.0, 6.0)
	MovementSpeed        (10.0)
	MovementAcceleration (2.0)
}
LightFlashlight (Player_Flashlight)
{
})~"}},{1, std::string{R"~(@CollisionTesting
GraphXPlayer (main_player)
{
	Position             (0.0, 3.0, 6.0)
	MovementSpeed        (13.0)
	MovementAcceleration (1.3)
}
LightFlashlight (Player_Flashlight)
{}
Environment (environment)
{}
Material (FullBright_1)
{
	mat_fullbright (True)
	Color (1.2, 1.0, 0.0)
	DiffuseTexture [SOURCE_LIGHT_GREY]
}
Material (Source_Yellow)
{
	Color (1.2, 1.0, 0.0)
	DiffuseTexture [SOURCE_LIGHT_GREY]
	SpecularStrength (0.0)
}
Material (Source_Green)
{
	Color (0.0, 1.0, 0.3)
	DiffuseTexture [SOURCE_LIGHT_GREY]
	SpecularStrength (0.0)
}
Material (Doom_Shiny)
{
	DiffuseTexture    [DOOM_TEXTURE_DIFF]
	SpecularTexture   [DOOM_TEXTURE_SPEC]
	SpecularSharpness (64)
	SpecularStrength  (0.8)
}
Material (Doom_Dull)
{
	DiffuseTexture    [DOOM_TEXTURE_DIFF]
	SpecularTexture   [DOOM_TEXTURE_SPEC]
	SpecularSharpness (16)
	SpecularStrength  (0.4)
}
Mesh (Cube)
{
	MeshData [GRAPHX_CUBE]
}
StaticBodyActor (Floor)
{
	Mesh:Material <Cube>:<Doom_Dull>
	Scale		  (200.0, 1.0, 200.0)
	Position      (0.0, -1.0, 0.0)
}
StaticBodyActor (Spire_1)
{
	Mesh:Material   <Cube>:<Doom_Dull>
	Scale		    (20.0, 100.0, 20.0)
	Position        (-28.3, 100.0, -43.2)
	Rotation        (0.0, 53.7, 0.0)
}
StaticBodyActor (Platform_1)
{
	Mesh:Material   <Cube>:<Doom_Shiny>
	Scale		    (10.0, 50.0, 10.0)
	Position        (32.1, 75.0, -29.7)
	Rotation        (0.0, 20.0, 0.0)
}
StaticBodyActor (Platform_Floor_1)
{
	Mesh:Material   <Cube>:<Doom_Shiny>
	Scale		    (10.0, 5.0, 10.0)
	Position        <Platform_1>
	LocalPosition   (0.0, -70.0, 0.0)
	Rotation        <Platform_1>
}
StaticBodyActor (Platform_Stair_1)
{
	Mesh:Material   <Cube>:<Doom_Shiny>
	Scale		    (11.0, 2.5, 11.0)
	Position        <Platform_1>
	LocalPosition   (0.0, -72.5, 0.0)
	Rotation        <Platform_1>
}
StaticBodyActor (Platform_Stair_2)
{
	Mesh:Material   <Cube>:<Doom_Shiny>
	Scale		    (12.0, 2.0, 12.0)
	Position        <Platform_1>
	LocalPosition   (0.0, -73.0, 0.0)
	Rotation        <Platform_1>
}
StaticBodyActor (Platform_Stair_3)
{
	Mesh:Material   <Cube>:<Doom_Shiny>
	Scale		    (13.0, 1.5, 13.0)
	Position        <Platform_1>
	LocalPosition   (0.0, -73.5, 0.0)
	Rotation        <Platform_1>
}
StaticBodyActor (Platform_Stair_4)
{
	Mesh:Material   <Cube>:<Doom_Shiny>
	Scale		    (14.0, 1.0, 14.0)
	Position        <Platform_1>
	LocalPosition   (0.0, -74.0, 0.0)
	Rotation        <Platform_1>
}
StaticBodyActor (Platform_Stair_5)
{
	Mesh:Material   <Cube>:<Doom_Shiny>
	Scale		    (15.0, 0.5, 15.0)
	Position        <Platform_1>
	LocalPosition   (0.0, -74.5, 0.0)
	Rotation        <Platform_1>
}
RigidBodyActor (Falling_Actor_1)
{
	Mesh:Material <Cube>:<Doom_Shiny>
	Scale (1.0, 1.0, 1.0)
	Position (0.14, 3.12, -2.13)
}
RigidBodyActor (Falling_Actor_2)
{
	Mesh:Material <Cube>:<Doom_Shiny>
	Scale (2.34, 0.87, 1.2)
	Position (0.14, 8.12, -2.13)
	Rotation (4.3, 67.3, 0.0)
}
RigidBodyActor (Falling_Actor_3)
{
	Mesh:Material <Cube>:<Source_Yellow>
	Scale (0.54, 0.37, 0.23)
	Position (0.14, 5.12, -2.13)
	Rotation (4.3, 67.3, -62.0)
}
RigidBodyActor (Falling_Actor_4)
{
	Mesh:Material <Cube>:<Source_Green>
	Scale (2.54, 0.77, 1.23)
	Position (-2.3, 12.0, -3.63)
	Rotation (4.3, 67.3, -62.0)
}
RigidBodyActor (Falling_Actor_5)
{
	Mesh:Material <Cube>:<Doom_Dull>
	Scale (2.54, 0.77, 2.23)
	Position (-1.7, 16.0, -4.63)
	Rotation (4.3, -43.3, 62.0)
}
LightDirectional (Sun)
{
	Direction (-0.1, -1.0, -0.3)
	Strength  (0.05)
	Color     (1.0, 1.0, 1.0)
}
LightTesterMover (test_light_1)
{
	Color (0.7, 0.2, 1.0)
	PivotPosition (-2.2, 3.8, -7.5)
	PivotRadius (3.0)
	PivotSpeed (2.2)
}
LightTesterMover (test_light_2)
{
	Color (1.0, 0.42, 0.1)
	PivotPosition (8.4, 1.8, -3.0)
	PivotRadius (2.0)
	PivotSpeed (1.2)
})~"}},{2, std::string{R"~(@RamielTheatre
LightDirectional (Sun)
{
	Strength (0.5)
	Direction (0.2, -0.2, -0.1)
}
LightFlashlight (Flashlight)
{}
GraphXPlayer (player)
{
	Position (0.0, 1.0, 170.0)
	MouseSensitivity (0.1)
}
Environment (Default_Environment)
{
	AmbientLightingStrength (0.3)
}
Material (Floor_Material)
{
	// Testing comments here
	DiffuseTexture [DOOM_TEXTURE_DIFF]
	SpecularTexture [DOOM_TEXTURE_SPEC]
	SpecularStrength (0.4)
	SpecularSharpness (8)
}
Material (Second_Material)
{
	DiffuseTexture <Floor_Material>
	SpecularTexture <Floor_Material>
	SpecularStrength (0.8)
	SpecularSharpness (128)
}
Mesh (Floor_Mesh)
{
	MeshData [GRAPHX_CUBE]
}
StaticBodyActor (Floor)
{
	Mesh:Material <Floor_Mesh>:<Floor_Material>
	Scale (200.0, 1.0, 200.0)
	Position (0.0, -1.0, 0.0)
}
Mesh (TestMesh)
{
	// Testing external references.
	// Both absolute and relative paths are accepted.
	// Absolute paths are used as-is, but relative paths are made relative to the program's location, not the user's
	// MeshData "../src/models/purely_for_testing.obj"
	// MeshData "theatres/test.obj"
	// MeshData "theatres/ramiel.obj"
	MeshData [Ramiel]
}
Light (fix_light)
{
	Position (0.0, 10.0, -4.0)
	Color (1.0, 1.0, 1.0)
	Strength (1.0)
	Range (200.0)
	Intensity (0.2)
}
Light (fix_light_2)
{
	Position (0.0, 50.0, 50.0)
	Color (1.0, 1.0, 1.0)
	Strength (1.0)
	Range (300.0)
	Intensity (0.2)
}
Light (fix_light_3)
{
	Position (-10.0, 20.0, -20.0)
	Color (1.0, 1.0, 1.0)
	Strength (1.0)
	Range (200.0)
	Intensity (0.2)
}
Light (fix_light_4)
{
	Position (20.0, 25.0, 80.0)
	Color (1.0, 1.0, 1.0)
	Strength (1.0)
	Range (200.0)
	Intensity (0.2)
}
Material (Ramiel_Mat)
{
	DiffuseTexture [NO_TEXTURE]	
	SpecularTexture [FLAT_SPEC]
	SpecularStrength (0.9)
	SpecularSharpness (256)
	// Color (0.6, 0.9, 0.8)
}
Ramiel (Ramiel)
{
	Mesh:Material <TestMesh>:<Ramiel_Mat>
	Scale (300.0, 300.0, 300.0)
	Position (600.0, 465.0, -800.0)
	Rotation (0.0, 148.0, 0.0)
	MovementType (1)
	MovementSpeed (0.5)
	// PivotPosition (0.0, 90.0, -20.0)
	// PivotRadius   (40.2)
	// PivotSpeed    (0.2)
}
Mesh (Cube_Mesh)
{
	MeshData [GRAPHX_CUBE]
}
RigidBodyActor (FallOnMe)
{
	Mesh <Cube_Mesh>
	Position (0.0, 1.5, -4.0)
	Scale (3.4, 0.3, 7.8)
}
RigidBodyActor (FallOnMe2)
{
	Mesh <Cube_Mesh>
	Position (0.0, 3.0, -4.0)
	Scale (5.1, 0.3, 3.2)
}
RigidBodyActor (FallOnMe3)
{
	Mesh <Cube_Mesh>
	Position (0.0, 5.0, -5.0)
	Scale (1.1, 3.3, 0.7)
}
Actor (fake_actor)
{
	Position (0.0, -100.0, 0.0)
	Scale (20.0, 10.0, 3.0)
})~"}}
};