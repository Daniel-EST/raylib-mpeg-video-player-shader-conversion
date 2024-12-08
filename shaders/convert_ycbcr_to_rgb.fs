#version 330

in vec2 fragTexCoord;

uniform sampler2D textureY;   // Luma (Y) texture
uniform sampler2D textureCb;  // Cb (blue-difference)
uniform sampler2D textureCr;  // Cr (red-difference)

out vec4 finalColor;

void main() {
    float Y = texture(textureY, fragTexCoord).r;
    float Cb = texture(textureCb, fragTexCoord).r;
    float Cr = texture(textureCr, fragTexCoord).r;

    // Combine Y, Cb, and Cr into a single vector
    vec3 ycc = vec3(Y, Cb, Cr);

    // Convert YCbCr to RGB
	mat4 bt601 = mat4(
		1.16438,  0.00000,  1.59603, -0.87079,
		1.16438, -0.39176, -0.81297,  0.52959,
		1.16438,  2.01723,  0.00000, -1.08139,
		0, 0, 0, 1
	);
	finalColor = vec4(ycc, 1.0) * bt601;
}
