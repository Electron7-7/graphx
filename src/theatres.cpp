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
Mesh (Cube)
{
	MeshData [GRAPHX_CUBE]
}
StaticBodyActor (Floor)
{
	Mesh:Material <Cube>:<Doom_Dull>
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
	AmbientLightingStrength (0.3)
}
LightDirectional (Sun)
{
	Direction (-0.2, -1.0, -0.3)
	Strength  (0.5)
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
Collider (Test_Collider_1)
{
	MotionType  [Dynamic]
	ObjectLayer [Moving]
	Activation  [Activate]
	Shape       [BoxShape]
}
RigidBodyActor (Falling_Cube_1)
{
	Mesh:Material <Cube>:<Doom_Shiny>
	Collider	  <Test_Collider_1>
	Scale		  (1.0, 1.0, 1.0)
	Position      (-2.0, 9.0, -6.0)
}
Collider (Test_Collider_2)
{
	MotionType      [Dynamic]
	ObjectLayer     [Moving]
	Activation      [Activate]
	Shape           [BoxShape]
}
RigidBodyActor (Falling_Cube_2)
{
	Mesh:Material   <Cube>:<Doom_Shiny>
	Collider	    <Test_Collider_2>
	Scale		    (3.0, 0.8, 1.0)
	Position        (-2.3, 11.5, -5.2)
	Rotation        (5.0, -2.0, 37.0)
}
Collider (Test_Collider_3)
{
	MotionType      [Dynamic]
	ObjectLayer     [Moving]
	Activation      [Activate]
	Shape           [BoxShape]
}
RigidBodyActor (Falling_Cube_3)
{
	Mesh:Material   <Cube>:<Doom_Dull>
	Collider	    <Test_Collider_3>
	Scale		    (4.2, 0.9, 2.7)
	Position        (-3.5, 6.7, -4.0)
	Rotation        (0.0, 60.0, 25.0)
}
Collider (Test_Collider_4)
{
	MotionType      [Dynamic]
	ObjectLayer     [Moving]
	Activation      [Activate]
	Shape           [BoxShape]
}
RigidBodyActor (Falling_Cube_4)
{
	Mesh:Material   <Cube>:<Doom_Dull>
	Collider	    <Test_Collider_4>
	Scale		    (0.78, 0.5, 0.8)
	Position        (-3.3, 7.2, -5.2)
	Rotation        (5.0, -2.0, 37.0)
}
Collider (Test_Collider_5)
{
	MotionType      [Dynamic]
	ObjectLayer     [Moving]
	Activation      [Activate]
	Shape           [BoxShape]
}
RigidBodyActor (Falling_Cube_5)
{
	Mesh:Material   <Cube>:<Doom_Shiny>
	Collider	    <Test_Collider_5>
	Scale		    (5.0, 3.5, 1.2)
	Position        (-5.3, 12.2, -6.2)
	Rotation        (15.0, -25.0, 37.0)
}
GraphXPlayer (main_player)
{
	Position             (0.0, 3.0, 6.0)
	MovementSpeed        (10.0)
	MovementAcceleration (2.0)
}
LightFlashlight (Player_Flashlight)
{
}
Collider (Test_Collider_6)
{
	MotionType      [Dynamic]
	ObjectLayer     [Moving]
	Activation      [Activate]
	Shape           [BoxShape]
}
RigidBodyActor (Falling_Cube_6)
{
	Mesh:Material   <Cube>:<Doom_Shiny>
	Collider	    <Test_Collider_6>
	Scale		    (0.3, 0.5, 0.3)
	Position        (-5.3, 9.2, -6.2)
	Rotation        (-45.0, 25.0, 8.0)
})~"}},{1, std::string{R"~(@CollisionTesting
GraphXPlayer (main_player)
{
	Position             (0.0, 3.0, 6.0)
	MovementSpeed        (13.0)
	MovementAcceleration (1.3)
}
LightFlashlight (Player_Flashlight)
{
}
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
})~"}},{0, std::string{R"~(@HelloWorldTheatre
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
Mesh (Cube)
{
	MeshData [GRAPHX_CUBE]
}
StaticBodyActor (Floor)
{
	Mesh:Material <Cube>:<Doom_Dull>
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
	AmbientLightingStrength (0.3)
}
LightDirectional (Sun)
{
	Direction (-0.2, -1.0, -0.3)
	Strength  (0.5)
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
Collider (Test_Collider_1)
{
	MotionType  [Dynamic]
	ObjectLayer [Moving]
	Activation  [Activate]
	Shape       [BoxShape]
}
RigidBodyActor (Falling_Cube_1)
{
	Mesh:Material <Cube>:<Doom_Shiny>
	Collider	  <Test_Collider_1>
	Scale		  (1.0, 1.0, 1.0)
	Position      (-2.0, 9.0, -6.0)
}
Collider (Test_Collider_2)
{
	MotionType      [Dynamic]
	ObjectLayer     [Moving]
	Activation      [Activate]
	Shape           [BoxShape]
}
RigidBodyActor (Falling_Cube_2)
{
	Mesh:Material   <Cube>:<Doom_Shiny>
	Collider	    <Test_Collider_2>
	Scale		    (3.0, 0.8, 1.0)
	Position        (-2.3, 11.5, -5.2)
	Rotation        (5.0, -2.0, 37.0)
}
Collider (Test_Collider_3)
{
	MotionType      [Dynamic]
	ObjectLayer     [Moving]
	Activation      [Activate]
	Shape           [BoxShape]
}
RigidBodyActor (Falling_Cube_3)
{
	Mesh:Material   <Cube>:<Doom_Dull>
	Collider	    <Test_Collider_3>
	Scale		    (4.2, 0.9, 2.7)
	Position        (-3.5, 6.7, -4.0)
	Rotation        (0.0, 60.0, 25.0)
}
Collider (Test_Collider_4)
{
	MotionType      [Dynamic]
	ObjectLayer     [Moving]
	Activation      [Activate]
	Shape           [BoxShape]
}
RigidBodyActor (Falling_Cube_4)
{
	Mesh:Material   <Cube>:<Doom_Dull>
	Collider	    <Test_Collider_4>
	Scale		    (0.78, 0.5, 0.8)
	Position        (-3.3, 7.2, -5.2)
	Rotation        (5.0, -2.0, 37.0)
}
Collider (Test_Collider_5)
{
	MotionType      [Dynamic]
	ObjectLayer     [Moving]
	Activation      [Activate]
	Shape           [BoxShape]
}
RigidBodyActor (Falling_Cube_5)
{
	Mesh:Material   <Cube>:<Doom_Shiny>
	Collider	    <Test_Collider_5>
	Scale		    (5.0, 3.5, 1.2)
	Position        (-5.3, 12.2, -6.2)
	Rotation        (15.0, -25.0, 37.0)
}
GraphXPlayer (main_player)
{
	Position             (0.0, 3.0, 6.0)
	MovementSpeed        (10.0)
	MovementAcceleration (2.0)
}
LightFlashlight (Player_Flashlight)
{
}
Collider (Test_Collider_6)
{
	MotionType      [Dynamic]
	ObjectLayer     [Moving]
	Activation      [Activate]
	Shape           [BoxShape]
}
RigidBodyActor (Falling_Cube_6)
{
	Mesh:Material   <Cube>:<Doom_Shiny>
	Collider	    <Test_Collider_6>
	Scale		    (0.3, 0.5, 0.3)
	Position        (-5.3, 9.2, -6.2)
	Rotation        (-45.0, 25.0, 8.0)
})~"}}
};
