#define _USE_MATH_DEFINES

#include <windows.h>
#include <GL/glut.h>
#include <cmath>
#include <iostream>
#include <queue>
#include <windows.h>

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
    GLdouble dotAngle(point p)
    {

        GLdouble temp = x * p.x + y * p.y + z * p.z;
        GLdouble k = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
        GLdouble l = sqrt(pow(p.x, 2) + pow(p.y, 2) + pow(p.z, 2));
        return acos(temp / (k * l));
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
    point point_on_circular_arc(point p, int i, int segment, int axis)
    {
        GLdouble r = modulus();
        GLdouble rad = dotAngle(p);
        point cross = crossProduct(p);
        GLdouble div_rad = (i * rad) / segment;
        point temp;
        if (axis == 3)
        {
            GLdouble g = x + p.x + (y + p.y) * (-cross.x / cross.y);
            temp.x = pow(r, 2) * ((cos(div_rad) + cos(rad - div_rad)) / g);
            GLdouble h = y + p.y + (x + p.x) * (-cross.y / cross.x);
            temp.y = pow(r, 2) * ((cos(div_rad) + cos(rad - div_rad)) / h);
            temp.z = pow(r, 2) * ((cos(div_rad) - cos(rad - div_rad)) / (z - p.z));
        }
        else if (axis == 2)
        {
            GLdouble g = x + p.x + (z + p.z) * (-cross.x / cross.z);
            temp.x = pow(r, 2) * ((cos(div_rad) + cos(rad - div_rad)) / g);
            GLdouble h = z + p.z + (x + p.x) * (-cross.z / cross.x);
            temp.z = pow(r, 2) * ((cos(div_rad) + cos(rad - div_rad)) / h);
            temp.y = pow(r, 2) * ((cos(div_rad) - cos(rad - div_rad)) / (y - p.y));
        }
        else
        {
            GLdouble g = z + p.z + (y + p.y) * (-cross.z / cross.y);
            temp.z = pow(r, 2) * ((cos(div_rad) + cos(rad - div_rad)) / g);
            GLdouble h = y + p.y + (z + p.z) * (-cross.y / cross.z);
            temp.y = pow(r, 2) * ((cos(div_rad) + cos(rad - div_rad)) / h);
            temp.x = pow(r, 2) * ((cos(div_rad) - cos(rad - div_rad)) / (x - p.x));
        }
        return temp;
    }
};

point position;
point look_vector;
point right_dir;
point up_dir;
point ball_center;
point ball_m_dir;
point ball_p_dir;
int arrow_rot, ball_rot, colusion_count, time_count, timer_value;
GLdouble ball_radius, boundary_length, clock_time, rtime, increment, velocity;

bool simulation_state;

point points[200][365];

void initGL()
{

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
}

void drawAxes()
{

    glLineWidth(3);
    glBegin(GL_LINES);
    {
        glColor3f(1, 0, 0); 
       
        glVertex3f(-25, 0, 0);
        glVertex3f(25, 0, 0);

        glColor3f(0, 1, 0); 
        
        glVertex3f(0, -25, 0);
        glVertex3f(0, 25, 0);
    }
    glEnd();
}

void initial_sphere(int vertical, int horizontal, GLdouble r)
{
    GLdouble angle1, angle2, radius;

    int j = 0;
    while (j < vertical + 1)
    {
        angle1 = (-M_PI / 2.0) + (j * M_PI) / vertical;
        radius = r * cos(angle1);

        int i = 0;
        while (i < horizontal + 1)
        {
            angle2 = (i * 2.0 * M_PI) / horizontal;
            points[j][i].x = radius * cos(angle2) + ball_center.x;
            points[j][i].y = radius * sin(angle2) + ball_center.y;
            points[j][i].z = r * sin(angle1);
            i++;
        }

        j++;
    }
}

