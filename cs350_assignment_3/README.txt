/**
* @file README.txt
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/02/22	(eus)
* @brief readme
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/

-------ASSIGNMENT 3-------

1. Execute as "cs350_project.exe test" to run tests.

2. Execute with no arguments to run demo.

CAMERA CONTROLLS:
	- WASD			/move on XZ (horizontal) plane
	- QE			/move on Y (vertical) axis
	- RIGHT CLICK		/rotate camera

LEFT CLICK: 	Raycast test (cronometrado), inactive at GUI hovering
		Select Object
+LEFT SHIFT:	Add object to selection

GUI:
	- WINDOW			/fps, window properties
	- CAMERA			/config
	- SCENE				/move objects
	- BV HIERARCHY			/manage bounding volume hierarchy
	- OBJECT PROPERTIES 		/all objects selectable, ADD RANDOM, DELETE LAST
	- INSPECTOR			/Shows when object(s) selected, EDIT OBJECTS HERE

KNOWN BUGS:
	- Somehow unit tests from Assignment2 where failing at "iterative sphere", so I change the code to not underestimate the sphere at random iteration.
	- Wanted to add ImGuizmo, but division by 0 was happening while touching the Manipulator.
