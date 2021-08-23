
#include "SpiderHero.h"
#include "../../../Utils/Gizmo.h"

ENTITY(SpiderHero)
ENTITY_PARAMS(SpiderHero,"SpiderHero","nodePanelSheetName", "nodePanelTexName")

/*

#define MAX_BODIES   512  //Maximum body/vertex/edgecount the physics simulation can handle
#define MAX_VERTICES 1024
#define MAX_EDGES    1024
#define MAX_BODY_VERTICES 64 //Maximum body/edge count a body can contain
#define MAX_BODY_EDGES    64

struct PhysicsBody; //Prototypes
struct Vertex;
struct Edge;

class Physics
{
	Vector Gravity; //Most of this should be clear after reading the article

	int BodyCount;
	int VertexCount;
	int EdgeCount;

	Vertex*      Vertices[ MAX_VERTICES ];
	Edge*        Edges   [ MAX_EDGES    ];
	PhysicsBody* Bodies  [ MAX_BODIES   ];

	int Iterations;

	void  UpdateForces();
	void  UpdateVerlet();
	void  UpdateEdges ();
	void  IterateCollisions();
	bool  DetectCollision( PhysicsBody* B1, PhysicsBody* B2 );
	void  ProcessCollision();
	float IntervalDistance( float MinA, float MaxA, float MinB, float MaxB );
	bool  BodiesOverlap( PhysicsBody* B1, PhysicsBody* B2 ); //Used for optimization to test if the bounding boxes of two bodies overlap

	struct {
		float Depth;
		Vector  Normal;

		Edge*   E;
		Vertex* V;
	} CollisionInfo;

public:
	void Update();
	void Render();

	void AddBody  ( PhysicsBody* Body ); //Adds new elements to the simulation
	void AddEdge  ( Edge* E );
	void AddVertex( Vertex* V );

	void Clear();

	Vertex* FindVertex( int X, int Y );

	float Timestep;

	Physics( float GravitationX = 0.0f, float GravitationY = 0.0f, int pIterations = 1 ) : //Constructor
					BodyCount( 0 ), VertexCount( 0 ), EdgeCount( 0 ),
					Gravity( Vector( GravitationX, GravitationY ) ),
					Iterations( pIterations ), Timestep( 0.3f ) {}
};

int GWidth  = 320; //Graphics resolution
int GHeight = 480;

Physics World( 0.0f, 0.5f, 2 );

struct PhysicsBody
{
	Vector Center; //Center of mass

	int MinX, MinY, MaxX, MaxY; //Min/max coordinates of the bounding box

	int VertexCount;
	int EdgeCount;

	Vertex* Vertices[ MAX_BODY_VERTICES ];
	Edge*   Edges   [ MAX_BODY_EDGES    ];

	PhysicsBody(); //The constructor is empty

	void AddEdge  ( Edge*   E );
	void AddVertex( Vertex* V );
	void ProjectToAxis( Vector& Axis, float& Min, float& Max );
	void CalculateCenter(); //Calculates the venter of mass

	void CreateBox( int X, int Y, int Width, int Height, bool is_const ); //Helper function to create a box primitive
};

struct Vertex
{
	Vector Position;
	Vector OldPosition;
	Vector Acceleration;

	PhysicsBody* Parent;

	bool constrait;
	float force;

	Vertex( PhysicsBody* Body, float PosX, float PosY, bool is_const = false ); //Constructor
};

struct Edge
{
	Vertex* V1;
	Vertex* V2;

	float Length;
	int Boundary; //Value used for optimization - see Physics::DetectCollision for more information

	PhysicsBody* Parent;

	Edge( PhysicsBody* Body, Vertex* pV1, Vertex* pV2, int pBoundary = true ); //Constructor
};


void Physics::AddBody( PhysicsBody* Body )
{
	Bodies[ BodyCount++ ] = Body;
}

void Physics::AddVertex( Vertex* V )
{
	Vertices[ VertexCount++ ] = V;
}

void Physics::AddEdge( Edge* E )
{
	Edges[ EdgeCount++ ] = E;
}

void Physics::Clear()
{
	BodyCount = 0;
	VertexCount = 0;
	EdgeCount = 0;
}

//Sets the force on each vertex to the gravity force. You could of course apply other forces like magnetism etc.
void Physics::UpdateForces()
{
	for( int I = 0; I < VertexCount; I++ )
	{
		if (Vertices[ I ]->force>0.01f)
		{
			Vertices[ I ]->Acceleration = -Gravity * Vertices[ I ]->force;
		}
		else
		{
			Vertices[ I ]->Acceleration = Gravity;
		}
	}
}

void Physics::UpdateVerlet()
{ //Updates the vertex position as explained in the article
	for( int I = 0; I < VertexCount; I++ )
	{
		Vertex& V = *Vertices[ I ];

		Vector Temp = V.Position;
		V.Position += V.Position - V.OldPosition + V.Acceleration*Timestep*Timestep;
		V.OldPosition = Temp;
	}
}

void Physics::UpdateEdges()
{
	for( int I = 0; I < EdgeCount; I++ )
	{
		Edge& E = *Edges[ I ];

		Vector V1V2 = E.V2->Position - E.V1->Position; //Calculate the vector between the two vertices

		float V1V2Length = V1V2.Length(); //Calculate the current distance
		float Diff       = V1V2Length - E.Length; //Calculate the difference from the original length
		
		V1V2.Normalize();

		if (!E.V1->constrait) E.V1->Position += V1V2*Diff*0.15f * Timestep; //Push both vertices apart by half of the difference respectively so the distance between them equals the original length
		if (!E.V2->constrait) E.V2->Position -= V1V2*Diff*0.15f * Timestep;
	}
}

void Physics::IterateCollisions()
{
	for( int I = 0; I < Iterations; I++ ) { //Repeat this a few times to give more exact results

		//A small 'hack' that keeps the vertices inside the screen. You could of course implement static objects and create
		//four to serve as screen boundaries, but the max/min method is faster
		for( int T = 0; T < VertexCount; T++ )
		{
			Vector& Pos = Vertices[ T ]->Position;
	
			Pos.x = max( min( Pos.x, (float)GWidth  ), 0.0f );
			Pos.y = max( min( Pos.y, (float)GHeight ), 0.0f );
		}

		UpdateEdges(); //Edge correction step

		for( int I = 0; I < BodyCount; I++ )
		{
			Bodies[ I ]->CalculateCenter(); //Recalculate the center
		}

		for( int B1 = 0; B1 < BodyCount; B1++ ) { //Iterate trough all bodies
			for( int B2 = 0; B2 < BodyCount; B2++ ) {
				if( B1 != B2 )
					if( BodiesOverlap( Bodies[ B1 ], Bodies[ B2 ] ) ) //Test the bounding boxes
						if( DetectCollision( Bodies[ B1 ], Bodies[ B2 ] ) ) //If there is a collision, respond to it
							ProcessCollision();
			}
		}
	}
}

bool Physics::DetectCollision( PhysicsBody* B1, PhysicsBody* B2 )
{
	float MinDistance = 10000.0f; //Initialize the length of the collision vector to a relatively large value
	
	for( int I = 0; I < B1->EdgeCount + B2->EdgeCount; I++ )
	{ //Just a fancy way of iterating through all of the edges of both bodies at once
		Edge* E;

		if( I < B1->EdgeCount )
			E = B1->Edges[ I ];
		else
			E = B2->Edges[ I - B1->EdgeCount ];

		//This will skip edges that lie totally inside the bodies, as they don't matter.
		//The boundary flag has to be set manually and defaults to true
		if( !E->Boundary )
			continue;

		Vector Axis( E->V1->Position.y - E->V2->Position.y, E->V2->Position.x - E->V1->Position.x ); //Calculate the perpendicular to this edge and normalize it
		Axis.Normalize();

		float MinA, MinB, MaxA, MaxB; //Project both bodies onto the perpendicular
		B1->ProjectToAxis( Axis, MinA, MaxA );
		B2->ProjectToAxis( Axis, MinB, MaxB );

		float Distance = IntervalDistance( MinA, MaxA, MinB, MaxB ); //Calculate the distance between the two intervals

		if( Distance > 0.0f ) //If the intervals don't overlap, return, since there is no collision
			return false;
		else if( abs( Distance ) < MinDistance ) {
			MinDistance = abs( Distance );

			CollisionInfo.Normal = Axis; //Save collision information for later
			CollisionInfo.E      = E;    //Store the edge, as it is the collision edge
		}
	}

	CollisionInfo.Depth = MinDistance;

	if( CollisionInfo.E->Parent != B2 ) { //Ensure that the body containing the collision edge lies in B2 and the one conatining the collision vertex in B1
		PhysicsBody* Temp = B2;
		B2 = B1;
		B1 = Temp;
	}

	int Sign = 0;
	
	float dt = CollisionInfo.Normal | ( B1->Center - B2->Center );

	if (dt>0) Sign = 1;
	if (dt<0) Sign =-1;
	
	//Remember that the line equation is N*( R - R0 ). We choose B2->Center as R0; the normal N is given by the collision normal

	if( Sign != 1 )
		CollisionInfo.Normal = -CollisionInfo.Normal; //Revert the collision normal if it points away from B1

	Vector CollisionVector = CollisionInfo.Normal*CollisionInfo.Depth;

	float SmallestD = 10000.0f; //Initialize the smallest distance to a large value
	for( int I = 0; I < B1->VertexCount; I++ )
	{
		float Distance = CollisionInfo.Normal | ( B1->Vertices[ I ]->Position - B2->Center ); //Measure the distance of the vertex from the line using the line equation

		if( Distance < SmallestD )
		{ 
			//If the measured distance is smaller than the smallest distance reported so far, set the smallest distance and the collision vertex
			SmallestD = Distance;
			CollisionInfo.V = B1->Vertices[ I ];
		}
	}

	return true; //There is no separating axis. Report a collision!
}

void Physics::ProcessCollision()
{
	Vertex* E1 = CollisionInfo.E->V1;
	Vertex* E2 = CollisionInfo.E->V2;

	Vector CollisionVector = CollisionInfo.Normal*CollisionInfo.Depth;

	float T;
	if( abs( E1->Position.x - E2->Position.x ) > abs( E1->Position.y - E2->Position.y ) )
		T = ( CollisionInfo.V->Position.x - CollisionVector.x - E1->Position.x )/(  E2->Position.x - E1->Position.x );
	else
		T = ( CollisionInfo.V->Position.y - CollisionVector.y - E1->Position.y )/(  E2->Position.y - E1->Position.y );

	float Lambda = 1.0f/( T*T + ( 1 - T )*( 1 - T ) );

	if (!E1->constrait) E1->Position -= CollisionVector*( 1 - T )*0.5f*Lambda;
	if (!E2->constrait) E2->Position -= CollisionVector*      T  *0.5f*Lambda;
	
	CollisionInfo.V->Position += CollisionVector*0.5f;
}

float Physics::IntervalDistance( float MinA, float MaxA, float MinB, float MaxB )
{
	if( MinA < MinB )
		return MinB - MaxA;
	else
		return MinA - MaxB;
}

bool Physics::BodiesOverlap( PhysicsBody* B1, PhysicsBody* B2 )
{ //Simple bounding box overlapping test
	return ( B1->MinX <= B2->MaxX ) && ( B1->MinY <= B2->MaxY ) && ( B1->MaxX >= B2->MinX ) && ( B2->MaxY >= B1->MinY );
}

void Physics::Update() {
	UpdateForces();
	UpdateVerlet();
	IterateCollisions();
}

void Physics::Render()
{ //Basic OGL rendering. Nothing fancy :)
	//glColor3f( 1.0f, 0.0f, 0.0f );

	//glBegin( GL_LINES );
	for( int I = 0; I < EdgeCount; I++ )
	{
		core.Render()->DrawLine2D(Edges[ I ]->V1->Position, 0xff0000ff,
								  Edges[ I ]->V2->Position, 0xff0000ff );
	}
	//glEnd();

}

Vertex* Physics::FindVertex( int X, int Y ) { //Helper function that finds the vertex laying closest to a given point
	Vertex* NearestVertex = 0;
	float MinDistance = 1000.0f;

	Vector Coords( (float)X, (float)Y );

	for( int I = 0; I < VertexCount; I++ )
	{
		float Distance = ( Vertices[ I ]->Position - Coords ).Length2();

		if( Distance < MinDistance ) {
			NearestVertex = Vertices[ I ];
			MinDistance = Distance;
		}
	}

	return NearestVertex;
}

Edge::Edge( PhysicsBody* Body, Vertex* pV1, Vertex* pV2, int pBoundary )
{
	//Constructor
	V1 = pV1; //Set boundary vertices
	V2 = pV2;

	Length   = ( pV2->Position - pV1->Position ).Length(); //Calculate the original length
	Boundary = pBoundary;

	Parent = Body;

	Body->AddEdge( this ); //Add the edge to the given body and to the physics simulator
	World.AddEdge( this );
}

Vertex::Vertex( PhysicsBody* Body, float PosX, float PosY, bool is_const)
{
	Position    = Vector( PosX, PosY );
	OldPosition = Vector( PosX, PosY );

	Parent = Body;

	Body->AddVertex( this ); //Add the vertex to the given body and to the physics simulator
	constrait = is_const;
	force = 0.0f;
	if (!is_const) World.AddVertex( this );
}

PhysicsBody::PhysicsBody()
{ //Constructor
	 VertexCount = EdgeCount = 0;

	 World.AddBody( this ); //Add body to the physics simulator
}

void PhysicsBody::AddEdge( Edge* E )
{
	Edges[ EdgeCount++ ] = E;
}

void PhysicsBody::AddVertex( Vertex *V )
{
	Vertices[ VertexCount++ ] = V;
}

void PhysicsBody::ProjectToAxis( Vector& Axis, float& Min, float& Max )
{
	float DotP = Axis | Vertices[ 0 ]->Position;

	Min = Max = DotP; //Set the minimum and maximum values to the projection of the first vertex

	for( int I = 0; I < VertexCount; I++ )
	{
		DotP = Axis | Vertices[ I ]->Position; //Project the rest of the vertices onto the axis and extend the interval to the left/right if necessary

		Min = min( DotP, Min );
		Max = max( DotP, Max );
	}
}

void PhysicsBody::CalculateCenter()
{
	//Recalculates the center of mass and the bounding box
	Center = Vector( 0.0f, 0.0f );

	MinX = MinY =  10000.0f;
	MaxX = MaxY = -10000.0f;

	for( int I = 0; I < VertexCount; I++ )
	{
		Center += Vertices[ I ]->Position;

		MinX = min( MinX, Vertices[ I ]->Position.x );
		MinY = min( MinY, Vertices[ I ]->Position.y );
		MaxX = max( MaxX, Vertices[ I ]->Position.x );
		MaxY = max( MaxX, Vertices[ I ]->Position.y );
	}

	Center.x /= VertexCount;
	Center.y /= VertexCount;
}

void PhysicsBody::CreateBox( int X, int Y, int Width, int Height, bool is_const )
{
	//Creates a box primitive
	Vertex* V1 = new Vertex( this, X        , Y          , is_const);
	Vertex* V2 = new Vertex( this, X + Width, Y          , is_const);
	Vertex* V3 = new Vertex( this, X + Width, Y + Height , is_const);
	Vertex* V4 = new Vertex( this, X        , Y + Height , is_const);

	new Edge( this, V1, V2, true );
	new Edge( this, V2, V3, true );
	new Edge( this, V3, V4, true );
	new Edge( this, V4, V1, true );

	new Edge( this, V1, V3, false );
	new Edge( this, V2, V4, false );
}
*/



