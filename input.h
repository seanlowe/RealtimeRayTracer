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
using namespace glm;
using namespace std;
class input_handler
{
        public:
                bool wasdqe[7] = {false,false,false,false,false,false,true};
                struct CamTrasform
                {
                    mat4 CamRot = mat4(1.);
                    vec3 CamPos = vec3(50,52,295.6);
                };
                CamTrasform Cam_Trasform;
                CamTrasform GetCamTrasform(float DeltaTime);
                float Speed,MouseSpeed = .001;
        private:
                vec2 rotInDegrees;
                int IndexFromSyn(SDL_Event Sym);
                float Delta_Time;
                SDL_Event event;
                void Update();
};
