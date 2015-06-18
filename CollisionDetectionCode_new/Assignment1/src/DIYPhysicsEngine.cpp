#include "DIYPhysicsEngine.h"
//rigid body functions
using namespace std;

typedef bool(*fn)(DIYPhysicScene*,PhysicsObject*, PhysicsObject*);

static fn FunctionPointerTable[] =
{
    0,                              DIYPhysicScene::Plane2Sphere,   DIYPhysicScene::Plane2Box,
    DIYPhysicScene::Sphere2Plane,   DIYPhysicScene::Sphere2Sphere,  DIYPhysicScene::Sphere2Box,
    DIYPhysicScene::Box2Plane,		DIYPhysicScene::Box2Sphere,     DIYPhysicScene::Box2Box
};


void DIYPhysicScene::checkForCollisions()
{
    int actor_count = actors.size();

    for (int first_actor = 0;
        first_actor < actor_count - 1;
        ++first_actor)
    {
        for (int second_actor = first_actor + 1;
            second_actor < actor_count;
            ++second_actor)
        {
            PhysicsObject * object1 = actors[first_actor];
            PhysicsObject * object2 = actors[second_actor];

            int shapeid1 = object1->_shapeID;
            int shapeid2 = object2->_shapeID;

            int index = (shapeid1 * NUMBERSHAPE) + shapeid2;

            fn collision_function = FunctionPointerTable[index];

            if (collision_function != nullptr)
            {
                collision_function(this,object1, object2);
            }
        }
    }
}   
 

//plane class functions
PlaneClass::PlaneClass(glm::vec2 normal,float distance)
{
	this->normal = glm::normalize(normal);
	this->distance = distance;
	_shapeID = PLANE;
}

void PlaneClass::makeGizmo()
{
	float lineSegmentLength = 100;
	glm::vec2 centrePoint = normal * distance;
	glm::vec2 parallel = glm::vec2(normal.y,-normal.x); //easy to rotate normal through 90degrees around z
	glm::vec4 colour(1,1,1,1);
	glm::vec2 start = centrePoint + (parallel * lineSegmentLength);
	glm::vec2 end = centrePoint - (parallel * lineSegmentLength);
	Gizmos::add2DLine(start.xy(),end.xy(),colour);
}

//sphere class functions

SphereClass::SphereClass(	glm::vec2 position,glm::vec2 velocity,float radius,float mass,glm::vec4& colour)
	: DIYRigidBody(position,velocity,0,mass)  //call the base class constructor
{
	this->_radius = radius;
	this->colour = colour;
	std::cout<<"adding sphere "<<this->position.x<<','<<this->position.y<<std::endl;
	_shapeID = SPHERE;
}

SphereClass::SphereClass(	glm::vec2 position,float angle,float speed,float radius,float mass,glm::vec4& colour)
		: DIYRigidBody(position,glm::vec2(speed * cos(angle),speed * sin(angle)),0,mass)  //call the base class constructor
{
	this->_radius = radius;
	this->colour = colour;
	std::cout<<"adding sphere "<<this->position.x<<','<<this->position.y<<std::endl;
	_shapeID = SPHERE;
}

void SphereClass::makeGizmo()
{
	glm::vec2 center = position.xy();
	Gizmos::add2DCircle(center, _radius,30, colour);
}

//box class functions

BoxClass::BoxClass(	glm::vec2 position,glm::vec2 velocity,float rotation,float mass,float width, float height,glm::vec4& colour)
	: DIYRigidBody(position,velocity,rotation,mass)  //call the base class constructor
{
	this->width = width;
	this->height = height;
	this->colour = colour;
	this->is_colliding = false;
	_shapeID = BOX;
}

BoxClass::BoxClass(	glm::vec2 position, float angle, float speed, float rotation, float width, float height, float mass, glm::vec4& colour)
	: DIYRigidBody(position,glm::vec2(speed * cos(angle),speed * sin(angle)),rotation,mass)  //call the base class constructor
{
	this->width = width;
	this->height = height;
	this->colour = colour;
	this->is_colliding = false;
	_shapeID = BOX;
}

void BoxClass::makeGizmo()
{
	//Get a rotation matrix from our 2D rotation so we can draw our gizmo
	glm::vec4 my_colour(1, 0, 0, 1);

	if (this->is_colliding)
	{
		my_colour = glm::vec4(0, 1, 0, 1);
	}

	Gizmos::add2DAABBFilled(position, glm::vec2(width, height), my_colour, &rotationMatrix);
}

DIYRigidBody::DIYRigidBody(	glm::vec2 position,glm::vec2 velocity,float rotation,float mass)
{
	std::cout<<"adding rigid body "<<position.x<<','<<position.y<<std::endl;
	this->position = position;
	this->velocity = velocity;
	this->rotation2D = rotation;
	this->mass = mass;
	this->dybanic_friction = 0.2f;

	colour = glm::vec4(1,1,1,1); //white by default
}

void DIYRigidBody::applyForce(glm::vec2 force)
{
		velocity += force/mass;
}

