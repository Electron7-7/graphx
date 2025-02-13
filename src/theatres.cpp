#include "theatres.hpp"
std::map<int, std::string> embedded_theatres =
{{0, std::string{R"~(@HelloWorldTheatre
Material (Doom_Shiny)
{
	DiffuseTexture    [DOOM_TEXTURE_DIFF]
	SpecularTexture   [DOOM_TEXTURE_SPEC]
	SpecularSharpness (256)
	SpecularStrength  (1.0)
}
Material (Doom_Dull)
{
	DiffuseTexture    [DOOM_TEXTURE_DIFF]
	SpecularTexture   [DOOM_TEXTURE_SPEC]
	SpecularSharpness (16)
	SpecularStrength  (0.5)
}
Mesh     (Cube)
{
	MeshData [GRAPHX_CUBE]
}
Collider (Test_Actor_1_Collider)
{
	MotionType      [Dynamic]
	ObjectLayer     [Moving]
	Activation      [Activate]
	Shape           [BoxShape]
	Scale           (1.2, 2.0, 1.0)
	Position        (0.0, 2.5, -6.0)
}
Collider (Floor_Collider)
{
	MotionType  [Static]
	ObjectLayer [NonMoving]
	Activation  [DontActivate]
	Shape       [BoxShape]
	Scale       (20.0, 1.0, 20.0)
}
RigidBodyActor (Test_Actor_1)
{
	Mesh:Material   <Cube>:<Doom_Shiny>
	Collider		<Test_Actor_1_Collider>
	Position        (0.0, 10.0, -6.0)
	Scale           (1.2, 2.0, 1.0)
}
RigidBodyActor (Floor)
{
	Mesh:Material <Cube>:<Doom_Dull>
	Collider	  <Floor_Collider>
	Scale		  (20.0, 1.0, 20.0)
}
GraphXPlayer (main_player)
{
	Position      (0.0, 6.0, 0.0)
	MovementSpeed (1.0)
}
LightDirectional (Sun)
{
	Direction (-0.2, -1.0, -0.3)
	Strength  (0.05)
}
LightFlashlight (Player_Flashlight)
{
})~"}}
};