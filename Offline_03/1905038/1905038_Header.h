#include <bits/stdc++.h>
#include "bitmap_image.hpp"
#include <GL/glut.h>
using namespace std;

class point
{
public:
    GLdouble x, y, z;
    point()
    {
        x = 0.0;
        y = 0.0;
        z = 0.0;
    }
    point(GLdouble a, GLdouble b, GLdouble c)
    {
        x = a;
        y = b;
        z = c;
    }

    point crossProduct(point p)
    {
        point temp;
        temp.x = y * p.z - p.y * z;
        temp.y = z * p.x - x * p.z;
        temp.z = x * p.y - p.x * y;
        return temp;
    }
    GLdouble dotProduct(point p)
    {
        GLdouble temp;
        temp = x * p.x;
        temp += y * p.y;
        temp += z * p.z;
        return temp;
    }

    point operator*(GLdouble k)
    {
        point temp;
        temp.x = x * k;
        temp.y = y * k;
        temp.z = z * k;
        return temp;
    }
    point operator+(point p)
    {
        point temp;
        temp.x = x + p.x;
        temp.y = y + p.y;
        temp.z = z + p.z;
        return temp;
    }
    point operator-(point p)
    {
        point temp;
        temp.x = x - p.x;
        temp.y = y - p.y;
        temp.z = z - p.z;
        return temp;
    }
    GLdouble dotAngle(point p)
    {

        GLdouble temp = x * p.x + y * p.y + z * p.z;
        GLdouble k = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
        GLdouble l = sqrt(pow(p.x, 2) + pow(p.y, 2) + pow(p.z, 2));
        return acos(temp / (k * l)) * (180.0 / M_PI);
    }
    void normalize()
    {
        GLdouble temp = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
        x = x / temp;
        y /= temp;
        z /= temp;
    }
    GLdouble distance(point p)
    {
        point temp;
        temp.x = x - p.x;
        temp.y = y - p.y;
        temp.z = z - p.z;
        return sqrt(temp.x * temp.x + temp.y * temp.y + temp.z * temp.z);
    }
    GLdouble modulus()
    {
        return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
    }
    GLdouble radius()
    {
        return pow(x, 2) + pow(y, 2) + pow(z, 2);
    }
    point operator=(point p)
    {
        x = p.x;
        y = p.y;
        z = p.z;
        point temp(x, y, z);
        return temp;
    }
};

class colour
{

public:
    GLdouble red;
    GLdouble green, blue;

    colour()
    {
        red = green = blue = 0.0;
    }
    colour(GLdouble a, GLdouble b, GLdouble c)
    {
        red = a;
        green = b;
        blue = c;
    }
    colour operator*(GLdouble x)
    {
        colour temp;
        temp.red = red * x;
        temp.blue = blue * x;
        temp.green = green * x;
        return temp;
    }
    colour operator+(colour x)
    {
        colour temp;
        temp.red = red + x.red;
        temp.blue = blue + x.blue;
        temp.green = green + x.green;
        return temp;
    }
    void clip()
    {
        if (red > 1)
        {
            red = 1;
        }
        if (green > 1)
            green = 1;
        if (blue > 1)
            blue = 1;
        if (red < 0)
            red = 0;
        if (green < 0)
            green = 0;
        if (blue < 0)
            blue = 0;
    }
};

class pointLight
{

public:
    point position;
    colour c;

    void draw()
    {
        glPointSize(7);
        glColor3d(c.red, c.green, c.blue);
        glBegin(GL_POINTS);
        {
            glVertex3d(position.x, position.y, position.z);
        }
        glEnd();
    }
};

class spotlight
{
public:
    pointLight light;
    point direction;
    GLdouble cut_off_angle;

    void draw()
    {
        glPointSize(17);
        glColor3d(light.c.red, light.c.green, light.c.blue);
        glBegin(GL_POINTS);
        {
            glVertex3d(light.position.x, light.position.y, light.position.z);
        }
        glEnd();
    }
};

class ray
{

public:
    point origin, direction;
    ray()
    {
    }
    ray(point ori, point dir)
    {
        origin = ori;
        direction = dir;
        direction.normalize();
    }
    ray doReflection(point p, ray normal)
    {
        GLdouble temp = direction.dotProduct(normal.direction);
        temp *= 2;
        normal.direction = normal.direction * temp;
        point k = direction - normal.direction;
        k.normalize();
        ray rr;
        rr.origin = p;
        rr.direction = k;
        return rr;
    }
};

class object
{

public:
    point point_ref;
    GLdouble width;
    GLdouble height;
    GLdouble length;
    colour color;

    GLdouble co_efficient[4];
    GLint shine;

    object()
    {
        co_efficient[0] = co_efficient[1] = co_efficient[2] = co_efficient[3] = 0.0;
    }

    void setColour(colour cc)
    {
        color.red = cc.red;
        color.green = cc.green;
        color.blue = cc.blue;
    }
    void setShine(GLint ss)
    {
        shine = ss;
    }
    void setCoefficients(GLdouble arr[])
    {
        co_efficient[0] = arr[0];
        co_efficient[1] = arr[1];
        co_efficient[2] = arr[2];
        co_efficient[3] = arr[3];
    }
    
    virtual void draw()
    {
        return;
    }
    virtual GLdouble intersect(colour &cc, GLint level, ray rr, GLint max_level)
    {
        return -1;
    }

    virtual ~object()
    {
    }
};

