#pragma once

const char* res_default_shader = ""\
"#version 440\n"\
"\n"\
"#ifdef VERTEX\n"\
"layout(location = 0) in vec3 vVertex;\n"\
"layout(location = 1) in vec2 vUV;\n"\
"layout(location = 2) in vec3 vNormal;\n"\
"\n"\
"layout(std140) uniform Matrix {\n"\
"	mat4 P;\n"\
"	mat4 V;\n"\
"	mat4 M;\n"\
"};\n"\
"\n"\
"out VS_OUT{\n"\
"	vec3 normal;\n"\
"vec3 v_normal;\n"\
"vec2 uv;\n"\
"vec3 frag_pos;\n"\
"} vs_out;\n"\
"\n"\
"void main() {\n"\
"	gl_Position = P * V * M * vec4(vVertex, 1);\n"\
"\n"\
"	vs_out.normal = mat3(transpose(inverse(M))) * vNormal;\n"\
"	vs_out.v_normal = vNormal;\n"\
"	vs_out.frag_pos = vec3(M * vec4(vVertex, 1.0f));\n"\
"	vs_out.uv = vUV;\n"\
"}\n"\
"#endif\n"\
"\n"\
"\n"\
"\n"\
"\n"\
"#ifdef FRAGMENT\n"\
"layout(location = 0) out vec4 FragColor;\n"\
"\n"\
"in VS_OUT{\n"\
"	vec3 normal;\n"\
"vec3 v_normal;\n"\
"vec2 uv;\n"\
"vec3 frag_pos;\n"\
"} fs_in;\n"\
"\n"\
"struct Info {\n"\
"	vec3 ambient;\n"\
"	vec3 diffuse;\n"\
"	vec3 specular;\n"\
"	float shininess;\n"\
"};\n"\
"\n"\
"\n"\
"struct _PointLight {\n"\
"	vec3 position;\n"\
"	vec3 ambient;\n"\
"	vec3 diffuse;\n"\
"	vec3 specular;\n"\
"	vec3 attenuation;\n"\
"};\n"\
"\n"\
"struct _SpotLight {\n"\
"	vec3 position;\n"\
"	vec3 direction;\n"\
"	vec3 ambient;\n"\
"	vec3 diffuse;\n"\
"	vec3 specular;\n"\
"	vec3 attenuation;\n"\
"	vec2 spot;\n"\
"};\n"\
"\n"\
"struct _DirLight {\n"\
"	vec3 position;\n"\
"	vec3 direction;\n"\
"	vec3 ambient;\n"\
"	vec3 diffuse;\n"\
"	vec3 specular;\n"\
"};\n"\
"\n"\
"\n"\
"vec3 CalcPointLight(_PointLight light, vec3 normal, vec3 viewDir, vec3 FragPos, Info mat);\n"\
"vec3 CalcSpotLight(_SpotLight light, vec3 normal, vec3 viewDir, vec3 FragPos, Info mat);\n"\
"vec3 CalcDirLight(_DirLight light, vec3 normal, vec3 viewDir, vec3 FragPos, Info mat);\n"\
"\n"\
"layout(std140) uniform Engine {\n"\
"	float time;\n"\
"	vec3 cameraPos;\n"\
"};\n"\
"\n"\
"layout(std430, binding = 3) buffer LightPoint {\n"\
"	uint point_size;\n"\
"	_PointLight point_lights[];\n"\
"};\n"\
"\n"\
"layout(std430, binding = 4) buffer LightSpot {\n"\
"	uint spot_size;\n"\
"	_SpotLight spot_lights[];\n"\
"};\n"\
"\n"\
"layout(std430, binding = 5) buffer LightDir {\n"\
"	uint dir_size;\n"\
"	_DirLight dir_lights[];\n"\
"};\n"\
"\n"\
"layout(std140) uniform Material {\n"\
"	vec4 ambient;\n"\
"	vec4 diffuse;\n"\
"	vec4 specular;\n"\
"	vec4 emission;\n"\
"	float shininess;\n"\
"	uint mat_tex_size;\n"\
"	uint mat_tex[10];\n"\
"};\n"\
"\n"\
"\n"\
"uniform sampler2D tex[20];\n"\
"\n"\
"void main() {\n"\
"	vec3 color = vec3(0);\n"\
"	vec3 viewDir = normalize(cameraPos - fs_in.frag_pos);\n"\
"	vec3 normal = normalize(fs_in.normal);\n"\
"\n"\
"	vec3 color_tex = vec3(0);\n"\
"	if (mat_tex_size >= 1) {\n"\
"		color_tex = texture(tex[mat_tex[0]], fs_in.uv).rgb;\n"\
"	}\n"\
"	else {\n"\
"		color_tex = diffuse.xyz;\n"\
"	}\n"\
"\n"\
"	Info mat;\n"\
"	mat.ambient = color_tex;\n"\
"	mat.diffuse = color_tex;\n"\
"	float fac = 1 - ((color_tex.r + color_tex.g + color_tex.b) / 3.0);\n"\
"	fac *= 1.5;\n"\
"	mat.specular = vec3(fac);\n"\
"	mat.shininess = shininess;\n"\
"\n"\
"	for (int i = 0; i < point_size; i++) {\n"\
"		color += CalcPointLight(point_lights[i], normal, viewDir, fs_in.frag_pos, mat);\n"\
"	}\n"\
"\n"\
"	for (int i = 0; i < spot_size; i++) {\n"\
"		color += CalcSpotLight(spot_lights[i], normal, viewDir, fs_in.frag_pos, mat);\n"\
"	}\n"\
"\n"\
"	for (int i = 0; i < dir_size; i++) {\n"\
"		color += CalcDirLight(dir_lights[i], normal, viewDir, fs_in.frag_pos, mat);\n"\
"	}\n"\
"\n"\
"	color += emission.rgb;\n"\
"\n"\
"	FragColor = vec4(color, 1);\n"\
"}\n"\
"\n"\
"vec3 CalcPointLight(_PointLight light, vec3 normal, vec3 viewDir, vec3 FragPos, Info mat) {\n"\
"	vec3 lightDir = normalize(light.position - FragPos);\n"\
"	float diff = max(dot(normal, lightDir), 0.0);\n"\
"\n"\
"	vec3 reflectDir = reflect(-lightDir, normal);\n"\
"	float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);\n"\
"\n"\
"	float distance = length(light.position - FragPos);\n"\
"	float attenuation_c = 1.0f / (light.attenuation.x + light.attenuation.y * distance + light.attenuation.z * (distance * distance));\n"\
"\n"\
"	vec3 ambient_l = light.ambient.xyz  * mat.ambient;\n"\
"	vec3 diffuse_l = light.diffuse.xyz  * diff * mat.diffuse;\n"\
"	vec3 specular_l = light.specular.xyz * spec * mat.specular;\n"\
"	ambient_l *= attenuation_c;\n"\
"	diffuse_l *= attenuation_c;\n"\
"	specular_l *= attenuation_c;\n"\
"\n"\
"\n"\
"	return (ambient_l + diffuse_l + specular_l);\n"\
"}\n"\
"\n"\
"vec3 CalcSpotLight(_SpotLight light, vec3 normal, vec3 viewDir, vec3 FragPos, Info mat) {\n"\
"	vec3 lightDir = normalize(light.position - FragPos);\n"\
"\n"\
"	vec3 ambient = light.ambient * mat.ambient;\n"\
"\n"\
"	float diff = max(dot(normal, lightDir), 0.0);\n"\
"	vec3 diffuse = light.diffuse * diff * mat.diffuse;\n"\
"\n"\
"	vec3 reflectDir = reflect(-lightDir, normal);\n"\
"	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);\n"\
"	vec3 specular = light.specular * spec * mat.specular;\n"\
"\n"\
"	float theta = dot(lightDir, normalize(-light.direction));\n"\
"	float epsilon = (light.spot.x - light.spot.y);\n"\
"	float intensity = clamp((theta - light.spot.y) / epsilon, 0.0, 1.0);\n"\
"	diffuse *= intensity;\n"\
"	specular *= intensity;\n"\
"\n"\
"	float distance = length(light.position - FragPos);\n"\
"	float attenuation = 1.0f / (light.attenuation.x + light.attenuation.y * distance + light.attenuation.z * (distance * distance));\n"\
"	ambient *= attenuation;\n"\
"	diffuse *= attenuation;\n"\
"	specular *= attenuation;\n"\
"\n"\
"	return (ambient + diffuse + specular);\n"\
"}\n"\
"\n"\
"vec3 CalcDirLight(_DirLight light, vec3 normal, vec3 viewDir, vec3 FragPos, Info mat) {\n"\
"	vec3 lightDir = normalize(-light.direction);\n"\
"	float diff = max(dot(normal, lightDir), 0.0);\n"\
"	vec3 reflectDir = reflect(-lightDir, normal);\n"\
"	float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);\n"\
"	vec3 ambient = light.ambient * mat.ambient;\n"\
"	vec3 diffuse = light.diffuse * diff * mat.diffuse;\n"\
"	vec3 specular = light.specular * spec * mat.specular;\n"\
"	return (ambient + diffuse + specular);\n"\
"}\n"\
"\n"\
"#endif\n";