#include<iostream>
#include<fstream>
#include<vector>
#include<stdio.h>
#include<GL/glew.h>
#include<GL/gl.h>
#include<SDL2/SDL.h>
#include<SDL2/SDL_opengl.h>
#include"main.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include"loadShader.h"
SDL_Window* Window = NULL;
SDL_GLContext Context;
GLuint VertexArrayID;
GLuint vertexbuffer;
    

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
		 SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 ); 
		 SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 5 ); 
		 //Create window 
		 Window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN ); 
		 if( Window == NULL ) 
		 { 
			 printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() ); 
	     }
	     else 
	     { 
			 //Create context 
			 Context = SDL_GL_CreateContext( Window ); 
			 if( Context == NULL ) 
			 { 
				 printf( "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError() ); 
			 } 
			 else 
			{ 
		     //Use Vsync 
		     if( SDL_GL_SetSwapInterval( 1 ) < 0 ) 
			 { 
				 printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
			 } 
			 
	        }
	        
	      }
     }
    
    
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
    //GLuint test = LoadShaders("vaffanculo","vaffanculo");
}

void DrawPlane()
{
	
	glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(VertexArrayID);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);
    glDrawArrays(GL_TRIANGLES, 0, 6); // Starting from vertex 0; 3 vertices total -> 1 triangle
    SDL_GL_SwapWindow(Window);
 
}

int main()
{
	 width  = 1920;
	 height = 1080;
	 glClearColor(0.0f,0.0f,0.0f,1.0f);
	 init();
	 DrawPlane();
	 glDisableVertexAttribArray(0);
	 SDL_Delay(5000);
     return 0;
}
