#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPosWorldSpace;
layout(location = 2) in vec3 fragNormalWorldSpace;

layout(location = 0) out vec4 outColor;

struct PointLight{
	vec4 position;
	vec4 color;
};

layout(set =0, binding = 0) uniform UBO{
	mat4 projectionMatrix;
	mat4 viewMatrix;
	mat4 inverseViewMatrix;
	vec4 ambientLightColor;
	PointLight pointLights[10];//TODO : specialization constant
	int nbLights;
} ubo;

layout(push_constant) uniform Push{
	mat4 modelMatrix;		
	mat4 normalMatrix; 
	float blinnExp;
} push;

void main(){
	vec3 diffuseLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
	vec3 specularLight  = vec3(0.0);
	vec3 surfaceNormal = normalize(fragNormalWorldSpace);

	vec3 cameraPosWorldSpace = ubo.inverseViewMatrix[3].xyz;
	vec3 viewDirection = normalize(cameraPosWorldSpace - fragPosWorldSpace);

	for(int i=0;i<ubo.nbLights;i++){
		PointLight l = ubo.pointLights[i];
		vec3 directionToLight = l.position.xyz - fragPosWorldSpace;
		float attenuation = 1.0 / dot(directionToLight,directionToLight);
		directionToLight = normalize(directionToLight);
		float cosAngleIncidence = max(dot(surfaceNormal, directionToLight),0);
		vec3 intensity = l.color.xyz * l.color.w * attenuation;

		diffuseLight += intensity * cosAngleIncidence;

		//specular
		vec3 halfAngle = normalize(directionToLight + viewDirection);
		float blinnTerm = dot(surfaceNormal, halfAngle);
		blinnTerm = clamp(blinnTerm, 0,1);
		blinnTerm = pow(blinnTerm, push.blinnExp);
		specularLight += intensity * blinnTerm;

	}



	outColor = vec4((diffuseLight * fragColor) + (specularLight * fragColor),1.0);
}
