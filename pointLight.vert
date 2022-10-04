#version 450

const vec2 OFFSETS[6] = vec2[](
  vec2(-1.0, -1.0),
  vec2(-1.0, 1.0),
  vec2(1.0, -1.0),
  vec2(1.0, -1.0),
  vec2(-1.0, 1.0),
  vec2(1.0, 1.0)
);

layout (location=0) out vec2 fragOffset;

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
	vec4 position;
	vec4 color;
	float radius;
}push;



void main(){
	fragOffset = OFFSETS[gl_VertexIndex];
	vec3 cameraRightWorldSpace = {ubo.viewMatrix[0][0], ubo.viewMatrix[1][0], ubo.viewMatrix[2][0]};
	vec3 cameraUpWorldSpace = {ubo.viewMatrix[0][1], ubo.viewMatrix[1][1], ubo.viewMatrix[2][1]};

	vec3 vertexPositionWorldSpace =
		push.position.xyz
		+ push.radius * fragOffset.x * cameraRightWorldSpace
		+ push.radius * fragOffset.y * cameraUpWorldSpace;

	gl_Position = ubo.projectionMatrix * ubo.viewMatrix * vec4(vertexPositionWorldSpace, 1.0);
}