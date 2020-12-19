/*
    Copyright (c) 2013 Randy Gaul http://RandyGaul.net

    This software is provided 'as-is', without any express or implied
    warranty. In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:
      1. The origin of this software must not be misrepresented; you must not
         claim that you wrote the original software. If you use this software
         in a product, an acknowledgment in the product documentation would be
         appreciated but is not required.
      2. Altered source versions must be plainly marked as such, and must not be
         misrepresented as being the original software.
      3. This notice may not be removed or altered from any source distribution.
*/

#ifndef BODY_H
#define BODY_H

#include <string>
#include "IEMath.h"

#define X_EPSILON 0.1

namespace PHY_NS {

struct Shape;
struct Manifold;

// http://gamedev.tutsplus.com/tutorials/implementation/how-to-create-a-custom-2d-physics-engine-the-core-engine/
struct Body
{
  struct Callback {
    virtual ~Callback() {};
    virtual void onCollision(Manifold* manifold) {};
    virtual const std::string getType() const { return "Body"; };
    static Callback* instance()
    {
      static Callback instance;
      return &instance;
    }
  };

  struct Config
  {
    Shape *shape;
    PHY_NS::real x, y;
    Callback* cb = Callback::instance();
    bool collidableSolid = true;
    bool collidableUnsolid = true;
    bool rotatable = true;
    bool solid = false;
    bool skip = false;
    PHY_NS::real staticFriction = 0.5f;
    PHY_NS::real dynamicFriction = 0.3f;
    PHY_NS::real restitution = 0.2f;
    PHY_NS::real orient = 0;
    PHY_NS::real vx = 0, vy = 0, av = 0;
    PHY_NS::real fx = 0, fy = 0, ft = 0;
  };

  Body( Config config );

  void ApplyForce( const PHY_NS::Vec2& f )
  {
    force += f;
  }

  void ApplyImpulse( const PHY_NS::Vec2& impulse, const PHY_NS::Vec2& contactVector )
  {
    velocity += im * impulse;
    // TODO speed lock to prevent glitching
    if(std::abs(velocity.x) < X_EPSILON)
      velocity.x = 0;
    if(rotatable)
      angularVelocity += iI * PHY_NS::Cross( contactVector, impulse );
    else
      angularVelocity = 0;
  }

  void SetSolid( bool s = true );

  void SetOrient( PHY_NS::real radians );
  void SetOrientAngle( PHY_NS::real radians );

  PHY_NS::real GetOrient( void );
  PHY_NS::real GetOrientAngle( void );

  PHY_NS::Vec2 position;
  PHY_NS::Vec2 velocity;

  bool rotatable; // deactivate rotation on collision
  bool collidableSolid; // activates collisions with solid bodies
  bool collidableUnsolid; // activates collisions with unsolid bodies
  bool solid;
  bool skip;
  PHY_NS::real angularVelocity;
  PHY_NS::real torque;
  PHY_NS::real orient; // radians

  PHY_NS::Vec2 force;

  // Set by shape
  PHY_NS::real I;  // moment of inertia
  PHY_NS::real iI; // inverse inertia
  PHY_NS::real m;  // mass
  PHY_NS::real im; // inverse masee

  // http://gamedev.tutsplus.com/tutorials/implementation/how-to-create-a-custom-2d-physics-engine-friction-scene-and-jump-table/
  PHY_NS::real staticFriction;
  PHY_NS::real dynamicFriction;
  PHY_NS::real restitution;

  // can be implemented outside of physics/
  Callback* cb;
  // Shape interface
  PHY_NS::Shape *shape;
};

}

#endif // BODY_H
