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

#include "Scene.h"

// Acceleration
//    F = mA
// => A = F * 1/m

// Explicit Euler
// x += v * dt
// v += (1/m * F) * dt

// Semi-Implicit (Symplectic) Euler
// v += (1/m * F) * dt
// x += v * dt

// see http://www.niksula.hut.fi/~hkankaan/Homepages/gravity.html
void IntegrateForces( PHY_NS::Body *b, PHY_NS::real dt, PHY_NS::Vec2 gravity )
{
  if(b->im == 0.0f || b->skip)
    return;

  // divide dt by 2 because its called twice in one step
  b->velocity += (b->force * b->im + gravity) * (dt / 2.0f);
  b->angularVelocity += b->torque * b->iI * (dt / 2.0f);
}

void IntegrateVelocity( PHY_NS::Body *b, PHY_NS::real dt, PHY_NS::Vec2 gravity )
{
  if(b->im == 0.0f || b->skip)
    return;

  b->position += b->velocity * dt;
  b->orient += b->angularVelocity * dt;
  b->SetOrient( b->orient );
  IntegrateForces( b, dt, gravity );
}

void PHY_NS::Scene::Step( PHY_NS::real dt, PHY_NS::Vec2 gravity )
{
  // Generate new collision info
  contacts.clear( );
  for(auto it = bodies.begin();it!=bodies.end();++it)
  {
    PHY_NS::Body *A = *it;
    if(A->skip)
      continue;
    for(auto it2 = std::next(it);it2!=bodies.end();++it2)
    {
      PHY_NS::Body *B = *it2;
      if(B->skip)
        continue;
      if(A->im == 0 && B->im == 0)
        continue;
      PHY_NS::Manifold m( A, B );
      m.Solve( );
      if(m.contact_count)
        contacts.emplace_back( m );
    }
  }

  // Integrate forces
  for(Body* b : bodies)
    IntegrateForces( b, dt, gravity );

  // Initialize collision
  for(PHY_NS::uint32 i = 0; i < contacts.size( ); ++i)
    contacts[i].Initialize( dt, gravity );

  // Solve collisions
  for(PHY_NS::uint32 j = 0; j < m_iterations; ++j)
    for(PHY_NS::uint32 i = 0; i < contacts.size( ); ++i)
      if(contacts[i].ShouldComputeCollision())
        contacts[i].ApplyImpulse( );

  // Integrate velocities
  for(Body* b : bodies)
    IntegrateVelocity( b, dt, gravity );

  // Correct positions
  for(PHY_NS::uint32 i = 0; i < contacts.size( ); ++i)
    if(contacts[i].ShouldComputeCollision())
      contacts[i].PositionalCorrection( );

  // Collision Callback
  for(PHY_NS::uint32 i = 0; i < contacts.size( ); ++i)
    contacts[i].CollisionCallback( );

  // Clear all forces
  for(Body* b : bodies)
  {
    b->force.Set( 0, 0 );
    b->torque = 0;
  }
}

void PHY_NS::Scene::Add( PHY_NS::Body *body )
{
  assert( body );
  assert( body->shape );

  bodies.push_back( body );
}

void PHY_NS::Scene::Remove( PHY_NS::Body *body )
{
  assert( body );
  assert( body->shape );

  bodies.remove_if( [&](const Body* b) -> bool {return b==body;} );
}

void PHY_NS::Scene::Clear( )
{
  bodies.clear();
}
