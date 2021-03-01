#include "KickUp.h"

KickUp::KickUp(GameObject* obj) : Extension(obj)
{}

void KickUp::onObjectCollision(ph::Manifold* manifold, GameObject* t)
{
	if(m_kick && t->getBody()->m <= m_obj->getBody()->m)
		t->setVelY(-KICK_VEL);
}

void KickUp::event(GameObject::Event event) {
	if (event.action4) {
		m_kick = true;
	} else {
		m_kick = false;
	}
}