void SpiderHero::Init()
{	
	PROP_REG( x )
	PROP_REG( y )
	PROP_REG( height )
	PROP_REG( width )
	PROP_REG( texName )

	SetUpdate((TaskHolder::Task)&SpiderHero::Render, lvl_gui2);	

	tex = null;
	blowed = false;

	world = null;
}

void SpiderHero::ReInit()
{
	if (tex)
	{
		tex->Release();
		tex = null;		
	}

	tex = core.Render()->LoadTexture(texName);

//	rope = null;
}

void SpiderHero::Start()
{
	num_collisions = 0;
	num_hooks = 0;
	num_active_hooks = 0;
	blowed = false;

	world = new b2World(b2Vec2(0.0f, 10.0f), true);
	workTime = 0.0f;

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(x + width * 0.5f, y + height * 0.5f);
	hero_body = world->CreateBody(&bodyDef);

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(width * 0.5f, height * 0.5f);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 0.01f;
	fixtureDef.friction = 0.3f;
	fixtureDef.filter.groupIndex = -1;

	hero_body->CreateFixture(&fixtureDef);

	b2Body* ground = NULL;
	{
		b2BodyDef bd;
		ground = world->CreateBody(&bd);

		b2PolygonShape shape;
		shape.SetAsEdge(b2Vec2(-40.0f, 0.0f), b2Vec2(40.0f, 0.0f));
		ground->CreateFixture(&shape, 0.0f);
	}

	{
		b2PolygonShape shape;
		shape.SetAsBox(0.6f, 0.125f);

		b2FixtureDef fd;
		fd.shape = &shape;
		fd.density = 20.0f;
		fd.friction = 0.2f;

		b2RevoluteJointDef jd;
		jd.collideConnected = false;

		const float32 y = 25.0f;
		b2Body* prevBody = ground;
		for (int32 i = 0; i < 30; ++i)
		{
			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			bd.position.Set(0.5f + i, y);
			bodies[i] = world->CreateBody(&bd);
			bodies[i]->CreateFixture(&fd);

			b2Vec2 anchor(float32(i), y);
			jd.Initialize(prevBody, bodies[i], anchor);
			world->CreateJoint(&jd);

			prevBody = bodies[i];
		}
	}
}

