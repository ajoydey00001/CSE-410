#include <bits/stdc++.h>
#include "bitmap_image.hpp"

using namespace std;

static unsigned long int seedForRandom = 1;
int generateRandomNumber()
{
    seedForRandom *= 214013;
    seedForRandom += 2531011;
    seedForRandom = seedForRandom >> 16;
    return seedForRandom & 0x7FFF;
}

float Top_Y, Bottom_Y, Right_X, Left_X;

class point
{
public:
    float x, y, z;
    point()
    {
        x = 0.0;
        y = 0.0;
        z = 0.0;
    }
    point(float a, float b, float c)
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
    float dotProduct(point p)
    {
        float temp;
        temp = x * p.x;
        temp += y * p.y;
        temp += z * p.z;
        return temp;
    }

    point operator*(float k)
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

    float modulus()
    {
        return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
    }

    point operator=(point p)
    {
        x = p.x;
        y = p.y;
        z = p.z;
        point temp(x, y, z);
        return temp;
    }
    void normalize()
    {
        float r = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
        x /= r;
        y /= r;
        z /= r;
    }
};
point EYE, LOOK, UP, c1, c2, c3;
float tx, ty, tz, n_plane, f_plane, Ratio, fovy;

class MATRIX
{

public:
    float arr[4][4];
    int r, c;

    MATRIX(string s)
    {

        r = c = 4;

        if (s == "allzeros")
        {
            int k = 0;
            while (k != 4)
            {
                /* code */
                int j = 0;
                while (j != 4)
                {
                    /* code */
                    arr[k][j] = 0.0;
                    j++;
                }

                k++;
            }
        }
        else
        {
            int k = 0;
            while (k != 4)
            {
                /* code */
                int j = 0;
                while (j != 4)
                {
                    /* code */
                    if (k == j)
                    {
                        arr[k][j] = 1.0;
                    }
                    else
                    {
                        arr[k][j] = 0.0;
                    }
                    j++;
                }

                k++;
            }

            if (s == "doTranslation")
            {
                arr[0][3] = tx;
                arr[1][3] = ty;
                arr[2][3] = tz;
            }
            else if (s == "doScale")
            {
                arr[0][0] = tx;
                arr[1][1] = ty;
                arr[2][2] = tz;
            }
            else if (s == "doRotation")
            {
                arr[0][0] = c1.x;
                arr[1][0] = c1.y;
                arr[2][0] = c1.z;

                arr[0][1] = c2.x;
                arr[1][1] = c2.y;
                arr[2][1] = c2.z;

                arr[0][2] = c3.x;
                arr[1][2] = c3.y;
                arr[2][2] = c3.z;
            }
           
        }
    }
    point convertToPoint()
    {
        point p;
        p.x = arr[0][0];
        p.y = arr[1][0];
        p.z = arr[2][0];
        return p;
    }

    MATRIX do_multiplication(MATRIX m)
    {
        MATRIX temp("allzeros");
        int i = 0;
        while (i != r)
        {
            /* code */
            int j = 0;
            while (j != c)
            {
                /* code */
                int k = 0;
                while (k != c)
                {
                    /* code */
                    temp.arr[i][j] = temp.arr[i][j] + arr[i][k] * m.arr[k][j];
                    k++;
                }
                if (temp.arr[i][j] > -0.0000001 && temp.arr[i][j] < 0.0000001)
                {
                    temp.arr[i][j] = 0.0;
                }

                j++;
            }

            i++;
        }
        return temp;
    }

    MATRIX point_transform(MATRIX m)
    {
        MATRIX temp("allzeros");

        int i = 0;
        while (i != r)
        {
            /* code */
            int j = 0;
            while (j != 1)
            {
                /* code */
                int k = 0;
                while (k != r)
                {
                    /* code */
                    temp.arr[i][j] = temp.arr[i][j] + arr[i][k] * m.arr[k][j];
                    k++;
                }

                j++;
            }

            i++;
        }
        if (temp.arr[3][0] != 1)
        {
            temp.arr[0][0] /= temp.arr[3][0];
            temp.arr[1][0] /= temp.arr[3][0];
            temp.arr[2][0] /= temp.arr[3][0];
            temp.arr[3][0] = 1;
        }

        return temp;
    }

