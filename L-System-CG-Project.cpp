#include <windows.h>
#include <GL/glut.h>
#include <iostream>
#include <string>
#include <cmath>
#include <stack>
#include <utility>
#include <vector>

#define PI  3.14159265358979323846
#define DEGTORAD PI/180.0f  

using namespace std;

const float WIN_WIDTH = 1010, WIN_HEIGHT = 660;

int cameraX, cameraY, cameraZ = 0;
float cameraZoom = 1.f;
float cameraFlipX = 1.f;
float cameraFlipY = 1.f;
// Features
string AXIOM = "", REGL = "";
unsigned LEVEL_PRODUCTION = 0;
float ANG = 0.0f;
bool orientacionLine; // Orientation Line [verticial(true) u horizontal(false)]

struct LSystem2D {
    string axiom; // initial word
    //
    pair<char, string> regl; // Ejm: F -> F+[F+F]F ...
    //
    float ang;
};

// Structure to store position and angle when we go through the axiom

struct Nodo {
    float x_;
    float y_;
    float angle_;
};

void inline DrawString(string message) {
    string::iterator it = message.begin();

    for (; it != message.end(); ++it)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *it);

}

float colorRp = 10.00, colorGp = 0.00, colorBp = 0.50;

float colorRp2 = 0.00, colorGp2 = 1.00, colorBp2 = 0.50;

int colorCountp2 = 0, colorCountp = 0;

void DrawLine(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2) {
    if (colorCountp == 0) {
        colorRp += 1.0001;
        colorGp -= 0.0001;
        colorBp += 0.0001;
        colorCountp++;
    }
    if (colorRp != 10.0 || colorGp != 0.0) {
        colorRp = colorRp + 0.000005;
        colorBp = colorBp + 0.000005;
        //colorCountp2++;
    }

    if (colorCountp2 == 0) {
        colorRp2 -= 0.0001;
        colorGp2 -= 0.0001;
        colorBp2 += 0.0001;
        colorCountp2++;
    }
    if (colorRp2 != 10.0 || colorGp2 != 0.0) {
        colorRp2 = colorRp2 + 0.000005;
        colorBp2 = colorBp2 + 0.000005;
        //colorCountp2++;
    }

    glBegin(GL_LINE_STRIP);
    glColor3f(colorRp, colorGp, colorBp);
    glVertex2f(x1, y1);
    glColor3f(colorRp2, colorGp2, colorBp);
    glVertex2f(x2, y2);
    glEnd();
}

static void resize(int width, int height) {
    const float ar = (float) width / (float) height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, WIN_WIDTH, 0.0, WIN_HEIGHT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

void LSystem(string w, float startX, float startY) {
    unsigned i = 0;

    float xo = startX; 
    float yo = startY; 
    float xf = xo;
    float yf = yo;

    stack<Nodo> nodos;
    Nodo nodoActual;

    float angleRot = 0.0f; // horizontal

    if (orientacionLine)
        angleRot = 90.0f; 

    while (i < w.size()) {
        switch (w[i]) {
            case 'F':
                xf = xo + 5.0f * cos(angleRot * DEGTORAD);
                yf = yo + 5.0f * sin(angleRot * DEGTORAD);
                DrawLine(xo, yo, xf, yf);
                xo = xf;
                yo = yf;
                break;

            case '[':
                nodoActual.x_ = xf;
                nodoActual.y_ = yf;
                nodoActual.angle_ = angleRot;
                nodos.push(nodoActual);
                break;

            case ']':
                xo = nodos.top().x_;
                yo = nodos.top().y_;
                xf = xo;
                yf = yo;
                angleRot = nodos.top().angle_;
                nodos.pop();
                break;

            case '+':
                angleRot += ANG;
                break;

            case '-':
                angleRot -= ANG;
                break;
        }

        i++;
    }
}

string GenerateWord(string axiom, string F, unsigned LevelProduction) {
    // // Count the position of the string (axiom) where you find
    unsigned n = 0;

    while (n <= LevelProduction) {
        int cont = 0;

        while (cont < axiom.size()) {
            if (axiom[cont] == 'F') {
                axiom.insert(cont, F); // We insert the string, that is, rule F
                cont += F.size(); // The position is updated by increasing the size of the axiom
                axiom.erase(cont, 1); // The previous F is deleted
            } else {
                cont++;
            }
        }

        n++;
    }

    cout << "n = " << LevelProduction << endl;
    cout << axiom << endl;

    return axiom;
}

static void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Text on screen
    glRasterPos2f(20, WIN_HEIGHT - 20);
    DrawString("TO Move SCREEN (Right,Left,Up,Down): Press Keys W,A,S,D");
    glRasterPos2f(20, WIN_HEIGHT - 45);
    DrawString("Grow tree: SHIFT+'+'");
    glRasterPos2f(WIN_WIDTH / 2 - 50, WIN_HEIGHT - 40);
    DrawString("Quit: Press Key Q");
    glRasterPos2f(20, WIN_HEIGHT - 70);
    DrawString("Zoom In and Zoom Out: Press Keys 1,2");
    glRasterPos2f(20, WIN_HEIGHT - 100);
    DrawString("Flip (Right,Left,Up,Down): Press Keys 3,4");
    glRasterPos2f(20, WIN_HEIGHT - 130);
    DrawString("To make Tree Center : Press Keys 5");

    float pad = 50; // padding from the borders
    float top = 30; // distance from title
    glTranslatef(cameraX, cameraY, 0);
    glTranslatef(WIN_WIDTH / 2, 
        WIN_HEIGHT / 2 // go to center
        - (WIN_HEIGHT / 2 - pad) * cameraFlipY // if flipped go to top or bottom by half height
        - 30, // go down from top to leave space to the text
        0);
    glScalef(cameraZoom * cameraFlipX, cameraZoom * cameraFlipY, 1);

    // Drawn from the LSystem
    LSystem(AXIOM, 0, 0);

    glutSwapBuffers();
}