void SpiderHero::Finish()
{
	delete world;
	world = null;
	/*if (rope)
	{
		World.Clear();
		delete rope;
				rope = null;
				
		for (int i=0;i<num_active_hooks;i++)
		{
			delete edge[i];
		}

		for (int i=0;i<num_active_hooks+1;i++)
		{
			delete vertices[i];
		}
	}*/
}

void SpiderHero::Render(float dt, int level)
{	
	core.Render()->EnableRenderState(RenderService::RS_ALPHABLEND);
	core.Render()->SetBlendParams(RenderService::BO_ADD, RenderService::BFA_SRC_ALPHA, RenderService::BFB_INV_SRC_ALPHA);
				

	/*World.Timestep = dt * 20.0f;
	World.Update();
	World.Render();*/

	float angle = 0.0f;

	if (Scene()->IsRunning())
	{
		workTime += dt * 3.0f;

		float step = 1.0f/60.0f;

		while (workTime > step)
		{
			world->Step(step, 10, 10);
			workTime -= step;
		}

		Vector pos(x + width * 0.5, y + height * 0.5);

		b2Vec2 position = hero_body->GetPosition();
		angle = hero_body->GetAngle();

		x = position.x - width * 0.5;
		y = position.y - height * 0.5;

		//if (num_active_hooks>0)
		{
			//x = vertices[0]->Position.x - width * 0.5;
			//y = vertices[0]->Position.y - height * 0.5;
		}
		/*else
		{
			y += dt * 30;
		}*/

		for (int32 i = 1; i < 30; ++i)
		{
			Vector pos1;
			Vector pos2;

			b2Vec2 position = bodies[i-1]->GetPosition();

			pos1.x = position.x;
			pos1.y = position.y;
			
			position = bodies[i]->GetPosition();

			pos2.x = position.x;
			pos2.y = position.y;

			core.Render()->DrawLine2D(pos1, 0xffff0000,
									  pos2, 0xffff0000);
		}



		for (int j=0;j<num_active_hooks;j++)
		{
			for (int i=1;i<active_hooks[j].num_segments;i++)
			{
				Vector pos1;
				Vector pos2;

				b2Vec2 position = active_hooks[j].segments[i-1]->GetPosition();

				pos1.x = position.x;
				pos1.y = position.y;
				
				position = active_hooks[j].segments[i]->GetPosition();

				pos2.x = position.x;
				pos2.y = position.y;

				core.Render()->DrawLine2D(pos1, 0xffff0000,
										  pos2, 0xffff0000);
			}
		}

		if (core.Controls()->MousePressed(0,ControlsService::Activated))
		{
			float mx = core.Controls()->GetMouseX();
			float my = core.Controls()->GetMouseY();

			for (int i=0;i<num_hooks;i++)
			{
				if (hooks[i]->x<mx && mx<hooks[i]->x + hooks[i]->width &&
					hooks[i]->y<my && my<hooks[i]->y + hooks[i]->height)
				{
					GrabHook(std::string(hooks[i]->GetName()));
					break;
				}
			}

			if (x<mx && mx<x + width &&
				y<my && my<y + height)
			{
				blowed = !blowed;

				/*float force = 0.0f;
				if (blowed)
				{
					if (num_active_hooks == 0)
					{
						force = 0.75f;
					}
					else
					if (num_active_hooks == 1)
					{
						force = 8.75f;
					}
					else
					{
						force = 12.75f;
					}
				}

				vertices[0]->force = force;*/
			}
		}
	}

	float k = 1.0;

	if (blowed)
	{
		k = 1.5f;
	}
	
	core.Render()->DrawSprite(tex,Vector(x, y), Vector(width, height) * k, angle, 0xffffffff);
		

	core.Render()->DisableRenderState(RenderService::RS_ALPHABLEND);
}