extern vector<object *> obj_arr;
extern vector<pointLight *> light_arr;
extern vector<spotlight *> spot_arr;

class sphere : public object
{

public:
    sphere(point center, GLdouble r)
    {
        point_ref = center;
        length = r;
    }

    virtual void draw()
    {
        // print();

        glColor3d(color.red, color.green, color.blue);

        int vertical = 20;
        int horizontal = 30;
        GLdouble r = length;
        GLdouble angle1, angle2, radius;
        point points[200][365];
        point ball_center = point_ref;
        int j = 0;
        while (j < vertical + 1)
        {
            angle1 = (-M_PI / 2.0) + (j * M_PI) / vertical;
            radius = r * cos(angle1);

            int i = 0;
            while (i < horizontal + 1)
            {
                angle2 = (i * 2.0 * M_PI) / horizontal;
                points[j][i].x = radius * cos(angle2) ;
                points[j][i].y = radius * sin(angle2);
                points[j][i].z = r * sin(angle1);

                i++;
            }

            j++;
        }

        j = 0;
        glBegin(GL_QUADS);
        while (j < vertical)
        {
            angle1 = (-M_PI / 2.0) + (j * M_PI) / vertical;
            radius = r * cos(angle1);

            int i = 0;
            while (i < horizontal)
            {
                

                glVertex3f(points[j][i].x + ball_center.x, points[j][i].y + ball_center.y, points[j][i].z + ball_center.z);
                glVertex3f(points[j][i + 1].x + ball_center.x, points[j][i + 1].y + ball_center.y, points[j][i + 1].z + ball_center.z);

                glVertex3f(points[j + 1][i + 1].x + ball_center.x, points[j + 1][i + 1].y + ball_center.y, points[j + 1][i + 1].z + ball_center.z);
                glVertex3f(points[j + 1][i].x + ball_center.x, points[j + 1][i].y + ball_center.y, points[j + 1][i].z + ball_center.z);

                i++;
            }

            j++;
        }

        glEnd();
    }

    virtual GLdouble intersect(colour &cc, GLint level, ray rr, GLint max_level)
    {
        ray container;
        container = rr;
        rr.origin = rr.origin - point_ref;
        GLdouble coefficient1 = 1;
        GLdouble coefficient2 = rr.direction.dotProduct(rr.origin);
        coefficient2 *= 2;
        GLdouble coefficient3 = rr.origin.dotProduct(rr.origin);
        coefficient3 -= length * length;
        GLdouble discriminant = pow(coefficient2, 2) - 4 * coefficient1 * coefficient3;
        GLdouble resultDistance;
        bool flag1 = (fabs(coefficient1) < 1e-5) ? true : false;

        rr = container;
        if (discriminant < 0)
        {
            
            return -1;
        }
        else if (flag1)
        {
            resultDistance = (-1) * (coefficient3 / coefficient2);
        }
        else
        {
            GLdouble option1 = (sqrt(discriminant) - coefficient2) / (2 * coefficient1);
            GLdouble option2 = (-sqrt(discriminant) - coefficient2) / (2 * coefficient1);
            GLdouble temp = max(option1, option2);
            option2 = min(option1, option2);
            option1 = temp;
            if (option2 > 0)
            {
                resultDistance = option2;
            }
            else if (option1 > 0)
            {
                resultDistance = option1;
            }
            else
            {
                // cout<<"--i am --"<<endl;
                return -1;
            }
        }
       
        

        if (!level)
        {
            return resultDistance;
        }

        point crosspoint = rr.origin + rr.direction * resultDistance;

        //Set ambient color
        cc.red = color.red * co_efficient[0];
        cc.green = color.green * co_efficient[0];
        cc.blue = color.blue * co_efficient[0];

        int j = 0;
        while (j != light_arr.size())
        {
            /* code */
            point dir_light = crosspoint - light_arr[j]->position;
            dir_light.normalize();
            ray ray_light = ray(light_arr[j]->position, dir_light);
            ray ray_normal = ray(crosspoint, crosspoint - point_ref );
            if (crosspoint.distance(light_arr[j]->position) < 1e-5)
            {
                j++;
                continue;
            }
           

            int k = 0;
            int myflag=0;
            
            while (k != obj_arr.size())
            {
                /* code */
                GLdouble tdistance=obj_arr[k]->intersect(cc, 0, ray_light, max_level) ;
                if (tdistance > 0 && (tdistance + 1e-5 < crosspoint.distance(light_arr[j]->position)))
                {
                    myflag=1;
                    break;
                }
                k++;
            }

            if (myflag==0)
            {
                GLdouble lambert_value = (-1) * (ray_light.direction.dotProduct(ray_normal.direction));
                lambert_value = max(0.0, lambert_value);
                ray Reflect = ray_light.doReflection(crosspoint, ray_normal);
                GLdouble value = (-1) * rr.direction.dotProduct(Reflect.direction);
                value = pow(value, shine);
                value = max(0.0, value);
                cc.red = cc.red + light_arr[j]->c.red * co_efficient[1] * lambert_value * color.red + light_arr[j]->c.red * co_efficient[2] * value * color.red;
                cc.green = cc.green + light_arr[j]->c.green * co_efficient[1] * lambert_value * color.green + light_arr[j]->c.green * co_efficient[2] * value * color.green;
                cc.blue = cc.blue + light_arr[j]->c.blue * co_efficient[1] * lambert_value * color.blue + light_arr[j]->c.blue * co_efficient[2] * value * color.blue;
            }

            j++;
        }

        j = 0;
        while (j != spot_arr.size())
        {
            /* code */

            point dir_light = crosspoint - spot_arr[j]->light.position;
            dir_light.normalize();

            if (fabs(dir_light.dotAngle(spot_arr[j]->direction)) < spot_arr[j]->cut_off_angle)
            {
                ray ray_light = ray(spot_arr[j]->light.position, dir_light);
                ray ray_normal = ray(crosspoint, crosspoint - point_ref);
                if (crosspoint.distance(light_arr[j]->position) < 1e-5)
                {
                    j++;
                    continue;
                }
                

                int k = 0 , myflag=0;
                
                while (k != obj_arr.size())
                {
                    /* code */
                    GLdouble tdistance=obj_arr[k]->intersect(cc, 0, ray_light, max_level) ;

                    if (tdistance > 0 && tdistance + 1e-5 < crosspoint.distance(light_arr[j]->position))
                    {
                        myflag=1;
                        break;
                    }
                    k++;
                }

                if (myflag==0)
                {
                    GLdouble lambert_value = (-1) * (ray_light.direction.dotProduct(ray_normal.direction));
                    lambert_value = max(0.0, lambert_value);
                    ray Reflect = ray_light.doReflection(crosspoint, ray_normal);
                    GLdouble value = (-1) * rr.direction.dotProduct(Reflect.direction);
                    value = pow(value, shine);
                    value = max(0.0, value);
                    cc.red = cc.red + spot_arr[j]->light.c.red * co_efficient[1] * lambert_value * color.red + spot_arr[j]->light.c.red * co_efficient[2] * value * color.red;
                    cc.green = cc.green + spot_arr[j]->light.c.green * co_efficient[1] * lambert_value * color.green + spot_arr[j]->light.c.green * co_efficient[2] * value * color.green;
                    cc.blue = cc.blue + spot_arr[j]->light.c.blue * co_efficient[1] * lambert_value * color.blue + spot_arr[j]->light.c.blue * co_efficient[2] * value * color.blue;
                }
            }

            j++;
        }

        if (level >= max_level)
        {
            return resultDistance;
        }

        ray ray_normal = ray(crosspoint, crosspoint - point_ref);
        ray Reflect = rr.doReflection(crosspoint, ray_normal);
        point move;
        move.x = Reflect.direction.x * 1e-5;
        move.y = Reflect.direction.y * 1e-5;
        move.z = Reflect.direction.z * 1e-5;
        Reflect.origin = Reflect.origin + move;

        GLint index_of_object = -20;
        GLdouble result = 1e9;
        colour trivial;
        j = 0;
        while (j != obj_arr.size())
        {
            /* code */
            GLdouble temp = obj_arr[j]->intersect(trivial, 0, Reflect, max_level);
            if (temp > 0)
            {
                if (temp < result)
                {
                    result = temp;
                    index_of_object = j;
                }
            }
            j++;
        }

        if (index_of_object != -20)
        {
            trivial.red = 0.0;
            trivial.blue = 0.0;
            trivial.green = 0.0;
            obj_arr[index_of_object]->intersect(trivial, level + 1, Reflect, max_level);
            trivial = trivial * co_efficient[3];
            cc = cc + trivial;
        }

        

        return resultDistance;
    }
};

