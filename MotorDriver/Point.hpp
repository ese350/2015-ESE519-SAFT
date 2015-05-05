/*Point class to represent the coordinates of the ball in a 2D rectangular coordinate system*/
#include <math.h>
#include <iostream>
#define PI 3.14159265

template<typename T>//Templated class for Point creation of various types
class Point{
public:
    T x;
    T y;
    explicit Point(T init_x=0,T init_y=0):x(init_x),y(init_y){}//constructor
    
    Point(const Point& another_point){//copy constructor
        x=another_point.x;
        y=another_point.y;
    }
    
    Point operator/(const Point &denominator) const{///operator overload function
        Point temp(x/denominator.x,y/denominator.y);
        return temp;
    }

    Point operator*(const Point &another_point) const{
        Point temp(x*another_point.x,y*another_point.y);
        return temp;
    }
    
    Point operator-(const Point &another_point) const{
        Point temp(x-another_point.x,y-another_point.y);
        return temp;
    }
    
    Point operator+(const Point &another_point) const{
        Point temp(x+another_point.x,y+another_point.y);
        return temp;
    }
    
    Point operator/(const double denominator) const{
        Point temp(x/denominator,y/denominator);
        return temp;
    }

    Point operator*(const double scale) const{
        Point temp(x*scale,y*scale);
        return temp;
    }

    Point operator-(const double var) const{
        Point temp(x-var,y-var);
        return temp;
    }
    
    Point operator+(const double var) const{
        Point temp(x+var,y+var);
        return temp;
    }
    
    void operator=(const Point &another_point){
        x=another_point.x;
        y=another_point.y;
    }
    
    template<typename C>//templated function to typecast one point to another point
    operator Point<C>(){
        return Point<C>(x,y);
    }
    
    double dist_from(const Point &p2) const{
        return (*this-p2);
    }
    
    double angle(char units='r') const{//returns the angle of the point at the origin
        double temp=atan2(y,x);
        if(units=='d') temp=(temp*180)/PI;
        return temp;
    }
    
    bool operator==(const Point &another_point) const{
        return ((x==another_point.x) && (y==another_point.y));
    }
    
    bool operator>(const Point &another_point) const{
        Point origin(0,0);
        return (dist_from(origin)>another_point.dist_from(origin));
    }
    
    bool operator<(const Point &another_point) const{
        Point origin(0,0);
        return (dist_from(origin)<another_point.dist_from(origin));
    }
    
    bool operator>=(const Point &another_point) const{
        return ((*this>another_point)||(*this==another_point));
    }
    
    bool operator<=(const Point &another_point) const{
        return ((*this<another_point)||(*this==another_point));
    }
};

typedef Point<int> Point2d;//specific instances of Point class which are considered to be useful for this project
typedef Point<float> Point2f;