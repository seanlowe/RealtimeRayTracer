#version 450
out vec3 color;
in vec2 ScreenCoord;
#define PI 3.14159265359
#define DIFF 0
#define SPEC 1
#define REFR 2
#define SPHERE 1
#define DISK 2
#define MINIMUMDEPTH 1
uniform int SAMPLESPF = 10;
uniform int MAXDEPTH = 4;


float seed = 0;
float rand() { return fract(sin(seed++)*43758.5453123); }
//float rand(){return .0;}

float fabs(float a){return a < 0 ? a * -1.0 : a;}

struct material
{
	vec3 e, c, r;//emissiion,color,reflection
	int refl_t;
};

struct ray
{
	vec3 o,d;
};
ray CreateRay(vec3 o,vec3 d){ray temp; temp.o = o; temp.d = d; return temp;}

struct sphere
{
	float rad;
	vec3 p;
	material Mat;
};
sphere CreateSphere(float rad,vec3 p,vec3 e,vec3 c,vec3 r,int refl_t){sphere temp; material tempmat; temp.rad = rad; temp.p = p; tempmat.e = e; tempmat.c = c; tempmat.r = r; tempmat.refl_t = refl_t; temp.Mat = tempmat; return temp;}
float IntersectSphere(ray r,sphere sph)
{
	vec3 op = sph.p-r.o;
	float t,eps = 1e-2;
	float b = dot(op , r.d);
	float det = b * b - dot(op,op) + sph.rad * sph.rad;
	         // ^ Solves t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0
	if(det < 0)return 0;else det = sqrt(det);
	return (t = b - det) > eps ? t : ((t=b+det)>eps ? t:0);
}

struct disk
{
	float rad;
	vec3 normal, center;
	material Mat;
};
disk CreateDisk(float rad, vec3 normal, vec3 center,vec3 e,vec3 c,vec3 r,int refl_t){disk temp; material tempmat; temp.rad = rad; temp.normal = normal; temp.center = center; tempmat.e = e; tempmat.c = c; tempmat.r = r; tempmat.refl_t = refl_t; temp.Mat = tempmat; return temp;}
float IntersectDisk(ray r,disk Disk)
{
	float t = 0,denom = 0;
	if((denom = dot(Disk.normal, r.d)) > 1e-6)
	{
		vec3 v;
		t = dot((Disk.center - r.o),Disk.normal) / denom;
		float d2 = dot((v = r.o + r.d * t - Disk.center),v);
		return (d2 <= (Disk.rad * Disk.rad)) ? t : .0;
	}
	return .0;
}

struct DirLight
{
	vec3 dir, intensity;
	float MaxAngle;
};

#define NumDirLights 1
#define DiskCount 1
#define SpheresCount 1

int NumSpheres,NumDisks;
vec3 backGroundColor;
disk Disks[DiskCount];
sphere Spheres[SpheresCount];
DirLight DirLights[NumDirLights];


bool Intersect(ray r,int NumSpheres,int NumDisks,out float t,out int id,out int ObjType)
{
	int i = 0;
	float tempt, inf = t = 1e8;
	for(i = 0;i < NumDisks;i++){if(((tempt = IntersectDisk(r,Disks[i])) > .0) && (tempt < t) ? true : false){t = tempt;id = i;ObjType = DISK;}}
	for(i = 0;i < NumSpheres;i++){if(((tempt = IntersectSphere(r,Spheres[i])) > .0) && (tempt < t) ? true : false){t = tempt; id = i;ObjType = SPHERE;}}
    return t < inf;

}



