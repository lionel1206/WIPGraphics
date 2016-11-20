=============================================================================
Observations made from playing around the phong values and other stuffs:
=============================================================================
Implementing Blinn-Phong:
Instead of calculating the hi-lights of the phong specular from the
dot product of R which is the reflected vector from the light and V,
which is the vector to the eye, we calculate the halfway vector H between
the normalized vector between the light source and the vector to the eye.
We then take the dot product of the normal vector and H and exponatiate by
the phong shininesss value n. This made the specular hi-lights better looking

=============================================================================
Changing the values of the phong shininess(n)
=============================================================================
! All hi-lights are white in color !

Setting n to 1 made the phong specular reflection cover a huge surface area
which actually made the teapot felt metalic.

Setting n to 0 in the shader code just white washed every thing

Setting n to about 16 made it looks very plastic, quite similar
to what Blinn-Phong gives

Setting n to 50~100 just made the specular hi-lights smaller
=============================================================================
Setting Phong Specular(ks) equal to Phong Diffuse(kd)
=============================================================================
In the Specular calculation of the phong equation:
Is = Li * Ks * dot(R,V)^n
where Li = Incoming Light
      Ks = PhongSpecular value
      R = Reflected vector
      V = Vector to the eye
      n - Phong Shininess
      
If I set Ks = Kd, I observed that all objects in the scene became more
metalic. The hi-lights of the teapot and the spheres are of their own 
color. If we were to set n to 1, the teapot will become what looks like
a brass teapot.
=============================================================================
Changing the color of the grounds or teapot
=============================================================================

Before drawing the teapot or the floor, you are able to blind the diffuse of
the object to any color desired.

//Blind diffuse color to phong params
loc = glGetUniformLocation(scene.shaderFINAL.program, "phongDiffuse");
glUniform3fv(loc, 1, < insert a array of 3 floats of any desired color here >);  
//blind color of specular hi-lights
loc = glGetUniformLocation(scene.shaderFINAL.program, "phongSpecular");
glUniform3fv(loc, 1, < insert a array of 3 floats of any desired color here >); 
//Blind shininess color
loc = glGetUniformLocation(scene.shaderFINAL.program, "phongShininess");
glUniform1f(loc, < Shiny values >);  

//Blind vertex array object of object
//Draw the element
//Unblind the vertex Array


