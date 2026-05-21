#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>

namespace nova3d::math {

constexpr float kPi = 3.14159265358979323846F;
inline float toRadians(float degrees) { return degrees * (kPi / 180.0F); }

struct Vector2 { float x=0,y=0; };
struct Vector3 { float x=0,y=0,z=0;
    Vector3 operator+(const Vector3& r) const { return {x+r.x,y+r.y,z+r.z}; }
    Vector3 operator-(const Vector3& r) const { return {x-r.x,y-r.y,z-r.z}; }
    Vector3 operator*(float s) const { return {x*s,y*s,z*s}; }
    float dot(const Vector3& r) const { return x*r.x+y*r.y+z*r.z; }
    Vector3 cross(const Vector3& r) const { return {y*r.z-z*r.y,z*r.x-x*r.z,x*r.y-y*r.x}; }
    float length() const { return std::sqrt(dot(*this)); }
    Vector3 normalized() const { float l=length(); return l>0?(*this)*(1.0F/l):Vector3{}; }
};
struct Vector4 { float x=0,y=0,z=0,w=0; };

struct Quaternion { float w=1,x=0,y=0,z=0;
    static Quaternion fromAxisAngle(const Vector3& axis, float angleRad){ Vector3 n=axis.normalized(); float s=std::sin(angleRad*0.5F); return {std::cos(angleRad*0.5F), n.x*s,n.y*s,n.z*s}; }
};

struct Matrix4 {
    std::array<float,16> m{1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
    static Matrix4 identity(){ return {}; }
    float& at(int r,int c){ return m[c*4+r]; }
    float at(int r,int c) const { return m[c*4+r]; }
    Matrix4 operator*(const Matrix4& b) const { Matrix4 r{}; r.m.fill(0); for(int row=0;row<4;++row)for(int col=0;col<4;++col)for(int k=0;k<4;++k) r.at(row,col)+=at(row,k)*b.at(k,col); return r; }
    Vector3 transformPoint(const Vector3& v) const { float x=at(0,0)*v.x+at(0,1)*v.y+at(0,2)*v.z+at(0,3); float y=at(1,0)*v.x+at(1,1)*v.y+at(1,2)*v.z+at(1,3); float z=at(2,0)*v.x+at(2,1)*v.y+at(2,2)*v.z+at(2,3); float w=at(3,0)*v.x+at(3,1)*v.y+at(3,2)*v.z+at(3,3); if(std::abs(w)>1e-6F){ x/=w; y/=w; z/=w; } return {x,y,z}; }
    static Matrix4 translation(const Vector3& t){ Matrix4 r=identity(); r.at(0,3)=t.x; r.at(1,3)=t.y; r.at(2,3)=t.z; return r; }
    static Matrix4 scaling(const Vector3& s){ Matrix4 r=identity(); r.at(0,0)=s.x; r.at(1,1)=s.y; r.at(2,2)=s.z; return r; }
    static Matrix4 rotation(const Quaternion& q){ Matrix4 r=identity(); float xx=q.x*q.x,yy=q.y*q.y,zz=q.z*q.z,xy=q.x*q.y,xz=q.x*q.z,yz=q.y*q.z,wx=q.w*q.x,wy=q.w*q.y,wz=q.w*q.z; r.at(0,0)=1-2*(yy+zz); r.at(0,1)=2*(xy-wz); r.at(0,2)=2*(xz+wy); r.at(1,0)=2*(xy+wz); r.at(1,1)=1-2*(xx+zz); r.at(1,2)=2*(yz-wx); r.at(2,0)=2*(xz-wy); r.at(2,1)=2*(yz+wx); r.at(2,2)=1-2*(xx+yy); return r; }
    static Matrix4 perspective(float fovYRad, float aspect, float nearZ, float farZ){ Matrix4 r{}; r.m.fill(0); float f=1/std::tan(fovYRad*0.5F); r.at(0,0)=f/aspect; r.at(1,1)=f; r.at(2,2)=(farZ+nearZ)/(nearZ-farZ); r.at(2,3)=(2*farZ*nearZ)/(nearZ-farZ); r.at(3,2)=-1; return r; }
    static Matrix4 orthographic(float l,float rgt,float b,float t,float nearZ,float farZ){ Matrix4 r=identity(); r.at(0,0)=2/(rgt-l); r.at(1,1)=2/(t-b); r.at(2,2)=-2/(farZ-nearZ); r.at(0,3)=-(rgt+l)/(rgt-l); r.at(1,3)=-(t+b)/(t-b); r.at(2,3)=-(farZ+nearZ)/(farZ-nearZ); return r; }
    static Matrix4 lookAt(const Vector3& eye,const Vector3& target,const Vector3& up){ Vector3 f=(target-eye).normalized(); Vector3 s=f.cross(up).normalized(); Vector3 u=s.cross(f); Matrix4 r=identity(); r.at(0,0)=s.x;r.at(0,1)=s.y;r.at(0,2)=s.z; r.at(1,0)=u.x;r.at(1,1)=u.y;r.at(1,2)=u.z; r.at(2,0)=-f.x;r.at(2,1)=-f.y;r.at(2,2)=-f.z; r.at(0,3)=-s.dot(eye); r.at(1,3)=-u.dot(eye); r.at(2,3)=f.dot(eye); return r; }
};

struct Plane { Vector3 normal{}; float d=0; float distance(const Vector3& p) const { return normal.dot(p)+d; } };
struct Ray { Vector3 origin{}; Vector3 direction{0,0,-1}; };
struct Sphere { Vector3 center{}; float radius=1; };
struct AABB { Vector3 min{0,0,0}; Vector3 max{0,0,0}; Vector3 center() const { return (min+max)*0.5F; } float radius() const { return (max-center()).length(); } };

struct Frustum { std::array<Plane,6> planes{};
    static Frustum fromViewProjection(const Matrix4& m){ Frustum f; auto ext=[&](int i,int a,int s){ return m.at(0,3)+s*m.at(0,a+i);};
        auto set=[&](int idx,float a,float b,float c,float d){ float inv=1.0F/std::sqrt(a*a+b*b+c*c); f.planes[idx]={{a*inv,b*inv,c*inv},d*inv};};
        set(0,m.at(0,3)+m.at(0,0),m.at(1,3)+m.at(1,0),m.at(2,3)+m.at(2,0),m.at(3,3)+m.at(3,0));
        set(1,m.at(0,3)-m.at(0,0),m.at(1,3)-m.at(1,0),m.at(2,3)-m.at(2,0),m.at(3,3)-m.at(3,0));
        set(2,m.at(0,3)+m.at(0,1),m.at(1,3)+m.at(1,1),m.at(2,3)+m.at(2,1),m.at(3,3)+m.at(3,1));
        set(3,m.at(0,3)-m.at(0,1),m.at(1,3)-m.at(1,1),m.at(2,3)-m.at(2,1),m.at(3,3)-m.at(3,1));
        set(4,m.at(0,3)+m.at(0,2),m.at(1,3)+m.at(1,2),m.at(2,3)+m.at(2,2),m.at(3,3)+m.at(3,2));
        set(5,m.at(0,3)-m.at(0,2),m.at(1,3)-m.at(1,2),m.at(2,3)-m.at(2,2),m.at(3,3)-m.at(3,2)); return f; }
    bool intersects(const Sphere& s) const { for (const auto& p:planes) if(p.distance(s.center)<-s.radius) return false; return true; }
    bool intersects(const AABB& b) const { for(const auto& p:planes){ Vector3 v{p.normal.x>=0?b.max.x:b.min.x,p.normal.y>=0?b.max.y:b.min.y,p.normal.z>=0?b.max.z:b.min.z}; if(p.distance(v)<0) return false;} return true; }
};

struct Transform {
    Vector3 position{}; Quaternion rotation{}; Vector3 scale{1,1,1};
    mutable bool dirty=true; mutable Matrix4 localCache=Matrix4::identity(); mutable Matrix4 worldCache=Matrix4::identity();
    Matrix4 localMatrix() const { if(dirty){ localCache=Matrix4::translation(position)*Matrix4::rotation(rotation)*Matrix4::scaling(scale); dirty=false;} return localCache; }
    Matrix4 worldMatrix(const Matrix4& parentWorld) const { worldCache=parentWorld*localMatrix(); return worldCache; }
    void markDirty(){ dirty=true; }
};

} // namespace nova3d::math
