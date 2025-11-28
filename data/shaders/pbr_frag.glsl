#version 330 core

in vec2 oTexCoord;
out vec4 FragColor;

uniform sampler2D uDiffuse;
uniform sampler2D uNormal;
uniform sampler2D uMetallic;
uniform sampler2D uRoughness;
uniform int uWhich = 0;

void main()
{   
	vec4 diff = texture2D(uDiffuse, oTexCoord);
	vec4 normal = texture2D(uNormal, oTexCoord);
	vec4 metallic = texture2D(uMetallic, oTexCoord);
	vec4 roughness = texture2D(uRoughness, oTexCoord);

	if (uWhich == 0){ FragColor = diff ;	}
	if (uWhich == 1){ FragColor = normal;	}
	if (uWhich == 2){ FragColor = metallic;	}
	if (uWhich == 3){ FragColor = roughness;}

}