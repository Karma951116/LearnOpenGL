#version 330 core

#define NR_POINT_LIGHTS 1
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
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
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

struct SpotLight {
	vec3 position;
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float cutoff;
	float outerCutoff;
	float constant;
	float linear;
	float quadratic;
};

uniform vec3 viewPos;
uniform Material material;
uniform DirLight dLight;
uniform PointLight pLights[NR_POINT_LIGHTS];
uniform SpotLight sLight;

vec3 calcDirLight(DirLight light, vec3 normal,  vec3 viewDir) {
	// DirLight
	// ambient
	vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
	// diffuse
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse =  light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
	// specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec  = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;
	return (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
	// diffuse
	vec3 lightDir = normalize(light.position - fragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	// specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec  = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + 
											 light.linear * distance + 
											 light.quadratic * (distance * distance));
	vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
	vec3 diffuse =  light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
	vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
	vec3 lightDir = normalize(light.position - fragPos);
		//diffuse
	float diff = max(dot(normal, lightDir), 0.0);
		// specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec  = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// calculate whether in the spotlight area
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutoff - light.outerCutoff;
	float intensity = clamp((theta - light.cutoff) / epsilon, 0.0, 1.0);
	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + 
											 light.linear * distance + 
											 light.quadratic * (distance * distance));

	vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
	vec3 diffuse =  light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
	vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;

	diffuse *= intensity;
	specular *= intensity;
	// compose
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
}

void main()
{
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 norm = normalize(Normal);
	// DirLight
	vec3 result = calcDirLight(dLight, norm, viewDir);
	// PointLight
	for (int i = 0; i < NR_POINT_LIGHTS; i++) {
		result += calcPointLight(pLights[i], norm, FragPos, viewDir);
	}
	result += calcSpotLight(sLight, norm, FragPos, viewDir);

    FragColor = vec4(result, 1.0);
}