class triangle : public object
{
public:
    point p1, p2, p3;
    triangle(point x, point y, point z)
    {
        p1 = x;
        p2 = y;
        p3 = z;
    }
    virtual void draw()
    {
        glColor3d(color.red, color.green, color.blue);
        glBegin(GL_TRIANGLES);
        glVertex3d(p3.x, p3.y, p3.z);
        glVertex3d(p1.x, p1.y, p1.z);
        glVertex3d(p2.x, p2.y, p2.z);
        glEnd();
    }
    GLdouble caldeterminant(GLdouble matrix[3][3])
    {
        GLdouble temp1 = matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1];
        GLdouble temp2 = matrix[1][0] * matrix[2][2] - matrix[1][2] * matrix[2][0];
        GLdouble temp3 = matrix[1][0] * matrix[2][1] - matrix[1][1] * matrix[2][0];
        return matrix[0][0] * temp1 - matrix[0][1] * temp2 + matrix[0][2] * temp3;
    }
    GLdouble calculateBeta(ray r)
    {

        GLdouble gmat[3][3], matrix[3][3];
        gmat[0][0] = p1.x - r.origin.x, gmat[0][1] = p1.x - p3.x, gmat[0][2] = r.direction.x;
        gmat[1][0] = p1.y - r.origin.y, gmat[1][1] = p1.y - p3.y, gmat[1][2] = r.direction.y;
        gmat[2][0] = p1.z - r.origin.z, gmat[2][1] = p1.z - p3.z, gmat[2][2] = r.direction.z;

        matrix[0][0] = p1.x - p2.x, matrix[0][1] = p1.x - p3.x, matrix[0][2] = r.direction.x;
        matrix[1][0] = p1.y - p2.y, matrix[1][1] = p1.y - p3.y, matrix[1][2] = r.direction.y;
        matrix[2][0] = p1.z - p2.z, matrix[2][1] = p1.z - p3.z, matrix[2][2] = r.direction.z;
        return caldeterminant(gmat) / caldeterminant(matrix);
    }
    GLdouble calculateGamma(ray r)
    {

        GLdouble gmat[3][3], matrix[3][3];
        gmat[0][0] = p1.x - p2.x, gmat[0][1] = p1.x - r.origin.x, gmat[0][2] = r.direction.x;
        gmat[1][0] = p1.y - p2.y, gmat[1][1] = p1.y - r.origin.y, gmat[1][2] = r.direction.y;
        gmat[2][0] = p1.z - p2.z, gmat[2][1] = p1.z - r.origin.z, gmat[2][2] = r.direction.z;

        matrix[0][0] = p1.x - p2.x, matrix[0][1] = p1.x - p3.x, matrix[0][2] = r.direction.x;
        matrix[1][0] = p1.y - p2.y, matrix[1][1] = p1.y - p3.y, matrix[1][2] = r.direction.y;
        matrix[2][0] = p1.z - p2.z, matrix[2][1] = p1.z - p3.z, matrix[2][2] = r.direction.z;
        return caldeterminant(gmat) / caldeterminant(matrix);
    }
    GLdouble calculateT(ray r)
    {

        GLdouble gmat[3][3], matrix[3][3];
        gmat[0][0] = p1.x - p2.x, gmat[0][1] = p1.x - p3.x, gmat[0][2] = p1.x - r.origin.x;
        gmat[1][0] = p1.y - p2.y, gmat[1][1] = p1.y - p3.y, gmat[1][2] = p1.y - r.origin.y;
        gmat[2][0] = p1.z - p2.z, gmat[2][1] = p1.z - p3.z, gmat[2][2] = p1.z - r.origin.z;

        matrix[0][0] = p1.x - p2.x, matrix[0][1] = p1.x - p3.x, matrix[0][2] = r.direction.x;
        matrix[1][0] = p1.y - p2.y, matrix[1][1] = p1.y - p3.y, matrix[1][2] = r.direction.y;
        matrix[2][0] = p1.z - p2.z, matrix[2][1] = p1.z - p3.z, matrix[2][2] = r.direction.z;
        return caldeterminant(gmat) / caldeterminant(matrix);
    }

    virtual GLdouble intersect(colour &cc, GLint level, ray rr, GLint max_level)
    {
        GLdouble Beta = calculateBeta(rr);
        GLdouble Gamma = calculateGamma(rr);
        GLdouble resultDistance = calculateT(rr);
        if (!((Beta + Gamma < 1) && (Gamma > 0) && (Beta > 0) && (resultDistance > 0)))
        {
            return -1;
        }

        if (!level)
        {
            return resultDistance;
        }

        point crosspoint = rr.origin + rr.direction * resultDistance;
        cc.red = color.red * co_efficient[0];
        cc.green = color.green * co_efficient[0];
        cc.blue = color.blue * co_efficient[0];

        int j = 0;
        while (j != light_arr.size())
        {
            /* code */
            point dir_light = crosspoint - light_arr[j]->position;
            dir_light.normalize();
            ray ray_light = ray(light_arr[j]->position, dir_light);
            point normal = (p2 - p1).crossProduct(p3 - p1);
            normal.normalize();
            ray ray_normal;
            ray_normal.origin = crosspoint;
            if (ray_light.direction.dotProduct(normal) >= 0)
            {
                ray_normal.direction = normal;
            }
            else
            {
                ray_normal.direction = normal * (-1);
            }
           
            if (crosspoint.distance(light_arr[j]->position) < 1e-5)
            {
                j++;
                continue;
            }
           

            int k = 0 , myflag=0;
            while (k != obj_arr.size())
            {
                /* code */
                GLdouble newtemp = obj_arr[k]->intersect(cc, 0, ray_light, max_level);
                if (newtemp > 0 && newtemp + 1e-5 < crosspoint.distance(light_arr[j]->position))
                {
                    myflag = 1;
                    break;
                }
                k++;
            }

            if (!myflag)
            {
                GLdouble lambert_value = (-1) * (ray_light.direction.dotProduct(ray_normal.direction));
                lambert_value = max(0.0, lambert_value);
                ray Reflect = ray_light.doReflection(crosspoint, ray_normal);
                GLdouble value = (-1) * rr.direction.dotProduct(Reflect.direction);
                value = pow(value, shine);
                value = max(0.0, value);
                cc.red = cc.red + light_arr[j]->c.red * co_efficient[1] * lambert_value * color.red + light_arr[j]->c.red * co_efficient[2] * value * color.red;
                cc.green = cc.green + light_arr[j]->c.green * co_efficient[1] * lambert_value * color.green + light_arr[j]->c.green * co_efficient[2] * value * color.green;
                cc.blue = cc.blue + light_arr[j]->c.blue * co_efficient[1] * lambert_value * color.blue + light_arr[j]->c.blue * co_efficient[2] * value * color.blue;
            }

            j++;
        }

        j = 0;
        while (j != spot_arr.size())
        {
            /* code */

            point dir_light = crosspoint - spot_arr[j]->light.position;
            dir_light.normalize();

            if (fabs(dir_light.dotAngle(spot_arr[j]->direction)) < spot_arr[j]->cut_off_angle)
            {
                ray ray_light = ray(spot_arr[j]->light.position, dir_light);
                point normal = (p2 - p1).crossProduct(p3 - p1);
                normal.normalize();
                ray ray_normal;
                ray_normal.origin = crosspoint;
                if (ray_light.direction.dotProduct(normal) >= 0)
                {
                    ray_normal.direction = normal;
                }
                else
                {
                    ray_normal.direction = normal * (-1);
                }
                
                if (crosspoint.distance(light_arr[j]->position) < 1e-5)
                {
                    j++;
                    continue;
                }
               
                int k = 0 , myflag=0;
                while (k != obj_arr.size())
                {
                    /* code */
                    GLdouble newtemp = obj_arr[k]->intersect(cc, 0, ray_light, max_level);
                    if (newtemp > 0 && newtemp + 1e-5 < crosspoint.distance(light_arr[j]->position))
                    {
                        myflag=1;
                        break;
                    }
                    k++;
                }

                if (!myflag)
                {
                    GLdouble lambert_value = (-1) * (ray_light.direction.dotProduct(ray_normal.direction));
                    lambert_value = max(0.0, lambert_value);
                    ray Reflect = ray_light.doReflection(crosspoint, ray_normal);
                    GLdouble value = (-1) * rr.direction.dotProduct(Reflect.direction);
                    value = pow(value, shine);
                    value = max(0.0, value);
                    cc.red = cc.red + spot_arr[j]->light.c.red * co_efficient[1] * lambert_value * color.red + spot_arr[j]->light.c.red * co_efficient[2] * value * color.red;
                    cc.green = cc.green + spot_arr[j]->light.c.green * co_efficient[1] * lambert_value * color.green + spot_arr[j]->light.c.green * co_efficient[2] * value * color.green;
                    cc.blue = cc.blue + spot_arr[j]->light.c.blue * co_efficient[1] * lambert_value * color.blue + spot_arr[j]->light.c.blue * co_efficient[2] * value * color.blue;
                }
            }

            j++;
        }

        if (level >= max_level)
        {
            return resultDistance;
        }
        point normal = (p2 - p1).crossProduct(p3 - p1);
        normal.normalize();
        ray ray_normal;
        ray_normal.origin = crosspoint;
        if (rr.direction.dotProduct(normal) >= 0)
        {
            ray_normal.direction = normal;
        }
        else
        {
            ray_normal.direction = normal * (-1);
        }
       
        ray Reflect = rr.doReflection(crosspoint, ray_normal);
        point move;
        move.x = Reflect.direction.x * 1e-5;
        move.y = Reflect.direction.y * 1e-5;
        move.z = Reflect.direction.z * 1e-5;
        Reflect.origin = Reflect.origin + move;

        GLint index_of_object = -20;
        GLdouble result = 1e9;
        colour trivial;
        j = 0;
        while (j != obj_arr.size())
        {
            /* code */
            GLdouble temp = obj_arr[j]->intersect(trivial, 0, Reflect, max_level);
            if (temp > 0)
            {
                if (temp < result)
                {
                    result = temp;
                    index_of_object = j;
                }
            }
            j++;
        }

        if (index_of_object != -20)
        {
            trivial.red = 0.0;
            trivial.blue = 0.0;
            trivial.green = 0.0;
            obj_arr[index_of_object]->intersect(trivial, level + 1, Reflect, max_level);
            trivial = trivial * co_efficient[3];
            cc = cc + trivial;
        }

        

        return resultDistance;
    }
};