void draw_sphere_points(int stacks, int slices)
{
    boolean flag1 = true;
    boolean flag2 = true;
    glBegin(GL_QUADS);
    for (int j = 0; j < stacks; j++)
    {
        flag1 = flag2;

        if (j == stacks / 2 - 1)
        {
            flag2 = !flag2;
        }
        for (int i = 0; i < slices; i++)
        {

            if (flag1)
            {
                glColor3f(1, 0, 0);
            }
            else
            {
                glColor3f(0, 1, 0);
            }
            flag1 = !flag1;
            glVertex3f(points[j][i].x + ball_center.x, points[j][i].y + ball_center.y, points[j][i].z);
            glVertex3f(points[j][i + 1].x + ball_center.x, points[j][i + 1].y + ball_center.y, points[j][i + 1].z);

            glVertex3f(points[j + 1][i + 1].x + ball_center.x, points[j + 1][i + 1].y + ball_center.y, points[j + 1][i + 1].z);
            glVertex3f(points[j + 1][i].x + ball_center.x, points[j + 1][i].y + ball_center.y, points[j + 1][i].z);
        }
    }
    glEnd();
}

void drawCheckerboard()
{

    boolean flag1 = true;
    boolean flag2 = true;

    for (int i = -25; i < 25; i += 1)
    {
        flag1 = flag2;
        flag2 = !flag2;
        for (int j = -25; j < 25; j += 1)
        {

            if (flag1)
            {
                glColor3f(0, 0, 0);
            }
            else
            {
                glColor3f(1, 1, 1);
            }
            flag1 = !flag1;
            glBegin(GL_QUADS);
            {
                glVertex3f(i, j, 0);
                glVertex3f(i + 1, j, 0);
                glVertex3f(i + 1, j + 1, 0);
                glVertex3f(i, j + 1, 0);
            }
            glEnd();
        }
    }
}

void drawBoundary(int i)
{

    glBegin(GL_QUADS);
    {

        glColor3f(1, 0, 0);
        glVertex3f(-i, -i, 0);
        glVertex3f(-i, -i, 1);
        glVertex3f(-i, i, 1);
        glVertex3f(-i, i, 0);

        glVertex3f(i, -i, 0);
        glVertex3f(i, -i, 1);
        glVertex3f(i, i, 1);
        glVertex3f(i, i, 0);

        glVertex3f(-i, -i, 0);
        glVertex3f(-i, -i, 1);
        glVertex3f(i, -i, 1);
        glVertex3f(i, -i, 0);

        glVertex3f(-i, i, 0);
        glVertex3f(-i, i, 1);
        glVertex3f(i, i, 1);
        glVertex3f(i, i, 0);
    }
    glEnd();
}

void draw_Cone(GLdouble r, GLdouble h)
{
    GLdouble tx = r, ty = 0, cx, cy;
    glColor3f(0, 0, 1);
    glBegin(GL_TRIANGLES);
    {
        for (int j = 1; j <= 360; j++)
        {
            GLdouble angle = (2.0 * M_PI * j) / 360;
            cy = r * sin(angle);
            cx = r * cos(angle);

            glVertex3f(0, 0, h);
            glVertex3f(cx, cy, 0);
            glVertex3f(tx, ty, 0);
            tx = cx;
            ty = cy;
        }
    }
    glEnd();
    glBegin(GL_POLYGON);
    {
        for (int j = 1; j <= 360; j++)
        {
            GLdouble angle = (2.0 * M_PI * j) / 360;
            cy = r * sin(angle);
            cx = r * cos(angle);
            glVertex3f(cx, cy, 0);
        }
    }
    glEnd();
}

void draw_Cylinder(GLdouble r, GLdouble h)
{
    GLdouble tx = r, ty = 0, cx, cy;
    glColor3f(0, 0, 1);
    glBegin(GL_QUADS);
    {
        for (int j = 1; j <= 360; j++)
        {
            GLdouble angle = (2.0 * M_PI * j) / 360;
            cy = r * sin(angle);
            cx = r * cos(angle);

            glVertex3f(cx, cy, h);
            glVertex3f(cx, cy, 0);
            glVertex3f(tx, ty, 0);
            glVertex3f(tx, ty, h);
            tx = cx;
            ty = cy;
        }
    }
    glEnd();
}

void rolling_sphere(GLdouble angle, int stacks, int slices)
{
    point axis;
    axis.x = axis.y = 0;
    axis.z = 1;
    ball_p_dir = axis.crossProduct(ball_m_dir);
    // ball_p_dir = ball_m_dir.crossProduct(axis);
    angle = (M_PI * angle) / 180;
    for (int i = 0; i <= stacks; i++)
    {

        for (int j = 0; j <= slices; j++)
        {
            point temp = points[i][j] * cos(angle);
            GLdouble k = (1 - cos(angle)) * (points[i][j].dotProduct(ball_p_dir));
            temp = temp + ball_p_dir * k;
            point ee = ball_p_dir.crossProduct(points[i][j]);
            points[i][j] = temp + ee * sin(angle);
        }
    }
}

