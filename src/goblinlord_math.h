#pragma once

union v2
{
    struct
    {
        r32 X, Y;
    };
    r32 E[2];
};

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

union v3
{
    struct
    {
        r32 X, Y, Z;
    };
    r32 E[3];
};

inline v3 V3(r32 X, r32 Y, r32 Z)
{
    v3 Result;

    Result.X = X;
    Result.Y = Y;
    Result.Z = Z;

    return(Result);
}

inline v3 operator*(r32 A, v3 B)
{
    v3 Result;

    Result.X = A*B.X;
    Result.Y = A*B.Y;
    Result.Z = A*B.Z;

    return(Result);
}

inline v3 operator*(v3 B, r32 A)
{
    v3 Result = A*B;

    return(Result);
}

inline v3 & operator*=(v3 &B, r32 A)
{
    B = A * B;

    return(B);
}

inline v3 operator-(v3 A)
{
    v3 Result;

    Result.X = -A.X;
    Result.Y = -A.Y;
    Result.Z = -A.Z;

    return(Result);
}

inline v3 operator+(v3 A, v3 B)
{
    v3 Result;

    Result.X = A.X + B.X;
    Result.Y = A.Y + B.Y;
    Result.Z = A.Z + B.Z;

    return(Result);
}

inline v3 & operator+=(v3 &A, v3 B)
{
    A = A + B;

    return(A);
}

inline v3 operator-(v3 A, v3 B)
{
    v3 Result;

    Result.X = A.X - B.X;
    Result.Y = A.Y - B.Y;
    Result.Z = A.Z - B.Z;

    return(Result);
}

union v4
{
    struct
    {
        r32 X, Y, Z, W;
    };
    r32 E[2];
};

inline v4 V4(r32 X, r32 Y, r32 Z, r32 W)
{
    v4 Result;

    Result.X = X;
    Result.Y = Y;
    Result.Z = Z;
    Result.W = W;

    return(Result);
}

inline v4 operator-(v4 A, v4 B)
{
    v4 Result;

    Result.X = A.X - B.X;
    Result.Y = A.Y - B.Y;
    Result.Z = A.Z - B.Z;
    Result.W = A.W - B.W;

    return(Result);
}

inline v4 operator-(v4 A)
{
    v4 Result;

    Result.X = -A.X;
    Result.Y = -A.Y;
    Result.Z = -A.Z;
    Result.W = -A.W;

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

inline r32 Determinant2D(v4 v0, v4 v1)
{
    return(v0.X * v1.Y - v0.Y * v1.X);
}

inline v4 Vector3DTo4D(v3 *v)
{
    return(V4(v->X, v->Y, v->Z, 0.0f));
}

inline v4 Point3DTo4D(v3 *v)
{
    return(V4(v->X, v->Y, v->Z, 1.0f));
}