void SpiderHero::MethodsToScript()
{
	core.Script()->Machine()->RegisterObjectMethod("SpiderHero", "void AddCollision(string& name)", asMETHOD(SpiderHero,AddCollision), asCALL_THISCALL);
	core.Script()->Machine()->RegisterObjectMethod("SpiderHero", "void AddHook(string& name)", asMETHOD(SpiderHero,AddHook), asCALL_THISCALL);
	core.Script()->Machine()->RegisterObjectMethod("SpiderHero", "void GrabHook(string& name)", asMETHOD(SpiderHero,GrabHook), asCALL_THISCALL);
}

void SpiderHero::AddCollision(std::string& name)
{
	SceneEntity* entity = Scene()->Find(name.c_str());

	if (entity)
	{
		if (entity->CheckClassName("GUIWidget"))
		{
			GUILabel* label = (GUILabel*)entity;
			collisions[num_collisions].label = label;
			
			collisions[num_collisions].def.position.Set(label->x + label->width * 0.5f,
														label->y + label->height * 0.5f);

			collisions[num_collisions].body = world->CreateBody(&collisions[num_collisions].def);

			collisions[num_collisions].shape.SetAsBox(label->width * 0.5f, label->height * 0.5f);

			collisions[num_collisions].body->CreateFixture(&collisions[num_collisions].shape, 10);

			num_collisions++;
		}
	}
}

