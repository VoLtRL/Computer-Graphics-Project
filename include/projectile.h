#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class Projectile {

public:
	Projectile(float x, float y, float speed, float direction);
	glm::vec3 Position;
	glm::vec3 Velocity;
	glm::vec3 Acceleration;

	void UpdatePhysics(float deltaTime);


private:

	float direction; // in degrees
	bool active;
};