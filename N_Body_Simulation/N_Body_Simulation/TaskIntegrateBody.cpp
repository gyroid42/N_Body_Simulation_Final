
// class header include
#include "TaskIntegrateBody.h"

// my class includes
#include "Body.h"


TaskIntegrateBody::TaskIntegrateBody()
{
}


TaskIntegrateBody::~TaskIntegrateBody()
{
}


void TaskIntegrateBody::Init(Body* newBody, float deltaTime) {

	// set reference to body and set teh delta time
	body_ = newBody;
	dt_ = deltaTime;
}


void TaskIntegrateBody::Run() {

	// run method to integrate the body
	body_->Integrate_SemiImplicitEuler(dt_);
}
