// DX Ball Game in GLUT with smaller bricks and dense upside-down triangular brick layout
#include <GL/glut.h>
#include <string>
#include <vector>
#include <cmath>
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <fstream>

#pragma comment(lib, "Winmm.lib")

enum GameState { STATE_MENU, STATE_PLAYING, STATE_CREDITS, STATE_GAMEOVER, STATE_ENTER_NAME };
GameState gameState = STATE_MENU;

int selectedOption = -1;
std::string difficulty = "Medium";
bool soundOn = true;
int lives = 3;
std::string playerName = "";
std::vector<std::string> highScores;

float paddleX = 0.0f;
float paddleWidth = 0.4f;
float paddleHeight = 0.05f;
float ballX = 0.0f, ballY = -0.3f;
float ballDX = 0.01f, ballDY = 0.015f;
bool ballMoving = false;

// Smaller bricks for dense triangular layout
float brickWidth = 0.06f;
float brickHeight = 0.025f;

struct Brick { float x,y; bool alive; };
std::vector<Brick> bricks;

float menuOffset = -1.0f;

// Set your WAV file path manually here
std::string arcadeMusicPath = "D:\\Books\\CSE 426 (Graphics Lab)\\Projects\\My First DX Ball Game Project\\arcade.wav";

// Forward declarations
void display();
void keyboard(int key,int x,int y);
void normalKeys(unsigned char key,int x,int y);
void mouse(int button,int state,int x,int y);
void update();

// Music functions
void playMusic(){
    if(soundOn){
        PlaySound(arcadeMusicPath.c_str(), NULL, SND_ASYNC | SND_LOOP);
    }
}

void stopMusic(){
    PlaySound(NULL, 0, 0);
}

void applyDifficulty(){
    if(difficulty=="Easy"){ paddleWidth = 0.5f; ballDX = 0.008f; ballDY = 0.012f; }
    else if(difficulty=="Medium"){ paddleWidth = 0.4f; ballDX = 0.01f; ballDY = 0.015f; }
    else if(difficulty=="Hard"){ paddleWidth = 0.3f; ballDX = 0.014f; ballDY = 0.02f; }
}

// Dense upside-down triangular brick layout with smaller bricks
void createDenseUpsideDownTriangleBricks() {
    bricks.clear();
    int rows = 15;          // more rows vertically
    float startY = 0.9f;    // top Y coordinate

    for(int r=0; r<rows; r++) {
        int bricksInRow = rows - r + 5;   // increasing number of bricks horizontally per row
        float rowWidth = bricksInRow * brickWidth;
        float startX = -rowWidth/2;       // center the row
        for(int c=0; c<bricksInRow; c++) {
            float x = startX + c*brickWidth;
            float y = startY - r*brickHeight;
            bricks.push_back({x, y, true});
        }
    }
}

void loadHighScores(){
    highScores.clear();
    std::ifstream file("highscores.txt");
    std::string line;
    while(std::getline(file,line)) highScores.push_back(line);
    file.close();
}

void drawText(std::string s,float x,float y,void *font=GLUT_BITMAP_HELVETICA_18){
    glRasterPos2f(x,y);
    for(char c:s) glutBitmapCharacter(font,c);
}

void drawMenu(){
    glClearColor(0.05f,0.05f,0.1f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    std::string options[5] = {
        "1. Play",
        std::string("2. Level: ")+difficulty,
        std::string("3. Sound: ")+(soundOn?"ON":"OFF"),
        "4. Credits",
        "6. Exit"
    };
    glColor3f(0.8f,0.8f,0.8f);
    drawText("== DX BALL ==",-0.18f,0.6f+menuOffset,GLUT_BITMAP_TIMES_ROMAN_24);
    for(int i=0;i<5;i++){
        if(i==selectedOption) glColor3f(1.0f,0.6f,0.2f);
        else glColor3f(0.9f,0.9f,0.9f);
        drawText(options[i],-0.15f,0.3f-0.15f*i+menuOffset);
    }
}

void drawCredits(){
    glClearColor(0.05f,0.05f,0.1f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.9f,0.9f,0.9f);
    drawText("Developed by   =^_^=",-0.25f,0.6f);
    drawText("1. Mahmudul Gony Miraz",-0.25f,0.45f);
    drawText("2. Mahara Zaman",-0.25f,0.3f);
    drawText("High Scores:",-0.25f,0.1f);
    loadHighScores();
    float y=-0.05f;
    for(size_t i=0;i<highScores.size() && i<5;i++){
        drawText(highScores[i],-0.25f,y); y-=0.1f;
    }
    drawText("(Click to return)",-0.25f,-0.5f);
}

void drawGame(){
    glClearColor(0,0,0,1); glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0,0,1);
    glBegin(GL_QUADS);
    glVertex2f(paddleX-paddleWidth/2,-0.9f);
    glVertex2f(paddleX+paddleWidth/2,-0.9f);
    glVertex2f(paddleX+paddleWidth/2,-0.9f+paddleHeight);
    glVertex2f(paddleX-paddleWidth/2,-0.9f+paddleHeight);
    glEnd();

    glColor3f(1,0,0);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(ballX,ballY);
    for(int i=0;i<=20;i++){
        float a=i*3.14159f*2/20;
        glVertex2f(ballX+cos(a)*0.02f,ballY+sin(a)*0.02f);
    }
    glEnd();

    for(auto &b:bricks){ if(!b.alive) continue;
        glColor3f(0,1,0);
        glBegin(GL_QUADS);
        glVertex2f(b.x-brickWidth/2,b.y-brickHeight/2);
        glVertex2f(b.x+brickWidth/2,b.y-brickHeight/2);
        glVertex2f(b.x+brickWidth/2,b.y+brickHeight/2);
        glVertex2f(b.x-brickWidth/2,b.y+brickHeight/2);
        glEnd();
    }

    glColor3f(1,1,0);
    drawText("Lives: "+std::to_string(lives), -0.95f, 0.9f);
}

