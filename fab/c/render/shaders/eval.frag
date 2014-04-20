#version 330

uniform sampler1D tape;
uniform sampler2D atlas;
uniform sampler1D xyz;

// Properties of the atlas texture
uniform int block_size;
uniform int block_count;

out vec4 fragColor;

#define    OP_ADD    0
#define    OP_SUB    1
#define    OP_MUL    2
#define    OP_DIV    3
#define    OP_MIN    4
#define    OP_MAX    5
#define    OP_POW    6

#define    OP_ABS    7
#define    OP_SQUARE 8
#define    OP_SQRT   9
#define    OP_SIN   10
#define    OP_COS   11
#define    OP_TAN   12
#define    OP_ASIN  13
#define    OP_ACOS  14
#define    OP_ATAN  15
#define    OP_NEG   16
#define    OP_EXP   17

#define    OP_X     18
#define    OP_Y     19
#define    OP_Z     20

float get_value(int slot, int index)
{
    ivec2 pos = ivec2(index + (slot % block_count) * block_size,
                      slot / block_count);
    return texelFetch(atlas, pos, 0).r;
}

void main()
{
    int index = int(floor(gl_FragCoord.x));
    int node = int(floor(gl_FragCoord.y));

    // Get the texel that containing evaluation information
    vec4 texel = texelFetch(tape, node, 0);
    int op = floatBitsToInt(texel.r);

    // Get LHS and RHS values, either immediate or look-up
    float lhs = 0;
    float rhs = 0;
    if (op <= OP_EXP)
    {
        if ((op & (1 << 8)) != 0)
            lhs = texel.g;
        else
            lhs = get_value(floatBitsToInt(texel.g), index);
    }

    if (op <= OP_POW)
    {
        if ((op & (1 << 9)) != 0)
            rhs = texel.b;
        else
            lhs = get_value(floatBitsToInt(texel.b), index);
    }

    float result;
    if      (op == OP_ADD)  result = lhs + rhs;
    else if (op == OP_SUB)  result = lhs - rhs;
    else if (op == OP_MUL)  result = lhs * rhs;
    else if (op == OP_DIV)  result = lhs / rhs;
    else if (op == OP_MIN)  result = min(lhs, rhs);
    else if (op == OP_MAX)  result = max(lhs, rhs);
    else if (op == OP_POW)  result = pow(lhs, rhs);

    else if (op == OP_ABS)  result = abs(lhs);
    else if (op == OP_SQUARE) result = lhs * lhs;
    else if (op == OP_SQRT) result = sqrt(lhs);
    else if (op == OP_SIN)  result = sin(lhs);
    else if (op == OP_COS)  result = cos(lhs);
    else if (op == OP_TAN)  result = tan(lhs);
    else if (op == OP_ASIN) result = asin(lhs);
    else if (op == OP_ACOS) result = acos(lhs);
    else if (op == OP_ATAN) result = atan(lhs);
    else if (op == OP_NEG)  result = -lhs;
    else if (op == OP_EXP)  result = exp(lhs);

    else if (op == OP_X)    result = texelFetch(xyz, index, 0).r;
    else if (op == OP_Y)    result = texelFetch(xyz, index, 0).g;
    else if (op == OP_Z)    result = texelFetch(xyz, index, 0).b;

    fragColor = vec4(result);
}