class general : public object
{

public:
    GLdouble a, b, c, d, e, f, g, h, i, j;

    general(GLdouble A, GLdouble B, GLdouble C, GLdouble D, GLdouble E, GLdouble F, GLdouble G, GLdouble H, GLdouble I, GLdouble J)
    {
        a = A, b = B, c = C, d = D, e = E, f = F, g = G, h = H, i = I, j = J;
    }

    virtual GLdouble intersect(colour &cc, GLint level, ray rr, GLint max_level)
    {

        GLdouble coefficient1 = rr.direction.x * rr.direction.x * a + rr.direction.y * rr.direction.y * b + rr.direction.z * rr.direction.z * c + rr.direction.x * rr.direction.y * d + rr.direction.x * rr.direction.z * e + f * rr.direction.y * rr.direction.z;

        GLdouble coefficient2 = rr.origin.x * rr.direction.x * a * 2 + rr.origin.y * rr.direction.y * 2 * b + rr.origin.z * rr.direction.z * c * 2 + rr.origin.x * rr.direction.y * d + rr.direction.x * rr.origin.y * d + (rr.origin.x * rr.direction.z + rr.direction.x * rr.origin.z) * e + (rr.origin.y * rr.direction.z + rr.direction.y * rr.origin.z) * f + rr.direction.x * g + rr.direction.y * h + rr.direction.z * i;

        GLdouble coefficient3 = rr.origin.x * rr.origin.x * a + rr.origin.y * rr.origin.y * b + rr.origin.z * rr.origin.z * c + rr.origin.x * rr.origin.y * d + rr.origin.x * rr.origin.z * e + rr.origin.y * rr.origin.z * f + rr.origin.x * g + rr.origin.y * h + rr.origin.z * i + j;

        GLdouble discriminant = pow(coefficient2, 2) - 4 * coefficient1 * coefficient3;
        GLdouble resultDistance;
        bool flag1 = (fabs(coefficient1) < 1e-5) ? true : false;
        if (discriminant < 0)
        {
            // cout<<"--i am --"<<endl;
            return -1;
        }
        else if (flag1)
        {
            resultDistance = (-1) * (coefficient3 / coefficient2);
        }
        else
        {
            GLdouble option1 = (sqrt(discriminant) - coefficient2) / (2 * coefficient1);
            GLdouble option2 = (-sqrt(discriminant) - coefficient2) / (2 * coefficient1);
            GLdouble temp = max(option1, option2);
            option2 = min(option1, option2);
            option1 = temp;
            bool flag = false;
            if (option2 > 0)
            {
                point crosspoint = rr.origin + rr.direction * option2;
                flag = ((fabs(length) > 1e-5) && (crosspoint.x < point_ref.x || crosspoint.x > point_ref.x + length)) ? true : false;
                flag += ((fabs(width) > 1e-5) && (crosspoint.y < point_ref.y || crosspoint.y > point_ref.y + width)) ? true : false;
                flag += ((fabs(height) > 1e-5) && (crosspoint.z < point_ref.z || crosspoint.z > point_ref.z + height)) ? true : false;
                if (!flag)
                {
                    resultDistance = option2;
                }
            }
            if ((flag) && option1 > 0)
            {
                flag = false;

                point crosspoint = rr.origin + rr.direction * option1;
                flag = ((fabs(length) > 1e-5) && (crosspoint.x < point_ref.x || crosspoint.x > point_ref.x + length)) ? true : false;
                flag += ((fabs(width) > 1e-5) && (crosspoint.y < point_ref.y || crosspoint.y > point_ref.y + width)) ? true : false;
                flag += ((fabs(height) > 1e-5) && (crosspoint.z < point_ref.z || crosspoint.z > point_ref.z + height)) ? true : false;
                if (!flag)
                {
                    resultDistance = option1;
                }
            }
            if (flag)
            {
                // cout<<"--i am --"<<endl;
                return -1;
            }
        }
        // cout<<"--i am --"<<endl;

        if (!level)
        {
            return resultDistance;
        }

        point crosspoint = rr.origin + rr.direction * resultDistance;
        cc.red = color.red * co_efficient[0];
        cc.green = color.green * co_efficient[0];
        cc.blue = color.blue * co_efficient[0];

        int j = 0;
        while (j != light_arr.size())
        {
            /* code */
            point dir_light = crosspoint - light_arr[j]->position;
            dir_light.normalize();
            ray ray_light = ray(light_arr[j]->position, dir_light);
            point dir_temp;
            dir_temp.x = 2 * crosspoint.x * a + crosspoint.y * d + crosspoint.z * e + g;
            dir_temp.y = 2 * crosspoint.y * b + crosspoint.x * d + crosspoint.z * f + h;
            dir_temp.z = 2 * crosspoint.z * c + crosspoint.x * e + crosspoint.y * f + i;

            ray ray_normal = ray(crosspoint, dir_temp);
            if (crosspoint.distance(light_arr[j]->position) < 1e-5)
            {
                j++;
                continue;
            }
            // line 225

            int k = 0, myflag=0;
            while (k != obj_arr.size())
            {
                /* code */
                GLdouble newTemp = obj_arr[k]->intersect(cc, 0, ray_light, max_level);
                if (newTemp > 0 && newTemp + 1e-5 < crosspoint.distance(light_arr[j]->position))
                {
                    myflag=1;
                    break;
                }
                k++;
            }

            if (myflag == 0)
            {
                GLdouble lambert_value = (-1) * (ray_light.direction.dotProduct(ray_normal.direction));
                lambert_value = max(0.0, lambert_value);
                ray Reflect = ray_light.doReflection(crosspoint, ray_normal);
                GLdouble value = (-1) * rr.direction.dotProduct(Reflect.direction);
                value = pow(value, shine);
                value = max(0.0, value);
                cc.red = cc.red + light_arr[j]->c.red * co_efficient[1] * lambert_value * color.red + light_arr[j]->c.red * co_efficient[2] * value * color.red;
                cc.green = cc.green + light_arr[j]->c.green * co_efficient[1] * lambert_value * color.green + light_arr[j]->c.green * co_efficient[2] * value * color.green;
                cc.blue = cc.blue + light_arr[j]->c.blue * co_efficient[1] * lambert_value * color.blue + light_arr[j]->c.blue * co_efficient[2] * value * color.blue;
            }

            j++;
        }

        j = 0;
        while (j != spot_arr.size())
        {
            /* code */

            point dir_light = crosspoint - spot_arr[j]->light.position;
            dir_light.normalize();

            if (fabs(dir_light.dotAngle(spot_arr[j]->direction)) < spot_arr[j]->cut_off_angle)
            {
                ray ray_light = ray(spot_arr[j]->light.position, dir_light);

                point dir_temp;
                dir_temp.x = 2 * crosspoint.x * a + crosspoint.y * d + crosspoint.z * e + g;
                dir_temp.y = 2 * crosspoint.y * b + crosspoint.x * d + crosspoint.z * f + h;
                dir_temp.z = 2 * crosspoint.z * c + crosspoint.x * e + crosspoint.y * f + i;

                ray ray_normal = ray(crosspoint, dir_temp);
                if (crosspoint.distance(light_arr[j]->position) < 1e-5)
                {
                    j++;
                    continue;
                }
                

                int k = 0 , myflag=0;
                while (k != obj_arr.size())
                {
                    /* code */
                    GLdouble newtemp = obj_arr[k]->intersect(cc, 0, ray_light, max_level);
                    if (newtemp > 0 && newtemp + 1e-5 < crosspoint.distance(light_arr[j]->position))
                    {
                        myflag = 1;
                        break;
                    }
                    k++;
                }

                if (myflag == 0)
                {
                    GLdouble lambert_value = (-1) * (ray_light.direction.dotProduct(ray_normal.direction));
                    lambert_value = max(0.0, lambert_value);
                    ray Reflect = ray_light.doReflection(crosspoint, ray_normal);
                    GLdouble value = (-1) * rr.direction.dotProduct(Reflect.direction);
                    value = pow(value, shine);
                    value = max(0.0, value);
                    cc.red = cc.red + spot_arr[j]->light.c.red * co_efficient[1] * lambert_value * color.red + spot_arr[j]->light.c.red * co_efficient[2] * value * color.red;
                    cc.green = cc.green + spot_arr[j]->light.c.green * co_efficient[1] * lambert_value * color.green + spot_arr[j]->light.c.green * co_efficient[2] * value * color.green;
                    cc.blue = cc.blue + spot_arr[j]->light.c.blue * co_efficient[1] * lambert_value * color.blue + spot_arr[j]->light.c.blue * co_efficient[2] * value * color.blue;
                }
            }

            j++;
        }

        if (level >= max_level)
        {
            return resultDistance;
        }

        point dir_temp;
        dir_temp.x = 2 * crosspoint.x * a + crosspoint.y * d + crosspoint.z * e + g;
        dir_temp.y = 2 * crosspoint.y * b + crosspoint.x * d + crosspoint.z * f + h;
        dir_temp.z = 2 * crosspoint.z * c + crosspoint.x * e + crosspoint.y * f + i;

        ray ray_normal = ray(crosspoint, dir_temp);
        ray Reflect = rr.doReflection(crosspoint, ray_normal);
        point move;
        move.x = Reflect.direction.x * 1e-5;
        move.y = Reflect.direction.y * 1e-5;
        move.z = Reflect.direction.z * 1e-5;
        Reflect.origin = Reflect.origin + move;

        GLint index_of_object = -20;
        GLdouble result = 1e9;
        colour trivial;
        j = 0;
        while (j != obj_arr.size())
        {
            /* code */
            GLdouble temp = obj_arr[j]->intersect(trivial, 0, Reflect, max_level);
            if (temp > 0)
            {
                if (temp < result)
                {
                    result = temp;
                    index_of_object = j;
                }
            }
            j++;
        }

        if (index_of_object != -20)
        {
            trivial.red = 0.0;
            trivial.blue = 0.0;
            trivial.green = 0.0;
            obj_arr[index_of_object]->intersect(trivial, level + 1, Reflect, max_level);
            trivial = trivial * co_efficient[3];
            cc = cc + trivial;
        }

        

        return resultDistance;
    }
};

