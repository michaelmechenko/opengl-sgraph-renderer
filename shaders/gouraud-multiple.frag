#version 330

in vec4 fPosition;
//in vec4 fTexCoord;
in vec4 fColor;


/* texture */
 uniform sampler2D image; 

out vec4 outColor;

void main()
{
    outColor = fColor * texture(image,fTexCoord.st);
}
