#version 330

in vec4 vertex;
in vec3 vertexNormal;
in vec3 vertexTexture;

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 ModelMatrix;

out vec3 worldVertex;
out vec2 uv;
out vec3 normal;

void main()
{
    worldVertex = (ModelMatrix * vertex).xyz;
    gl_Position = ProjectionMatrix*ViewMatrix*ModelMatrix*vertex;
    uv = vertexTexture.xy;
    
    normal = normalize(vec3(transpose(inverse(ModelMatrix)) * vec4(vertexNormal, 0.f))); 
}