    MATRIX view_trans()
    {
        point vec_l(LOOK.x - EYE.x, LOOK.y - EYE.y, LOOK.z - EYE.z);
        vec_l.normalize();
        point vec_r = vec_l.crossProduct(UP);
        vec_r.normalize();
        point vec_u = vec_r.crossProduct(vec_l);
        tx = -EYE.x;
        ty = -EYE.y;
        tz = -EYE.z;
        MATRIX T("doTranslation");
        c1.x = vec_r.x;
        c1.y = vec_u.x;
        c1.z = -vec_l.x;
        c2.x = vec_r.y;
        c2.y = vec_u.y;
        c2.z = -vec_l.y;
        c3.x = vec_r.z;
        c3.y = vec_u.z;
        c3.z = -vec_l.z;

        MATRIX V("doRotate");
        return V.do_multiplication(T);
    }

    MATRIX projection()
    {
        MATRIX pmatrix("allzeros");
        float fovx = Ratio * fovy;
        float t = (tan((fovy * M_PI) / (2 * 180.0))) * n_plane;
        float rr = (tan((fovx * M_PI) / (2 * 180.0))) * n_plane;
        pmatrix.arr[0][0] = (n_plane / rr);
        pmatrix.arr[1][1] = (n_plane / t);
        pmatrix.arr[2][2] = -(f_plane + n_plane) / (f_plane - n_plane);
        pmatrix.arr[2][3] = -(2 * f_plane * n_plane) / (f_plane - n_plane);
        pmatrix.arr[3][2] = -1;
        pmatrix.arr[3][3] = 0;
        return pmatrix;
    }
};

point apply_rodriguos(point p, point axis, float angle)
{
    angle = (M_PI * angle) / 180;
    point temp = p * cos(angle);
    float k = (1 - cos(angle)) * (p.dotProduct(axis));
    temp = temp + axis * k;
    point ee = axis.crossProduct(p);
    return temp + ee * sin(angle);
}

float get_max_y(vector<point> v)
{
    float temp = max(v[1].y, v[2].y);
    temp = max(temp, v[0].y);
    if (temp > Top_Y)
    {
        return Top_Y;
    }
    return temp;
}

float get_min_y(vector<point> v)
{
    float temp = min(v[1].y, v[2].y);
    temp = min(temp, v[0].y);
    if (temp < Bottom_Y)
    {
        return Bottom_Y;
    }
    return temp;
}

float get_max_x(vector<point> v)
{
    float temp = max(v[1].x, v[2].x);
    temp = max(temp, v[0].x);
    if (temp > Right_X)
    {
        return Right_X;
    }
    return temp;
}

float get_min_x(vector<point> v)
{
    float temp = min(v[1].x, v[2].x);
    temp = min(temp, v[0].x);
    if (temp < Left_X)
    {
        return Left_X;
    }
    return temp;
}

