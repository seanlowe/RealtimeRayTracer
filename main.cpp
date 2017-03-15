#include<iostream>
#include<fstream>
#include<vector>
#include<stdio.h>
#include<GL/glew.h>
#include<GL/gl.h>
#include<SDL2/SDL.h>
#include<SDL2/SDL_opengl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include"main.h"
#include"loadShader.h"
#include"input.h"
SDL_Window* Window = NULL;
SDL_GLContext Context;
GLuint VertexArrayID;
GLuint vertexbuffer;
GLuint PathTracerPogram;
GLuint CamRotMatrix ,CamRotMatrixOld ,CamPosition ,OldPosition ,samplecount ,bounces , AspectRatio, Gtime;

#define TARG_FPS 58

/*bool initGL()
{
	bool success = true;
	GLenum error = GL_NO_ERROR;
	 //Sets clear color
	 glClearColor( 0.f, 0.f, 0.f, 1.f );
	 //Check for error
	 error = glGetError();
	 if( error != GL_NO_ERROR )
	 {
		 printf( "Error initializing OpenGL! \n" );
		 success = false;
	 }
	 return success;
}*/

void init()
{
	//Initialize SDL
	 if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	 {

		 printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
     }
     else
     {
		 //Use OpenGL 4.5
		 SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
		 SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
		 SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
		 //Create window
		 Window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
		 if( Window == NULL )
		 {
			 printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
	     }
	     else
	     {
	     //SDL_SetWindowFullscreen(Window,SDL_WINDOW_FULLSCREEN_DESKTOP);

			 //Create context
			 Context = SDL_GL_CreateContext( Window );
			 if( Context == NULL )
			 {
				 printf( "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError() );
			 }
			 else
			{
		     //Use Vsync(coomment to disable)
		     /*if( SDL_GL_SetSwapInterval( 1 ) < 0 )
			 {
				 printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
			 }*/

	        }

	      }
     }

    SDL_SetRelativeMouseMode(SDL_TRUE);
    GLenum rev;
    glewExperimental = GL_TRUE;
    rev = glewInit();
    if(rev != GLEW_OK)
    {
		std::cout<<"Error: " << glewGetErrorString(rev) << std::endl;
    }


    glGenVertexArrays(1, &VertexArrayID);

    static const GLfloat g_vertex_buffer_data[] =
    {
     -1.0f, -1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
     -1.0f,  1.0f, 0.0f,
     1.0f, 1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
     -1.0f,  1.0f, 0.0f,
    };

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    PathTracerPogram = LoadShaders("PT.vert","PT.frag");

    glBindVertexArray(VertexArrayID);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);
    glUseProgram(PathTracerPogram);
    CamRotMatrix = glGetUniformLocation(PathTracerPogram,"rotation");
    CamRotMatrixOld = glGetUniformLocation(PathTracerPogram,"OldRotation");
    OldPosition    = glGetUniformLocation(PathTracerPogram,"positionOld");
    CamPosition  = glGetUniformLocation(PathTracerPogram,"position");
    bounces      = glGetUniformLocation(PathTracerPogram,"MAXDEPTH");
    samplecount  = glGetUniformLocation(PathTracerPogram,"SAMPLESPF");
    AspectRatio = glGetUniformLocation(PathTracerPogram,"AspectRatio");
    Gtime         = glGetUniformLocation(PathTracerPogram,"Time");
}

void DrawPlane()
{

	//glClear(GL_COLOR_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLES, 0, 6); // Starting from vertex 0; 3 vertices total -> 1 triangle
    SDL_GL_SwapWindow(Window);

}

int main( int argc, char *argv[])
{
    float Delta,TickNow = 0.,TickLast  = 0.;
     input_handler::CamTrasform CTr;
     input_handler testInput;
     testInput.Speed = .1;
	 width  = 1920;
	 height = 1080;
	 glClearColor(0.0f,0.0f,0.0f,1.0f);
	 init();
	 glUniform1f(AspectRatio, float(width)/height);
     glUniform1i(samplecount,100);
     float time;
     mat4 CamRotOld;
     vec3 PosOld;

     float fspp = 10;
     while (testInput.wasdqe[6])
     {
        Delta = (TickNow = SDL_GetTicks()) - TickLast;
        TickLast = TickNow;
        float fps = 1000/Delta;

        float d_speed = -1.0f * fps - TARG_FPS;
        d_speed /= 1.0f;
        d_speed = clamp(d_speed, -10.0f, -1.0f);
        fspp += clamp(fps - TARG_FPS, d_speed, 0.1f);
        int spp = clamp(int(fspp), 1, 150);
        glUniform1i(samplecount, spp);
        fprintf(stderr,"\r fps: %f %f ms spp: %i      ", fps, Delta, spp);

        //SDL_Delay(clamp(1000.0f/TARG_FPS-Delta, 0.0f, 1000.0f/TARG_FPS));

        CTr = testInput.GetCamTrasform(Delta);
        glUniform3f(CamPosition,CTr.CamPos.x,CTr.CamPos.y,CTr.CamPos.z);
        glUniform3f(OldPosition,PosOld.x,PosOld.y,PosOld.z);
        PosOld = CTr.CamPos;
        glUniformMatrix4fv(CamRotMatrix, 1,GL_FALSE, &CTr.CamRot[0][0]);
        glUniformMatrix4fv(CamRotMatrixOld, 1,GL_FALSE, &CamRotOld[0][0]);
        CamRotOld = CTr.CamRot;
        glUniform1f(Gtime,SDL_GetTicks());
        DrawPlane();
     }
	 glDisableVertexAttribArray(0);
     return 0;
}