class Floor : public object
{
public:
    Floor(GLint x, GLint y)
    {
        point_ref.x = (-1) * (x / 2);
        point_ref.y = (-1) * (x / 2);
        point_ref.z = 0;
        length = y;
        co_efficient[0]=0.4;
        co_efficient[1]=0.2;
        co_efficient[2]=0.2;
        co_efficient[3]=0.2;
        color.red=color.blue=color.green=0;
        shine=1;
    }
    virtual void draw()
    {
        int p = 0, total_tile = (-1) * 2 * point_ref.x;
        total_tile /= length;
        while (p != total_tile)
        {
            /* code */
            int q = 0;
            while (q != total_tile)
            {
                /* code */
                bool flag = ((p + q) % 2 == 0) ? true : false;
                if (flag)
                {
                    glColor3d(0, 0, 0);
                }
                else
                {
                    glColor3d(1.0, 1.0, 1.0);
                }
                GLdouble tempx = point_ref.x + length * p;
                GLdouble tempy = point_ref.y + length * q;

                glBegin(GL_QUADS);
                glVertex3d(tempx + length, tempy, 0);
                glVertex3d(tempx + length, tempy + length, 0);
                glVertex3d(tempx, tempy + length, 0);
                glVertex3d(tempx, tempy, 0);
                glEnd();

                q++;
            }

            p++;
        }
    }

