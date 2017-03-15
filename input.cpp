#include "input.h"

int input_handler::IndexFromSyn(SDL_Event Sym)
{
    switch(Sym.key.keysym.sym)
        {
            case SDLK_w:
                return 0;

            case SDLK_a:
                return 1;

            case SDLK_s:
                return 2;

            case SDLK_d:
                return 3;

            case SDLK_q:
                return 4;

            case SDLK_e:
                return 5;

            case SDLK_ESCAPE:
                return 6;

            default:
                return -1;
        }
}


void input_handler::Update()
{
     while(SDL_PollEvent(&event))
     {
        int index = IndexFromSyn(event);
        if(index == -1) continue;
         switch(event.type)
		{
			case SDL_KEYDOWN:
			  wasdqe[index] =  true;
			  break;
		    case SDL_KEYUP:
		      wasdqe[index] = false;
		      break;
	    }
     }
   /* if(wasdqe[0]){Cam_Trasform.CamPos.z += Speed * Delta_Time; }
    if(wasdqe[2]){Cam_Trasform.CamPos.z -= Speed * Delta_Time; }
    if(wasdqe[1]){Cam_Trasform.CamPos.x -= Speed * Delta_Time; }
    if(wasdqe[3]){Cam_Trasform.CamPos.x += Speed * Delta_Time; }
    if(wasdqe[4]){Cam_Trasform.CamPos.y -= Speed * Delta_Time; }
    if(wasdqe[5]){Cam_Trasform.CamPos.y += Speed * Delta_Time; }*/
    int DeltaMouseX,DeltaMouseY;
    SDL_GetRelativeMouseState(&DeltaMouseX,&DeltaMouseY);
    rotInDegrees.x +=  DeltaMouseX *  MouseSpeed;
    rotInDegrees.y +=  DeltaMouseY *  MouseSpeed;
    vec3 direction = vec3
    (
         cos(rotInDegrees.y) * sin(rotInDegrees.x),
         sin(rotInDegrees.y),
         cos(rotInDegrees.y) * cos(rotInDegrees.x)
    );
    vec3 right = vec3
    (
         sin(rotInDegrees.x - 3.14f/2.0f),
         0,
         cos(rotInDegrees.x - 3.14f/2.0f)
    );
    vec3 up = cross(right,direction);
    Cam_Trasform.CamRot = lookAt(vec3(0.),direction,up);
    Cam_Trasform.CamPos -= vec3((wasdqe[0]? 1. : 0.) * Speed * Delta_Time) * vec3(direction.x,direction.y,direction.z);
    Cam_Trasform.CamPos += vec3((wasdqe[2]? 1. : 0.) * Speed * Delta_Time) * vec3(direction.x,direction.y,direction.z);
    Cam_Trasform.CamPos -= vec3((wasdqe[1]? 1. : 0.) * Speed * Delta_Time) * vec3(right.x,right.y,right.z);
    Cam_Trasform.CamPos += vec3((wasdqe[3]? 1. : 0.) * Speed * Delta_Time) *  vec3(right.x,right.y,right.z );
    Cam_Trasform.CamPos -= vec3((wasdqe[4]? 1. : 0.) * Speed * Delta_Time) * vec3(up.x,up.y,up.z );
    Cam_Trasform.CamPos += vec3((wasdqe[5]? 1. : 0.) * Speed * Delta_Time) *  vec3(up.x,up.y,up.z );
}


input_handler::CamTrasform input_handler::GetCamTrasform(float DeltaTime)
{
    Delta_Time = DeltaTime;
    Update();
    return Cam_Trasform;
}
