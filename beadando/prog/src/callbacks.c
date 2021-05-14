#include "callbacks.h"
#include <GL/glu.h>

#define VIEWPORT_RATIO (4.0 / 3.0)
#define VIEWPORT_ASPECT 50.0

struct {
    int x;
    int y;
} mouse_position;

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();
    set_view(&camera);
    draw_scene(&scene);
    glPopMatrix();

    if (is_preview_visible) {
        show_texture_preview();
    }

    glutSwapBuffers();
}

void reshape(GLsizei width, GLsizei height)
{
    int x, y, w, h;
    double ratio;

    ratio = (double)width / height;
    if (ratio > VIEWPORT_RATIO) {
        w = (int)((double)height * VIEWPORT_RATIO);
        h = height;
        x = (width - w) / 2;
        y = 0;
    }
    else {
        w = width;
        h = (int)((double)width / VIEWPORT_RATIO);
        x = 0;
        y = (height - h) / 2;
    }

    glViewport(x, y, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(VIEWPORT_ASPECT, VIEWPORT_RATIO, 0.01, 10000.0);
}

int lastButton = 0;

void mouse(int button, int state, int x, int y)
{
	lastButton = button;
	//printf("button: %d | state: %d\n", button, state);
	
    mouse_position.x = x;
    mouse_position.y = y;
	
	if(button == 0 && state == 1)
	{
		/*
		float z = 0.0f;
		
		double modelview[16] = {0};
		double projection[16] = {0};
		
		int viewport[4] = {0};
		
		glGetDoublev(GL_PROJECTION_MATRIX, projection);
		
		glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
		
		glGetIntegerv(GL_VIEWPORT, viewport);
		
		glReadPixels(x, viewport[3]-y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
		
		double objx, objy, objz;
		
		gluUnProject(x, viewport[3]-y, z, modelview, projection, viewport, &objx, &objy, &objz);
		

		try_to_select_terrain_object(objx, objy, objz);
		*/
		
		//printf("%f, %f, %f\n", objx, objy, objz);
		
		//printf("z: %f | mousepos: %d %d\n", z, mouse_position.x, mouse_position.y);
	}
}

void motion(int x, int y)
{
	if(lastButton == 1)
	{
		rotate_camera(&camera, mouse_position.x - x, mouse_position.y - y);
		mouse_position.x = x;
		mouse_position.y = y;
		glutPostRedisplay();
	}
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
		case 'w':
			set_camera_speed(&camera, 3);
			break;
		case 's':
			set_camera_speed(&camera, -3);
			break;
		case 'a':
			set_camera_side_speed(&camera, 3);
			break;
		case 'd':
			set_camera_side_speed(&camera, -3);
			break;
		case 't':
			if (is_preview_visible) {
				is_preview_visible = FALSE;
			}
			else {
				is_preview_visible = TRUE;
			}
			break;
    }
    glutPostRedisplay();
}

void keyboard_up(unsigned char key, int x, int y)
{
    switch (key) {
    case 'w':
    case 's':
        set_camera_speed(&camera, 0.0);
        break;
    case 'a':
    case 'd':
        set_camera_side_speed(&camera, 0.0);
        break;
    }

    glutPostRedisplay();
}

void idle()
{
    static int last_frame_time = 0;
    int current_time;
    double elapsed_time;
   
    current_time = glutGet(GLUT_ELAPSED_TIME);
    elapsed_time = (double)(current_time - last_frame_time) / 1000;
    last_frame_time = current_time;

    update_camera(&camera, elapsed_time);

    glutPostRedisplay();
}
