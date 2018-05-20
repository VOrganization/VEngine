#version 440
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

struct Material{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

#ifdef VERTEX

layout(binding = 0) uniform Matrix {
    mat4 P;
    mat4 V;
    mat4 T;
};

layout(binding = 1) uniform Model {
    mat4 M;
    uint matID;
};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;

layout(std430, binding = 3) buffer materialBuffer{
	uint materialSize;
	Material materials[];
};

layout(location = 0) out vec2 outUV;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outFragPos;
layout(location = 3) out Material material;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    gl_Position = P * V * M * vec4(inPosition, 1.0);
    gl_Position.y = -gl_Position.y;
    gl_Position.z = (gl_Position.z + gl_Position.w) / 2.0;
    outUV = inUV;
    outNormal = mat3(transpose(inverse(M))) * inNormal;
    outFragPos = vec3(M * vec4(inPosition, 1.0f));
    if(matID < materialSize){
        material = materials[matID];
    }
}
#endif

#ifdef FRAGMENT
layout(location = 0) out vec4 outColor;

layout(location = 0) in vec2 inUV;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inFragPos;
layout(location = 3) in Material material;

layout(binding = 2) uniform engineD{
	vec3 camPos;
};


struct _lightPoint{
    vec4 position;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 dist;
};
vec3 CalcPointLight(_lightPoint light, vec3 normal, vec3 viewDir, vec3 FragPos, Material material);
layout(std430, binding = 4) buffer LightP{
	uint lightPointSize;
	_lightPoint lightPoints[];
};

struct _lightSpot{
    vec4 position;
    vec4 direction;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 dist;
    vec3 spot;
};
vec3 CalcSpotLight(_lightSpot light, vec3 normal, vec3 viewDir, vec3 FragPos, vec3 ambient, vec3 diffuse, vec3 specular, float shininess);
layout(std430, binding = 5) buffer LightS{
	uint lightSpotSize;
	_lightSpot lightSpots[];
};


void main() {
    vec3 color = vec3(0);
	vec3 viewDir = normalize(camPos - inFragPos);
    vec3 normal = normalize(inNormal);

    vec3 ambient = vec3(0.1);
    vec3 diffuse = material.diffuse.rgb;
    vec3 specular = vec3(0.1);
    float shininess = 30.0;

    for(uint i = 0; i < lightPointSize; i++){
        _lightPoint l = lightPoints[i];
        color += CalcPointLight(l, normal, viewDir, inFragPos, material);
    }

    // for(uint i = 0; i < lightSpotSize; i++){
    //     _lightSpot l = lightSpots[i];
    //     color += CalcSpotLight(l, normal, viewDir, inFragPos, ambient, diffuse, specular, shininess);
    // }

    outColor = vec4(color, 1);
}

vec3 CalcPointLight(_lightPoint light, vec3 normal, vec3 viewDir, vec3 FragPos, Material material){
 	vec3 lightDir = normalize(light.position.xyz - FragPos);
 	float diff = max( dot(normal, lightDir), 0.0 );
	
 	vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow( max( dot(viewDir, reflectDir), 0.0 ), material.shininess );
	
 	float d = length(light.position.xyz - FragPos);
    float attenuation_c = 1.0f / ( light.dist.x + light.dist.y * d + light.dist.z * (d * d) );
	
 	vec3 ambient_l  =  light.ambient.rgb;
    vec3 diffuse_l  =  light.diffuse.rgb  * diff * material.diffuse.rgb;
    vec3 specular_l = light.specular.rgb * spec * material.specular.rgb;
    ambient_l  *= attenuation_c;
    diffuse_l  *= attenuation_c;
    specular_l *= attenuation_c;
	
    
    //return (ambient_l + diffuse_l + specular_l);
    return material.diffuse.rgb;
}

vec3 CalcSpotLight(_lightSpot light, vec3 normal, vec3 viewDir, vec3 FragPos, vec3 ambient, vec3 diffuse, vec3 specular, float shininess){
    vec3 lightDir = normalize(light.position.xyz - FragPos);

	vec3 _ambient = light.ambient.rgb * ambient;

	float diff = max(dot(normal, lightDir), 0.0);
	vec3 _diffuse = light.diffuse.rgb * diff * diffuse;

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
	vec3 _specular = light.specular.rgb * spec * specular;

	float theta = dot(lightDir, normalize(-vec3(0,-1,0)));
	float epsilon = (light.spot.x - light.spot.y);
	float intensity = clamp((theta - light.spot.y) / epsilon, 0.0, 1.0);
	_diffuse *= intensity;
	_specular *= intensity;

	float distance = length(light.position.xyz - FragPos);
	float attenuation = 1.0f / (light.dist.x + light.dist.y * distance + light.dist.z * (distance * distance));
	_ambient *= attenuation;
	_diffuse *= attenuation;
	_specular *= attenuation;

	return (_ambient + _diffuse + _specular);
}

#endif