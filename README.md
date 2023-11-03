# PhysicsEngine

## Table of Contents

1. [Introduction](#1-introduction)
2. [Core Concepts](#2-core-concepts)
3. [Math Behind the Engine](#3-math-behind-the-engine)
4. [Rendering & Graphics](#4-rendering--graphics)
5. [How to Build](#5-how-to-build)
6. [How to Use the Engine](#6-how-to-use-the-engine)
7. [Dependencies and Submodules](#7-dependencies-and-submodules)
8. [Acknowledgements and References](#8-acknowledgments-and-references)

## 1. Introduction

- **Platform & Dependencies**: This is a standalone application built using C++17 and does not require any specific game engine to function. Core Dependencies: C++17, compatible with various operating systems.
- **Brief Description**: This physics engine provides a C++ framework for simulating rigid bodies in a 3D environment. Its modular design and straightforward integration make it ideal for learning and practical application in larger projects.

- **Inspiration**: The drive to create this engine originated from a desire to transform theoretical knowledge and mathematical formulas into a tangible and interactive application. This engine encapsulates that journey, taking abstract concepts from physics and implementing them in a real-world C++ framework.

- **Uniqueness**: The engine uniquely offers a rigid body system complemented by an interactive GUI. This GUI allows users to not only modify physical attributes but also to tweak graphical attributes, such as textures, on the fly.

## 2. Core Concepts

- **RigidBody**: As evident in the `RigidObject` class, a rigid body in this physics engine contains properties such as mass, position, and velocity. It acts as the fundamental simulation unit, responding to forces and participating in collisions.

- **Collider**: The engine supports multiple collider types, such as spheres and boxes. These colliders define the physical shape of a rigid body and dictate how it interacts with its surroundings and other colliders.

- **PhysicsWorld**: This component serves as the ecosystem in which all physical entities exist. It is responsible for updating the state of the world, managing collisions, and simulating the physical behavior of all objects.

## 3. Math Behind the Engine

- **Vector3**: Represents a 3D vector, a crucial component in many physics calculations like force, position, and velocity.

- **Matrix3**: Used for transformations, especially in the context of 3D rotations and scaling.

- **Quaternion**: These are pivotal in 3D rotations, helping to prevent gimbal lock and offer smoother interpolations.

- **Interactions and Collisions**: The engine computes interactions based on the type and parameters of the colliders. Collisions are resolved ensuring the conservation of momentum and energy, resulting in realistic simulations.

## 4. Rendering & Graphics

- **Shape**: The engine can render basic shapes, like cubes and spheres. These graphical representations are tightly integrated with their physics counterparts to ensure consistent visuals.

- **Renderer**: This component is responsible for drawing the objects on the screen, translating the physical world's state into visuals.

- **Texture & Material**: The engine allows texturing of objects, enhancing realism. The choice of material can influence the simulation's physical properties.

## 5. How to Build
The project is configured using premake. To build it on Windows, please run:
```Make_Solution-vs2022.bat```

## 6. How to Use the Engine

**Setup**:

Run the application to access the interactive GUI for the physics engine.

**Object Creation**:

GUI Tabs: Located on the right side of the screen.
Sphere and Cube: Click to create standard objects.
Bomb: Spawns other objects on collision, including with the ground.

**World Outliner**:

lists all objects in the simulator.
Select an object to access its customization GUI on the left side.
Modify the physical attributes and textures of the selected object.
Duplicate Objects: With an object selected, press "ctrl+d".

**Global Settings**:

Adjust global variables like gravity and time steps in the "Settings" tab.

**Scenario Management**:

"Save & Load" tab for managing preset scenarios.
Pause/Resume: Use GUI buttons or press the space bar.

**Camera Controls**:

Pan: W, A, S, and D keys
Vertical Movement: E (up) and Q (down).
Rotate: Drag with the right mouse button.
Additional Panning: Drag with the left mouse button.

**Object Interactions**:

Objects have behaviors based on their attributes and settings.
Example: A bomb spawns objects on impact.

**Matrix-Test Project**:

uses the Google Test framework.
validates the accuracy of matrix multiplication in the engine.
ensures the reliability of the physics simulation.
Run tests after updates to the core engine for best practice.

## 7. Dependencies and Submodules
The project uses git submodules to integrate the latest versions of essential libraries:

ImGui provides a graphical user interface.
JSON: For data handling and storage.
stb_image: Used for image loading.

## 8. Acknowledgments and References
Utilized libraries and versions:

- **GLFW**
- **Glad**
- **GLM**


Target operating system: Windows (64-bit)

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/J3xmOxF0B_E/0.jpg)](https://youtu.be/J3xmOxF0B_E)
[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/HJ4vcf8csIU/0.jpg)](https://youtu.be/HJ4vcf8csIU)
