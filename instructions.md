## Ray Tracing (Continued)

Please continue adding to your ray tracer program. If you did not get it to work by its deadline, please contact the instructor to get a solution which you can use as starter code for this assignment.

#### 1.1 Shadows

A point is in shadow if the light cannot see it. Conversely, if you place a camera at the point and look towards the light, you won’t see the light.

In the shade function, before performing the shading calculations for each light, create a shadow ray from the point towards that light, and see if the point sees the light. If it does not, simply ignore that light in the shading calculations for that point. Don’t forget to "fudge" the shadow ray a bit to avoid precision errors!

#### 1.2 Reflections

The provided Material class has variables to store the absorption, reflection and transparency of a material. The sum of absorption, reflection and transparency should be 1 for every object. By default, a material is fully absorbent (i.e. reflection=transparency=0, absorption=1).

Augment your scene graph parser to take these extra properties for materials. After determining the color of a pixel due to direct illumination (shading), check if the object is reflective. If so, create a reflection ray and cast it to determine the color of the pixel (recursion may be helpful here). Finally, blend the color produced by direct illumination and that produced by reflection using the absorption and reflection coefficients of the material.

In order to prevent rays bouncing infinitely, add a parameter called "bounce". This represents the maximum number of times a ray is allowed to bounce after it started from the camera. When the number of bounces reaches this threshold, we simply return the background color instead of spawning another ray. A value of '5' is usually sufficient, but the appropriate value depends on the scene.

## 2 Extra Credit

In order to prove that you have done any of the extra credit, we expect you to include a ray traced image for each feature. We will not run your ray tracer to check each feature. We will rely on a screen shot you have submitted and a reading of your code to determine extra credit.

#### 2.1 Texture Mapping (Extra credit 6 points)

The hit record returned by the ray caster renderer should also contain the appropriate texture coordinates for the point of intersection. Use the TextureObject object to determine the pixel color from the texture. You are required to support texture mapping only for spheres and boxes.

To determine the texture coordinates for various faces of a box, use this illustration of texture coordinates:
