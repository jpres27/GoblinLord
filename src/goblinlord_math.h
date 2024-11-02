#pragma once

union v2
{
    struct
    {
        r32 X, Y;
    };
    r32 E[2];
};

// TODO(casey): Consider v2 A = v2{5, 3}; ?
inline v2 V2(r32 X, r32 Y)
{
    v2 Result;

    Result.X = X;
    Result.Y = Y;

    return(Result);
}

inline v2 operator*(r32 A, v2 B)
{
    v2 Result;

    Result.X = A*B.X;
    Result.Y = A*B.Y;

    return(Result);
}

inline v2 operator*(v2 B, r32 A)
{
    v2 Result = A*B;

    return(Result);
}

inline v2 & operator*=(v2 &B, r32 A)
{
    B = A * B;

    return(B);
}

inline v2 operator-(v2 A)
{
    v2 Result;

    Result.X = -A.X;
    Result.Y = -A.Y;

    return(Result);
}

inline v2 operator+(v2 A, v2 B)
{
    v2 Result;

    Result.X = A.X + B.X;
    Result.Y = A.Y + B.Y;

    return(Result);
}

inline v2 & operator+=(v2 &A, v2 B)
{
    A = A + B;

    return(A);
}

inline v2 operator-(v2 A, v2 B)
{
    v2 Result;

    Result.X = A.X - B.X;
    Result.Y = A.Y - B.Y;

    return(Result);
}

inline r32 Square(r32 A)
{
    r32 Result = A*A;

    return(Result);
}

inline r32 Inner(v2 A, v2 B)
{
    r32 Result = A.X*B.X + A.Y*B.Y;

    return(Result);
}

inline r32 LengthSq(v2 A)
{
    r32 Result = Inner(A, A);

    return(Result);
}