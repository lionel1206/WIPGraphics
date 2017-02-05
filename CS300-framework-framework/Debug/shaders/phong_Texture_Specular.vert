#version 330
// These are inputs from the application.
uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 ViewInverse, ModelMatrix, NormalMatrix;

in vec4 vertex;
in vec3 vertexNormal;
in vec3 vertexTexture;

out vec3 normalVec, eyeVec, worldVertex;
out vec2 uv;

void main()
{
    worldVertex = vec3(vertex * ModelMatrix);
    normalVec = normalize(vec3(transpose(inverse(ModelMatrix)) * vec4(vertexNormal, 0.f)));
    eyeVec = normalize(cameraPos - worldVertex);
    
    uv = vec2(vertexTexture.x, 1.0 - vertexTexture.y);
    gl_Position = ProjectionMatrix*ViewMatrix*ModelMatrix*vertex;
}
