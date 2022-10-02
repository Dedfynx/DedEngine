#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragPosWorldSpace;
layout(location = 2) out vec3 fragNormalWorldSpace;

layout(set =0, binding = 0) uniform UBO{
	mat4 projectionViewMatrix;
	vec4 ambientLightColor;
	vec3 lightPosition;
	vec4 lightColor;
} ubo;

layout(push_constant) uniform Push{
	mat4 modelMatrix;		
	mat4 normalMatrix; 
} push;

void main(){
	vec4 posWorldSpace = push.modelMatrix * vec4(position, 1.0);
	gl_Position = ubo.projectionViewMatrix * posWorldSpace;
	
	fragNormalWorldSpace = normalize(mat3(push.normalMatrix) * normal);
	fragPosWorldSpace = posWorldSpace.xyz;
	fragColor = color;
}