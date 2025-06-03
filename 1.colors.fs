#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;


struct Material {


    float shininess;
};

struct DirLight {

    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse; 
    vec3 specular; 
    
};
#define NR_POINT_LIGHTS 4


uniform sampler2D texture_diffuse1;
uniform vec3 viewPos;
uniform Material material;
uniform DirLight dirlight;
uniform PointLight pointlights[NR_POINT_LIGHTS];


vec3 CalcPointLights(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);



void main()
{    
     vec3 norm  = normalize(Normal);

     vec3 viewDir = normalize(viewPos - FragPos);

     vec3 result = CalcDirLight(dirlight, norm, viewDir);

     for(int i = 0; i < NR_POINT_LIGHTS; i++)
     {
        result+=CalcPointLights(pointlights[i], norm, FragPos, viewDir);
     }
     FragColor = vec4(result, 1.0);
}

vec3 CalcPointLights(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    vec3 diff = vec3(max(dot(normal, lightDir), 0.0));

    vec3 reflectDir = reflect(-lightDir, normal);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float distance = length(light.position - fragPos);

    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));

    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));
    
    vec3 specular = light.specular * spec * vec3(texture(texture_diffuse1, TexCoords));

    ambient *= attenuation;

    diffuse *= attenuation;

    specular *= attenuation;

    return (ambient + diffuse + specular);

}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    vec3 reflectDir = reflect(-lightDir, normal);

    vec3 diff = vec3(max(dot(normal, lightDir), 0.0));

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));

    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));

    vec3 specular = light.specular * spec * vec3(texture(texture_diffuse1, TexCoords));

    return (ambient + diffuse + specular);
}
