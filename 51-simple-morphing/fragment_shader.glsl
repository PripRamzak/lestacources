#version 300 es
precision mediump float;

in vec4 v_position;
uniform float time;
out vec4 frag_color;

void main()
{
    float blue_color = abs((v_position.x - v_position.y) / 2. * sin(time));
    frag_color = vec4(abs(sin(time) * v_position.x), abs(sin(time) * v_position.y), blue_color, 1.0);
}