#include "theatres.hpp"
std::map<int, std::string> embedded_theatres =
{{0, std::string{R"~(@HelloWorldTheatre
Material (Doom_Shiny)
{
	DiffuseTexture    [DOOM_TEXTURE_DIFF]
	SpecularTexture   [DOOM_TEXTURE_SPEC]
	SpecularSharpness (256)
	SpecularStrength  (0.8)
}
Material (Doom_Dull)
{
	DiffuseTexture    [DOOM_TEXTURE_DIFF]
	SpecularTexture   [DOOM_TEXTURE_SPEC]
	SpecularSharpness (16)
	SpecularStrength  (0.5)
}
Mesh (Cube)
{
	MeshData [GRAPHX_CUBE]
}
Collider (Floor_Collider)
{
	MotionType  [Static]
	ObjectLayer [NonMoving]
	Activation  [DontActivate]
	Shape       [BoxShape]
}
RigidBodyActor (Floor)
{
	Mesh:Material <Cube>:<Doom_Dull>
	Collider	  <Floor_Collider>
	Scale		  (50.0, 1.0, 50.0)
}
Collider (Wall_Collider)
{
	MotionType  [Static]
	ObjectLayer [NonMoving]
	Activation  [DontActivate]
	Shape       [BoxShape]
}
RigidBodyActor (Wall)
{
	Mesh:Material <Cube>:<Doom_Dull>
	Collider	  <Wall_Collider>
	Scale		  (50.0, 50.0, 1.0)
	Position      (0.0, 25.0, -51.0)
}
LightDirectional (Sun)
{
	Direction (-0.2, -1.0, -0.3)
	Strength  (0.05)
	Color     (0.8, 0.9, 1.0)
}
LightTesterMover (spinny_light)
{
	PivotPosition (5.3, 3.0, -3.8)
	PivotRadius   (1.2)
	PivotSpeed    (1.8)
	Color         (1.0, 0.15, 0.3)
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
	RotationDegrees (5.0, -2.0, 37.0)
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
	RotationDegrees (0.0, 60.0, 25.0)
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
	RotationDegrees (5.0, -2.0, 37.0)
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
	RotationDegrees (15.0, -25.0, 37.0)
}
GraphXPlayer (main_player)
{
	Position             (0.0, 3.0, 6.0)
	MovementSpeed        (10.0)
	MovementAcceleration (2.0)
	Friction             (0.7)
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
	RotationDegrees (-45.0, 25.0, 8.0)
})~"}},{1, std::string{R"~(@CollisionTesting
GraphXPlayer (main_player)
{
	Position             (0.0, 3.0, 6.0)
	MovementSpeed        (10.0)
	MovementAcceleration (2.0)
	Friction             (0.7)
}
LightFlashlight (Player_Flashlight)
{
}
Material (Doom_Shiny)
{
	DiffuseTexture    [DOOM_TEXTURE_DIFF]
	SpecularTexture   [DOOM_TEXTURE_SPEC]
	SpecularSharpness (256)
	SpecularStrength  (0.8)
}
Material (Doom_Dull)
{
	DiffuseTexture    [DOOM_TEXTURE_DIFF]
	SpecularTexture   [DOOM_TEXTURE_SPEC]
	SpecularSharpness (16)
	SpecularStrength  (0.5)
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
	RotationDegrees (0.0, 53.7, 0.0)
}
StaticBodyActor (Platform_1)
{
	Mesh:Material   <Cube>:<Doom_Shiny>
	Scale		    (10.0, 50.0, 10.0)
	Position        (32.1, 75.0, -29.7)
	RotationDegrees (0.0, 20.0, 0.0)
}
StaticBodyActor (Platform_Floor_1)
{
	Mesh:Material   <Cube>:<Doom_Shiny>
	Scale		    (10.0, 5.0, 10.0)
	Position        (32.1, 5.0, -29.7)
	RotationDegrees <Platform_1>
}
StaticBodyActor (Platform_Floor_1)
{
	Mesh:Material   <Cube>:<Doom_Shiny>
	Scale		    (11.0, 0.5, 11.0)
	Position        (32.1, 0.0, -29.7)
	RotationDegrees <Platform_1>
}
Actor (Stairs_Visualizer)
{
	Mesh:Material   <Cube>:<Doom_Shiny>
	Scale           (10.0, 5.0, 1.0)
	Position        (0.0, 0.0, -10.0)
	RotationDegrees (80.0, 0.0, 0.0)
}
Collider (Stairs_1)
{
	MotionType      [Static]
	ObjectLayer     [NonMoving]
	Activation      [Activate]
	Shape           [BoxShape]
	Scale           <Stairs_Visualizer>
	Position        <Stairs_Visualizer>
	RotationDegrees <Stairs_Visualizer>
	ForeverAlone    (True)
}
LightDirectional (Sun)
{
	Direction (-0.2, -1.0, -0.3)
	Strength  (0.05)
	Color     (1.0, 1.0, 1.0)
})~"}}
};