void drawGameOver(){
    glClearColor(0.1f,0,0,1); glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1,1,1);
    drawText("== GAME OVER ==", -0.25f, 0.3f, GLUT_BITMAP_TIMES_ROMAN_24);
    drawText("Enter your name: "+playerName, -0.35f, 0.0f);
}

void display(){
    if(gameState==STATE_MENU) drawMenu();
    else if(gameState==STATE_PLAYING) drawGame();
    else if(gameState==STATE_CREDITS) drawCredits();
    else if(gameState==STATE_ENTER_NAME) drawGameOver();
    glutSwapBuffers();
}

void update(){
    if(gameState==STATE_PLAYING && ballMoving){
        ballX += ballDX; ballY += ballDY;
        if(ballX>1 || ballX<-1) ballDX*=-1;
        if(ballY>1) ballDY*=-1;
        if(ballY<-0.9f && ballX>paddleX-paddleWidth/2 && ballX<paddleX+paddleWidth/2) ballDY*=-1;
        if(ballY<-1.0f){ lives--; if(lives<=0){ gameState=STATE_ENTER_NAME; stopMusic(); } else { ballX=0; ballY=-0.3f; ballMoving=false; paddleX=0; } }
        for(auto &b:bricks){ if(b.alive && fabs(ballX-b.x)<0.05f && fabs(ballY-b.y)<0.02f){ b.alive=false; ballDY*=-1; break; } }
    }
    if(gameState==STATE_MENU && menuOffset<0.0f) menuOffset += 0.02f;
    glutPostRedisplay();
}

void keyboard(int key,int x,int y){ if(gameState==STATE_PLAYING){ if(key==GLUT_KEY_LEFT) paddleX-=0.05f; if(key==GLUT_KEY_RIGHT) paddleX+=0.05f; } }

void normalKeys(unsigned char key,int x,int y){
    if(gameState==STATE_PLAYING && key==' ') ballMoving=true;
    else if(gameState==STATE_ENTER_NAME){
        if(key==8 && !playerName.empty()) playerName.pop_back();
        else if(key==13){ std::ofstream file("highscores.txt", std::ios::app); file<<playerName<<" Level: "<<difficulty<<"\n"; file.close(); playerName=""; gameState=STATE_MENU; menuOffset=-1.0f; }
        else playerName+=key;
    }
}

void mouse(int button,int state,int x,int y){
    if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN){
        float normX = (float)x/800*2 - 1;
        float normY = 1 - (float)y/600*2;
        if(gameState==STATE_MENU){
            for(int i=0;i<5;i++){
                float optionY = 0.3f - 0.15f*i + menuOffset;
                if(normX>-0.2f && normX<0.2f && normY>optionY-0.05f && normY<optionY+0.05f){
                    if(i==0){ gameState=STATE_PLAYING; lives=3; ballX=0; ballY=-0.3f; ballMoving=false; paddleX=0; applyDifficulty(); createDenseUpsideDownTriangleBricks(); playMusic(); }
                    if(i==1){ if(difficulty=="Easy") difficulty="Medium"; else if(difficulty=="Medium") difficulty="Hard"; else difficulty="Easy"; }
                    if(i==2){ soundOn=!soundOn; if(soundOn) playMusic(); else stopMusic(); }
                    if(i==3) gameState=STATE_CREDITS;
                    if(i==4) exit(0);
                }
            }
        } else if(gameState==STATE_CREDITS) gameState=STATE_MENU;
    }
}

int main(int argc,char**argv){
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(800,600);
    glutCreateWindow("DX Ball - Dense Upside-Down Triangle Bricks");
    glClearColor(0,0,0,1);

    glutDisplayFunc(display);
    glutSpecialFunc(keyboard);
    glutKeyboardFunc(normalKeys);
    glutMouseFunc(mouse);
    glutIdleFunc(update);

    // Auto-start music on program launch
    if(soundOn) playMusic();

    glutMainLoop();
}
