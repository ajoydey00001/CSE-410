#include <bits/stdc++.h>
#include <GL/glut.h>

#include "1905038_Header.h"
#include "bitmap_image.hpp"
#include <windows.h>


using namespace std;

point position;
point look_vector;
point right_dir;
point up_dir;

GLint depth_of_recursion, height_img, width_img, count_picture;
bitmap_image picture;

vector<object *> obj_arr;
vector<pointLight *> light_arr;
vector<spotlight *> spot_arr;

void drawAxes()
{

    glLineWidth(3);
    glBegin(GL_LINES);
    {
        glColor3f(1, 0, 0);

        glVertex3f(-90, 0, 0);
        glVertex3f(90, 0, 0);

        glColor3f(0, 1, 0);

        glVertex3f(0, -90, 0);
        glVertex3f(0, 90, 0);

        glColor3f(0, 0, 1);

        glVertex3f(0, 0, 90);
        glVertex3f(0, 0, -90);
    }
    glEnd();
}

void loadData()
{
    ifstream input("scene.txt");
    input >> depth_of_recursion >> height_img;
    int temp;
    GLint shine;
    colour temp_c;
    GLdouble arrr[4];
    point temp_p;

    input >> temp;

    int j = 0;
    object *obb;
    while (j != temp)
    {
        /* code */
        string str;
        input >> str;
        if (str == "sphere")
        {
            input >> temp_p.x >> temp_p.y >> temp_p.z;
            GLdouble rrr;
            input >> rrr;
            obb = new sphere(temp_p, rrr);
        }
        else if (str == "triangle")
        {
            point tt1, tt2, tt3;
            input >> tt1.x >> tt1.y >> tt1.z;
            input >> tt2.x >> tt2.y >> tt2.z;
            input >> tt3.x >> tt3.y >> tt3.z;
            obb = new triangle(tt1, tt2, tt3);
        }
        else if (str == "general")
        {
            GLdouble a, b, c, d, e, f, g, h, i, j;
            input >> a >> b >> c >> d >> e >> f >> g >> h >> i >> j;
            obb = new general(a, b, c, d, e, f, g, h, i, j);
            input>>obb->point_ref.x>>obb->point_ref.y>>obb->point_ref.z>>obb->length>>obb->width>>obb->height;

        }
        else{
            cout<<"not found "<<j<<" "<<temp<<endl;
            return;
        }

        input >> temp_c.red >> temp_c.green >> temp_c.blue;
        obb->setColour(temp_c);
        input >> arrr[0] >> arrr[1] >> arrr[2] >> arrr[3];
        obb->setCoefficients(arrr);
        input >> shine;
        obb->setShine(shine);

        obj_arr.push_back(obb);
        j++;
    }

    obb = new Floor(420,10);
    obj_arr.push_back(obb);

    j = 0;
    input >> temp;

    while (j != temp)
    {
        /* code */
        pointLight *ll = new pointLight();
        input >> ll->position.x >> ll->position.y >> ll->position.z;
        input >> ll->c.red >> ll->c.green >> ll->c.blue;
        light_arr.push_back(ll);

        j++;
    }

    j = 0;
    input >> temp;

    while (j != temp)
    {
        /* code */
        spotlight *ll = new spotlight();
        input >> ll->light.position.x >> ll->light.position.y >> ll->light.position.z;
        input >> ll->light.c.red >> ll->light.c.green >> ll->light.c.blue;
        input >> ll->direction.x >> ll->direction.y >> ll->direction.z;
        input >> ll->cut_off_angle;
        spot_arr.push_back(ll);

        j++;
    }

    width_img = height_img;
    picture = bitmap_image(width_img, height_img);
}

void initGL()
{

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    loadData();
}