void SpiderHero::AddHook(std::string& name)
{
	SceneEntity* entity = Scene()->Find(name.c_str());

	if (entity)
	{
		if (entity->CheckClassName("GUIWidget"))
		{
			hooks[num_hooks] = (GUILabel*)entity;
			num_hooks++;
		}
	}
}

void SpiderHero::ConnectToHook()
{
	Vector pos(x + width * 0.5, y + height * 0.5);

	GUILabel* label = hooks[active_hooks[num_active_hooks-1].index];
	b2Vec2 hk(label->x + label->width * 0.5,
			  label->y + label->height * 0.5);

	int num = 15;

	active_hooks[num_active_hooks-1].num_segments = num;

	b2PolygonShape shape;
	shape.SetAsBox(1.0f, 1.0f);

	b2FixtureDef fd;
	fd.shape = &shape;
	fd.density = 10.0f;
	fd.friction = 0.8f;
	fd.filter.groupIndex = -1;

	b2RevoluteJointDef jd;
	jd.collideConnected = false;

	b2Body* prevBody = collisions[0].body;

	for (int i=0; i<active_hooks[num_active_hooks-1].num_segments;i++)
	{
		float blend0 = 1.0f - ((float)i-1.0f)/(active_hooks[num_active_hooks-1].num_segments-1);
		float blend1 = 1.0f - ((float)i-0.5f)/(active_hooks[num_active_hooks-1].num_segments-1);
		float blend2 = 1.0f - (float)i    /(active_hooks[num_active_hooks-1].num_segments-1);

		b2Vec2 pt0(pos.x + (hk.x - pos.x) * blend0, pos.y + (hk.y - pos.y) * blend0);
		b2Vec2 pt1(pos.x + (hk.x - pos.x) * blend1, pos.y + (hk.y - pos.y) * blend1);
		b2Vec2 pt2(pos.x + (hk.x - pos.x) * blend2, pos.y + (hk.y - pos.y) * blend2);
		
		b2BodyDef bd;
		bd.type = b2_dynamicBody;
		bd.position.Set(pt2.x, pt2.y);
		active_hooks[num_active_hooks-1].segments[i] = world->CreateBody(&bd);
		active_hooks[num_active_hooks-1].segments[i]->CreateFixture(&fd);

		jd.Initialize(prevBody, active_hooks[num_active_hooks-1].segments[i], pt1);
		world->CreateJoint(&jd);

		prevBody = active_hooks[num_active_hooks-1].segments[i];
	}

	//jd.Initialize(hero_body, active_hooks[num_active_hooks-1].segments[num-1], b2Vec2(pos.x,pos.y));
	//world->CreateJoint(&jd);
}

