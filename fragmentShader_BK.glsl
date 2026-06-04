#version 330 core

out vec4 FragColor;

uniform vec2 iResolution;
uniform float iTime;

void main()
{
    vec2 uv = (gl_FragCoord.xy / iResolution.xy) - 0.5;
    uv.x *= iResolution.x / iResolution.y; 

    // Distanza dal centro
    float dist = length(uv);

    
    float glow = pow(1.0 - dist * 2.0, 2.0); 
    glow = clamp(glow, 0.0, 1.0);

    // Pulsazione lenta 
    float pulse = 0.5 + 0.5 * sin(iTime * 2.0);

    vec3 centerColor = vec3(0.2, 0.3, 0.8); // centro blu chiaro
    vec3 background  = vec3(0.0, 0.0, 0.1); // bordo blu scuro

    vec3 color = mix(background, centerColor, glow * pulse);

    FragColor = vec4(color, 1.0);
}
