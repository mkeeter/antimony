#version 330

uniform usampler1D tape;
uniform sampler2D values;

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

void main()
{
    int index = int(floor(gl_FragCoord.x));
    int node = int(floor(gl_FragCoord.y));

    uvec4 texel = texelFetch(tape, node, 0);
    int op = int(texel.r);

    // Get LHS and RHS values, either immediate or look-up
    float lhs = 0;
    float rhs = 0;
    if (op <= OP_EXP)
    {
        if ((op & (1 << 8)) != 0)
            lhs = uintBitsToFloat(texel.g);
        else
            lhs = texelFetch(values, ivec2(index, texel.g), 0).r;
    }

    if (op <= OP_POW)
    {
        if ((op & (1 << 9)) != 0)
            rhs = uintBitsToFloat(texel.b);
        else
            rhs = texelFetch(values, ivec2(index, texel.b), 0).r;
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

    else if (op == OP_X)    result = 0;
    else if (op == OP_Y)    result = 0;
    else if (op == OP_Z)    result = 0;

    fragColor = vec4(result);

}
