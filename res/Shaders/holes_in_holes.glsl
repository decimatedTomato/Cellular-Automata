#version 330 core

precision mediump float;

// varying vec4 v_color;

in vec4 gl_FragCoord;
in vec2 v_texCoords;

out vec4 fragColor;

uniform vec2 u_resolution;
uniform float u_time;
// uniform sampler2D u_texture;

void main() {
    // use uv as normalized coordinates (x {0, 1}, y {0, 1})
    vec2 uv = gl_FragCoord.xy / u_resolution.xy * 2. - 1.;

    // Adjust x axis in order to maintain square aspect ratio
    uv = vec2(uv.x / u_resolution.y * u_resolution.x, uv.y);
    uv /= 2.;

    float angle = sin(-0.4 * u_time);
    
    angle += length(uv);
    for (float i = 0.; i < 32.; i += 1.) {
        uv *= mat2(
            cos(angle), -sin(angle),
            sin(angle),  cos(angle)
        );
    }

    uv *= (uv + 0.);
    
    vec3 col = vec3(length(uv));
    col += vec3(0., 1./3., 2./3.);
    col = vec3(mod(col.r - u_time, 1.),
               mod(col.g + u_time, 1.),
               mod(col.b, 1.));
    
    fragColor = vec4(col, 1.);
}