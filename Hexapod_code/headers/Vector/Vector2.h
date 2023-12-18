#ifndef VECTOR2_H
#define VECTOR2_H

struct Vector2 {
    union {
        float x;
        float r;
    };
    union {
        float y;
        float fi;
    };
};

#endif // VECTOR2_H