void capture()
{

    GLdouble angle_of_view = 40, width_win = 500, height_win = 500;

    int j = 0;
    while (j != width_img)
    {
        /* code */
        int i = 0;
        while (i != height_img)
        {
            /* code */
            picture.set_pixel(j, i, 0, 0, 0);
            i++;
        }

        j++;
    }

    GLdouble distance_of_plane = height_win / (2.0 * tan((M_PI * angle_of_view) / (2 * 180.0)));

    GLdouble du = width_win / (width_img * 1.0);
    GLdouble dv = height_win / (height_img * 1.0);
    point left_top = position + look_vector * distance_of_plane;
    left_top = left_top + up_dir * height_win * 0.5;
    left_top = left_top - right_dir * width_win * 0.5;
    left_top = left_top + right_dir * (du / 2.0);
    left_top = left_top - up_dir * (dv / 2.0);

    int i = 0;
    while (i != width_img)
    {
        /* code */
        int j = 0;
        while (j != height_img)
        {
            /* code */
            point temp1 = right_dir * (i * du);
            point temp2 = up_dir * (j * dv);
            ray rtemp(position, left_top + temp1 - temp2 - position);
            colour resultColour;
            GLdouble result = 1e9;
            GLint index = -200;
            int k = 0;
            while (k != obj_arr.size())
            {
                /* code */
                GLdouble d = obj_arr[k]->intersect(resultColour, 0, rtemp, depth_of_recursion);

                if (d > 0)
                {
                    if (d < result)
                    {
                        index = k;
                        result = d;
                    }
                }
                k++;
            }

            if (index != -200)
            {
                obj_arr[index]->intersect(resultColour, 1, rtemp, depth_of_recursion);
                resultColour.clip();
                picture.set_pixel(i,j, 255 * resultColour.red, 255 * resultColour.green, 255 * resultColour.blue);
            }

            j++;
        }

        i++;
    }

    // cout<<"flag1 : "<<flag1<<endl;

    picture.save_image("Output_1" + to_string(count_picture) + ".bmp");
    cout<<count_picture<<"th capturing picture is done "<<endl;
    count_picture++;
}

void display()
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW); 
    glLoadIdentity();

    gluLookAt(position.x, position.y, position.z,
              position.x + look_vector.x, position.y + look_vector.y, position.z + look_vector.z,
              up_dir.x, up_dir.y, up_dir.z);

    // drawAxes();

    int j = 0;
    while (j != obj_arr.size())
    {
        /* code */
        obj_arr[j]->draw();

        j++;
    }

    j = 0;
    while (j != light_arr.size())
    {
        /* code */
        light_arr[j]->draw();
        j++;
    }
    j = 0;
    while (j != spot_arr.size())
    {
        /* code */
        spot_arr[j]->draw();
        j++;
    }

    glutSwapBuffers();
}

void reshape(GLsizei width, GLsizei height)
{
    if (height == 0)
        height = 1;
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40, 1, 0.1, 1000);
}

