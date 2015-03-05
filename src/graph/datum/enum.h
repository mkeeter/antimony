#ifndef DATUM_TYPES_H
#define DATUM_TYPES_H

namespace DatumType
{
    enum DatumType
    {
        FLOAT,
        FLOAT_OUTPUT,
        INT,
        NAME,
        SCRIPT,
        STRING,
        SHAPE_OUTPUT,
        SHAPE_FUNCTION, // DEPRECATED, but still here to preserve numbering
        SHAPE_INPUT, // DEPRECATED, but still here to preserve numbering
        SHAPE
    };
}

#endif
