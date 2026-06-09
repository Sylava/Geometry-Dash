uniform float time;
uniform vec2  resolution;

void main()
{
    vec2 uv = gl_FragCoord.xy / resolution;
    uv.y = 1.0 - uv.y;

    float t = time * 0.35;

    float w1 = sin(uv.x * 3.8 + t)          * sin(uv.y * 2.5 - t * 0.7);
    float w2 = sin((uv.x + uv.y) * 4.5 + t * 0.9);
    float w3 = sin(uv.x * 1.8 - uv.y * 3.2 + t * 1.3);

    float plasma = (w1 + w2 + w3) / 3.0 * 0.5 + 0.5;

    vec3 deep   = vec3(0.01, 0.01, 0.06);
    vec3 blue   = vec3(0.05, 0.14, 0.58);
    vec3 purple = vec3(0.40, 0.04, 0.60);

    vec3 col = mix(deep, mix(blue, purple, plasma), 0.70);

    float vignette = 1.0 - length(uv - 0.5) * 1.1;
    col *= 0.5 + 0.5 * clamp(vignette, 0.0, 1.0);
    col += vec3(0.0, 0.02, 0.08) * (1.0 - uv.y) * 0.6;

    gl_FragColor = vec4(col, 1.0);
}