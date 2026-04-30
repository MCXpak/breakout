#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;
in vec3 pos;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float aMix;

const vec4 innerColor = vec4(0.5, 0.0, 0.0, 1.0);
const vec2 center = vec2(0.5, 0.5);
const float cutoffHalfSize = 0.4;

void main()
{
    //Chebyshev distance calculation
    vec2 offset = abs(TexCoord - center);
    float maxDist = max(offset.x, offset.y);
    float factor = step(cutoffHalfSize, maxDist);

    //float factor = clamp(distance(TexCoord.xy, vec2(0.5,0.5)) / 0.7071, 0.0, 1.0);
    //FragColor = mix(texture(texture1, vec2(TexCoord.x, TexCoord.y) ), texture(texture2, vec2(TexCoord.x, TexCoord.y)), aMix);
    FragColor = mix(vec4(ourColor, 1.0), vec4(0.0), factor);
}