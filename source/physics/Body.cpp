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

#include "Body.h"
#include "Shape.h"

PHY_NS::Body::Body( PHY_NS::Body::Config config )
  : shape( config.shape->Clone( ) )
{
  shape->body = this;
  position.Set( config.x, config.y );
  cb = config.cb;
  velocity.Set( config.vx, config.vy );
  angularVelocity = config.av;
  torque = config.ft;
  orient = config.o;
  force.Set( config.fx, config.fy );
  staticFriction = config.sf;
  dynamicFriction = config.df;
  restitution = config.r;
  shape->Initialize( );
}

void PHY_NS::Body::SetOrient( PHY_NS::real radians )
{
  orient = radians;
  shape->SetOrient( radians );
}
