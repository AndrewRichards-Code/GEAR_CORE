#include "gear_core_common.h"
#include "Camera.h"

using namespace gear;
using namespace graphics;
using namespace objects;
using namespace mars;

Camera::Camera(CreateInfo* pCreateInfo)
{
	m_CI = *pCreateInfo;

	InitialiseUB();
	Update();
}

Camera::~Camera()
{
}

void Camera::Update()
{
	DefineProjection();
	DefineView();
	SetPosition();
	m_UB->SubmitData();
}

void Camera::DefineProjection()
{
	if (m_CI.projectionType == ProjectionType::ORTHOGRAPHIC)
	{
		OrthographicParameters& op = m_CI.orthographicsParams;
		m_UB->proj= Mat4::Orthographic(op.left, op.right, op.bottom, op.top, op.near, op.far);
	}
	else if (m_CI.projectionType == ProjectionType::PERSPECTIVE)
	{
		PerspectiveParameters& pp = m_CI.perspectiveParams;
		m_UB->proj = Mat4::Perspective(pp.horizonalFOV, pp.aspectRatio, pp.zNear, pp.zFar);
	}
	else
	{
		GEAR_ASSERT(/*Level::ERROR,*/ ErrorCode::OBJECTS | ErrorCode::INVALID_VALUE, "Unknown projection type.");
		throw;
	}
	if (m_CI.flipX)
		m_UB->proj.a *= -1;
	if (m_CI.flipY)
		m_UB->proj.f *= -1;

	if (miru::crossplatform::GraphicsAPI::IsVulkan())
		m_UB->proj.f *= -1;
}

void Camera::DefineView()
{
	const Mat4& orientation = m_CI.transform.orientation.ToMat4();

	m_Direction	= -Vec3(orientation * Vec4(0, 0, 1, 0));
	m_Up		= +Vec3(orientation * Vec4(0, 1, 0, 0));
	m_Right		= +Vec3(orientation * Vec4(1, 0, 0, 0));

	//m_UB->view = orientation * Mat4::Translation(-m_CI.transform.translation);
	auto transform = TransformToMat4(m_CI.transform);
	transform.Inverse();
	m_UB->view = transform;
}


void Camera::SetPosition()
{
	m_UB->cameraPosition = Vec4(m_CI.transform.translation, 1.0f);
}

void Camera::InitialiseUB()
{
	float zero[sizeof(CameraUB)] = { 0 };
	Uniformbuffer<CameraUB>::CreateInfo ubCI;
	ubCI.debugName = "GEAR_CORE_Camera_CameraUB: " + m_CI.debugName;
	ubCI.device = m_CI.device;
	ubCI.data = zero;
	m_UB = CreateRef<Uniformbuffer<CameraUB>>(&ubCI);
}