#version 460 core

out vec4 frag_color;

in vec2 tex_coord;

/* layout(binding = 0) uniform sampler2D our_texture; */
uniform sampler2D our_texture;

void main() {
    frag_color = texture(our_texture, tex_coord);
}
