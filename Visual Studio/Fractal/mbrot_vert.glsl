#version 400 core
precision highp float;

layout(location = 0) in vec3 position;
out vec4 mbrot_pos;

uniform float min_X;
uniform float max_X;
uniform float max_Y;
uniform float min_Y;

uniform float delta_X;
uniform float delta_Y;

void main() {
	gl_Position = vec4(position.x, position.y, position.z, 1.0f);

	vec2 mbrot = vec2((float(position.x) + 1.0f) / 2.0f, (float(position.y) + 1.0f) / 2.0f) \
		* vec2(delta_X, delta_Y) + vec2(min_X, min_Y);
	//mbrot = vec2(delta_X * (mbrot.x), delta_Y * (mbrot.y));

	mbrot_pos = vec4(mbrot, 0.0f, 1.0f);
}