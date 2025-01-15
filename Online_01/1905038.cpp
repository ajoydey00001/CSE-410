#define _USE_MATH_DEFINES

#include <windows.h>
#include <GL/glut.h>
#include <cmath>

#define PI acos(-1.0)
#define degToRad(x) (x * PI / 180.0)
#define radToDeg(x) (x * 180.0 / PI)

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 640




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

void initGL()
{

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
}

GLdouble lenght;
GLdouble decrement;

GLdouble rotation;

void DRAW_tri_with_colour(int i, int j, int k)
{
    glPushMatrix();

    GLdouble d = (1 - lenght) / 3.0;
    glTranslated(d, d, d);

    glBegin(GL_TRIANGLES);
    {
        glColor3f(i, j, k);

        glVertex3f(lenght, 0, 0);
        glVertex3f(0, lenght, 0);
        glVertex3f(0, 0, lenght);
    }
    glEnd();

    glPopMatrix();
}

void DRAW_Octahedron()
{
    glPushMatrix();
    DRAW_tri_with_colour(0, 1, 1);
    glPopMatrix();
    glPushMatrix();
    glRotatef(180, 1, 0, 0);
    DRAW_tri_with_colour(0, 1, 1);
    glPopMatrix();

    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    DRAW_tri_with_colour(1, 0, 1);
    glPopMatrix();
    glPushMatrix();
    glRotatef(180, 1, 0, 0);
    glRotatef(90, 0, 1, 0);
    DRAW_tri_with_colour(1, 0, 1);
    glPopMatrix();

    glPushMatrix();
    glRotatef(180, 0, 1, 0);
    DRAW_tri_with_colour(0, 1, 1);
    glPopMatrix();
    glPushMatrix();
    glRotatef(180, 1, 0, 0);
    glRotatef(180, 0, 1, 0);
    DRAW_tri_with_colour(0, 1, 1);
    glPopMatrix();

    glPushMatrix();
    glRotatef(270, 0, 1, 0);
    DRAW_tri_with_colour(1, 0, 1);
    glPopMatrix();
    glPushMatrix();
    glRotatef(180, 1, 0, 0);
    glRotatef(270, 0, 1, 0);
    DRAW_tri_with_colour(1, 0, 1);
    glPopMatrix();
}

void segment_of_cylinder(int n)
{

    point p1(lenght + decrement, decrement, decrement), p2(lenght + decrement, decrement, -decrement), p3(decrement, lenght + decrement, decrement), p4(decrement, lenght + decrement, -decrement);

    point t1 = p1;
    point t2 = p3;

    int j = 1;
    while (j != n + 1)
    {
        /* code */
        point current_1 = p1.point_on_circular_arc(p2, j, n, 3);
        point current_2 = p3.point_on_circular_arc(p4, j, n, 3);

        glColor3f(1, 1, 0);
        glBegin(GL_QUADS);
        {
            glVertex3f(t1.x, t1.y, t1.z);
            glVertex3f(current_1.x, current_1.y, current_1.z);
            glVertex3f(current_2.x, current_2.y, current_2.z);
            glVertex3f(t2.x, t2.y, t2.z);

            t1 = current_1;
            t2 = current_2;
        }
        glEnd();

        j++;
    }
}

void DRAWING_cylinder()
{

    segment_of_cylinder(15);

    glPushMatrix();
    glRotatef(90, 1, 0, 0);
    segment_of_cylinder(15);
    glPopMatrix();

    glPushMatrix();
    glRotatef(180, 1, 0, 0);
    segment_of_cylinder(15);
    glPopMatrix();

    glPushMatrix();
    glRotatef(270, 1, 0, 0);
    segment_of_cylinder(15);
    glPopMatrix();

    glPushMatrix();
    glRotatef(90, 0, 0, 1);
    segment_of_cylinder(15);
    glPopMatrix();

    glPushMatrix();

    glRotatef(90, -1, 0, 0);
    glRotatef(90, 0, 0, 1);
    segment_of_cylinder(15);
    glPopMatrix();

    glPushMatrix();

    glRotatef(180, -1, 0, 0);
    glRotatef(90, 0, 0, 1);
    segment_of_cylinder(15);
    glPopMatrix();

    glPushMatrix();

    glRotatef(270, -1, 0, 0);
    glRotatef(90, 0, 0, 1);
    segment_of_cylinder(15);
    glPopMatrix();

    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    segment_of_cylinder(15);
    glPopMatrix();

    glPushMatrix();
    glRotatef(-90, 0, 1, 0);
    segment_of_cylinder(15);
    glPopMatrix();

    glPushMatrix();
    glRotatef(90, 0, -1, 0);
    glRotatef(-90, 0, 0, 1);
    segment_of_cylinder(15);
    glPopMatrix();

    glPushMatrix();
    glRotatef(270, 0, -1, 0);
    glRotatef(-90, 0, 0, 1);
    segment_of_cylinder(15);
    glPopMatrix();
}

