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

#ifndef MANIFOLD_H
#define MANIFOLD_H

#include "Body.h"
#include "Shape.h"

namespace PHY_NS {

// http://gamedev.tutsplus.com/tutorials/implementation/create-custom-2d-physics-engine-aabb-circle-impulse-resolution/
struct Manifold
{
  Manifold( PHY_NS::Body *a, PHY_NS::Body *b )
    : A( a )
    , B( b )
  {}

  void Solve( void );                       // Generate contact information
  void Initialize( PHY_NS::real dt, PHY_NS::Vec2 gravity ); // Precalculations for impulse solving
  void ApplyImpulse( void );                // Solve impulse and apply
  void PositionalCorrection( void );        // Naive correction of positional penetration
  void InfiniteMassCorrection( void );
  void CollisionCallback( void );
  bool ShouldComputeCollision( void );

  PHY_NS::Body *A;
  PHY_NS::Body *B;

  PHY_NS::real penetration;     // Depth of penetration from collision
  PHY_NS::Vec2 normal;          // From A to B
  PHY_NS::Vec2 contacts[2];     // Points of contact during collision
  PHY_NS::uint32 contact_count; // Number of contacts that occured during collision
  PHY_NS::real e;               // Mixed restitution
  PHY_NS::real df;              // Mixed dynamic friction
  PHY_NS::real sf;              // Mixed static friction
};

}

#endif // MANIFOLD_H
