uniform float time;

void main()
{
    vec2 uv = gl_TexCoord[0].xy;

    float edgeX = smoothstep(0.0, 0.18, uv.x) * smoothstep(1.0, 0.82, uv.x);
    float edgeY = smoothstep(0.0, 0.25, uv.y) * smoothstep(1.0, 0.75, uv.y);
    float soft  = edgeX * edgeY;

    float pulse = sin(time * 2.8) * 0.225 + 0.775;
    float shift = sin(time * 1.4) * 0.5 + 0.5;

    vec3 blue   = vec3(0.10, 0.35, 1.00);
    vec3 purple = vec3(0.65, 0.10, 1.00);
    vec3 col    = mix(blue, purple, shift);

    gl_FragColor = vec4(col, soft * pulse * 0.55);
}