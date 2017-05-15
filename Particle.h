#include <stdio.h>
#include "Vector.h"

class Particle
{
public:
    Particle();
    Particle(const Vector3d &p, const Vector3d &v, const Vector4d &c, const double m, const double l, const double s);
    ~Particle();
    
    void setPosition(const Vector3d&);
    void setVelocity(const Vector3d&);
    void setColor(const Vector4d&);
    void setMass(const double);
    void setLifeSpan(const double);
    void setPointSize(const double);
    
    const Vector3d& getPosition();
    const Vector3d& getVelocity();
    const Vector4d& getColor();
    const double   getMass();
    const double   getLifeSpan();
    const double   getPointSize();
    
private:
    Vector3d position;
    Vector3d velocity;
    Vector4d color;
    double mass;
    int lifeSpan;
    double pointSize;
    
};

