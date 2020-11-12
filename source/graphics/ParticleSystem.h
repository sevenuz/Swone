#ifndef SWONE_GRAPHICS_PARTICLESYSTEM_H
#define SWONE_GRAPHICS_PARTICLESYSTEM_H

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Time.hpp>
#include <Updateable.h>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <sstream>
#include <vector>
#include <random>
#include <cmath>

class Randomizer {
public:
	Randomizer() : device_(), engine_(device_()) {};
	int rnd(int a, int b) {
		std::uniform_int_distribution<int> uni_dist(a, b);
		return uni_dist(engine_);
	};
	double rnd(double a, double b) {
		std::uniform_real_distribution<double> uni_dist(a, b);
		return uni_dist(engine_);
	};
private:
	std::random_device device_;
	std::default_random_engine engine_;
};

namespace Shape {
	enum { CIRCLE, SQUARE };
}
namespace Origin {
	enum { ON_POINT, ON_AREA, ON_BORDER };
}

struct Particle {
	sf::Vector2f pos; // Position
	sf::Vector2f vel; // Velocity
	sf::Color color;  // RGBA
	sf::Time lifetime;// Lifetime
};

class ParticleSystem : public sf::Drawable, public sf::Transformable, public Updateable {
public:
	explicit ParticleSystem(unsigned int particleNumber);
	~ParticleSystem();

	void reset();//creates new vectors and fuel them
	void update(sf::Time elapsed); // Updates position, velocity and opacity of all particles

	void setLifetime(sf::Time lt);
	void changeLifetime(int n);//change Lifetime in seconds
	sf::Time getLifetime();

	void setParticleNumber(unsigned int count);//reset = false
	void setParticleNumber(unsigned int count, bool reset);
	void changeParticleNumber(int n);//reset = false
	void changeParticleNumber(int n, bool reset);
	int getNumberOfParticles();//real size of the array
	unsigned int getParticleNumber();//ideal size, most of the time same like getNumberOfParticles
	std::string getNumberOfParticlesString();//real size as string

	void setOrigin(sf::Vector2f p, sf::Vector2f d, unsigned char ot = Origin::ON_POINT);
	void setOrigin(float x, float y, float w, float h, unsigned char ot = Origin::ON_POINT);

	void setOriginType(unsigned char ot);
	unsigned char getOriginType();

	void setPosition(float x, float y);
	sf::Vector2f getPosition();

	void setDimension(float x, float y);
	sf::Vector2f getDimension();

	void setGravity(float x, float y);
	sf::Vector2f getGravity();

	void setParticleSpeed(float speed);
	void changeParticleSpeed(float n);
	float getParticleSpeed();

	void setEndless(bool enable);
	bool isEndless();

	void setDissolve(bool enable);
	bool isDissolve();

	void setDissolutionRate(unsigned char rate);
	unsigned char getDissolutionRate();

	void setShape(unsigned char shape);
	unsigned char getShape();

	void setColor(sf::Color c);
	sf::Color getColor();

	void setDrawingType(sf::PrimitiveType t);
	sf::PrimitiveType getDrawingType();

	void setDrawingSize(float s);
	float getDrawingSize();

	void setDrawingFactor(unsigned char c);
	unsigned char getDrawingFactor();

private:

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
		// apply the transform
		states.transform *= getTransform();

		// our particles don't use a texture
		states.texture = NULL;
		//sf::VertexArray v(m_vertices.getVertexCount()*m_drawingTypeSingle);

		// draw the vertex array
		target.draw(m_vertices, states);
	};

	void resetParticle(std::size_t index);
	void resetParticle(Particle* p);
	void resetParticles();
	void resetVertices();
	void updateVertices();

	sf::Vector2f    m_position; // Particle origin (pixel co-ordinates)
	sf::Vector2f    m_dimension; //
	sf::Vector2f    m_gravity;  // Affects particle velocities
	sf::Color   m_color;  // standard particle color
	sf::Time m_lifetime;    //Ini-Lifetime for particles
	sf::PrimitiveType m_drawingType;//Vertex draw-type
	sf::VertexArray m_vertices;
	Randomizer  m_randomizer;
	float       m_particleSpeed;// Pixels per second (at most)
	float       m_drawingSize;
	bool        m_dissolve; // Dissolution enabled?
	bool        m_endless; //true if the system should create new particles after their death
	unsigned char   m_drawingFactor;//if true create more particles for own shapes by m_drawingType
	unsigned char   m_dissolutionRate;
	unsigned char   m_shape;//particle movement-type
	unsigned char   m_originType;//spawn type
	unsigned int    m_particleNumber;

	std::vector<Particle> m_particles;
};

#endif //SWONE_GRAPHICS_PARTICLESYSTEM_H
