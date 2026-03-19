uniform float time;
uniform vec2  resolution;
uniform int   currentPlane;  // 0 = fantasy, 1 = scifi
uniform float switchFlash;   // 0..1 — animé lors du switch de plan

void main()
{
    vec2 uv = gl_FragCoord.xy / resolution;
    uv.y = 1.0 - uv.y;

    // --- Vignette (coins sombres) ---
    vec2  vig      = uv * 2.0 - 1.0;
    float vignette = 1.0 - dot(vig * vec2(0.75, 0.9), vig * vec2(0.75, 0.9));
    vignette = pow(clamp(vignette, 0.0, 1.0), 0.55);

    // --- Scanlines subtiles ---
    float scanline = sin(gl_FragCoord.y * 3.14159 * 0.5) * 0.025 + 0.975;

    // --- Tint d'ambiance selon le plan ---
    vec3 fantasyTint = vec3(0.08, 0.02, 0.14); // violet sombre
    vec3 scifiTint   = vec3(0.12, 0.01, 0.01); // rouge sombre
    vec3 tint = (currentPlane == 0) ? fantasyTint : scifiTint;

    // --- Pulsation lente des bords ---
    float pulse = sin(time * 1.6) * 0.012 + 0.012;

    // --- Flash blanc au switch de plan ---
    float flash = switchFlash * 0.35;

    // --- Composition ---
    float alpha = (1.0 - vignette) * 0.60 * scanline + pulse + flash;
    vec3  col   = tint + vec3(flash);

    gl_FragColor = vec4(col, clamp(alpha, 0.0, 0.75));
}