    bool boundaryCheck(point p)
    {
        if (p.x < point_ref.x)
            return false;
        if (p.x > abs(point_ref.x))
            return false;
        if (p.y < point_ref.y)
            return false;
        if (p.y > abs(point_ref.y))
            return false;
        return true;
    }
    virtual GLdouble intersect(colour &cc, GLint level, ray rr, GLint max_level)
    {
        
        GLdouble resultDistance;
        point plane_normal;
        plane_normal.x = 0;
        plane_normal.y = 0;
        plane_normal.z = 1;
        int round_value = round(rr.direction.dotProduct(plane_normal) * 100);
        bool flagg = round_value == 0 ? true : false;
        if (flagg)
        {
            // Because ray is parallel with floor
            return -1;
        }

        resultDistance = (-1) * (rr.origin.dotProduct(plane_normal) / (rr.direction.dotProduct(plane_normal)));
        point crosspoint = rr.origin + rr.direction * resultDistance;

        if (!boundaryCheck(crosspoint))
        {
            return -1;
        }

        if (!level)
        {
            return resultDistance;
        }
        GLint sum = (crosspoint.x - point_ref.x ) / length;
        sum += ((crosspoint.y - point_ref.y)/length);
        if (sum % 2 == 1)
        {
            color.red = 1;
            color.green = 1;
            color.blue = 1;
        }
        else
        {
            color.blue = 0;
            color.red = 0;
            color.green = 0;
        }

        cc.red = color.red * co_efficient[0];
        cc.green = color.green * co_efficient[0];
        cc.blue = color.blue * co_efficient[0];

        int j = 0;
        while (j != light_arr.size())
        {
            /* code */
            point dir_light = crosspoint - light_arr[j]->position;
            dir_light.normalize();
            ray ray_light = ray(light_arr[j]->position, dir_light);
            point p;
            if (ray_light.direction.z > 0)
                p.z = 1;
            else
                p.z = -1;
            ray ray_normal = ray(crosspoint, p);
            if (crosspoint.distance(light_arr[j]->position) < 1e-5)
            {
                j++;
                continue;
            }
            

            int k = 0 , myflag = 0;
            while (k != obj_arr.size())
            {
                /* code */
                GLdouble newTemp = obj_arr[k]->intersect(cc, 0, ray_light, max_level);
                if (newTemp> 0 && newTemp + 1e-5 < crosspoint.distance(light_arr[j]->position))
                {
                    myflag = 1;
                    break;
                }
                k++;
            }

            if (myflag == 0)
            {
                GLdouble lambert_value = (-1) * (ray_light.direction.dotProduct(ray_normal.direction));
                lambert_value = max(0.0, lambert_value);
                ray Reflect = ray_light.doReflection(crosspoint, ray_normal);
                GLdouble value = (-1) * rr.direction.dotProduct(Reflect.direction);
                value = pow(value, shine);
                value = max(0.0, value);
                cc.red = cc.red + light_arr[j]->c.red * co_efficient[1] * lambert_value * color.red + light_arr[j]->c.red * co_efficient[2] * value * color.red;
                cc.green = cc.green + light_arr[j]->c.green * co_efficient[1] * lambert_value * color.green + light_arr[j]->c.green * co_efficient[2] * value * color.green;
                cc.blue = cc.blue + light_arr[j]->c.blue * co_efficient[1] * lambert_value * color.blue + light_arr[j]->c.blue * co_efficient[2] * value * color.blue;
            }

            j++;
        }

        j = 0;
        while (j != spot_arr.size())
        {
            /* code */

            point dir_light = crosspoint - spot_arr[j]->light.position;
            dir_light.normalize();

            if (fabs(dir_light.dotAngle(spot_arr[j]->direction)) < spot_arr[j]->cut_off_angle)
            {
                ray ray_light = ray(spot_arr[j]->light.position, dir_light);

                point p;
                if (ray_light.direction.z > 0)
                    p.z = 1;
                else
                    p.z = -1;
                ray ray_normal = ray(crosspoint, p);

                if (crosspoint.distance(light_arr[j]->position) < 1e-5)
                {
                    j++;
                    continue;
                }
                

                int k = 0 , myflag =0;
                while (k != obj_arr.size())
                {
                    /* code */
                    GLdouble newtemp = obj_arr[k]->intersect(cc, 0, ray_light, max_level);
                    if (newtemp > 0 && newtemp + 1e-5 < crosspoint.distance(light_arr[j]->position))
                    {
                        myflag = 1;
                        break;
                    }
                    k++;
                }

                if (myflag == 0)
                {
                    GLdouble lambert_value = (-1) * (ray_light.direction.dotProduct(ray_normal.direction));
                    lambert_value = max(0.0, lambert_value);
                    ray Reflect = ray_light.doReflection(crosspoint, ray_normal);
                    GLdouble value = (-1) * rr.direction.dotProduct(Reflect.direction);
                    value = pow(value, shine);
                    value = max(0.0, value);
                    cc.red = cc.red + spot_arr[j]->light.c.red * co_efficient[1] * lambert_value * color.red + spot_arr[j]->light.c.red * co_efficient[2] * value * color.red;
                    cc.green = cc.green + spot_arr[j]->light.c.green * co_efficient[1] * lambert_value * color.green + spot_arr[j]->light.c.green * co_efficient[2] * value * color.green;
                    cc.blue = cc.blue + spot_arr[j]->light.c.blue * co_efficient[1] * lambert_value * color.blue + spot_arr[j]->light.c.blue * co_efficient[2] * value * color.blue;
                }
            }

            j++;
        }

        if (level >= max_level)
        {
            return resultDistance;
        }

        point p;
        if (rr.direction.z > 0)
            p.z = 1;
        else
            p.z = -1;
        ray ray_normal = ray(crosspoint, p);

        ray Reflect = rr.doReflection(crosspoint, ray_normal);
        point move;
        move.x = Reflect.direction.x * 1e-5;
        move.y = Reflect.direction.y * 1e-5;
        move.z = Reflect.direction.z * 1e-5;
        Reflect.origin = Reflect.origin + move;

        GLint index_of_object = -20;
        GLdouble result = 1e9;
        colour trivial;
        j = 0;
        while (j != obj_arr.size())
        {
            /* code */
            GLdouble temp = obj_arr[j]->intersect(trivial, 0, Reflect, max_level);
            if (temp > 0)
            {
                if (temp < result)
                {
                    result = temp;
                    index_of_object = j;
                }
            }
            j++;
        }

        if (index_of_object != -20)
        {
            trivial.red = 0.0;
            trivial.blue = 0.0;
            trivial.green = 0.0;
            obj_arr[index_of_object]->intersect(trivial, level + 1, Reflect, max_level);
            trivial = trivial * co_efficient[3];
            cc = cc + trivial;
        }

        

        return resultDistance;
    }
};