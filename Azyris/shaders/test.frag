#version 450 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

uniform bool useDiffuseTexture;
uniform bool hasDiffuseColor;
uniform vec4 materialDiffuseColor;


uniform bool enableLight = false;

/* lighting */
uniform vec3 lightDirection;   // normalized
uniform vec3 lightColor;       // e.g. vec3(1.0)
uniform float lightIntensity;  // e.g. 3.0
uniform vec3 viewPos;

void main()
{
    // ---------------- Base color ----------------
    vec4 baseColor = vec4(1.0);

    if (useDiffuseTexture)
    {
        baseColor = texture(texture_diffuse1, TexCoords);
        if (baseColor.a < 0.1)
            discard;
    }
    else if (hasDiffuseColor)
    {
        baseColor = vec4(materialDiffuseColor);
    }

    if(enableLight)
    {
        vec3 N = normalize(Normal);
        vec3 L = normalize(-lightDirection);
        vec3 V = normalize(viewPos - FragPos);
        vec3 R = reflect(-L, N); // reflection vector for Phong specular

        // ---------------- Ambient ----------------
        vec3 ambient = 0.1 * baseColor.rgb; // simple ambient term

        // ---------------- Diffuse ----------------
        float diff = max(dot(N, L), 0.0);
        vec3 diffuse = diff * lightColor * lightIntensity;

        // ---------------- Specular ----------------
        float shininess = 32.0; // controls highlight size
        float spec = pow(max(dot(R, V), 0.0), shininess);
        vec3 specular = spec * lightColor * 0.5 * lightIntensity; // scale specular

        // ---------------- Final Color ----------------
        vec3 color = ambient + diffuse * baseColor.rgb + specular;

        // ---------------- Tone Mapping ----------------
        color = color / (color + vec3(1.0)); // Reinhard
        color = pow(color, vec3(1.0 / 2.2)); // Gamma correction

        FragColor = vec4(color, baseColor.a);
    }
    else
    {
        FragColor = baseColor;
    }

    
}
