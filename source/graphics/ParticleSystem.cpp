#include <graphics/ParticleSystem.h>
#include <sstream>

//ParticleSystem::ParticleSystem() {}

ParticleSystem::ParticleSystem(unsigned int n) {
    m_particleNumber = n;
    m_drawingType = sf::Points;
    m_color = sf::Color::Transparent;
    m_lifetime=sf::seconds(3);
    m_position=sf::Vector2f(320.0f, 240.0f);
    m_particleSpeed = 20.0f;
    m_dissolve  = false;
    m_endless = true;
    m_dissolutionRate = 4;
    m_shape = Shape::CIRCLE;
    m_drawingFactor = 1;
    m_drawingSize = 3;

    reset();
}

ParticleSystem::~ParticleSystem() {
    delete &m_particles;
    //deleting m_verticles crashs
}

void ParticleSystem::resetParticle(Particle* particle) {
    float angle;

    switch( m_shape ) {
    case Shape::CIRCLE:
        angle = m_randomizer.rnd(0.0f, 6.2832f);
        particle->vel.x = m_randomizer.rnd(0.0f, cos( angle ));
        particle->vel.y = m_randomizer.rnd(0.0f, sin( angle ));
        break;
    case Shape::SQUARE:
        particle->vel.x = m_randomizer.rnd(-1.0f, 1.0f);
        particle->vel.y = m_randomizer.rnd(-1.0f, 1.0f);
        break;
    default:
        particle->vel.x = 0.5f; // Easily detected
        particle->vel.y = 0.5f; // Easily detected
    }

    if(m_originType == Origin::ON_AREA) {
        particle->pos = sf::Vector2f(m_randomizer.rnd(m_position.x,m_position.x + m_dimension.x),m_randomizer.rnd(m_position.y,m_position.y + m_dimension.y));
    } else if(m_originType == Origin::ON_BORDER) {
        int b = m_randomizer.rnd(0, 3);//auf welcher Seiter der borders gespornt wird
        if(b==0) {//oben
            particle->vel.y = -(std::abs(particle->vel.y));
            particle->pos = sf::Vector2f(m_randomizer.rnd(m_position.x,m_position.x + m_dimension.x),m_position.y);
        } else if(b==1) {//rechts
            particle->vel.x = std::abs(particle->vel.x);
            particle->pos = sf::Vector2f(m_position.x + m_dimension.x,m_randomizer.rnd(m_position.y,m_position.y + m_dimension.y));
        } else if(b==2) { //unten
            particle->vel.y = std::abs(particle->vel.y);
            particle->pos = sf::Vector2f(m_randomizer.rnd(m_position.x,m_position.x + m_dimension.x),m_position.y + m_dimension.y);
        } else { //b==3,links
            particle->vel.x = -(std::abs(particle->vel.x));
            particle->pos = sf::Vector2f(m_position.x,m_randomizer.rnd(m_position.y,m_position.y + m_dimension.y));
        }
    } else { //m_originType == Origin::ON_POINT
        particle->pos.x = m_position.x;
        particle->pos.y = m_position.y;
    }

    if( particle->vel.x == 0.0f && particle->vel.y == 0.0f ) {
        delete particle;
    }
    particle->lifetime = sf::seconds(m_randomizer.rnd(0.0, m_lifetime.asSeconds()*2));

    if(m_color == sf::Color::Transparent) {
        particle->color.r = m_randomizer.rnd(0, 255);
        particle->color.g = m_randomizer.rnd(0, 255);
        particle->color.b = m_randomizer.rnd(0, 255);
        particle->color.a = 255;
    } else {
        particle->color = m_color;
    }
}

void ParticleSystem::resetParticle(std::size_t index) {
    resetParticle(&m_particles[index]);
    m_vertices[index].position = m_particles[index].pos;
    m_vertices[index].color = m_particles[index].color;
}

float t;

void ParticleSystem::update(sf::Time elapsed) {
    float time = elapsed.asSeconds();
    t += time;

    for(std::size_t i = 0; i < m_particles.size(); i++) {
        Particle& p = m_particles[i];

        p.vel.x += m_gravity.x * time;
        p.vel.y += m_gravity.y * time;

        p.pos.x += p.vel.x * time * m_particleSpeed;
        p.pos.y += p.vel.y * time * m_particleSpeed;

        p.lifetime -= elapsed;

        if(m_dissolve) {
            p.color.a -= m_dissolutionRate;
        } else {
            float ratio = p.lifetime.asSeconds() / m_lifetime.asSeconds();
            p.color.a = static_cast<sf::Uint8>(ratio * 255);
        }

        //m_vertices[i].position = p.pos;
        //m_vertices[i].color = p.color;

        if(p.lifetime <= sf::Time::Zero || p.color.a < 10 ) {
            if(t>=2) {
                t=0;
            }
            if(m_endless && m_particles.size()<=m_particleNumber) {
                resetParticle(i);
                if(m_particles.size()<m_particleNumber) {
                    m_particles.push_back(*(new Particle()));
                }
            } else {
                m_particles.erase(m_particles.begin() + i);
                m_vertices.resize(m_particles.size() * m_drawingFactor);
            }

        }
    }
    updateVertices();
}

void ParticleSystem::reset() {
    resetParticles();
    resetVertices();
}

void ParticleSystem::resetVertices() {
    m_vertices = sf::VertexArray(m_drawingType,m_particleNumber*m_drawingFactor);
}

void ParticleSystem::resetParticles() {
    m_particles = std::vector<Particle>(m_particleNumber);
}

