uniform mat4 projection;
uniform mat4 view;
uniform mat4 world;

in vec4 a_position;
in vec2 a_uv;
in vec4 a_color;

out vec2 v_uv;
out vec4 v_color;

void main()
{
    vec4 pos = a_position;
    gl_Position = projection * view * world * pos;
    v_uv = a_uv;
    v_color = a_color;
}

