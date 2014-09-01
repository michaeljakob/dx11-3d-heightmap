#include "Camera.h"


const float Camera::DEFAULT_ROTATION_SPEED = (float) GH_DEGTORAD(45);
const float Camera::DEFAULT_MOVING_SPEED = 10;

using namespace std;

const int Camera::TOGGLE_INTERVAL = 256;

Camera::Camera() :
mDistanceFromFloor(-1.f), mPitch(0), mYaw(0), mRoll(0), mMovingSpeed(DEFAULT_MOVING_SPEED), mRotationSpeed(DEFAULT_ROTATION_SPEED), mToggleFreeMoveHelper(0)
{
	XMStoreFloat4(&mPosition,XMVectorSet(50, 60, 50, 1));
	XMStoreFloat4(&mLookAt,	 XMVectorSet(0, 0, 1, 1));
	XMStoreFloat4(&mUp,		 XMVectorSet(0, 1, 0, 1));
	XMStoreFloat4(&mRight,	 XMVectorSet(1, 0, 0, 1));
}

Camera::Camera(XMVECTOR position, float movingSpeed, float rotationSpeed) :
mDistanceFromFloor(-1.f), mPitch(0), mYaw(0), mRoll(0), mMovingSpeed(movingSpeed), mRotationSpeed(rotationSpeed), mToggleFreeMoveHelper(0)
{
	XMStoreFloat4(&mPosition, position);
	XMStoreFloat4(&mLookAt,	 XMVectorSet(0, 0, 1, 1));
	XMStoreFloat4(&mUp,		 XMVectorSet(0, 1, 0, 1));
	XMStoreFloat4(&mRight,	 XMVectorSet(1, 0, 0, 1));
}

Camera::~Camera()
{
}


/**
 Interpolates bilinear between 4 given XMVECTOR's. If the array is larger, only the first 4 vectors will be used, the rest will be ignored.
 The Y-Coordinate (height) is interpolated.
**/
FLOAT bilinearInterpolation4(float x, float z, const XMVECTOR* v)
{
	float px = x - floor(x);
	float pz = z - floor(z);

	float weights[4];
	weights[0] = (1-px)*(1-pz);
	weights[1] = px*(1-pz);
	weights[2] = (1-px)*pz;
	weights[3] = px*pz;

	
	float y = 0;
	
	for(int i=0; i<4; ++i)
		y += XMVectorGetY(v[i]) * weights[i];

	return y;
}

void Camera::update(float deltaTime)
{
	XMVECTOR vLook		= XMVectorSet(0, 0, 1, 1);
	XMVECTOR vUp		= XMVectorSet(0, 1, 0, 1);
	XMVECTOR vRight		= XMVectorSet(1, 0, 0, 1);
	XMVECTOR vPos		= XMLoadFloat4(&mPosition);

	// yaw
	XMMATRIX yawMat = XMMatrixRotationAxis( vUp, mYaw);
	vLook = XMVector3TransformCoord(vLook, yawMat);
	vRight = XMVector3TransformCoord(vRight, yawMat);
	// pitch
	XMMATRIX pitchMat = XMMatrixRotationAxis( vRight, mPitch );	
	vLook = XMVector3TransformCoord(vLook, pitchMat);
	vUp = XMVector3TransformCoord(vUp, pitchMat);
	// roll
	XMMATRIX rollMat = XMMatrixRotationAxis( vLook, mRoll );
	vRight = XMVector3TransformCoord(vRight, rollMat);
	vUp = XMVector3TransformCoord(vUp, rollMat);



	float movingSpeed = mMovingSpeed;

	if(GetAsyncKeyState(VK_SHIFT))
		movingSpeed *= 3;

	// position
	if(GetAsyncKeyState('W'))
	{
		vPos = XMVectorAdd(vPos, vLook * deltaTime * movingSpeed);
	}	
	if(GetAsyncKeyState('S'))
	{
		vPos = XMVectorSubtract(vPos, vLook * deltaTime * movingSpeed);
	}
	if(GetAsyncKeyState('D'))
	{
		vPos = XMVectorAdd(vPos, vRight * deltaTime * movingSpeed);
	}
	if(GetAsyncKeyState('A'))
	{
		vPos = XMVectorSubtract(vPos, vRight * deltaTime * movingSpeed);
	}


	if(GetAsyncKeyState('M') && mToggleFreeMoveHelper + TOGGLE_INTERVAL < getSystemTime())
	{
		mToggleFreeMoveHelper = getSystemTime();
#ifdef GH_LOGGING_ENABLED
		printf("[M] Toggled free move\n");
#endif
		mDistanceFromFloor = -mDistanceFromFloor;
	}

	// Do a collission detection with floor
	if(mDistanceFromFloor >= 0.0f)
	{
		float y = getHeightOfPosition(vPos, mVertexData, mRowOffset);

		vPos = XMVectorSetY(vPos, y + mDistanceFromFloor);
	}
	else
	{
		// We can move freely up and down
		if(GetAsyncKeyState('Q'))
		{
			vPos = XMVectorAdd(vPos, vUp * deltaTime * movingSpeed);
		}
		if(GetAsyncKeyState('E'))
		{
			vPos = XMVectorSubtract(vPos, vUp * deltaTime * movingSpeed);
		}
	}

	// rotation
	if(GetAsyncKeyState(VK_RIGHT))
	{
		mYaw += deltaTime * mRotationSpeed;
	}
	if(GetAsyncKeyState(VK_LEFT))
	{
		mYaw -= deltaTime * mRotationSpeed;
	}
	if(GetAsyncKeyState(VK_UP))
	{
		mPitch -= deltaTime * mRotationSpeed;
	}
	if(GetAsyncKeyState(VK_DOWN))
	{
		mPitch += deltaTime * mRotationSpeed;
	}


	XMStoreFloat4(&mLookAt, vLook);
	XMStoreFloat4(&mUp, vUp);
	XMStoreFloat4(&mRight, vRight);
	XMStoreFloat4(&mPosition, vPos);

}