void segment_of_sphere(GLdouble n)
{

    point fp1(lenght + decrement, decrement, decrement), fp2(lenght + decrement, decrement, -decrement), fp3(lenght + decrement, -decrement, decrement), fp4(lenght + decrement, -decrement, -decrement);
    point p1, p2, p3, p4;
    p1 = fp1;

    for (int j = 1; j <= n; j++)
    {

        p4 = fp1.point_on_circular_arc(fp3, j, n, 2);
        point curr1 = p1, curr4 = p4;
        point t1(p1.x, p1.y, (-1) * p1.z), t4(p4.x, p4.y, (-1) * p4.z);

        for (int i = 1; i <= n; i++)
        {
            p2 = curr1.point_on_circular_arc(t1, i, n, 3);
            p3 = curr4.point_on_circular_arc(t4, i, n, 3);

            glBegin(GL_QUADS);
            {
                glVertex3f(p1.x, p1.y, p1.z);
                glVertex3f(p2.x, p2.y, p2.z);
                glVertex3f(p3.x, p3.y, p3.z);
                glVertex3f(p4.x, p4.y, p4.z);
            }
            glEnd();

            p1 = p2;
            p4 = p3;
        }

        p1 = curr4;
    }
}

void DRAWING_sphere()
{
    glColor3f(0, 0, 1);
    segment_of_sphere(50);

    glPushMatrix();
    glRotatef(90, 0, 0, 1);
    glColor3f(0, 1, 0);
    segment_of_sphere(50);
    glPopMatrix();

    glPushMatrix();
    glRotatef(180, 0, 0, 1);
    glColor3f(0, 0, 1);
    segment_of_sphere(50);
    glPopMatrix();

    glPushMatrix();
    glRotatef(270, 0, 0, 1);
    glColor3f(0, 1, 0);
    segment_of_sphere(50);
    glPopMatrix();

    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glColor3f(1, 0, 0);
    segment_of_sphere(50);
    glPopMatrix();

    glPushMatrix();
    glRotatef(270, 0, 1, 0);
    glColor3f(1, 0, 0);
    segment_of_sphere(50);
    glPopMatrix();
}

void drawAxes() {
    glLineWidth(1);
    glBegin(GL_LINES);
        glColor3f(1,1,1);   // Red
        // X axis
        glVertex3f(3,0,0);
        glVertex3f(-3,0,0);

        
        // Y axis
        glVertex3f(0,3,0);
        glVertex3f(0,-3,0);

        
        // Z axis
        glVertex3f(0,0,3);
        glVertex3f(0,0,-3);
    glEnd();
}



void drawCircle(double radius, point center){
    glBegin(GL_LINE_LOOP);
    for(int i=0; i<360; i++){
        double angle = degToRad(i);
        double x = center.x + radius*cos(angle);
        double y = center.y + radius*sin(angle);
        glVertex2d(x, y);
    }
    glEnd();
}

void drawSquare() {
    glBegin(GL_QUADS);
        {
            glVertex3f(0.1,0.1,-0.1);
            glVertex3f(-0.1,0.1,-0.1);
            glVertex3f(-0.1,-0.1,-0.1);
            glVertex3f(0.1,-0.1,-0.1);

        }
    glEnd();
}

void drawLine(){
    glBegin(GL_LINES);
    {
        glVertex3f(-0.1,0,0);
    }
    glEnd();
}

double anglee=10;
void display()
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW); // it is a matrix
    glLoadIdentity();

    gluLookAt(position.x, position.y, position.z,
              position.x + look_vector.x, position.y + look_vector.y, position.z + look_vector.z,
              up_dir.x, up_dir.y, up_dir.z);

    // glRotatef(rotation, 0, 1, 0);

    // DRAW_Octahedron();
    // DRAWING_cylinder();
    // DRAWING_sphere();

    drawAxes();
    point p;
    p.x=p.y=p.z=0;
    

    glRotatef(anglee,0,0,1);

    drawCircle(1,p);
    glColor3f(1,0,0);
    glPushMatrix();
    glTranslatef(1,0,0);
    drawSquare();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1,0,0);
    drawSquare();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,1,0);
    drawSquare();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,-1,0);
    drawSquare();
    glPopMatrix();
    

    anglee+=5;
    glutSwapBuffers();
    Sleep(100);
     glutPostRedisplay(); 
}

// void idle() {
//     glutPostRedisplay();    // Post a re-paint request to activate display()
// }

void reshape(GLsizei width, GLsizei height)
{
    if (height == 0)
        height = 1;
    // GLfloat aspect = (GLfloat)width / (GLfloat)height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, (GLfloat)width / (GLfloat)height, 0.1, 100);
}

void listen_keyboard(unsigned char key, int key_x, int key_y)
{
    double angle = 0.01; // in radian

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
    case ',':
        if (lenght == 0.0)
            break;
        decrement = decrement + 0.0208333;
        lenght = lenght - 0.0625;
        break;
    case '.':
        if (lenght == 1)
            break;
        decrement = decrement - 0.0208333;
        lenght = lenght + 0.0625;
        break;
    case 'a':
        rotation = rotation - 6;
        break;
    case 'd':
        rotation = rotation + 6;
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

    position.x = 0, position.y = 0, position.z = 20;
    look_vector.x = 0, look_vector.y = 0, look_vector.z = -1;
    up_dir.x = 0, up_dir.y = 1, up_dir.z = 0;
    right_dir.x = 1, right_dir.y = 0, right_dir.z = 0;

    lenght = 1;
    decrement = 0;
    rotation = 0;

    glutInit(&argc, argv);
    glutInitWindowSize(640, 640);
    glutInitWindowPosition(50, 50);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
    glutCreateWindow("task ");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutKeyboardFunc(listen_keyboard);
    glutSpecialFunc(listen_special);

    initGL();
    glutMainLoop();
    return 0;
}