void display()
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW); // it is a matrix
    glLoadIdentity();

    gluLookAt(position.x, position.y, position.z,
              position.x + look_vector.x, position.y + look_vector.y, position.z + look_vector.z,
              up_dir.x, up_dir.y, up_dir.z);

    drawAxes();
    drawCheckerboard();
    drawBoundary(boundary_length);

    glPushMatrix();
    glTranslatef(0, 0, 1);
    glPushMatrix();

    draw_sphere_points(8, 8);
    glPushMatrix();

    glTranslatef(ball_center.x, ball_center.y, 0);
    glRotatef(arrow_rot, 0, 0, 1);
    glTranslatef(0.4, 0, 0);

    glRotatef(90, 0, 1, 0);
    draw_Cylinder(0.05, 0.3);
    glTranslatef(0, 0, 0.3);
    draw_Cone(0.1, 0.3);
    glPopMatrix();
    glPopMatrix();

    glutSwapBuffers();
}

void Timer(int value)
{

    if (value == timer_value || value == timer_value + 1)
    {
        ball_m_dir = ball_m_dir * (1 / sqrt(ball_m_dir.dotProduct(ball_m_dir)));

        if (simulation_state)
        {
            GLdouble rad;
            GLdouble temp;
            GLdouble ball_m_rad;
            point na;

            if (value == timer_value + 1)
            {
                rad = (ball_rot * M_PI) / 180;

                // ball_center.x = ball_center.x + (ball_radius * rad) * ball_m_dir.x;
                // cout << ball_radius * rad << endl;
                if (ball_center.x + ball_radius > boundary_length)
                {
                    ball_center.x = boundary_length - ball_radius;
                    // ball_m_dir.x *= (-1);

                    na = point(-1, 0, 0);

                    ball_m_dir = ball_m_dir + na * (-2 * ball_m_dir.dotProduct(na));
                    ball_m_dir = ball_m_dir * (1 / sqrt((ball_m_dir.dotProduct(ball_m_dir))));
                    // std::cout<<ball_m_dir.x<<" "<<ball_m_dir.y<<std::endl;///////////////
                    ball_m_rad = atan(ball_m_dir.y / ball_m_dir.x);
                    if (ball_m_dir.x < 0)
                        ball_m_rad += M_PI;
                    arrow_rot = ball_m_rad * 180 / M_PI;

                    // std::cout<<ball_center.x<<std::endl;
                }
                if (ball_center.x - ball_radius < (-1) * boundary_length)
                {
                    ball_center.x = boundary_length * (-1) + ball_radius;
                    na = point(1, 0, 0);

                    ball_m_dir = ball_m_dir + na * (-2 * ball_m_dir.dotProduct(na));
                    ball_m_dir = ball_m_dir * (1 / sqrt((ball_m_dir.dotProduct(ball_m_dir))));

                    ball_m_rad = atan(ball_m_dir.y / ball_m_dir.x);
                    if (ball_m_dir.x < 0)
                        ball_m_rad += M_PI;
                    arrow_rot = ball_m_rad * 180 / M_PI;
                    // std::cout<<ball_center.x<<std::endl;
                }
                // ball_center.y = ball_center.y + (ball_radius * rad) * ball_m_dir.y;
                if (ball_center.y + ball_radius > boundary_length)
                {
                    ball_center.y = boundary_length - ball_radius;
                    na = point(0, -1, 0);

                    ball_m_dir = ball_m_dir + na * (-2 * ball_m_dir.dotProduct(na));
                    ball_m_dir = ball_m_dir * (1 / sqrt((ball_m_dir.dotProduct(ball_m_dir))));

                    ball_m_rad = atan(ball_m_dir.y / ball_m_dir.x);
                    if (ball_m_dir.x < 0)
                        ball_m_rad += M_PI;
                    arrow_rot = ball_m_rad * 180 / M_PI;
                    // std::cout<<ball_center.y<<std::endl;
                }
                else if (ball_center.y - ball_radius < (-1) * boundary_length)
                {
                    ball_center.y = boundary_length * (-1) + ball_radius;
                    na = point(0, 1, 0);

                    ball_m_dir = ball_m_dir + na * (-2 * ball_m_dir.dotProduct(na));
                    ball_m_dir = ball_m_dir * (1 / sqrt((ball_m_dir.dotProduct(ball_m_dir))));

                    ball_m_rad = atan(ball_m_dir.y / ball_m_dir.x);
                    if (ball_m_dir.x < 0)
                        ball_m_rad += M_PI;
                    arrow_rot = ball_m_rad * 180 / M_PI;
                    // std::cout<<ball_center.y<<std::endl;
                }
                // std::cout<<ball_center.x<<" "<<ball_center.y<<std::endl;
                rolling_sphere(ball_rot, 8, 8);
                point v = ball_m_dir * velocity;
                GLdouble time_1 = 0.0, time_2 = 0.0;

                // std::cout<<ball_center.x<<" "<<ball_center.y<<std::endl;
                if (v.x > 0)
                {

                    time_1 = ((boundary_length - ball_radius - ball_center.x)) / v.x;
                }
                else if (v.x < 0)
                {
                    time_1 = ((ball_center.x + boundary_length - ball_radius)) / ((-1) * v.x);
                }
                else
                {
                    time_1 = -1;
                }
                if (v.y > 0)
                {
                    time_2 = ((boundary_length - ball_radius - ball_center.y)) / v.y;
                }
                else if (v.y < 0)
                {
                    time_2 = ((boundary_length - ball_radius + ball_center.y)) / ((-1) * v.y);
                }
                else
                {
                    time_2 = -1;
                }
                // cout << "t1 " << time_1 << "  t2 : " << time_2 << endl;
                if (time_1 < 0)
                {
                    time_1 = time_2;
                }
                else if (time_2 >= 0)
                {
                    time_1 = time_1 < time_2 ? time_1 : time_2;
                }
                int uh = (int)time_1;
                int hh = (int)rtime;
                uh = uh / hh;
                time_1 = 1.0 * uh * hh;
                time_count = uh;
                // time_count = uh -1;
                glutPostRedisplay();
                // Sleep(10000);
                if (time_1 > 1000)
                    time_1 = time_1 - 0;
                glutTimerFunc(time_1, Timer, timer_value + 1);
                // cout << "Prediction :  " << time_1 << endl;

                // Sleep(1);
                glutTimerFunc(rtime, Timer, timer_value);
            }
            else if (value == timer_value)
            {

                // Sleep(50);
                time_count--;

                rad = (ball_rot * M_PI) / 180;

                ball_center.x = ball_center.x + (ball_radius * rad) * ball_m_dir.x;

                ball_center.y = ball_center.y + (ball_radius * rad) * ball_m_dir.y;

                rolling_sphere(ball_rot, 8, 8);

                glutPostRedisplay();
                if (time_count == 0)
                {
                    //  Sleep(200);
                    //  glutTimerFunc(rtime, Timer, 0);
                    // cout << "time count 0" << endl;
                    ball_center.x = ball_center.x + (ball_radius * rad) * ball_m_dir.x;

                    ball_center.y = ball_center.y + (ball_radius * rad) * ball_m_dir.y;
                }
                if (time_count != 0)
                {
                    Sleep(45);
                    glutTimerFunc(rtime, Timer, timer_value);
                }
            }
        }
    }
}

