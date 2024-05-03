out vec4 PixelColor;

uniform vec4 p_color;
uniform sampler2D tex1;

in vec2 v_uv;
in vec4 v_color;


void main()
{

    PixelColor = v_color * texture(tex1, v_uv);
}