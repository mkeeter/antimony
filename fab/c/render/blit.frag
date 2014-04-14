#version 330

uniform sampler2D values;

// Properties of the atlas texture
uniform int block_size;
uniform int block_count;

// Starting slot and slot count
uniform int start_slot;
uniform int slot_count;

out vec4 fragColor;

void main()
{
    int i = int(floor(gl_FragCoord.x));
    int j = int(floor(gl_FragCoord.y));

    int slot = i / block_size + j * block_count;
    int index = i % block_size;

    if (slot < start_slot || slot >= start_slot + slot_count)
    {
        discard;
    }
    else
    {
        fragColor = texelFetch(values, ivec2(index, slot - start_slot), 0);
    }
}

