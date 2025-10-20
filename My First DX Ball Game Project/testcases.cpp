Ball to triangle
------------------

void draw() const {
    glBegin(GL_TRIANGLES);
    
     Three points of the triangle (equilateral)
    glVertex2f(x, y + r);            Top vertex
    glVertex2f(x - r, y - r);        Bottom-left vertex
    glVertex2f(x + r, y - r);        Bottom-right vertex
    
    glEnd();
}


Bricks in circle
------------------
void draw() const {
    if (!alive()) return;

    // Center of the brick
    float cx = x + w / 2.0f;
    float cy = y + h / 2.0f;
    float radius = std::min(w, h) / 2.0f;

    // Brick color (you can change these values)
    glColor3f(1.0f, 1.0f, 1.0f);

    // Draw filled circle
    const int segments = 30; // smoothness
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= segments; i++) {
        float angle = i * 2.0f * 3.1415926535f / segments;
        float dx = cosf(angle) * radius;
        float dy = sinf(angle) * radius;
        glVertex2f(cx + dx, cy + dy);
    }
    glEnd();

    // Draw circle outline
    glColor3f(0, 0, 0);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= segments; i++) {
        float angle = i * 2.0f * 3.1415926535f / segments;
        float dx = cosf(angle) * radius;
        float dy = sinf(angle) * radius;
        glVertex2f(cx + dx, cy + dy);
    }
    glEnd();
}

Brick in a single color
-------------------------
glColor3f(1.0f, 1.0f, 1.0f);

Paddle speed increase
-----------------------
speed=1000.0f;

Futher
--------------

test 1
test 2
test 3
test 4