#include "Camera.h"

using namespace DirectX;

namespace RocketCore::Graphics
{
	Camera::Camera()
		: _position(0.0f, 2.0f, -10.0f),
		_rotation(0.0f, 0.0f, 0.0f, 1.0f),
		_nearZ(0.01f), _farZ(50.0f), _aspect(1.0f), _fovY(90.0f),
		_nearWindowHeight(), _farWindowHeight(),
		_viewMatrix(), _projectionMatrix()
	{
		SetFrustum(_fovY, _aspect, _nearZ, _farZ);
	}

	Camera::~Camera()
	{

	}

	DirectX::XMFLOAT3 Camera::GetPosition() const
	{
		return _position;
	}

	void Camera::SetPosition(float x, float y, float z)
	{
		_position = { x,y,z };
	}

	void Camera::SetRotation(float w, float x, float y, float z)
	{
		_rotation = { x,y,z,w };
	}

	float Camera::GetNearZ() const
	{
		return _nearZ;
	}

	float Camera::GetFarZ() const
	{
		return _farZ;
	}

	float Camera::GetAspect() const
	{
		return _aspect;
	}

	float Camera::GetFovX() const
	{
		return XMConvertToDegrees(GetRadianFovX());
	}

	float Camera::GetRadianFovX() const
	{
		float halfWidth = GetNearWindowWidth() / 2;
		return 2.0f * atan(halfWidth / _nearZ);
	}

	float Camera::GetFovY() const
	{
		return _fovY;
	}

	float Camera::GetRadianFovY() const
	{
		return XMConvertToRadians(_fovY);
	}

	float Camera::GetNearWindowWidth() const
	{
		return _aspect * _nearWindowHeight;
	}

	float Camera::GetNearWindowHeight() const
	{
		return _nearWindowHeight;
	}

	float Camera::GetFarWindowWidth() const
	{
		return _aspect * _farWindowHeight;
	}

	float Camera::GetFarWindowHeight() const
	{
		return _farWindowHeight;
	}

	/// 카메라의 세팅을 설정한다.
	/// 들어온 값에 맞춰 여러 멤버들도 재설정해준다.
	/// 지금은 XMMatrixPerspectiveFovLH 함수를 이용해서 투영행렬을 만든다.
	/// 이 부분은 내가 직접 투영행렬을 만들어보고 싶다.
	/// 
	/// 23.04.20 강석원 인재원
	void Camera::SetFrustum(float fovY, float aspect, float nearZ, float farZ)
	{
		_fovY = fovY;
		_aspect = aspect;
		_nearZ = nearZ;
		_farZ = farZ;

		_nearWindowHeight = 2.0f * _nearZ * std::tanf(XMConvertToRadians(_fovY / 2));
		_farWindowHeight = 2.0f * _farZ * std::tanf(XMConvertToRadians(_fovY / 2));

		XMMATRIX temp = XMMatrixPerspectiveFovLH(XMConvertToRadians(_fovY / 2), _aspect, _nearZ, _farZ);
		XMStoreFloat4x4(&_projectionMatrix, temp);
	}

	/// ViewMatrix를 갱신해준다.
	/// 교수님 코드에서는 잘못된 벡터가 되어있을 시 look을 기준으로 다시 점검해서 변경하는 듯 하다.
	/// 나는 그냥 viewMatrix만 갱신해줬다.
	/// 
	/// 그리고 position은 음수로 줘야한다! 왜냐하면 카메라 기준으로 옮기는거니까!
	/// 
	/// 23.04.20 강석원 인재원
	void Camera::UpdateViewMatrix()
	{
		XMVECTOR R = GetRight();
		XMVECTOR U = GetUp();
		XMVECTOR L = GetForward();
		XMVECTOR P = XMLoadFloat3(&_position);

		// Keep camera's axes orthogonal to each other and of unit length.
		L = XMVector3Normalize(L);
		U = XMVector3Normalize(XMVector3Cross(L, R));

		// U, L already ortho-normal, so no need to normalize cross product.
		R = XMVector3Cross(U, L);

		// Fill in the view matrix entries.
		float x = -XMVectorGetX(XMVector3Dot(P, R));
		float y = -XMVectorGetX(XMVector3Dot(P, U));
		float z = -XMVectorGetX(XMVector3Dot(P, L));

		DirectX::XMFLOAT3 _right;
		DirectX::XMFLOAT3 _up;
		DirectX::XMFLOAT3 _look;

		XMStoreFloat3(&_right, R);
		XMStoreFloat3(&_up, U);
		XMStoreFloat3(&_look, L);

		_viewMatrix(0, 0) = _right.x;
		_viewMatrix(1, 0) = _right.y;
		_viewMatrix(2, 0) = _right.z;
		_viewMatrix(3, 0) = x;

		_viewMatrix(0, 1) = _up.x;
		_viewMatrix(1, 1) = _up.y;
		_viewMatrix(2, 1) = _up.z;
		_viewMatrix(3, 1) = y;

		_viewMatrix(0, 2) = _look.x;
		_viewMatrix(1, 2) = _look.y;
		_viewMatrix(2, 2) = _look.z;
		_viewMatrix(3, 2) = z;

		_viewMatrix(0, 3) = 0.0f;
		_viewMatrix(1, 3) = 0.0f;
		_viewMatrix(2, 3) = 0.0f;
		_viewMatrix(3, 3) = 1.0f;
		// 
		// 	viewMatrix_ =
		// 	{
		// 		right_.x,		up_.x,			look_.x,		0.0f,
		// 		right_.y,		up_.y,			look_.y,		0.0f,
		// 		right_.z,		up_.z,			look_.z,		0.0f,
		// 		-position_.x,	-position_.y,	-position_.z,	1.0f
		// 	};
	}

	DirectX::XMMATRIX Camera::GetViewMatrix() const
	{
		return XMLoadFloat4x4(&_viewMatrix);
	}

	DirectX::XMMATRIX Camera::GetProjectionMatrix() const
	{
		return XMLoadFloat4x4(&_projectionMatrix);
	}

	DirectX::XMMATRIX Camera::GetViewProjectionMatrix() const
	{
		return XMMatrixMultiply(GetViewMatrix(), GetProjectionMatrix());
	}

	DirectX::XMVECTOR Camera::GetForward() const
	{
		XMFLOAT3 forward = { 0.0f,0.0f,1.0f };
		auto rotationMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&_rotation));
		auto result = DirectX::XMVector3Transform(XMLoadFloat3(&forward), rotationMatrix);
		return result;
	}

	DirectX::XMVECTOR Camera::GetUp() const
	{
		XMFLOAT3 up = { 0.0f,1.0f,0.0f };
		auto rotationMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&_rotation));
		auto result = DirectX::XMVector3Transform(XMLoadFloat3(&up), rotationMatrix);
		return result;
	}

	DirectX::XMVECTOR Camera::GetRight() const
	{
		XMFLOAT3 right = { 1.0f,0.0f,0.0f };
		auto rotationMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&_rotation));
		auto result = DirectX::XMVector3Transform(XMLoadFloat3(&right), rotationMatrix);
		return result;
	}

}
