### Intro to GraphXTheatres
The filename is pretty important and must follow this syntax: `#.Name.gt`
The number at the start designates the Theatre's UID and load order (starting at 0 and going up), the name can be whatever you want as long as it doesn't contain any whitespace, and the filename ".gt" stands for "graphxtheatre".
As an example, the filename for my HelloWorld Theatre is "0.HelloWorld.gt", and the filename for my CollisionTesting Theatre is "1.CollisionTesting.gt". Since the CollisionTesting Theatre's UID is greater than the HelloWorld Theatre's, it gets loaded last and is the one you actually see when the "game" launches.

Below is the general syntax for GraphXTheatre files:
```
@NameOfThisTheatre
Class (Name)
{
	RawDataVariable    (RawData)
	ReferenceVariable  [Reference]
	PointerVariable    <Name>
	Sand:Wich:Variable <Name>:<Name>:<Name>
}
```
Before I show you a full Theatre file with annotations, there are three important things to keep in mind:
1. The first line will name the Theatre and must always be included.
2. Class & Theatre names can include whitespace, but that can complicate things and potentially introduce errors depending on how I write the syntax reader, so use whitespace at your own risk. I recommend not using whitespace.
3. After the Theatre line, you're free to start creating Devices and Actors; however, keep in mind that their order is important! If you need to reference an object, it has to have already been created first! Here's an example of what I mean:
```
Material (Doom_Shiny)
{
	Diffuse           [DOOM_TEXTURE_DIFF]
	Specular          [DOOM_TEXTURE_SPEC]
	SpecularSharpness (256)
	SpecularStrength  (1.0)
}
Mesh (Cube)
{
	MeshData [GRAPHX_CUBE]
	Material <Doom_Shiny> This works because "Doom_Shiny" was already created before "Cube" was.
}

Don't do this:
Mesh (Cube)
{
	MeshData [GRAPHX_CUBE]
	Material <Doom_Shiny> This fails because "Doom_Shiny" doesn't exist yet!!
}
Material (Doom_Shiny)
{
	Diffuse           [DOOM_TEXTURE_DIFF]
	Specular          [DOOM_TEXTURE_SPEC]
	SpecularSharpness (256)
	SpecularStrength  (1.0)
}
```
Why did I make it this way? Because I couldn't be fucked to write a more complex parser.

Anyways, here's the [HelloWorldTheatre](https://github.com/Electron7-7/graphx/edit/trunk/src/theatres/0.HelloWorld.gt) file with some annotations to help explain the format. These annotations have been stylized to kinda look like C++ comments, but keep in mind that **comments don't exist in GraphXTheatre files**, so don't try to write any!
### 0.HelloWorldTheatre.gt
```
@HelloWorldTheatre
Material (Doom_Shiny) // A Material named "Doom_Shiny"
{
	// A reference variable is simply a variable that references something in the actual C++ code.
	// In this example, the variable "Diffuse" is referencing a preprocessor definition called "DOOM_TEXTURE_DIFF"
	Diffuse           [DOOM_TEXTURE_DIFF]
	Specular          [DOOM_TEXTURE_SPEC]

	// A raw data variable can be a string, a number, or a series of numbers separated by commas.
	// Currently, there's only support for up to three numbers at a time (since their main purpose is for filling
	// GLM vectors). Yes, that means that the "Quaternion" variable would crash the engine if used. I intend on
	// fixing that later but it's not a priority since the "RotationDegrees" variable gets turned into a quaternion
	// almost immediately after being read.
	SpecularSharpness (256)
	SpecularStrength  (1.0)
}
Material (Doom_Dull)
{
	Diffuse           [DOOM_TEXTURE_DIFF]
	Specular          [DOOM_TEXTURE_SPEC]
	SpecularSharpness (16)
	SpecularStrength  (0.5)
}
Mesh (Cube)
{
	MeshData [GRAPHX_CUBE]
}
@HelloWorldTheatre
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
	// This is an example of setting/overriding a variable inside of a referenced variable. To do this, add a colon
	// to the variable, followed by the name of the variable you want to override. Then, make sure you do the same
	// with the values. These are called "Sandwich Variables". You can only make sandwiches out of Pointer Variables
	// (the angled brackets). This is because fuck you go make your own lexer+parser+interpreter and see how fun
	// it is to try and account for that shit. In this example, I set Test_Actor_1's Mesh variable to the "Cube"
	// Mesh that I made earlier, but I override Cube's "Material" variable to the "Doom_Shiny" Material. I didn't
	// set Cube's "Material" variable, but if I had, this would have overridden the value. Keep in mind that when
	// referencing classes in variables that take that class, a copy of that class is used. This means that I can
	// override Cube's "Material" variable here without it affecting other classes
	// (e.g: Mesh <Cube> sets the class' "Mesh" variable to a copy of the "Cube" Mesh, and
	// Mesh:Material <Cube>:<Doom_Dull> sets the class' "Mesh" variable to a copy of the "Cube" Mesh and changes
	// that copy's "Material" variable to be the "Doom_Dull" Material).
	Mesh:Material <Cube>:<Doom_Dull>
	Scale         (50.0, 1.0, 50.0)
	Position      (0.0, -1.0, 0.0)
}
StaticBodyActor (Wall)
{
	Mesh:Material <Cube>:<Doom_Dull>
	Scale         (50.0, 50.0, 1.0)
	Position      (0.0, 50.0, -51.0)
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
	Collider      <Test_Collider_1>
	Scale	      (1.0, 1.0, 1.0)
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
	Collider	<Test_Collider_2>
	Scale		(3.0, 0.8, 1.0)
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
	Collider	<Test_Collider_3>
	Scale		(4.2, 0.9, 2.7)
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
	Collider	<Test_Collider_4>
	Scale		(0.78, 0.5, 0.8)
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
	Collider	<Test_Collider_5>
	Scale		(5.0, 3.5, 1.2)
	Position        (-5.3, 12.2, -6.2)
	Rotation        (15.0, -25.0, 37.0)
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
	Collider	<Test_Collider_6>
	Scale		(0.3, 0.5, 0.3)
	Position        (-5.3, 9.2, -6.2)
	Rotation        (-45.0, 25.0, 8.0)
}
```
### Some final notes
If you don't set any variables, they will use their default C++ values. If you misspell a variable name, (theoretically) nothing will happen and that variable will use its default value. This file format was written by me with no tutorials or guides on how to create a lexer, parser, interpreter, or anything else like that. That means that if you follow the syntax exactly, you'll have (hopefully) no problems, but if you miss a bracket, or use <> when you should've used [], or misspell a reference... the program will probably crash. I'll try to write as many error messages as I can remember to, but if the engine crashes and you don't know why, check your Theatre file first.

Oh, and if you want to see debug messages, you have to define GRAPHX_DEBUG when compiling (or just run `make debug [linux/windows]`).