void SpiderHero::GrabHook(std::string& name)
{
	for (int i=0;i<num_hooks;i++)
	{
		if (String::IsEqual(hooks[i]->GetName(), name.c_str()))
		{
			Vector pos(x + width * 0.5, y + height * 0.5);

			Vector last_pos = pos;

			/*if (rope)
			{
				last_pos =  vertices[0]->OldPosition;

				World.Clear();
				delete rope;
				rope = null;
				
				for (int i=0;i<num_active_hooks;i++)
				{
					delete edge[i];
				}

				for (int i=0;i<num_active_hooks+1;i++)
				{
					delete vertices[i];
				}
			}*/

			bool need_add = true;

			for (int j=0;j<num_active_hooks;j++)
			{
				if (active_hooks[j].index == i)
				{
					need_add = false;

					for (int k=j;k<num_active_hooks-1;k++)
					{
						active_hooks[k] = active_hooks[k+1];
					}

					num_active_hooks--;

					break;
				}
			}

			if (need_add)
			{
				GUILabel* label = hooks[i];

				Vector hk (label->x + label->width * 0.5,
							label->y + label->height * 0.5);

				float len = (pos - hk).Length();

				if (num_active_hooks<2 && len <100.0f)
				{
					active_hooks[num_active_hooks].index = i;
					num_active_hooks++;
					ConnectToHook();
				}
			}

			//if (num_active_hooks > 0)
			/*{
				rope = new PhysicsBody();
			}

			if (num_active_hooks == 0)
			{
				vertices[0] = new Vertex( rope, pos.x, pos.y, false );
				vertices[0]->OldPosition = last_pos;
			}

			float dist = 55.0f;
			
			if (num_active_hooks == 1)
			{
				float len = dist;

				GUILabel* label = hooks[active_hooks[0]];
			
				Vector hk (label->x + label->width * 0.5,
							label->y + label->height * 0.5);

				int num = 10;

				for (int i=0;i<num;i++)
				{
					float blend = 1.0f - (float)i/(num-1);

					bool cns = false;
					if (i==num-1) cns = true;

					vertices[i] = new Vertex( rope, hk.x + (pos.x - hk.x) * blend,
													hk.y + (pos.y - hk.y) * blend, cns  );

					if (i!=0)
					{
						edge[i-1] = new Edge( rope, vertices[i-1], vertices[i] );
						edge[i-1]->Length = (float) dist / (num-1);
					}
				}

				vertices[0]->OldPosition = last_pos;
			}
	
			if (num_active_hooks == 2)
			{
				float len = dist;

				GUILabel* label = hooks[active_hooks[0]];
			
				Vector hk (label->x + label->width * 0.5,
							label->y + label->height * 0.5);

				int num = 10;

				for (int i=0;i<num;i++)
				{
					float blend = 1.0f - (float)i/(num-1);

					bool cns = false;
					if (i==num-1) cns = true;

					vertices[i] = new Vertex( rope, hk.x + (pos.x - hk.x) * blend,
													hk.y + (pos.y - hk.y) * blend, cns  );

					if (i!=0)
					{
						edge[i-1] = new Edge( rope, vertices[i-1], vertices[i] );
						edge[i-1]->Length = (float) dist / (num-1);
					}
				}

				vertices[0]->OldPosition = last_pos;

				label = hooks[active_hooks[1]];

				hk = Vector(label->x + label->width * 0.5,
							 label->y + label->height * 0.5);

				for (int i=1;i<num;i++)
				{
					float blend = 1.0f - (float)i/(num-1);

					bool cns = false;
					if (i==num-1) cns = true;

					vertices[i+num - 1] = new Vertex( rope, hk.x + (pos.x - hk.x) * blend,
													hk.y + (pos.y - hk.y) * blend, cns  );

					if (i==1)
					{
						edge[num - 2 + i-1] = new Edge( rope, vertices[0], vertices[i+num - 1] );
						edge[num - 2 + i-1]->Length = (float) dist / (num-1);
					}
					else
					if (i>1)
					{
						edge[num - 2 + i-1] = new Edge( rope, vertices[i-1+num - 1], vertices[i+num - 1] );
						edge[num - 2 + i-1]->Length = (float) dist / (num-1);
					}
				}

			}

			if (num_active_hooks == 3)
			{
				GUILabel* label = hooks[active_hooks[0]];
				
				Vector hk (label->x + label->width * 0.5,
							label->y + label->height * 0.5);

				label = hooks[active_hooks[1]];

				Vector hk2(label->x + label->width * 0.5,
							label->y + label->height * 0.5);

				label = hooks[active_hooks[2]];

				Vector hk3(label->x + label->width * 0.5,
							label->y + label->height * 0.5);

				float Side12 = (hk - hk2).Length(); 
				float Side23 = (hk2 - hk3).Length(); 
				float Side31 = (hk - hk3).Length(); 

				float Perim = 1 / (Side12 + Side23 + Side31); 

				Vector center((Side23 * hk.x + Side31 * hk2.x + Side12 * hk3.x) * Perim,
							   (Side23 * hk.y + Side31 * hk2.y + Side12 * hk3.y) * Perim);

				vertices[0] = new Vertex( rope, pos.x, pos.y, false );
				vertices[1] = new Vertex( rope, hk.x, hk.y, true  );
				vertices[2] = new Vertex( rope, hk2.x, hk2.y, true  );
				vertices[3] = new Vertex( rope, hk3.x, hk3.y, true  );
  
				edge[0] = new Edge( rope, vertices[0], vertices[1]);
				edge[0]->Length = (hk  - center).Length();

				edge[1] = new Edge( rope, vertices[0], vertices[2]);
				edge[1]->Length = (hk2 - center).Length();

				edge[2] = new Edge( rope, vertices[0], vertices[3]);
				edge[2]->Length = (hk3 - center).Length();
			}
	
			float force = 0.0f;
				if (blowed)
				{
					if (num_active_hooks == 0)
					{
						force = 0.75f;
					}
					else
					if (num_active_hooks == 1)
					{
						force = 8.75f;
					}
					else
					{
						force = 12.75f;
					}
				}

				vertices[0]->force = force;

			for (int j=0;j<num_collisions;j++)
			{
				( new PhysicsBody() )->CreateBox( collisions[j]->x, collisions[j]->y,
												  collisions[j]->width, collisions[j]->height, true );
			}*/

			break;
		}
	}
}

void SpiderHero::SetGizmo(Gizmo* gz)
{
	if (gizmo)
	{
		gizmo->enabled = false;
		gizmo->mode2d = false;
	}
	
	gizmo = gz;
	
	if (gizmo)
	{
		gizmo->enabled = true;
		gizmo->mode2d = true;
	}
}

void SpiderHero::DataToGizmo()
{
	if (!gizmo) return;

	gizmo->transform.Identity();

	gizmo->transform._11 = width;
	gizmo->transform._22 = height;	

	gizmo->transform._41 = x;
	gizmo->transform._42 = y;
}

void SpiderHero::DataFromGizmo()
{
	if (!gizmo) return;

	width = gizmo->transform._11;
	height = gizmo->transform._22;

	x = gizmo->transform._41;
	y = gizmo->transform._42;

	ReverseTieValues();
}
