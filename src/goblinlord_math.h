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
    r32 E[4];
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

inline v4 operator+(const v4& A, const v4& B)
{
    v4 Result;
    Result.X = A.X + B.X; 
    Result.Y = A.Y + B.Y; 
    Result.Z = A.Z + B.Z;
    Result.W = A.W + B.W;
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

inline v4 operator*(r32 A, v4 B)
{
    v4 Result;

    Result.X = A*B.X;
    Result.Y = A*B.Y;
    Result.Z = A*B.Z;
    Result.W = A*B.W;

    return(Result);
}

inline v4 operator*(v4 B, r32 A)
{
    v4 Result = A*B;

    return(Result);
}

inline v4 & operator*=(v4 &B, r32 A)
{
    B = A * B;

    return(B);
}

union RGBA32
{
    struct 
    {
        r32 red, green, blue, alpha;
    };
    r32 E[4];
};

inline RGBA32 CreateRGBA32(r32 red, r32 green, r32 blue, r32 alpha)
{
    RGBA32 Result;

    Result.red = red;
    Result.green = green;
    Result.blue = blue;
    Result.alpha = alpha;

    return(Result);
}

inline RGBA32 operator+(const RGBA32& A, const RGBA32& B)
{
    RGBA32 Result;
    
    Result.red = A.red + B.red; 
    Result.green = A.green + B.green; 
    Result.blue = A.blue + B.blue;
    Result.alpha = A.alpha + B.alpha;

    return(Result);
}

inline RGBA32 operator*(r32 A, const RGBA32& B)
{
    RGBA32 Result;

    Result.red = A*B.red;
    Result.green = A*B.green;
    Result.blue = A*B.blue;
    Result.alpha = A*B.alpha;

    return(Result);
}

inline RGBA32 operator*(RGBA32 B, r32 A)
{
    RGBA32 Result = A*B;

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

inline r32 Determinant2D(const v4& v0, const v4& v1)
{
    return(v0.X * v1.Y - v0.Y * v1.X);
}

inline v4 Vector3DTo4D(const v3& v)
{
    return(V4(v.X, v.Y, v.Z, 0.0f));
}

inline v4 Point3DTo4D(const v3& v)
{
    return(V4(v.X, v.Y, v.Z, 1.0f));
}

struct m4x4
{
    r32 values[16];
};

inline v4 operator*(const m4x4& m, const v4& v)
{
    v4 result = V4(0.f, 0.f, 0.f, 0.f);

    result.X = m.values[ 0] * v.X + m.values[ 1] * v.Y + m.values[ 2] * v.Z + m.values[ 3] * v.W;
    result.Y = m.values[ 4] * v.X + m.values[ 5] * v.Y + m.values[ 6] * v.Z + m.values[ 7] * v.W;
    result.Z = m.values[ 8] * v.X + m.values[ 9] * v.Y + m.values[10] * v.Z + m.values[11] * v.W;
    result.W = m.values[12] * v.X + m.values[13] * v.Y + m.values[14] * v.Z + m.values[15] * v.W;

    return result;
}

internal m4x4 IdentityMatrix()
{
    m4x4 m = { {
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        0.f, 0.f, 0.f, 1.f,
    } };

    return(m);
}