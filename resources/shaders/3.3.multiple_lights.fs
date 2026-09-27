#version 330 core
out vec4 FragColor;

struct Material {
    vec3 specular;
    float shininess;
}; 

struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 color;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 color;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       

    vec3 color;
};

#define NR_POINT_LIGHTS 4

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;
uniform sampler2D texture_diffuse1;

// toon shading attributes
uniform bool gCellShadingEnabled = true;

const int toon_color_levels = 2;
const float toon_scale_factor = 1.0f / toon_color_levels;

float computeToonIntensity(float dotProduct);
void CalcDirLight(DirLight light, vec3 normal, vec3 viewDir,
                   out vec3 ambient, out vec3 diffuse, out vec3 specular);
void CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 texColor,
                   out vec3 ambient, out vec3 diffuse, out vec3 specular);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 texColor);

void main()
{    
    vec3 texColor = texture(texture_diffuse1, TexCoords).rgb;
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 totalAmbient = vec3(0.0);
    vec3 totalDiffuse = vec3(0.0);
    vec3 totalSpecular = vec3(0.0);

    vec3 a, d, s;

    CalcDirLight(dirLight, norm, viewDir, a, d, s);
    totalAmbient += a;
    totalDiffuse += d;
    totalSpecular += s;
     
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
    {
        CalcPointLight(pointLights[i], norm, FragPos, viewDir, texColor, a, d, s);
        totalAmbient += a;
        totalDiffuse += d;
        totalSpecular += s;
    }

    // result += CalcSpotLight(spotLight, norm, FragPos, viewDir, texColor);    

    if (gCellShadingEnabled)
    {
        totalDiffuse = clamp(totalDiffuse, 0.0, 1.0);
        totalDiffuse = ceil(totalDiffuse * toon_color_levels) * toon_scale_factor;
        totalSpecular = vec3(0.0); // stylized specular comes later; flat off for now
    }
    
    vec3 result = (totalAmbient + totalDiffuse + totalSpecular) * texColor;
    result = clamp(result, 0.0, 1.0);
    FragColor = vec4(result, 1.0);
}

void CalcDirLight(DirLight light, vec3 normal, vec3 viewDir,
                   out vec3 ambient, out vec3 diffuse, out vec3 specular)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    ambient = light.ambient * light.color;
    diffuse = light.diffuse * diff * light.color;
    specular = light.specular * spec * material.specular * light.color;
}


void CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 texColor,
                     out vec3 ambient, out vec3 diffuse, out vec3 specular)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    ambient = light.ambient * attenuation * light.color;
    diffuse = light.diffuse * diff * attenuation * light.color;
    specular = light.specular * spec * material.specular * attenuation * light.color;
}

/*
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 texColor)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = light.ambient * texColor;
    vec3 diffuse = light.color * light.diffuse * diff * texColor;
    vec3 specular = light.specular * spec * material.specular;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}
*/