#pragma once

#include "math.h"

inline r32 SquareRoot(r32 value)
{
    r32 Result = sqrtf(value);
    return(Result);
}

inline r32 AbsoluteValue(r32 value)
{
    r32 Result = fabs(value);
    return(Result);
}

inline i32 AbsoluteValue(i32 value)
{
    i32 Result = abs(value);
    return(Result);
}

inline u32 RotateLeft(u32 Value, i32 Amount)
{
    u32 Result = _rotl(Value, Amount);

    return(Result);
}

inline u32 RotateRight(u32 Value, i32 Amount)
{
    u32 Result = _rotr(Value, Amount);

    return(Result);
}

inline i32 RoundReal32ToInt32(r32 value)
{
    i32 Result = (i32)roundf(value);
    return(Result);
}

inline u32 RoundReal32ToUInt32(r32 value)
{
    u32 Result = (u32)roundf(value);
    return(Result);
}

inline i32  FloorReal32ToInt32(r32 value)
{
    i32 Result = (i32)floorf(value);
    return(Result);
}

inline i32 TruncateReal32ToInt32(r32 value)
{
    i32 Result = (i32)value;
    return(Result);
}

inline r32 Sin(r32 Angle)
{
    r32 Result = sinf(Angle);
    return(Result);
}

inline r32 Cos(r32 Angle)
{
    r32 Result = cosf(Angle);
    return(Result);
}

inline r32 ATan2(r32 Y, r32 X)
{
    r32 Result = atan2f(Y, X);
    return(Result);
}