XMMATRIX Camera::getViewMatrix() const
{
	XMVECTOR vLook		= XMLoadFloat4(&mLookAt);
	XMVECTOR vUp		= XMLoadFloat4(&mUp);
	XMVECTOR vRight		= XMLoadFloat4(&mRight);
	XMVECTOR vPos		= XMLoadFloat4(&mPosition);

	XMMATRIX viewMat;

	viewMat._11 = XMVectorGetX(vRight);
	viewMat._12 = XMVectorGetX(vUp);
	viewMat._13 = XMVectorGetX(vLook);
	viewMat._14 = 0;

	viewMat._21 = XMVectorGetY(vRight);
	viewMat._22 = XMVectorGetY(vUp);
	viewMat._23 = XMVectorGetY(vLook);
	viewMat._24 = 0;

	viewMat._31 = XMVectorGetZ(vRight);
	viewMat._32 = XMVectorGetZ(vUp);
	viewMat._33 = XMVectorGetZ(vLook);
	viewMat._34 = 0;

	viewMat._41 = - XMVectorGetX(XMVector3Dot(vPos, vRight));	// XMVector3Dot gets copied to each component of the returned vector.
	viewMat._42 = - XMVectorGetX(XMVector3Dot(vPos, vUp));		// XMVector // THREE, NOT FOUR!
	viewMat._43 = - XMVectorGetX(XMVector3Dot(vPos, vLook));
	viewMat._44 = 1;

	return viewMat;
}


float getHeightOfPosition(const FXMVECTOR& pos, const std::vector<VertexStruct>& vertexData, size_t rowOffset)
{
	// get x,y values
	const FLOAT x = XMVectorGetX(pos);
	const FLOAT z = XMVectorGetZ(pos);
	const int iX = static_cast<int>(x);
	const int iZ = static_cast<int>(z);

	// get 4 nearest vertices
	try
	{
		XMVECTOR vertices[4];
		vertices[0] = XMLoadFloat3(&vertexData.at(iZ*rowOffset+iX).pos);
		vertices[1] = XMLoadFloat3(&vertexData.at(iZ*rowOffset+iX+1).pos);
		vertices[2] = XMLoadFloat3(&vertexData.at((iZ+1)*rowOffset+iX).pos);
		vertices[3] = XMLoadFloat3(&vertexData.at((iZ+1)*rowOffset+iX+1).pos);
		return bilinearInterpolation4(x, z, &vertices[0]);
	} catch (std::out_of_range oor) 
	{
		return 0;
	}
}