void DIYRigidBody::applyForceToActor(DIYRigidBody* actor2, glm::vec2 force)
{
	actor2->applyForce(force);
	applyForce(-force);
}

void DIYRigidBody::collisionResponse(glm::vec2 collisionPoint)
{
	// for now we'll just set the velocity to zero
	collisionPoint = collisionPoint;
}

void DIYRigidBody::update(glm::vec2 gravity,float timeStep)
{

	applyForce(gravity * mass * timeStep);
	oldPosition = position; //keep our old position for collision response
	position += velocity * timeStep;
	rotationMatrix = glm::rotate(rotation2D,glm::vec3(0.0f,0.0f,1.0f)); 

	float normal_force = this->mass;

	glm::vec2 friction_force_vector = -normal_force * this->dybanic_friction * this->velocity;

	glm::vec2 friction_acceleration = friction_force_vector / this->mass;
	this->velocity += friction_acceleration * timeStep;

}

void DIYRigidBody::debug()
{
	cout<<"position "<<position.x<<','<<position.y<<endl;
}

//scene functions

void DIYPhysicScene::addActor(PhysicsObject* object)
{
	actors.push_back(object);
}
	
void DIYPhysicScene::removeActor(PhysicsObject* object)
{
	auto item = std::find(actors.begin(), actors.end(), object);
	if(item < actors.end())
	{
		actors.erase(item);
	}
}

void DIYPhysicScene::upDate()
{
	bool runPhysics = true;
	int maxIterations = 10; //emergency count to stop us repeating for ever in extreme situations

	for(auto actorPtr:actors)
	{
		actorPtr->update(gravity,timeStep);
	}

    //ADD THIS
    if (collisionEnabled)
    {
        checkForCollisions();
    }

	maxIterations--;
}








void DIYPhysicScene::debugScene()
{
	int count = 0;
	for (auto actorPtr:actors)
	{
		cout<<count<<" : ";
		actorPtr->debug();
		count++;
	}
}

void DIYPhysicScene::upDateGizmos()
{
	for (auto actorPtr:actors)
	{
		actorPtr->makeGizmo();
	}
}

bool DIYPhysicScene::Sphere2Sphere(DIYPhysicScene* scene ,PhysicsObject* first, PhysicsObject* second)
{
    SphereClass * first_sphere = (SphereClass*)first;
    SphereClass * second_sphere = (SphereClass*)second;

    //get the vector from the first to the second sphere
    glm::vec2 delta = second_sphere->position - first_sphere->position;

    //the length of the delta is the distance
    float distance = glm::length(delta);
    
    float raddii_sum = first_sphere->_radius + second_sphere->_radius;

    if (distance < raddii_sum)
    {
        //get the normal of the collision
        glm::vec2 collision_normal = glm::normalize(delta);

        //update velocities

		//split velocities into normal and non-normal
		glm::vec2 first_in_line_with_normal = collision_normal * glm::dot(collision_normal, first_sphere->velocity);
		glm::vec2 first_perpendicular_to_normal = first_sphere->velocity - first_in_line_with_normal;

		glm::vec2 second_in_line_with_normal = collision_normal * glm::dot(collision_normal, second_sphere->velocity);
		glm::vec2 second_perpendicular_to_normal = second_sphere->velocity - second_in_line_with_normal;

		//get mass and velocity amounts in line with the normal
		float u1 = glm::dot(collision_normal, first_sphere->velocity);
		float m1 = first_sphere->mass;

		float u2 = glm::dot(collision_normal, second_sphere->velocity);
		float m2 = second_sphere->mass;

		//conservation of energy
		float v1 = (u1 * (m1 - m2) / (m1 + m2) + 2 * m2 * u2) / (m1 + m2);
		float v2 = (u2 * (m2 - m1) / (m1 + m2) + 2 * m1 * u1) / (m1 + m2);

		float restitution = 0.8f;
		v1 *= restitution;
		v2 *= restitution;

		//rebuild velocity vectors
		glm::vec2 new_first_in_line_with_normal = collision_normal * v1;
		glm::vec2 new_second_in_line_with_normal = collision_normal * v2;

		glm::vec2 new_first_velocity = new_first_in_line_with_normal + first_perpendicular_to_normal;
		glm::vec2 new_second_velocity = new_second_in_line_with_normal + second_perpendicular_to_normal;

		first_sphere->velocity = new_first_velocity;
		second_sphere->velocity = new_second_velocity;

		//move circles apart
		float intersection = raddii_sum - distance;
		first_sphere->position -= intersection * collision_normal;
		second_sphere->position += intersection * collision_normal;



        return true;
    }

    return false;
}


bool DIYPhysicScene::Sphere2Plane(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second)
{
    SphereClass* sphere = (SphereClass*)first;
    PlaneClass* plane = (PlaneClass*)second;

    float perpendicular_distance = 
        glm::dot(sphere->position, plane->normal) - plane->distance;

    if (perpendicular_distance < sphere->_radius)
    {
        float intersection = sphere->_radius - perpendicular_distance;

		glm::vec2 in_line_with_normal = plane->normal * glm::dot(plane->normal, sphere->velocity);
		glm::vec2 perpendicular = sphere->velocity - in_line_with_normal;

		float restitution = 0.8f;
		in_line_with_normal *= -1 * restitution;

		sphere->velocity = perpendicular + in_line_with_normal;
		sphere->position += plane->normal * intersection;


        return true;
    }

    return false;
}