vec3 radiance(ray r)
{
	//vec3 fc = backGroundColor;//color will be accumulated in this vector
	vec3 ca = vec3(.0),mask = vec3(1.0);
	ray CRay = r;
	float ie = 1.0;
	vec3 x;
	for(int depth = 0; depth < MAXDEPTH;depth++)
	{	//does intersection
	    float t;
	    int id = 0;
	    int ObjType;
	    if(!(Intersect(CRay,NumSpheres,NumDisks,t,id,ObjType))){ ca += backGroundColor * mask;break;}
	    x = CRay.o + CRay.d * t;
	    vec3 n,nl;
	    material mat;

		 if(ObjType == SPHERE)
		 {
			  n = normalize(x - Spheres[id].p);
			  mat = Spheres[id].Mat;
	     }
	     else if(ObjType == DISK)
	     {
			n = Disks[id].normal;
			mat = Disks[id].Mat;
		 }
		 nl = n * sign(-dot(n, CRay.d));

		if(mat.refl_t == DIFF)//diffuse reflection
		{

			float r1 = 2*PI * rand();          //random angle
			float r2 = rand(), r2s = sqrt(r2); //random distance
		    vec3 w = nl;
		    vec3 u = normalize(cross((fabs(w.x) > .1 ? vec3(.0,1.0,.0):vec3(1.0)),w));
		    vec3 v=cross(w,u);
		    vec3 d = normalize(u*vec3(cos(r1))*vec3(r2s) + v*vec3(sin(r1))*vec3(r2s) + w*vec3(sqrt(1-r2)));//random reflection ray


		    vec3 e = vec3(0.);
		    for(int i = 0;i < SpheresCount; i++)
		    {
				if(Spheres[i].Mat.e.x <= 0 && Spheres[i].Mat.e.y <= 0 && Spheres[i].Mat.e.z <= 0) continue;

				vec3 sw=Spheres[i].p-x, su=normalize(cross((fabs(sw.x)>.1?vec3(.0,1.,.0):vec3(1.)),sw)), sv=cross(sw,su);
				float cos_a_max = sqrt(1.-Spheres[i].rad*Spheres[i].rad/dot(x-Spheres[i].p,x-Spheres[i].p));
				float eps1 = rand(), eps2 = rand();
				float cos_a = 1.-eps1+eps1*cos_a_max;
				float sin_a = sqrt(1-cos_a*cos_a);
				float phi = 2.*PI*eps2;
				vec3 l = su*vec3(cos(phi)*sin_a)+sv*vec3(sin(phi)*sin_a)+sw*vec3(cos_a);
				l = normalize(l);
				if(Intersect(ray(x,l),NumSpheres,NumDisks,t,id,ObjType) && (id == i) && (ObjType == SPHERE))
				{
					float omega =  2*PI*(1-cos_a_max);
					e = e + mat.c * (Spheres[i].Mat.e * clamp(dot(l, nl),0.,1.) * omega) / PI ;
				}
		    }
		    for(int i = 0;i < NumDirLights; i++)
		    {
				vec3 D = normalize(DirLights[i].dir * vec3(10.) + vec3(DirLights[i].MaxAngle) * vec3(rand(),rand(),rand()));
				if(!Intersect(ray(x,D),NumSpheres,NumDisks,t,id,ObjType))
				{
					e += DirLights[i].intensity * dot(D,nl) * mat.c;
				}
		    }
		      ca += e * mask + mat.e  * mask * vec3(ie);
		    mask *= mat.c;
		    ie = 0.;
		    CRay = CreateRay(x,d);
		}
		else if (mat.refl_t == SPEC)
		{
			 ie = 1.;
		     ca += mat.e * mask;
		     mask *= mat.c;
			 CRay = CreateRay(x,reflect(CRay.d,n));
		}
		else/*REFRACTIVE*/
		{
			ie = 1.;
			ray reflRay = CreateRay(x,reflect(CRay.d,n));
			bool into = dot(n,nl)>0.;
			float nc = 1, nt = 1.5, nnt=into ? nc/nt : nt/nc, ddn=dot(CRay.d,nl) ,cos2t;

			//total internal reflection
			if((cos2t=1.-nnt*nnt*(1.-ddn*ddn))<0.)
			{
				ca += mat.e * mask;
		        mask *= mat.c;
			    CRay = reflRay;
			}

			//otherwise randomly chose reflection or refraction
			vec3 tdir = normalize(CRay.d*vec3(nnt) - n*((into?vec3(1):vec3(-1))* vec3(ddn*nnt+sqrt(cos2t))));
			float a = nt-nc, b=nt+nc, R0=a*a/(b*b), c = 1-(into?-ddn:dot(tdir,n));
			float Re=R0+(1-R0)*c*c*c*c*c, Tr=1-Re,P=.25+.5*Re,RP=Re/P,TP=Tr/(1-P);

			if(rand() < P){mask *=RP;CRay=reflRay;}
			else{mask*=TP;CRay=CreateRay(x,tdir);}
	    }
    }
    return ca;
}

