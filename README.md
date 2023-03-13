# basic-opengl-shader
A coursework project done in university that renders a basic terrain with water reflection using OpenGL.
Shader can draw triangle normals or interpolate between normals for a smoother look by changing the gNormals value
to use tessNormals[i] and vice versa.

Known issues:
Water reflection does not render reflected skybox


![tes1](https://user-images.githubusercontent.com/106027747/224691074-2cdf3efd-d778-4512-b2c7-4ffe523e3675.png)
using gNormals = getNormals(); in the calcNormals shader


![tes2](https://user-images.githubusercontent.com/106027747/224691173-29d83b0b-f0a1-4cdd-8787-1327da78c947.png)
using gNormals = tessNormals[i]; in the calcNormals shader