bool DIYPhysicScene::Plane2Sphere(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second)
{
    return Sphere2Plane(scene,second, first);
}


void BuildBoxPoints(BoxClass* box, glm::vec2* points)
{
    points[0] =
        (box->rotationMatrix *
        glm::vec4(-box->width, -box->height, 0, 1)).xy()
        + box->position;
    points[1] =
        (box->rotationMatrix *
        glm::vec4(-box->width, box->height, 0, 1)).xy()
        + box->position;
    points[2] =
        (box->rotationMatrix *
        glm::vec4(box->width, box->height, 0, 1)).xy()
        + box->position;
    points[3] =
        (box->rotationMatrix *
        glm::vec4(box->width, -box->height, 0, 1)).xy()
        + box->position;
}


bool DIYPhysicScene::Box2Box(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second)
{
    BoxClass* first_box = (BoxClass*)first;
    BoxClass* second_box = (BoxClass*)second;

    glm::vec2 points[8];


    BuildBoxPoints(first_box, points);
    BuildBoxPoints(second_box, points + 4);

	glm::vec2* first_points = points;
	glm::vec2* second_points = points + 4;

	for (int box_index = 0; box_index < 2; box_index++)
	{


		for (int point_index = 0;
			point_index < 3;
			++point_index)
		{
			glm::vec2 edge_vector = first_points[point_index] - first_points[point_index + 1];
			edge_vector = glm::normalize(edge_vector);
			glm::vec2 perp_vector(edge_vector.y, -edge_vector.x);

			float first_min = FLT_MAX, first_max = -FLT_MAX;
			float second_min = FLT_MAX, second_max = -FLT_MAX;

			for (int check_index = 0;
				check_index < 4;
				++check_index)
			{
				float first_projected = glm::dot(first_points[check_index], perp_vector);
				if (first_projected < first_min) first_min = first_projected;
				if (first_projected > first_max) first_max = first_projected;

				float second_projected = glm::dot(second_points[check_index], perp_vector);
				if (second_projected < second_min) second_min = second_projected;
				if (second_projected > second_max) second_max = second_projected;
			}

			if (first_min > second_max || second_min > first_max)
			{
				return false;
			}
			else
			{

			}
		}

		first_points = points + 4;
		second_points = points;
	}

    //if we made it this far and did not return yet, that means they are colliding
	first_box->is_colliding = true;
	second_box->is_colliding = true;

    //collision response

    return true;
}

bool DIYPhysicScene::Box2Plane(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second)
{
    BoxClass* box = (BoxClass*)first;
    PlaneClass* plane = (PlaneClass*)second;

	glm::vec2 points[4];
	BuildBoxPoints(box, points);

	int point_sides = 0;

	for (int point_index = 0; point_index < 4; point_index++)
	{
		float distance = glm::dot(plane->normal, points[point_index]) - plane->distance;

		if (distance > 0)
		{
			++point_sides;
		}
		else
		{
			--point_sides;
		}
	}

	if (point_sides == 4 || point_sides == -4)
	{
		return false;
	}

	box->is_colliding = true;
    return true;
}

bool DIYPhysicScene::Plane2Box(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second)
{
    return Box2Plane(scene,second, first);
}

bool DIYPhysicScene::Sphere2Box(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second)
{
	return Box2Sphere(scene,second, first);
}

bool DIYPhysicScene::Box2Sphere(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second)
{
	BoxClass* box = (BoxClass*)first;
	SphereClass* sphere = (SphereClass*)second;

	glm::vec2 vector_to_circle = sphere->position - box->position;

	float sin_theta = sinf(-box->rotation2D);
	float cos_theta = cosf(-box->rotation2D);

	vector_to_circle = glm::vec2(cos_theta * vector_to_circle.x - sin_theta * vector_to_circle.y,
								 sin_theta * vector_to_circle.x + cos_theta * vector_to_circle.y);

	float dist_sq = 0;

	if (vector_to_circle.x > box->width)	//if the circle is to the right
	{
		float dist = (vector_to_circle.x - box->width);
		dist_sq += dist * dist;
	}
	else if (vector_to_circle.x < -box->width)	//else if the circle is to the left
	{
		float dist = (vector_to_circle.x + box->width);
		dist_sq += dist * dist;
	}

	if (vector_to_circle.y > box->height)	//if the circle is above
	{
		float dist = (vector_to_circle.y - box->height);
		dist_sq += dist * dist;
	}
	else if (vector_to_circle.y < -box->height)	//else if the circle is below
	{
		float dist = (vector_to_circle.y + box->height);
		dist_sq += dist * dist;
	}

	if (sphere->_radius * sphere->_radius > dist_sq)
	{
		box->is_colliding = true;
		return true;
	}


	return false;
}