#version 330 core
layout(location = 0) in vec3 position;
out vec4 mbrot_pos;

uniform mat4 model_transform;
uniform mat4 view;
uniform mat4 projection;

void main() {
	gl_Position = vec4(position.x, position.y, position.z, 1.0);
	mbrot_pos = projection * view * model_transform * vec4(position.x, position.y, 0.0f, 1.0f);
}