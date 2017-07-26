# 3D Graphics Engine

A 3D graphics engine written in c++ with openGL. Used primarily for testing global illumination algorithms.

![editor sample](https://github.com/Gregjksmith/Global-Illumination/blob/master/images/editorSample.png?raw=true)

## Features

+ add entities, supports wavefront (.obj) / point clouds (.pcl, custom data structure).
+ set entity parameters:
	* position, scale, rotation, index of refraction, microfacet roughness, absorption, reduced scattering.
+ add lights.
+ set light parameters:
	* light intensity.
+ set shading type:
	* Direct Shadow Map.
	* Stochastic Ray tracing (offline).
	* Photon Map (offline).
	* Reflective Shadow Map.
	* instant radiosity.
+ file->Export Scene, exports the scene data to a file.
+ file->Import Scene, imports the scene data from a file.
+ delete entities.
+ delete lights.

### Controls:
+ wasd or right mouse button moves camera.
+ shift moves the camera down.
+ space bar moves the camera up.
+ left mouse selects entities.
+ choose an entity from a list to edit.

## Dependencies

Headers and library files must be included into your project:

GLEW, freeglut, OpenCL, OpenCV

## Install

Include and build source located at 'src'.

Two separate Builds may exist: an editor build and a viewer build. 
The Editor build includes editing function whereas the viewer build does not. The viewer build simply loads the scene file and allows
the user to move the camera.

For editor builds add preprocessor '_EDITOR' for viewer builds add '_VIEWER'.

## Algorithms

A comprehensive global illumination survey can be found here: [Global illumination survey](https://gregjksmith.github.io/3D-Graphics-Engine/)

### Offline

#### Stochastic Ray Tracing [1]

![Stochastic Ray Tracing](https://github.com/Gregjksmith/3D-Graphics-Engine/blob/master/images/rayTracedImage_uniform_10000.png?raw=true)

#### Photon Map [2]

![Photon Map](https://github.com/Gregjksmith/3D-Graphics-Engine/blob/master/images/photonMapDiffuse.png?raw=true)

## Online

#### Reflective Shadow Map [3]

Direct Lighting + Indirect Lighting + Subsurface Scattering

![Reflective Shadow Map](https://github.com/Gregjksmith/3D-Graphics-Engine/blob/master/images/Reflective%20Shadow%20Map.png?raw=true)

#### Instant Radiosity [4]

![Instant Radiosity](https://github.com/Gregjksmith/3D-Graphics-Engine/blob/master/images/Instant%20Radiosity%203.png?raw=true)

## License

This project is licensed under the terms of the MIT license.

[1] [Kajiya, James T. "The rendering equation." ACM Siggraph Computer Graphics. Vol. 20. No. 4. ACM, 1986.]

[2] [Jensen, Henrik Wann. "Global illumination using photon maps." Rendering Techniques’ 96. Springer Vienna, 1996. 21-30.]

[3] [Dachsbacher, Carsten, and Marc Stamminger. "Reflective shadow maps." Proceedings of the 2005 symposium on Interactive 3D graphics and games. ACM, 2005.]

[4] [Keller, Alexander. "Instant radiosity." Proceedings of the 24th annual conference on Computer graphics and interactive techniques. ACM Press/Addison-Wesley Publishing Co., 1997.]