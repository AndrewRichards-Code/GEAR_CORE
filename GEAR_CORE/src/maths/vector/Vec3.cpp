#include "Vec3.h"

namespace ARM
{
	//Constructs a Vec3 of 0.
	Vec3::Vec3()
		:x(0), y(0), z(0) {}

	//Constructs a Vec3 taking x, y, z.
	Vec3::Vec3(float x, float y, float z)
		: x(x), y(y), z(z) {}

	//Constructs a Vec3 from another Vec3.
	Vec3::Vec3(const Vec3& copy)
		: x(copy.x), y(copy.y), z(copy.z) {}

	//Constructs a Vec3 from the struct CoordCart3D.
	Vec3::Vec3(const CoordCart3D& other)
		: x((float)other.x), y((float)other.y), z((float)other.z) {}

	//Constructs a Vec3 from the struct CoordSph.
	Vec3::Vec3(const CoordSph& other)
		: x((float)(other.r * sin(other.theta) * cos(other.phi))),
		y((float)(other.r * sin(other.theta) * sin(other.phi))),
		z((float)(other.r * cos(other.theta))) {}

	//Destructs the Vec3.
	Vec3::~Vec3() {}

	//Takes the dot product of the current object and another Vec3.
	float Vec3::Dot(const Vec3& other)
	{
		return (this->x * other.x) + (this->y * other.y) + (this->z * other.z);
	}

	//Takes the dot product of two Vec3s.
	float Vec3::Dot(const Vec3& a, const Vec3& b)
	{
		return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
	}

	//Takes the cross product of the current object and another Vec3. LHRule.
	Vec3 Vec3::Cross(const Vec3& other)
	{
		Mat3 mat(Vec3(1, 1, 1), Vec3(x, y, z), Vec3(other.x, other.y, other.z));
		Vec3 output = mat.VecDet();
		return output;
	}

	//Takes the cross product of two Vec3s. LHRule.
	Vec3 Vec3::Cross(const Vec3& a, const Vec3& b)
	{
		Mat3 mat(Vec3(1, 1, 1), Vec3(a.x, a.y, a.z), Vec3(b.x, b.y, b.z));
		Vec3 output = mat.VecDet();
		return output;
	}

	//Normalise the current object.
	Vec3 Vec3::Normalise()
	{
		return *this * (1 / sqrt(pow(this->x, 2) + pow(this->y, 2) + pow(this->z, 2)));
	}

	//Normalise the input object and return a new Vec3.
	Vec3 Vec3::Normalise(const Vec3& other)
	{
		return other * (1 / sqrt(pow(other.x, 2) + pow(other.y, 2) + pow(other.z, 2)));
	}

	//Rotates the current object via quaternions and returns a new Vec3.
	Vec3 Vec3::RotQuat(double theta, const Vec3& axis)
	{
		float sinOfHalfTheta = static_cast<float>(sin(theta / 2));
		float cosOfHalfTheta = static_cast<float>(cos(theta / 2));

		float quat_s = cosOfHalfTheta;
		float quat_i = axis.x * sinOfHalfTheta;
		float quat_j = axis.y * sinOfHalfTheta;
		float quat_k = axis.z * sinOfHalfTheta;
		
		Quat rotation(quat_s, quat_i, quat_j, quat_k);
		Quat rotationConjugate = rotation.Conjugate();

		Quat temp = (rotation * axis) * rotationConjugate;
		return Vec3(temp.i, temp.j, temp.k);
	}

	//Adds two Vec3s.
	Vec3 Vec3::operator+ (const Vec3& other) const
	{
		return Vec3 (x + other.x, y + other.y, z + other.z);
	}

	//Subtracts two Vec3s.
	Vec3 Vec3::operator- (const Vec3& other) const
	{
		return Vec3 (x - other.x, y - other.y, z - other.z);
	}

	//Scales the Vec3 by the scaler a. floathe scaler go on the rhs of the object.
	Vec3 Vec3::operator* (float a) const
	{
		return Vec3 (a * x, a * y, a * z);
	}

	//Compare the Vec3 with another Vec3. If it's equal, it'll return true.
	bool Vec3::operator==(const Vec3& other) const
	{
		if (x == other.x && y == other.y && z == other.z)
			return true;
		else
			return false;
	}

	//Compare the Vec3 with another Vec3. If it's not equal, it'll return true.
	bool Vec3::operator!=(const Vec3& other) const
	{
		if (x != other.x && y != other.y && z != other.z)
			return true;
		else
			return false;
	}
}