void listen_keyboard(unsigned char key, int key_x, int key_y)
{
    GLdouble angle = 0.01; // in radian
    GLdouble rad;
    GLdouble temp;
    GLdouble ball_m_rad, time_1, time_2;
    point na;

    switch (key)
    {
    case '0':
        cout<<"Starting capture ----------------"<<endl;
        capture();
        break;
    case '1':
        /* code */
        {

            point rr1((right_dir.x * cos(angle)) + (look_vector.x * sin(angle)), (right_dir.y * cos(angle)) + (look_vector.y * sin(angle)), (right_dir.z * cos(angle)) + (look_vector.z * sin(angle)));
            right_dir = rr1;

            point ll1(look_vector.x * cos(angle) - right_dir.x * sin(angle), look_vector.y * cos(angle) - right_dir.y * sin(angle), look_vector.z * cos(angle) - right_dir.z * sin(angle));
            look_vector = ll1;
        }

        break;
    case '2':
        /* code */
        {

            point rr(right_dir.x * cos(-angle) + look_vector.x * sin(-angle), right_dir.y * cos(-angle) + look_vector.y * sin(-angle), right_dir.z * cos(-angle) + look_vector.z * sin(-angle));
            right_dir = rr;

            point ll(look_vector.x * cos(-angle) - right_dir.x * sin(-angle), look_vector.y * cos(-angle) - right_dir.y * sin(-angle), look_vector.z * cos(-angle) - right_dir.z * sin(-angle));
            look_vector = ll;
        }
        break;

    case '3':
    {

        point ll(look_vector.x * cos(angle) + up_dir.x * sin(angle), look_vector.y * cos(angle) + up_dir.y * sin(angle), look_vector.z * cos(angle) + up_dir.z * sin(angle));
        look_vector = ll;

        point up(up_dir.x * cos(angle) - look_vector.x * sin(angle), up_dir.y * cos(angle) - look_vector.y * sin(angle), up_dir.z * cos(angle) - look_vector.z * sin(angle));
        up_dir = up;
    }
    break;
    case '4':
    {

        point ll(look_vector.x * cos(-angle) + up_dir.x * sin(-angle), look_vector.y * cos(-angle) + up_dir.y * sin(-angle), look_vector.z * cos(-angle) + up_dir.z * sin(-angle));
        look_vector = ll;

        point up(up_dir.x * cos(-angle) - look_vector.x * sin(-angle), up_dir.y * cos(-angle) - look_vector.y * sin(-angle), up_dir.z * cos(-angle) - look_vector.z * sin(-angle));
        up_dir = up;
    }
    break;
    case '5':
    {

        point up(up_dir.x * cos(angle) + right_dir.x * sin(angle), up_dir.y * cos(angle) + right_dir.y * sin(angle), up_dir.z * cos(angle) + right_dir.z * sin(angle));
        up_dir = up;

        point rd(right_dir.x * cos(angle) - up_dir.x * sin(angle), right_dir.y * cos(angle) - up_dir.y * sin(angle), right_dir.z * cos(angle) - up_dir.z * sin(angle));
        right_dir = rd;
    }
    break;
    case '6':
    {

        point up(up_dir.x * cos(-angle) + right_dir.x * sin(-angle), up_dir.y * cos(-angle) + right_dir.y * sin(-angle), up_dir.z * cos(-angle) + right_dir.z * sin(-angle));
        up_dir = up;

        point rd(right_dir.x * cos(-angle) - up_dir.x * sin(-angle), right_dir.y * cos(-angle) - up_dir.y * sin(-angle), right_dir.z * cos(-angle) - up_dir.z * sin(-angle));
        right_dir = rd;
    }
    break;

    case 27:
        exit(0);
        break;

    default:
        break;
    }

    glutPostRedisplay();
}

void listen_special(int key, int key_x, int key_y)
{
    switch (key)
    {
    case GLUT_KEY_UP:

    {
        point pp(position.x + look_vector.x, position.y + look_vector.y, position.z + look_vector.z);
        position = pp;
    }

    break;

    case GLUT_KEY_DOWN:

    {
        point pp(position.x - look_vector.x, position.y - look_vector.y, position.z - look_vector.z);
        position = pp;
    }

    break;
    case GLUT_KEY_RIGHT:

    {
        point pp(position.x + right_dir.x, position.y + right_dir.y, position.z + right_dir.z);
        position = pp;
    }

    break;
    case GLUT_KEY_LEFT:
    {
        point pp(position.x - right_dir.x, position.y - right_dir.y, position.z - right_dir.z);
        position = pp;
    }

    break;

    case GLUT_KEY_PAGE_UP:

    {
        point pp(position.x + up_dir.x, position.y + up_dir.y, position.z + up_dir.z);
        position = pp;
    }

    break;
    case GLUT_KEY_PAGE_DOWN:
    {
        point pp(position.x - up_dir.x, position.y - up_dir.y, position.z - up_dir.z);
        position = pp;
    }

    break;

    default:
        break;
    }

    glutPostRedisplay();
}

int main(int argc, char **argv)
{

    

    position.x = 200;
    position.y = 0;
    position.z = 10;
    look_vector.x = (-1)/sqrt(2);
    look_vector.y = (-1)/sqrt(2);
    look_vector.z = 0;
    up_dir.x = 0;
    up_dir.y = 0;
    up_dir.z = 1;
    right_dir.x = (-1)/sqrt(2);
    right_dir.y = 1/sqrt(2);
    right_dir.z = 0;

    count_picture = 1;

    glutInit(&argc, argv);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(50, 50);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
    glutCreateWindow("Ray Tracing");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutKeyboardFunc(listen_keyboard);
    glutSpecialFunc(listen_special);

    initGL();
    
    glutMainLoop();

    obj_arr.clear();
    light_arr.clear();
    spot_arr.clear();

    return 0;
}