void reshape(GLsizei width, GLsizei height)
{
    if (height == 0)
        height = 1;
    // GLdouble aspect = (GLdouble)width / (GLdouble)height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, (GLdouble)width / (GLdouble)height, 0.1, 100);
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

    case 'j':
        if (simulation_state)
        {
            timer_value += 2;
            arrow_rot = (arrow_rot + 15) % 360;
            rad = (arrow_rot * M_PI) / 180;

            ball_m_dir.x = cos(rad);
            ball_m_dir.y = sin(rad);

            point v = ball_m_dir * velocity;
            time_1 = 0.0, time_2 = 0.0;
            if (v.x > 0)
            {
                time_1 = ((boundary_length - ball_radius - ball_center.x)) / v.x;
            }
            else if (v.x < 0)
            {
                time_1 = ((ball_center.x + boundary_length - ball_radius)) / ((-1) * v.x);
            }
            else
            {
                time_1 = -1;
            }
            if (v.y > 0)
            {
                time_2 = ((boundary_length - ball_radius - ball_center.y)) / v.y;
            }
            else if (v.y < 0)
            {
                time_2 = ((boundary_length - ball_radius + ball_center.y)) / ((-1) * v.y);
            }
            else
            {
                time_2 = -1;
            }
            // cout << "t1 " << time_1 << "  t2 : " << time_2 << endl;
            if (time_1 < 0)
            {
                time_1 = time_2;
            }
            else if (time_2 >= 0)
            {
                time_1 = time_1 < time_2 ? time_1 : time_2;
            }
            int uh = (int)time_1;
            int hh = (int)rtime;
            uh = uh / hh;
            time_1 = 1.0 * uh * hh;
            time_count = uh - 1;

            glutTimerFunc(time_1, Timer, timer_value + 1);
            // cout << "Prediction :  " << time_1 << endl;
            glutTimerFunc(rtime, Timer, timer_value);
        }
        else
        {
            arrow_rot = (arrow_rot + 5) % 360;
            rad = (arrow_rot * M_PI) / 180;

            ball_m_dir.x = cos(rad);
            ball_m_dir.y = sin(rad);
        }

        break;
    case 'l':
        if (simulation_state)
        {
            timer_value += 2;
            arrow_rot = (arrow_rot - 15 + 360) % 360;
            rad = (arrow_rot * M_PI) / 180;

            ball_m_dir.x = cos(rad);
            ball_m_dir.y = sin(rad);

            point v = ball_m_dir * velocity;
            time_1 = 0.0, time_2 = 0.0;
            if (v.x > 0)
            {
                time_1 = ((boundary_length - ball_radius - ball_center.x)) / v.x;
            }
            else if (v.x < 0)
            {
                time_1 = ((ball_center.x + boundary_length - ball_radius)) / ((-1) * v.x);
            }
            else
            {
                time_1 = -1;
            }
            if (v.y > 0)
            {
                time_2 = ((boundary_length - ball_radius - ball_center.y)) / v.y;
            }
            else if (v.y < 0)
            {
                time_2 = ((boundary_length - ball_radius + ball_center.y)) / ((-1) * v.y);
            }
            else
            {
                time_2 = -1;
            }
            // cout << "t1 " << time_1 << "  t2 : " << time_2 << endl;
            if (time_1 < 0)
            {
                time_1 = time_2;
            }
            else if (time_2 >= 0)
            {
                time_1 = time_1 < time_2 ? time_1 : time_2;
            }
            int uh = (int)time_1;
            int hh = (int)rtime;
            uh = uh / hh;
            time_1 = 1.0 * uh * hh;
            time_count = uh - 1;

            glutTimerFunc(time_1, Timer, timer_value + 1);
            // cout << "Prediction :  " << time_1 << endl;
            glutTimerFunc(rtime, Timer, timer_value);
        }
        else
        {
            arrow_rot = (arrow_rot - 5 + 360) % 360;
            rad = (arrow_rot * M_PI) / 180;

            ball_m_dir.x = cos(rad);
            ball_m_dir.y = sin(rad);
        }

        break;
    case 'i':

        if (!simulation_state)
        {
            rad = (ball_rot * M_PI) / 180;

            ball_center.x = ball_center.x + (ball_radius * rad) * ball_m_dir.x;
            // cout << ball_radius * rad << endl;
            if (ball_center.x + ball_radius > boundary_length)
            {
                ball_center.x = boundary_length - ball_radius;
                // ball_m_dir.x *= (-1);

                na = point(-1, 0, 0);

                ball_m_dir = ball_m_dir + na * (-2 * ball_m_dir.dotProduct(na));
                ball_m_dir = ball_m_dir * (1 / sqrt((ball_m_dir.dotProduct(ball_m_dir))));
                // std::cout<<ball_m_dir.x<<" "<<ball_m_dir.y<<std::endl;///////////////
                ball_m_rad = atan(ball_m_dir.y / ball_m_dir.x);
                if (ball_m_dir.x < 0)
                    ball_m_rad += M_PI;
                arrow_rot = ball_m_rad * 180 / M_PI;

                // std::cout<<ball_center.x<<std::endl;
            }
            if (ball_center.x - ball_radius < (-1) * boundary_length)
            {
                ball_center.x = boundary_length * (-1) + ball_radius;
                na = point(1, 0, 0);

                ball_m_dir = ball_m_dir + na * (-2 * ball_m_dir.dotProduct(na));
                ball_m_dir = ball_m_dir * (1 / sqrt((ball_m_dir.dotProduct(ball_m_dir))));

                ball_m_rad = atan(ball_m_dir.y / ball_m_dir.x);
                if (ball_m_dir.x < 0)
                    ball_m_rad += M_PI;
                arrow_rot = ball_m_rad * 180 / M_PI;
                // std::cout<<ball_center.x<<std::endl;
            }
            ball_center.y = ball_center.y + (ball_radius * rad) * ball_m_dir.y;
            if (ball_center.y + ball_radius > boundary_length)
            {
                ball_center.y = boundary_length - ball_radius;
                na = point(0, -1, 0);

                ball_m_dir = ball_m_dir + na * (-2 * ball_m_dir.dotProduct(na));
                ball_m_dir = ball_m_dir * (1 / sqrt((ball_m_dir.dotProduct(ball_m_dir))));

                ball_m_rad = atan(ball_m_dir.y / ball_m_dir.x);
                if (ball_m_dir.x < 0)
                    ball_m_rad += M_PI;
                arrow_rot = ball_m_rad * 180 / M_PI;
                // std::cout<<ball_center.y<<std::endl;
            }
            else if (ball_center.y - ball_radius < (-1) * boundary_length)
            {
                ball_center.y = boundary_length * (-1) + ball_radius;
                na = point(0, 1, 0);

                ball_m_dir = ball_m_dir + na * (-2 * ball_m_dir.dotProduct(na));
                ball_m_dir = ball_m_dir * (1 / sqrt((ball_m_dir.dotProduct(ball_m_dir))));

                ball_m_rad = atan(ball_m_dir.y / ball_m_dir.x);
                if (ball_m_dir.x < 0)
                    ball_m_rad += M_PI;
                arrow_rot = ball_m_rad * 180 / M_PI;
                // std::cout<<ball_center.y<<std::endl;
            }
            // std::cout<<ball_center.x<<" "<<ball_center.y<<std::endl;
            rolling_sphere(ball_rot, 8, 8);
        }
        break;
    case 'k':

        if (!simulation_state)
        {
            rad = (ball_rot * M_PI) / 180;

            ball_center.x = ball_center.x - (ball_radius * rad) * ball_m_dir.x;
            if (ball_center.x + ball_radius > boundary_length)
            {
                ball_center.x = boundary_length - ball_radius;
                point na = point(-1, 0, 0);

                ball_m_dir = ball_m_dir + na * (-2 * ball_m_dir.dotProduct(na));
                ball_m_dir = ball_m_dir * (1 / sqrt((ball_m_dir.dotProduct(ball_m_dir))));

                ball_m_rad = atan(ball_m_dir.y / ball_m_dir.x);
                if (ball_m_dir.x < 0)
                    ball_m_rad += M_PI;
                arrow_rot = ball_m_rad * 180 / M_PI;
                // std::cout<<ball_center.x<<std::endl;
            }
            else if (ball_center.x - ball_radius < (-1) * boundary_length)
            {
                ball_center.x = boundary_length * (-1) + ball_radius;
                na = point(1, 0, 0);

                ball_m_dir = ball_m_dir + na * (-2 * ball_m_dir.dotProduct(na));
                ball_m_dir = ball_m_dir * (1 / sqrt((ball_m_dir.dotProduct(ball_m_dir))));

                ball_m_rad = atan(ball_m_dir.y / ball_m_dir.x);
                if (ball_m_dir.x < 0)
                    ball_m_rad += M_PI;
                arrow_rot = ball_m_rad * 180 / M_PI;
                // std::cout<<ball_center.x<<std::endl;
            }
            ball_center.y = ball_center.y - (ball_radius * rad) * ball_m_dir.y;
            if (ball_center.y + ball_radius > boundary_length)
            {
                ball_center.y = boundary_length - ball_radius;
                na = point(0, -1, 0);

                ball_m_dir = ball_m_dir + na * (-2 * ball_m_dir.dotProduct(na));
                ball_m_dir = ball_m_dir * (1 / sqrt((ball_m_dir.dotProduct(ball_m_dir))));

                ball_m_rad = atan(ball_m_dir.y / ball_m_dir.x);
                if (ball_m_dir.x < 0)
                    ball_m_rad += M_PI;
                arrow_rot = ball_m_rad * 180 / M_PI;
                // std::cout<<ball_center.y<<std::endl;
            }
            else if (ball_center.y - ball_radius < (-1) * boundary_length)
            {
                ball_center.y = boundary_length * (-1) + ball_radius;
                na = point(0, 1, 0);

                ball_m_dir = ball_m_dir + na * (-2 * ball_m_dir.dotProduct(na));
                ball_m_dir = ball_m_dir * (1 / sqrt((ball_m_dir.dotProduct(ball_m_dir))));

                ball_m_rad = atan(ball_m_dir.y / ball_m_dir.x);
                if (ball_m_dir.x < 0)
                    ball_m_rad += M_PI;
                arrow_rot = ball_m_rad * 180 / M_PI;
                // std::cout<<ball_center.y<<std::endl;
            }
            // std::cout<<ball_center.x<<" "<<ball_center.y<<std::endl;
            rolling_sphere(-1 * ball_rot, 8, 8);
        }
        break;
    case ' ':
        if (simulation_state)
        {
            simulation_state = false;
            time_count = 0;
            timer_value = -2;
        }
        else
        {
            timer_value = 0;
            simulation_state = true;
            // clock_time = 0.0;
            point v = ball_m_dir * velocity;
            GLdouble time_1 = 0.0, time_2 = 0.0;
            if (v.x > 0)
            {
                time_1 = ((boundary_length - ball_radius - ball_center.x)) / v.x;
            }
            else if (v.x < 0)
            {
                time_1 = ((ball_center.x + boundary_length - ball_radius)) / ((-1) * v.x);
            }
            else
            {
                time_1 = -1;
            }
            if (v.y > 0)
            {
                time_2 = ((boundary_length - ball_radius - ball_center.y)) / v.y;
            }
            else if (v.y < 0)
            {
                time_2 = ((boundary_length - ball_radius + ball_center.y)) / ((-1) * v.y);
            }
            else
            {
                time_2 = -1;
            }
            // cout << "t1 " << time_1 << "  t2 : " << time_2 << endl;
            if (time_1 < 0)
            {
                time_1 = time_2;
            }
            else if (time_2 >= 0)
            {
                time_1 = time_1 < time_2 ? time_1 : time_2;
            }
            int uh = (int)time_1;
            int hh = (int)rtime;
            uh = uh / hh;
            time_1 = 1.0 * uh * hh;
            time_count = uh - 1;

            glutTimerFunc(time_1, Timer, 1);
            // cout << "Prediction :  " << time_1 << endl;
            glutTimerFunc(rtime, Timer, 0);
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

    position.x = 0;
    position.y = 0;
    position.z = 20;
    look_vector.x = 0;
    look_vector.y = 0;
    look_vector.z = -1;
    up_dir.x = 0;
    up_dir.y = 1;
    up_dir.z = 0;
    right_dir.x = 1;
    right_dir.y = 0;
    right_dir.z = 0;

    ball_center.x = ball_center.y = ball_center.z = 0;
    ball_m_dir.x = 1, ball_m_dir.y = 0, ball_m_dir.z = 0;
    ball_p_dir.x = 0, ball_p_dir.y = 1, ball_p_dir.z = 0;
    arrow_rot = 0;
    ball_radius = 0.5;
    ball_rot = 30;
    boundary_length = 6;
    clock_time = 0.0;
    simulation_state = false;
    colusion_count = 0;
    rtime = 50;
    increment = 0.261799;
    velocity = increment / rtime;
    time_count = 0;
    timer_value = 0;

    glutInit(&argc, argv);
    glutInitWindowSize(800, 640);
    glutInitWindowPosition(50, 50);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
    glutCreateWindow("task 2 ");
    initial_sphere(8, 8, ball_radius);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutKeyboardFunc(listen_keyboard);
    glutSpecialFunc(listen_special);

    initGL();
    // glutIdleFunc(idle);
    //  glutTimerFunc(50,Timer,0);
    glutMainLoop();
    return 0;
}