static void key(unsigned char key, int x, int y) {
    int id;
    switch (key) {
        case 27:
        case 'q':
            exit(0);
            break;

        case 'w':
            cameraY -= 5;
            break;

        case 's':
            cameraY += 5;
            break;

        case 'a':
            cameraX += 5;
            break;

        case 'd':
            cameraX -= 5;
            break;

        case '+':
            AXIOM = GenerateWord(AXIOM, REGL, 0);
            break;
            
        /*case '-':
        AXIOMA = GenerarPalabra(AXIOMA, REGLA, 0);
        break;*/

        case '1':
        cameraZoom += 0.05;
        break;

        case '2':
        cameraZoom -= 0.05;
        break;

       case '3':
        cameraFlipX *= -1;
        break;

       case '4':
        cameraFlipY *= -1;
        break;
        
        case '5':
            cameraX = 0;
            cameraY = 0;
            cameraZ = 0;
            break;
        
        default: // not a valid key -- just ignore it
            return;         
    }

    glutPostRedisplay();
}

static void idle(void) {
    glutPostRedisplay();
}

void menuOption() {
    cout << "\t L-System 2D" << endl << endl;
    cout << " 1. FF-[-F+F+F]+[+F-F-F] (Plant-like structures generated using L-systems)" << endl;
    cout << " 2. F[+F]F[-F]F (Plant-like structures generated using L-systems)" << endl;
    cout << " 3. F[+F]F[-F][F] (Plant-like structures generated using L-systems)" << endl;
    cout << " 4. F-FF--F-F (Koch Curves generated using L-systems)" << endl;
    cout << " 5. F-F+F-F-F (Koch Curves generated using L-systems)" << endl;
    cout << " 6. FF-F+F-F-FF (Koch Curves generated using L-systems)" << endl;
    cout << " 7. FF-F--F-F (Koch Curves generated using L-systems)" << endl;
    cout << " 8. FF-F-F-F-F-F+F (Koch Curves generated using L-systems)" << endl;
    cout << " 9. FF-F-F-F-FF (Koch Curves generated using L-systems)" << endl;
	cout << " 10. F+F-F-F+F (Quadratic Koch Island Curve generated using L-systems)" << endl;
	cout << " 11. F+FF-FF-F-F+F+F (Quadratic Modification of Snowflake Curve generated using L-systems)" << endl;    
    cout << " 12. Exit" << endl << endl;
    cout << " Option > ";
}

