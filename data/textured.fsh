uniform sampler2D tex1;
uniform vec3 ViewDir;

struct DirLight {
	vec3 direction;
	
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};
uniform DirLight dirLight;

struct PointLight {
	vec3 position;
	float range;

	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];

in vec4 v_normal;
in vec2 v_uv1;
in vec3 v_pos;

out vec4 PixelColor;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
	vec3 lightDir = normalize(light.direction);

	// diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
    vec3 reflectDir = reflect(lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1.0); // iffy

	vec3 ambient = light.ambient.rgb * vec3(texture(tex1, v_uv1));
	vec3 diffuse = light.diffuse.rgb * diff * vec3(texture(tex1, v_uv1));
	vec3 specular = light.specular.rgb * spec * vec3(texture(tex1, v_uv1));

	return (ambient + diffuse + specular);
}  

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
	vec3 lightDir = normalize(light.position - fragPos);

	// diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(reflectDir, viewDir), 0.0), 0); // iffy

	float distance = length(light.position - fragPos);
	float attenuation = max(1 - (distance / light.range), 0.0);

	vec3 ambient = light.ambient.rgb * vec3(texture(tex1, v_uv1));
	vec3 diffuse = light.diffuse.rgb * diff * vec3(texture(tex1, v_uv1));
	vec3 specular = light.specular.rgb * spec * vec3(texture(tex1, v_uv1));

	ambient *= attenuation;
	diffuse  *= attenuation;
	specular  *= attenuation;

	return (ambient + specular + specular);
}

void main()
{
    vec3 n = normalize( v_normal ).xyz;
    vec3 output = CalcDirLight(dirLight, n, ViewDir);

	for(int i = 0; i < NR_POINT_LIGHTS; i++) {
        output += CalcPointLight(pointLights[i], n, v_pos, ViewDir);    
	}

	PixelColor = vec4(output, 1.0);
}


