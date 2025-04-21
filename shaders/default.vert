#version 330

in vec4 vPosition;
uniform vec4 vColor;
uniform mat4 modelview;
uniform mat4 projection;
out vec4 outColor;

void main()
{
    gl_Position = projection * modelview * vPosition;
    outColor = vColor;
}
