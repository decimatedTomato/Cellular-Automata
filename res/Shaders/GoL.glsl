#version 330 core

precision mediump float;

in vec4 gl_FragCoord;
in vec2 v_texCoords;

out vec4 fragColor;

uniform vec2 u_resolution;
uniform sampler2D u_texture;

void main() {
    vec4 color = texture2D(u_texture, v_texCoords);
    // Make samples and apply GoL rules
    fragColor = color.rrra;
}