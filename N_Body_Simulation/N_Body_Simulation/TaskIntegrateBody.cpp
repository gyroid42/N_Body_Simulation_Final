#include "TaskIntegrateBody.h"
#include "Body.h"


TaskIntegrateBody::TaskIntegrateBody()
{
}


TaskIntegrateBody::~TaskIntegrateBody()
{
}


void TaskIntegrateBody::Init(Body* newBody, float deltaTime) {

	body_ = newBody;
	dt_ = deltaTime;
}


void TaskIntegrateBody::Run() {

	body_->Integrate_SemiImplicitEuler(dt_);
}