void ParticleSystem::updateVertices() {
    //m_vertices.clear();
    //m_vertices = sf::VertexArray(m_drawingType,m_particleNumber);
    for(std::size_t i = 0; i < m_particles.size(); i++) {
        for(std::size_t e = 0; e < m_drawingFactor; e++) {
            unsigned int index = i * m_drawingFactor + e;
            m_vertices[index].color = m_particles[i].color;
            if(e == 0)
                m_vertices[index].position = sf::Vector2f(m_particles[i].pos.x,m_particles[i].pos.y);
            else if(e == 1)
                m_vertices[index].position = sf::Vector2f(m_particles[i].pos.x + m_drawingSize,m_particles[i].pos.y);
            else if(e == 2)
                m_vertices[index].position = sf::Vector2f(m_particles[i].pos.x + m_drawingSize,m_particles[i].pos.y + m_drawingSize);
            else if(e == 3)
                m_vertices[index].position = sf::Vector2f(m_particles[i].pos.x,m_particles[i].pos.y + m_drawingSize);
        }
    }
}



//Getter & Setter

void ParticleSystem::setLifetime(sf::Time lt) {
    m_lifetime = lt;
}

void ParticleSystem::changeLifetime(int n) {
    if(m_lifetime.asSeconds()+n>0) {
        m_lifetime += sf::seconds(n);
    }
}

sf::Time ParticleSystem::getLifetime() {
    return m_lifetime;
}

void ParticleSystem::setParticleNumber(unsigned int count) {
    setParticleNumber(count,false);
}

void ParticleSystem::setParticleNumber(unsigned int count, bool r) {
    if(m_particleNumber<count) {
        m_vertices.resize(count * m_drawingFactor);
    }
    m_particleNumber = count;

    if(r) {
        reset();
    }
}

void ParticleSystem::changeParticleNumber(int n, bool reset) {
    if (n>0)
        setParticleNumber(m_particleNumber + n, reset);
    else if(static_cast<unsigned int>(n*-1)<m_particleNumber)//cast ist eigentlich unnötig
        setParticleNumber(m_particleNumber + n, reset);
}

void ParticleSystem::changeParticleNumber(int n) {
    changeParticleNumber(n, false);
}

void ParticleSystem::setParticleSpeed(float speed ) {
    m_particleSpeed = speed;
}

void ParticleSystem::changeParticleSpeed(float n) {
    if(m_particleSpeed+n>0) {
        m_particleSpeed += n;
    }
}

float ParticleSystem::getParticleSpeed() {
    return m_particleSpeed;
}

void ParticleSystem::setOrigin(sf::Vector2f p,sf::Vector2f d,unsigned char ot) {
    m_position = p;
    m_dimension = d;
    m_originType = ot;
}

void ParticleSystem::setOrigin(float x,float y,float w,float h,unsigned char ot) {
    setOrigin(sf::Vector2f(x,y),sf::Vector2f(w,h),ot);
}

void ParticleSystem::setPosition( float x, float y ) {
    m_position.x = x;
    m_position.y = y;
}

sf::Vector2f ParticleSystem::getPosition() {
    return m_position;
}

void ParticleSystem::setDimension( float x, float y ) {
    m_dimension.x = x;
    m_dimension.y = y;
}

sf::Vector2f ParticleSystem::getDimension() {
    return m_dimension;
}

void ParticleSystem::setGravity( float x, float y ) {
    m_gravity.x = x;
    m_gravity.y = y;
}

sf::Vector2f ParticleSystem::getGravity() {
    return m_gravity;
}

void ParticleSystem::setEndless( bool enable ) {
    m_endless = enable;
    if(enable && m_particles.size() == 0)
        reset();
}

bool ParticleSystem::isEndless() {
    return m_endless;
}

void ParticleSystem::setDissolve( bool enable ) {
    m_dissolve = enable;
}

bool ParticleSystem::isDissolve() {
    return m_dissolve;
}

void ParticleSystem::setDissolutionRate( unsigned char rate ) {
    m_dissolutionRate = rate;
}

unsigned char ParticleSystem::getDissolutionRate() {
    return m_dissolutionRate;
}

void ParticleSystem::setShape( unsigned char shape ) {
    m_shape = shape;
}

unsigned char ParticleSystem::getShape() {
    return m_shape;
}

int ParticleSystem::getNumberOfParticles() {
    return m_particles.size();
}

unsigned int ParticleSystem::getParticleNumber() {
    return m_particleNumber;
}

std::string ParticleSystem::getNumberOfParticlesString() {
    std::ostringstream oss;
    oss << m_particles.size();
    return oss.str();
}

void ParticleSystem::setColor(sf::Color c) {
    m_color = c;
}

sf::Color ParticleSystem::getColor() {
    return m_color;
}

void ParticleSystem::setDrawingType(sf::PrimitiveType t) {
    m_drawingType = t;
    m_vertices.setPrimitiveType(m_drawingType);

    if(t == sf::Points)
        m_drawingFactor = 1;
    else if(t == sf::Lines)
        m_drawingFactor = 2;
    else if(t == sf::Triangles)
        m_drawingFactor = 3;
    else if(t == sf::Quads)
        m_drawingFactor = 4;
    else
        m_drawingFactor = 1;
    resetVertices();
};

sf::PrimitiveType ParticleSystem::getDrawingType() {
    return m_drawingType;
};

void ParticleSystem::setDrawingSize(float s) {
    m_drawingSize = s;
}

float ParticleSystem::getDrawingSize() {
    return m_drawingSize;
}

void ParticleSystem::setDrawingFactor(unsigned char c) {
    m_drawingFactor = c;
    resetVertices();
}

unsigned char ParticleSystem::getDrawingFactor() {
    return m_drawingFactor;
}

void ParticleSystem::setOriginType(unsigned char ot) {
    m_originType = ot;
}

unsigned char ParticleSystem::getOriginType() {
    return m_originType;
}
