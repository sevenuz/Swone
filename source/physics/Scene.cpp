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

#include "Precompiled.h"

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
void IntegrateForces( Body *b, real dt, Vec2 gravity )
{
  if(b->im == 0.0f)
    return;

  // divide dt by 2 because its called twice in one step
  b->velocity += (b->force * b->im + gravity) * (dt / 2.0f);
  b->angularVelocity += b->torque * b->iI * (dt / 2.0f);
}

void IntegrateVelocity( Body *b, real dt, Vec2 gravity )
{
  if(b->im == 0.0f)
    return;

  b->position += b->velocity * dt;
  b->orient += b->angularVelocity * dt;
  b->SetOrient( b->orient );
  IntegrateForces( b, dt, gravity );
}

void Scene::Step( f32 dt, Vec2 gravity )
{
  // Generate new collision info
  contacts.clear( );
  for(uint32 i = 0; i < bodies.size( ); ++i)
  {
    Body *A = bodies[i];

    for(uint32 j = i + 1; j < bodies.size( ); ++j)
    {
      Body *B = bodies[j];
      if(A->im == 0 && B->im == 0)
        continue;
      Manifold m( A, B );
      m.Solve( );
      if(m.contact_count)
        contacts.emplace_back( m );
    }
  }

  // Integrate forces
  for(uint32 i = 0; i < bodies.size( ); ++i)
    IntegrateForces( bodies[i], dt, gravity );

  // Initialize collision
  for(uint32 i = 0; i < contacts.size( ); ++i)
    contacts[i].Initialize( dt, gravity );

  // Solve collisions
  for(uint32 j = 0; j < m_iterations; ++j)
    for(uint32 i = 0; i < contacts.size( ); ++i)
      contacts[i].ApplyImpulse( );

  // Integrate velocities
  for(uint32 i = 0; i < bodies.size( ); ++i)
    IntegrateVelocity( bodies[i], dt, gravity );

  // Correct positions
  for(uint32 i = 0; i < contacts.size( ); ++i)
    contacts[i].PositionalCorrection( );

  // Clear all forces
  for(uint32 i = 0; i < bodies.size( ); ++i)
  {
    Body *b = bodies[i];
    b->force.Set( 0, 0 );
    b->torque = 0;
  }
}

Body *Scene::Add( Shape *shape, uint32 x, uint32 y )
{
  assert( shape );
  Body *b = new Body( shape, x, y );
  bodies.push_back( b );
  return b;
}
