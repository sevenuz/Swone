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

#include "Manifold.h"
#include "Collision.h"

void PHY_NS::Manifold::Solve( void )
{
  PHY_NS::Dispatch[A->shape->GetType( )][B->shape->GetType( )]( this, A, B );
}

void PHY_NS::Manifold::Initialize( PHY_NS::real dt, PHY_NS::Vec2 gravity )
{
  // Calculate average restitution
  e = std::min( A->restitution, B->restitution );

  // Calculate static and dynamic friction
  sf = std::sqrt( A->staticFriction * A->staticFriction );
  df = std::sqrt( A->dynamicFriction * A->dynamicFriction );

  for(PHY_NS::uint32 i = 0; i < contact_count; ++i)
  {
    // Calculate radii from COM to contact
    PHY_NS::Vec2 ra = contacts[i] - A->position;
    PHY_NS::Vec2 rb = contacts[i] - B->position;

    PHY_NS::Vec2 rv = B->velocity + PHY_NS::Cross( B->angularVelocity, rb ) -
              A->velocity - PHY_NS::Cross( A->angularVelocity, ra );


    // Determine if we should perform a resting collision or not
    // The idea is if the only thing moving this object is gravity,
    // then the collision should be performed without any restitution
    if(rv.LenSqr( ) < (dt * gravity).LenSqr( ) + PHY_NS::EPSILON)
      e = 0.0f;
  }
}

void PHY_NS::Manifold::ApplyImpulse( void )
{
  // Early out and positional correct if both objects have infinite mass
  if(PHY_NS::Equal( A->im + B->im, 0 ))
  {
    InfiniteMassCorrection( );
    return;
  }

  for(PHY_NS::uint32 i = 0; i < contact_count; ++i)
  {
    // Calculate radii from COM to contact
    PHY_NS::Vec2 ra = contacts[i] - A->position;
    PHY_NS::Vec2 rb = contacts[i] - B->position;

    // Relative velocity
    PHY_NS::Vec2 rv = B->velocity + PHY_NS::Cross( B->angularVelocity, rb ) -
              A->velocity - PHY_NS::Cross( A->angularVelocity, ra );

    // Relative velocity along the normal
    PHY_NS::real contactVel = PHY_NS::Dot( rv, normal );

    // Do not resolve if velocities are separating
    if(contactVel > 0)
      return;

    PHY_NS::real raCrossN = PHY_NS::Cross( ra, normal );
    PHY_NS::real rbCrossN = PHY_NS::Cross( rb, normal );
    PHY_NS::real invMassSum = A->im + B->im + PHY_NS::Sqr( raCrossN ) * A->iI + PHY_NS::Sqr( rbCrossN ) * B->iI;

    // Calculate impulse scalar
    PHY_NS::real j = -(1.0f + e) * contactVel;
    j /= invMassSum;
    j /= (PHY_NS::real)contact_count;

    // Apply impulse
    PHY_NS::Vec2 impulse = normal * j;
    A->ApplyImpulse( -impulse, ra );
    B->ApplyImpulse(  impulse, rb );

    // Friction impulse
    rv = B->velocity + PHY_NS::Cross( B->angularVelocity, rb ) -
         A->velocity - PHY_NS::Cross( A->angularVelocity, ra );

    PHY_NS::Vec2 t = rv - (normal * PHY_NS::Dot( rv, normal ));
    t.Normalize( );

    // j tangent magnitude
    PHY_NS::real jt = -PHY_NS::Dot( rv, t );
    jt /= invMassSum;
    jt /= (PHY_NS::real)contact_count;

    // Don't apply tiny friction impulses
    if(PHY_NS::Equal( jt, 0.0f ))
      return;

    // Coulumb's law
    PHY_NS::Vec2 tangentImpulse;
    if(std::abs( jt ) < j * sf)
      tangentImpulse = t * jt;
    else
      tangentImpulse = t * -j * df;

    // Apply friction impulse
    A->ApplyImpulse( -tangentImpulse, ra );
    B->ApplyImpulse(  tangentImpulse, rb );
  }
}

void PHY_NS::Manifold::PositionalCorrection( void )
{
  const PHY_NS::real k_slop = 0.0f; // TODO Penetration allowance
  const PHY_NS::real percent = 0.4f; // Penetration percentage to correct
  PHY_NS::Vec2 correction = (std::max( penetration - k_slop, (PHY_NS::real)0.0 ) / (A->im + B->im)) * normal * percent;
  A->position -= correction * A->im;
  B->position += correction * B->im;
}

void PHY_NS::Manifold::InfiniteMassCorrection( void )
{
  A->velocity.Set( 0, 0 );
  B->velocity.Set( 0, 0 );
}

void PHY_NS::Manifold::CollisionCallback( void )
{
  A->cb->onCollision(this);
  B->cb->onCollision(this);
}