int main(int argc, char *argv[]) {
    unsigned option;

    /// L-System predefined
    struct LSystem2D LS1, LS2, LS3, LS4, LS5, LS6, LS7, LS8, LS9, LS10, LS11;

    LS1.axiom = "F";
    LS1.regl = make_pair('F', "FF-[-F+F+F]+[+F-F-F]");
    LS1.ang = 22.5f;

    LS2.axiom = "F";
    LS2.regl = make_pair('F', "F[+F]F[-F]F");
    LS2.ang = 25.7f;    

    LS3.axiom = "F";
    LS3.regl = make_pair('F', "F[+F]F[-F][F]");
    LS3.ang = 20.0f;
    
    LS4.axiom = "F";
    LS4.regl = make_pair('F', "F-FF--F-F");
    LS4.ang = 90.0f;
    
    LS5.axiom = "F";
    LS5.regl = make_pair('F', "F-F+F-F-F");
    LS5.ang = 90.0f;
    
    LS6.axiom = "F";
    LS6.regl = make_pair('F', "FF-F+F-F-FF");
    LS6.ang = 90.0f;
    
    LS7.axiom = "F";
    LS7.regl = make_pair('F', "FF-F--F-F");
    LS7.ang = 90.0f;
    
    LS8.axiom = "F";
    LS8.regl = make_pair('F', "FF-F-F-F-F-F+F");
    LS8.ang = 90.0f;
    
    LS9.axiom = "F";
    LS9.regl = make_pair('F', "FF-F-F-F-FF");
    LS9.ang = 90.0f;
    
    LS10.axiom = "F";
    LS10.regl = make_pair('F', "F+F-F-F+F");
    LS10.ang = 90.0f;
    
    LS11.axiom = "F";
    LS11.regl = make_pair('F', "F+FF-FF-F-F+F+F");
    LS11.ang = 90.0f;
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glShadeModel(GL_SMOOTH);

    glutInit(&argc, argv);
    glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
    glutInitWindowPosition(0, 0);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("L-System");
    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutIdleFunc(idle);
    glClearColor(0, 0, 0, 1);

    do {
        menuOption();
        cin >> option;
        cout << endl;

        switch (option) {
            case 1:
                AXIOM = LS1.axiom;
                REGL = LS1.regl.second;
                ANG = LS1.ang;
                LEVEL_PRODUCTION = 0;
                orientacionLine = true; // vertical

                AXIOM = GenerateWord(AXIOM, REGL, LEVEL_PRODUCTION);
                glutMainLoop();
                break;

            case 2:

                AXIOM = LS2.axiom;
                REGL = LS2.regl.second;
                ANG = LS2.ang;
                LEVEL_PRODUCTION = 0;
                orientacionLine = true; // vertical

                AXIOM = GenerateWord(AXIOM, REGL, LEVEL_PRODUCTION);
                glutMainLoop();
                break;
            case 3:
                AXIOM = LS3.axiom;
                REGL = LS3.regl.second;
                ANG = LS3.ang;
                LEVEL_PRODUCTION = 0;
                orientacionLine = true; // horizontal

                AXIOM = GenerateWord(AXIOM, REGL, LEVEL_PRODUCTION);
                glutMainLoop();
                
            case 4:
                AXIOM = LS4.axiom;
                REGL = LS4.regl.second;
                ANG = LS4.ang;
                LEVEL_PRODUCTION = 0;
                orientacionLine = true; // horizontal

                AXIOM = GenerateWord(AXIOM, REGL, LEVEL_PRODUCTION);
                glutMainLoop();
				
			case 5:
                AXIOM = LS5.axiom;
                REGL = LS5.regl.second;
                ANG = LS5.ang;
                LEVEL_PRODUCTION = 0;
                orientacionLine = true; // horizontal

                AXIOM = GenerateWord(AXIOM, REGL, LEVEL_PRODUCTION);
                glutMainLoop();
				
			case 6:
                AXIOM = LS6.axiom;
                REGL = LS6.regl.second;
                ANG = LS6.ang;
                LEVEL_PRODUCTION = 0;
                orientacionLine = true; // horizontal

                AXIOM = GenerateWord(AXIOM, REGL, LEVEL_PRODUCTION);
                glutMainLoop();
			
			case 7:
                AXIOM = LS7.axiom;
                REGL = LS7.regl.second;
                ANG = LS7.ang;
                LEVEL_PRODUCTION = 0;
                orientacionLine = true; // horizontal

                AXIOM = GenerateWord(AXIOM, REGL, LEVEL_PRODUCTION);
                glutMainLoop(); 
				
			case 8:
                AXIOM = LS8.axiom;
                REGL = LS8.regl.second;
                ANG = LS8.ang;
                LEVEL_PRODUCTION = 0;
                orientacionLine = true; // horizontal

                AXIOM = GenerateWord(AXIOM, REGL, LEVEL_PRODUCTION);
                glutMainLoop(); 
				
			case 9:
                AXIOM = LS9.axiom;
                REGL = LS9.regl.second;
                ANG = LS9.ang;
                LEVEL_PRODUCTION = 0;
                orientacionLine = true; // horizontal

                AXIOM = GenerateWord(AXIOM, REGL, LEVEL_PRODUCTION);
                glutMainLoop();  
				
			case 10:
                AXIOM = LS10.axiom;
                REGL = LS10.regl.second;
                ANG = LS10.ang;
                LEVEL_PRODUCTION = 0;
                orientacionLine = true; // horizontal

                AXIOM = GenerateWord(AXIOM, REGL, LEVEL_PRODUCTION);
                glutMainLoop(); 
				
			case 11:
                AXIOM = LS11.axiom;
                REGL = LS11.regl.second;
                ANG = LS11.ang;
                LEVEL_PRODUCTION = 0;
                orientacionLine = true; // horizontal

                AXIOM = GenerateWord(AXIOM, REGL, LEVEL_PRODUCTION);
                glutMainLoop();                
            
        }

    } while (option != 12);

    return EXIT_SUCCESS;
}
