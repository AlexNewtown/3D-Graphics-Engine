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

### Photon Map
[Jensen, Henrik Wann. "Global illumination using photon maps." Rendering Techniques’ 96. Springer Vienna, 1996. 21-30.]

![Photon Map](https://github.com/Gregjksmith/Global-Illumination/blob/master/images/photonMapDiffuse.png?raw=true)

### Reflective Shadow Map
[Dachsbacher, Carsten, and Marc Stamminger. "Reflective shadow maps." Proceedings of the 2005 symposium on Interactive 3D graphics and games. ACM, 2005.]

#### Indirect Lighting + Subsurface Scattering

![Reflective Shadow Map Indirect](https://github.com/Gregjksmith/Global-Illumination/blob/master/images/Reflective%20Shadow%20Map%20Indirect.png?raw=true)

#### Direct Lighting + Indirect Lighting + Subsurface Scattering

![Reflective Shadow Map](https://github.com/Gregjksmith/Global-Illumination/blob/master/images/Reflective%20Shadow%20Map.png?raw=true)

### Instant Radiosity
[Keller, Alexander. "Instant radiosity." Proceedings of the 24th annual conference on Computer graphics and interactive techniques. ACM Press/Addison-Wesley Publishing Co., 1997.]

![Instant Radiosity 1](https://github.com/Gregjksmith/Global-Illumination/blob/master/images/Instant%20Radiosity%203.png?raw=true)

![Instant Radiosity 2](https://github.com/Gregjksmith/Global-Illumination/blob/master/images/Instant%20Radiosity%201.png?raw=true)

![Instant Radiosity 3](https://github.com/Gregjksmith/Global-Illumination/blob/master/images/Instant%20Radiosity%202.png?raw=true)