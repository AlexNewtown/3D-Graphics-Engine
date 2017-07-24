# Global Illumination

A graphics engine written in c++ with openGL. Used for testing global illumination algorithms.

## Editor

+ add entite(s), supports wavefront (.obj).
+ entity parameters: BRDF type, index of refraction, absorption coefficient, reduced scattering coefficient.
+ add lights.
+ set shading type.

### Controls:
+ wasd or right mouse button moves camera.
+ left mouse selects entities.
+ file->Export Scene, exports the scene data to a file.
+ file->Import Scene, imports the scene data from a file.

## Algorithms

### Offline

#### Stochastic Ray Tracing [1]

![Stochastic Ray Tracing](https://github.com/Gregjksmith/Global-Illumination/blob/master/images/rayTracedImage_uniform_10000.png?raw=true)

#### Photon Map [2]

![Photon Map](https://github.com/Gregjksmith/Global-Illumination/blob/master/images/photonMapDiffuse.png?raw=true)

## Online

#### Reflective Shadow Map [3]

#### Direct Lighting + Indirect Lighting + Subsurface Scattering
![Reflective Shadow Map](https://github.com/Gregjksmith/Global-Illumination/blob/master/images/Reflective%20Shadow%20Map.png?raw=true)

#### Instant Radiosity [4]

![Reflective Shadow Map](https://github.com/Gregjksmith/Global-Illumination/blob/master/images/Instant%20Radiosity%203.png?raw=true)

## License

This project is licensed under the terms of the MIT license.

[1] [Kajiya, James T. "The rendering equation." ACM Siggraph Computer Graphics. Vol. 20. No. 4. ACM, 1986.]

[2] [Jensen, Henrik Wann. "Global illumination using photon maps." Rendering Techniques’ 96. Springer Vienna, 1996. 21-30.]

[3] [Dachsbacher, Carsten, and Marc Stamminger. "Reflective shadow maps." Proceedings of the 2005 symposium on Interactive 3D graphics and games. ACM, 2005.]

[4] [Keller, Alexander. "Instant radiosity." Proceedings of the 24th annual conference on Computer graphics and interactive techniques. ACM Press/Addison-Wesley Publishing Co., 1997.]