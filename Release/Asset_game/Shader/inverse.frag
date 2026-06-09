uniform sampler2D texture;

void main() {
    // 1. Récupère la couleur du pixel de la texture
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);
    
    // 2. Inversion du RGB uniquement
    vec3 invertedRGB = vec3(1.0) - pixel.rgb;
    
    // 3. Application de la couleur SFML (gl_Color) 
    // On multiplie le RGB inversé par le RGB de gl_Color
    // Et on multiplie l'alpha du pixel par l'alpha de gl_Color
    gl_FragColor = vec4(invertedRGB * gl_Color.rgb, pixel.a * gl_Color.a);
}