uniform vec3 position ;
uniform vec3 positionOld;
uniform mat4 rotation = mat4(1);
uniform mat4 OldRotation = mat4(1);
uniform float HFov = 2.5;
uniform float AspectRatio ;
uniform float Time;
#define MotionBlur 1

vec3 tone_map(vec3 inc)
{
    inc = inc / (inc + vec3(1.0));
    return pow( clamp(inc,.0,1.),vec3(1/2.2));
}


void main()
{
	ray Cam = CreateRay(vec3(0., 0., 160.), normalize(vec3(0.,0.,1)));
	vec3 UpRCoord = vec3(cos(HFov),cos(HFov),1.);
	vec3 fcolor;
	vec2 coords = vec2(ScreenCoord.x * AspectRatio * -1.,ScreenCoord.y * -1.);
	seed = coords.x * coords.y * Time;
	vec3 RayDir = (vec4(normalize(UpRCoord * vec3(coords,1.)),1.0) * rotation ).xyz;
	vec3 OldDir = (vec4(normalize(UpRCoord * vec3(coords,1.)),1.0) * OldRotation ).xyz;
	ray PrimaryRay = CreateRay(position  ,RayDir);



    Spheres[0] = CreateSphere(30.0,vec3(0.,30.,0.), vec3(0.)  ,vec3(.75, .25, .25),vec3(0.),DIFF);
	//Spheres[1] = CreateSphere(10.0,vec3(80.,60.,0.), vec3(10.)  ,vec3(.75),vec3(0.),DIFF);
	//Spheres[1] = CreateSphere(3.0,vec3(100.,80.,0.), vec3(0.)  ,vec3(.75),vec3(0.),DIFF);
    Disks[0]   = CreateDisk(200.0,vec3(.0,-1.0,0.0) ,vec3(0.)   ,vec3(0.),vec3(.75),vec3(0.),DIFF);


     DirLights[0].intensity = vec3(7.0)*vec3(255.,205.,164.)/vec3(255.);
     DirLights[0].dir = normalize(vec3(1.,0.43,0.));
     DirLights[0].MaxAngle = 1.0;



	int id,objType;
	backGroundColor = vec3 (.145,.21,.36) * vec3(7.);
	NumSpheres = SpheresCount;
	NumDisks = DiskCount;
	//color = !(Intersect(PrimaryRay,NumSpheres,NumDisks,t,id,objType))? vec3(.0): /*(objType == SPHERE)? Spheres[id].Mat.e : Disks[id].Mat.e;*/ vec3(1.0);
	for(int y = 0;y < SAMPLESPF;y++)
	{
		vec3 Dir = mix(RayDir,OldDir,rand() * MotionBlur);
		vec3 JPos = mix(position,positionOld,rand() * MotionBlur);
		vec3 antialiasing = normalize(Dir + vec3(2*rand() - 1,2*rand() - 1,2*rand() - 1)/vec3(1200));
	    fcolor += radiance(CreateRay(JPos,antialiasing))/ SAMPLESPF;
    }

    color = tone_map(fcolor);
}
