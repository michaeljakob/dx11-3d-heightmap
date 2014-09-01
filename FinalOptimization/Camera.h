#pragma once

#include <D3D11.h>
#include <xnamath.h>
#include <vector>
#include "VertexStruct.h"

#include "GraphicsHelper.h"


class Camera
{
private:
	float mPitch, mYaw, mRoll;
	float mDistanceFromFloor;
	float mMovingSpeed, mRotationSpeed;
	XMFLOAT4 mPosition, mLookAt, mUp, mRight;

	size_t mRowOffset;
	std::vector<VertexStruct> mVertexData;


	static const float DEFAULT_MOVING_SPEED;
	static const float DEFAULT_ROTATION_SPEED;
	static const int TOGGLE_INTERVAL;

	unsigned long mToggleFreeMoveHelper;

public:
	Camera();
	Camera(XMVECTOR position, float movingSpeed = DEFAULT_MOVING_SPEED, float rotationSpeed = DEFAULT_ROTATION_SPEED);
	virtual ~Camera();

	void update(float deltaTime);

	XMMATRIX getViewMatrix() const;
	XMVECTOR getLookAt() const { return XMVector4Normalize(XMLoadFloat4(&mLookAt)); }

	void setPosition(XMVECTOR newPosition) { XMStoreFloat4(&mPosition, newPosition); }
	void setPositionY(FLOAT y) { XMVECTOR vPos = XMLoadFloat4(&mPosition); vPos = XMVectorSetY(vPos, y); XMStoreFloat4(&mPosition, vPos);};
	XMVECTOR getPosition() const { return XMLoadFloat4(&mPosition); }
	
	void setRotationSpeedDEG(float newRotationSpeedInDegree) { mRotationSpeed = static_cast<float>(GH_DEGTORAD(newRotationSpeedInDegree)); }
	float getRotationSpeedDEG() const { return static_cast<float>(GH_RADTODEG(mRotationSpeed)); }

	void setRotationSpeedRAD(float newRotationSpeedInRadian) { mRotationSpeed = newRotationSpeedInRadian; }
	float getRotationSpeedRAD() const { return mRotationSpeed; }

	void setMovingSpeed(float newMovingSpeed) { mMovingSpeed = newMovingSpeed; }
	float getMovingSpeed() const { return mMovingSpeed; }


	/**
		Set how far the camera should be away from the floor.
		@param distance The distance. Use a negative value to disable it.
		@param vertexData the Vertexdata of the current terrain.
	**/
	void setDistanceFromFloor(float distance, const std::vector<VertexStruct>& vertexData, size_t rowOffset) { mDistanceFromFloor = distance; mVertexData = vertexData; mRowOffset = rowOffset; };


};


float getHeightOfPosition(const FXMVECTOR&, const std::vector<VertexStruct>&, size_t);


