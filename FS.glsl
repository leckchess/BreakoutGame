#version 330

in vec2 TexCoords;

uniform sampler2D image;
uniform vec3 spriteColor;

out vec4 color;
void main()
{
	vec4 textureColor = texture(image,TexCoords);
	if(textureColor.a != 1.0f)
    {
        discard;
    }

	color = vec4(spriteColor,1.0) * textureColor;
	//gl_FragColor = color;
}