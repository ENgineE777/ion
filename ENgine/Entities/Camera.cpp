
#include "Camera.h"

ENTITY(Camera)
ENTITY_PARAMS(Camera,"Camera","Camera", "Static Camera")

void Camera::RegisterProperties()
{
	SceneEntity::RegisterProperties();

	PROP_REG( fov )
	PROP_REG( test_cam )
}

void Camera::Init()
{				
	en_core.Render()->Tasks()->AddTask("camera", this, Scene(), (Camera::Task)&Camera::Update, 0);

	//cam_model = core->Models()->LoadModel("Models\\Camera\\camera.md1");
}

Camera::~Camera()
{
	en_core.Render()->Tasks()->DelTask("camera", this);
}

void Camera::Update(float dt, int level)
{		
	if (Scene()->IsRunning() || (test_cam && edit_mode))
	{	
		if (IsActive())
		{
			Matrix view;
			view.BuildView( transform.Pos(), transform.Pos() + transform.Vz(), transform.Vy() );
			
			en_core.Render()->Display()->SetView( view );
				
			Matrix proj;
			proj.BuildProjection(fov * RADIAN, (float)en_core.Render()->Display()->GetHeight() / (float)en_core.Render()->Display()->GetWidth(), 0.1f, 1500.0f );		

			en_core.Render()->Display()->SetProj( proj );
		}
	}
}

CALLWRAPPERARG2(Camera, SetTarget, CALLWRAPPERGETADRESS(Vector,arg1), CALLWRAPPERGETADRESS(Vector,arg2), CALLWRAPPERNORET())	

void Camera::MethodsToScript()
{
	core->Script()->Machine()->RegisterObjectMethod(Desc()->GetName(), "void SetTarget(Vector&in from, Vector&in to)", CALLWRAPPERNAMEAS(SetTarget));
}

void Camera::SetTarget(Vector& from, Vector& to)
{	
	transform.BuildView( from, to, Vector(0.0f,1.0f,0.0f) );
	transform.Inverse();
}