int main()
{

    ifstream input("scene.txt");
    if (!input.is_open())
    {
        cout << "Error in opening input file" << endl;
        return 1;
    }
    ofstream output1("stage1.txt");
    output1 << fixed << setprecision(7);

    ofstream output2("stage2.txt");
    output2 << fixed << setprecision(7);
    ofstream output3("stage3.txt");
    output3 << fixed << setprecision(7);
    ofstream outbuffer("z_buffer.txt");
    outbuffer << fixed << setprecision(6);

    input >> EYE.x >> EYE.y >> EYE.z;
    input >> LOOK.x >> LOOK.y >> LOOK.z;
    input >> UP.x >> UP.y >> UP.z;

    input >> fovy >> Ratio >> n_plane >> f_plane;

    MATRIX state_cur("identity");
    stack<MATRIX> stack_matrix;
    stack_matrix.push(state_cur);

    MATRIX mat_view = state_cur.view_trans();
    MATRIX mat_projection = state_cur.projection();

    vector<vector<point>> all_triangles;

    string str;

    for (;;)
    {
        input >> str;
        if (str == "translate")
        {
            input >> tx >> ty >> tz;
            MATRIX matrix_translation("doTranslation");
            state_cur = state_cur.do_multiplication(matrix_translation);
        }
        else if (str == "scale")
        {
            input >> tx >> ty >> tz;
            MATRIX matrix_scale("doScale");
            state_cur = state_cur.do_multiplication(matrix_scale);
        }
        else if (str == "rotate")
        {
            float theta;
            point p;
            input >> theta >> p.x >> p.y >> p.z;
            p.normalize();
            point a(1, 0, 0), b(0, 1, 0), c(0, 0, 1);
            c1 = apply_rodriguos(a, p, theta);
            c2 = apply_rodriguos(b, p, theta);
            c3 = apply_rodriguos(c, p, theta);
            MATRIX matrix_rot("doRotation");
            state_cur = state_cur.do_multiplication(matrix_rot);
        }
        else if (str == "push")
        {
            stack_matrix.push(state_cur);
        }
        else if (str == "pop")
        {
            state_cur = stack_matrix.top();
            stack_matrix.pop();
        }
        else if (str == "end")
        {

            break;
        }
        else if (str == "triangle")
        {

            MATRIX p1("allzeros"), p2("allzeros"), p3("allzeros");
            input >> p1.arr[0][0] >> p1.arr[1][0] >> p1.arr[2][0];
            p1.arr[3][0] = 1;
            input >> p2.arr[0][0] >> p2.arr[1][0] >> p2.arr[2][0];
            p2.arr[3][0] = 1;
            input >> p3.arr[0][0] >> p3.arr[1][0] >> p3.arr[2][0];
            p3.arr[3][0] = 1;
            p1 = state_cur.point_transform(p1);
            output1 << p1.arr[0][0] << " " << p1.arr[1][0] << " " << p1.arr[2][0] << endl;
            p2 = state_cur.point_transform(p2);
            output1 << p2.arr[0][0] << " " << p2.arr[1][0] << " " << p2.arr[2][0] << endl;
            p3 = state_cur.point_transform(p3);
            output1 << p3.arr[0][0] << " " << p3.arr[1][0] << " " << p3.arr[2][0] << endl
                    << endl;

            p1 = mat_view.point_transform(p1);
            output2 << p1.arr[0][0] << " " << p1.arr[1][0] << " " << p1.arr[2][0] << endl;
            p2 = mat_view.point_transform(p2);
            output2 << p2.arr[0][0] << " " << p2.arr[1][0] << " " << p2.arr[2][0] << endl;
            p3 = mat_view.point_transform(p3);
            output2 << p3.arr[0][0] << " " << p3.arr[1][0] << " " << p3.arr[2][0] << endl
                    << endl;

            p1 = mat_projection.point_transform(p1);
            output3 << p1.arr[0][0] << " " << p1.arr[1][0] << " " << p1.arr[2][0] << endl;
            p2 = mat_projection.point_transform(p2);
            output3 << p2.arr[0][0] << " " << p2.arr[1][0] << " " << p2.arr[2][0] << endl;
            p3 = mat_projection.point_transform(p3);
            output3 << p3.arr[0][0] << " " << p3.arr[1][0] << " " << p3.arr[2][0] << endl
                    << endl;

            point gp1 = p1.convertToPoint();
            point gp2 = p2.convertToPoint();
            point gp3 = p3.convertToPoint();

            if (gp2.y == gp1.y)
            {
                point yy = gp1;
                gp1 = gp3;
                gp3 = yy;
            }
            else if (gp3.y == gp1.y)
            {
                point yy = gp1;
                gp1 = gp2;
                gp2 = yy;
            }
            if (gp3.x < gp2.x)
            {
                point yy = gp2;
                gp2 = gp3;
                gp3 = yy;
            }

            vector<point> vec;
            vec.push_back(gp1);
            vec.push_back(gp2);
            vec.push_back(gp3);

            point color;
            color.x = generateRandomNumber() % 256;
            color.y = generateRandomNumber() % 256;
            color.z = generateRandomNumber() % 256;
            vec.push_back(color);
            all_triangles.push_back(vec);
        }
    }

    input.close();
    output1.close();
    output2.close();
    output3.close();

    ifstream inputfile("config.txt");
    if (!inputfile.is_open())
    {
        cout << "Error in opening input file 2" <<endl;
        return 1;
    }
    int width, height;
    inputfile >> width >> height;
    inputfile.close();

    float buffer_of_z[width][height];
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            buffer_of_z[i][j] = 2.0;
        }
    }
    bitmap_image plot(width, height);
    plot.set_all_channels(0, 0, 0);

    float left_limit = -1, right_limit = 1, top_limit = 1, bottom_limit = -1;
    float dx = (right_limit - left_limit) / width;
    float dy = (top_limit - bottom_limit) / height;

    Left_X = left_limit + (dx / 2.0);
    Right_X = right_limit - (dx / 2.0);
    Top_Y = top_limit - (dy / 2.0);
    Bottom_Y = bottom_limit + (dy / 2.0);

    int j = 0;
    while (j != all_triangles.size())
    {
        
        float xMax = get_max_x(all_triangles[j]);
        float xMin = get_min_x(all_triangles[j]);
        float yMax = get_max_y(all_triangles[j]);
        float yMin = get_min_y(all_triangles[j]);
        
        
        int number_of_row = ceil((Top_Y - yMax) / dy);
      
        while (number_of_row <= (int)((Top_Y - yMin) / dy))
        {
            /* code */
            float y_p = Top_Y - number_of_row * dy;
            float temp = (y_p - all_triangles[j][0].y) / (all_triangles[j][1].y - all_triangles[j][0].y);
            float z_a = temp * (all_triangles[j][1].z - all_triangles[j][0].z) + all_triangles[j][0].z;
            float x_a = temp * (all_triangles[j][1].x - all_triangles[j][0].x) + all_triangles[j][0].x;
            x_a = (xMin > x_a) ? xMin : x_a;

            temp = (y_p - all_triangles[j][0].y) / (all_triangles[j][1].y - all_triangles[j][0].y);
            float z_b = temp * (all_triangles[j][2].z - all_triangles[j][0].z) + all_triangles[j][0].z;
            float x_b = temp * (all_triangles[j][2].x - all_triangles[j][0].x) + all_triangles[j][0].x;
            x_b = (x_b > xMax) ? xMax : x_b;

            int number_of_column = round((x_a - Left_X) / dx);

            
            while (number_of_column <= round((x_b - Left_X) / dx))
            {
                
                float x_p = Left_X + number_of_column * dx;
                temp = (x_p - x_a) / (x_b - x_a);
                float z_p = temp * (z_b - z_a) + z_a;

                if (z_p - 1 > 0.0)
                    {
                        number_of_column++;
                        continue;
                    }
                if (z_p + 1 < 0.0)
                    {
                        number_of_column++;
                        continue;
                    }

                
                switch (buffer_of_z[number_of_row][number_of_column] > z_p)
                {
                case true:
                    /* code */
                    buffer_of_z[number_of_row][number_of_column] = z_p;
                    plot.set_pixel(number_of_column, number_of_row, all_triangles[j][3].x, all_triangles[j][3].y, all_triangles[j][3].z);

                    break;

                default:
                    break;
                }

                


                number_of_column++;
            }

            number_of_row++;
        }
       

        j++;
    }

    
    int g = 0;
    while (g != height)
    {
        /* code */
        int h = 0;
        while (h != width)
        {
            /* code */
            switch (2.0 > buffer_of_z[g][h])
            {
            case true:
                /* code */
                outbuffer << buffer_of_z[g][h] << "\t";
                break;

            default:
                break;
            }
            
            h++;
        }
        outbuffer << endl;

        g++;
    }

    outbuffer.close();
    
    plot.save_image("out.bmp");

   

    return 0;
}
