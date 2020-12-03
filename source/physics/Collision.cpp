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

#include "Collision.h"

PHY_NS::CollisionCallback PHY_NS::Dispatch[PHY_NS::Shape::eCount][PHY_NS::Shape::eCount] =
{
  {
    PHY_NS::CircletoCircle, PHY_NS::CircletoPolygon
  },
  {
    PHY_NS::PolygontoCircle, PHY_NS::PolygontoPolygon
  },
};

void PHY_NS::CircletoCircle( PHY_NS::Manifold *m, PHY_NS::Body *a, PHY_NS::Body *b )
{
  PHY_NS::Circle *A = reinterpret_cast<PHY_NS::Circle *>(a->shape);
  PHY_NS::Circle *B = reinterpret_cast<PHY_NS::Circle *>(b->shape);

  // Calculate translational vector, which is normal
  PHY_NS::Vec2 normal = b->position - a->position;

  PHY_NS::real dist_sqr = normal.LenSqr( );
  PHY_NS::real radius = A->radius + B->radius;

  // Not in contact
  if(dist_sqr >= radius * radius)
  {
    m->contact_count = 0;
    return;
  }

  PHY_NS::real distance = std::sqrt( dist_sqr );

  m->contact_count = 1;

  if(distance == 0.0f)
  {
    m->penetration = A->radius;
    m->normal = PHY_NS::Vec2( 1, 0 );
    m->contacts [0] = a->position;
  }
  else
  {
    m->penetration = radius - distance;
    m->normal = normal / distance; // Faster than using Normalized since we already performed sqrt
    m->contacts[0] = m->normal * A->radius + a->position;
  }
}

void PHY_NS::CircletoPolygon( PHY_NS::Manifold *m, PHY_NS::Body *a, PHY_NS::Body *b )
{
  PHY_NS::Circle *A       = reinterpret_cast<PHY_NS::Circle *>      (a->shape);
  PHY_NS::PolygonShape *B = reinterpret_cast<PHY_NS::PolygonShape *>(b->shape);

  m->contact_count = 0;

  // Transform circle center to Polygon model space
  PHY_NS::Vec2 center = a->position;
  center = B->u.Transpose( ) * (center - b->position);

  // Find edge with minimum penetration
  // Exact concept as using support points in Polygon vs Polygon
  PHY_NS::real separation = -FLT_MAX;
  PHY_NS::uint32 faceNormal = 0;
  for(PHY_NS::uint32 i = 0; i < B->m_vertexCount; ++i)
  {
    PHY_NS::real s = PHY_NS::Dot( B->m_normals[i], center - B->m_vertices[i] );

    if(s > A->radius)
      return;

    if(s > separation)
    {
      separation = s;
      faceNormal = i;
    }
  }

  // Grab face's vertices
  PHY_NS::Vec2 v1 = B->m_vertices[faceNormal];
  PHY_NS::uint32 i2 = faceNormal + 1 < B->m_vertexCount ? faceNormal + 1 : 0;
  PHY_NS::Vec2 v2 = B->m_vertices[i2];

  // Check to see if center is within polygon
  if(separation < PHY_NS::EPSILON)
  {
    m->contact_count = 1;
    m->normal = -(B->u * B->m_normals[faceNormal]);
    m->contacts[0] = m->normal * A->radius + a->position;
    m->penetration = A->radius;
    return;
  }

  // Determine which voronoi region of the edge center of circle lies within
  PHY_NS::real dot1 = PHY_NS::Dot( center - v1, v2 - v1 );
  PHY_NS::real dot2 = PHY_NS::Dot( center - v2, v1 - v2 );
  m->penetration = A->radius - separation;

  // Closest to v1
  if(dot1 <= 0.0f)
  {
    if(PHY_NS::DistSqr( center, v1 ) > A->radius * A->radius)
      return;

    m->contact_count = 1;
    PHY_NS::Vec2 n = v1 - center;
    n = B->u * n;
    n.Normalize( );
    m->normal = n;
    v1 = B->u * v1 + b->position;
    m->contacts[0] = v1;
  }

  // Closest to v2
  else if(dot2 <= 0.0f)
  {
    if(PHY_NS::DistSqr( center, v2 ) > A->radius * A->radius)
      return;

    m->contact_count = 1;
    PHY_NS::Vec2 n = v2 - center;
    v2 = B->u * v2 + b->position;
    m->contacts[0] = v2;
    n = B->u * n;
    n.Normalize( );
    m->normal = n;
  }

  // Closest to face
  else
  {
    PHY_NS::Vec2 n = B->m_normals[faceNormal];
    if(PHY_NS::Dot( center - v1, n ) > A->radius)
      return;

    n = B->u * n;
    m->normal = -n;
    m->contacts[0] = m->normal * A->radius + a->position;
    m->contact_count = 1;
  }
}

void PHY_NS::PolygontoCircle( PHY_NS::Manifold *m, PHY_NS::Body *a, PHY_NS::Body *b )
{
  CircletoPolygon( m, b, a );
  m->normal = -m->normal;
}

PHY_NS::real FindAxisLeastPenetration( PHY_NS::uint32 *faceIndex, PHY_NS::PolygonShape *A, PHY_NS::PolygonShape *B )
{
  PHY_NS::real bestDistance = -FLT_MAX;
  PHY_NS::uint32 bestIndex;

  for(PHY_NS::uint32 i = 0; i < A->m_vertexCount; ++i)
  {
    // Retrieve a face normal from A
    PHY_NS::Vec2 n = A->m_normals[i];
    PHY_NS::Vec2 nw = A->u * n;

    // Transform face normal into B's model space
    PHY_NS::Mat2 buT = B->u.Transpose( );
    n = buT * nw;

    // Retrieve support point from B along -n
    PHY_NS::Vec2 s = B->GetSupport( -n );

    // Retrieve vertex on face from A, transform into
    // B's model space
    PHY_NS::Vec2 v = A->m_vertices[i];
    v = A->u * v + A->body->position;
    v -= B->body->position;
    v = buT * v;

    // Compute penetration distance (in B's model space)
    PHY_NS::real d = PHY_NS::Dot( n, s - v );

    // Store greatest distance
    if(d > bestDistance)
    {
      bestDistance = d;
      bestIndex = i;
    }
  }

  *faceIndex = bestIndex;
  return bestDistance;
}

void FindIncidentFace( PHY_NS::Vec2 *v, PHY_NS::PolygonShape *RefPoly, PHY_NS::PolygonShape *IncPoly, PHY_NS::uint32 referenceIndex )
{
  PHY_NS::Vec2 referenceNormal = RefPoly->m_normals[referenceIndex];

  // Calculate normal in incident's frame of reference
  referenceNormal = RefPoly->u * referenceNormal; // To world space
  referenceNormal = IncPoly->u.Transpose( ) * referenceNormal; // To incident's model space

  // Find most anti-normal face on incident polygon
  PHY_NS::int32 incidentFace = 0;
  PHY_NS::real minDot = FLT_MAX;
  for(PHY_NS::uint32 i = 0; i < IncPoly->m_vertexCount; ++i)
  {
    PHY_NS::real dot = PHY_NS::Dot( referenceNormal, IncPoly->m_normals[i] );
    if(dot < minDot)
    {
      minDot = dot;
      incidentFace = i;
    }
  }

  // Assign face vertices for incidentFace
  v[0] = IncPoly->u * IncPoly->m_vertices[incidentFace] + IncPoly->body->position;
  incidentFace = incidentFace + 1 >= (PHY_NS::int32)IncPoly->m_vertexCount ? 0 : incidentFace + 1;
  v[1] = IncPoly->u * IncPoly->m_vertices[incidentFace] + IncPoly->body->position;
}

PHY_NS::int32 Clip( PHY_NS::Vec2 n, PHY_NS::real c, PHY_NS::Vec2 *face )
{
  PHY_NS::uint32 sp = 0;
  PHY_NS::Vec2 out[2] = {
    face[0],
    face[1]
  };

  // Retrieve distances from each endpoint to the line
  // d = ax + by - c
  PHY_NS::real d1 = PHY_NS::Dot( n, face[0] ) - c;
  PHY_NS::real d2 = PHY_NS::Dot( n, face[1] ) - c;

  // If negative (behind plane) clip
  if(d1 <= 0.0f) out[sp++] = face[0];
  if(d2 <= 0.0f) out[sp++] = face[1];

  // If the points are on different sides of the plane
  if(d1 * d2 < 0.0f) // less than to ignore -0.0f
  {
    // Push interesection point
    PHY_NS::real alpha = d1 / (d1 - d2);
    out[sp] = face[0] + alpha * (face[1] - face[0]);
    ++sp;
  }

  // Assign our new converted values
  face[0] = out[0];
  face[1] = out[1];

  assert( sp != 3 );

  return sp;
}

void PHY_NS::PolygontoPolygon( PHY_NS::Manifold *m, PHY_NS::Body *a, PHY_NS::Body *b )
{
  PHY_NS::PolygonShape *A = reinterpret_cast<PHY_NS::PolygonShape *>(a->shape);
  PHY_NS::PolygonShape *B = reinterpret_cast<PHY_NS::PolygonShape *>(b->shape);
  m->contact_count = 0;

  // Check for a separating axis with A's face planes
  PHY_NS::uint32 faceA;
  PHY_NS::real penetrationA = FindAxisLeastPenetration( &faceA, A, B );
  if(penetrationA >= 0.0f)
    return;

  // Check for a separating axis with B's face planes
  PHY_NS::uint32 faceB;
  PHY_NS::real penetrationB = FindAxisLeastPenetration( &faceB, B, A );
  if(penetrationB >= 0.0f)
    return;

  PHY_NS::uint32 referenceIndex;
  bool flip; // Always point from a to b

  PHY_NS::PolygonShape *RefPoly; // Reference
  PHY_NS::PolygonShape *IncPoly; // Incident

  // Determine which shape contains reference face
  if(PHY_NS::BiasGreaterThan( penetrationA, penetrationB ))
  {
    RefPoly = A;
    IncPoly = B;
    referenceIndex = faceA;
    flip = false;
  }

  else
  {
    RefPoly = B;
    IncPoly = A;
    referenceIndex = faceB;
    flip = true;
  }

  // World space incident face
  PHY_NS::Vec2 incidentFace[2];
  FindIncidentFace( incidentFace, RefPoly, IncPoly, referenceIndex );

  //        y
  //        ^  ->n       ^
  //      +---c ------posPlane--
  //  x < | i |\
  //      +---+ c-----negPlane--
  //             \       v
  //              r
  //
  //  r : reference face
  //  i : incident poly
  //  c : clipped point
  //  n : incident normal

  // Setup reference face vertices
  PHY_NS::Vec2 v1 = RefPoly->m_vertices[referenceIndex];
  referenceIndex = referenceIndex + 1 == RefPoly->m_vertexCount ? 0 : referenceIndex + 1;
  PHY_NS::Vec2 v2 = RefPoly->m_vertices[referenceIndex];

  // Transform vertices to world space
  v1 = RefPoly->u * v1 + RefPoly->body->position;
  v2 = RefPoly->u * v2 + RefPoly->body->position;

  // Calculate reference face side normal in world space
  PHY_NS::Vec2 sidePlaneNormal = (v2 - v1);
  sidePlaneNormal.Normalize( );

  // Orthogonalize
  PHY_NS::Vec2 refFaceNormal( sidePlaneNormal.y, -sidePlaneNormal.x );

  // ax + by = c
  // c is distance from origin
  PHY_NS::real refC = PHY_NS::Dot( refFaceNormal, v1 );
  PHY_NS::real negSide = -PHY_NS::Dot( sidePlaneNormal, v1 );
  PHY_NS::real posSide =  PHY_NS::Dot( sidePlaneNormal, v2 );

  // Clip incident face to reference face side planes
  if(Clip( -sidePlaneNormal, negSide, incidentFace ) < 2)
    return; // Due to floating point error, possible to not have required points

  if(Clip(  sidePlaneNormal, posSide, incidentFace ) < 2)
    return; // Due to floating point error, possible to not have required points

  // Flip
  m->normal = flip ? -refFaceNormal : refFaceNormal;

  // Keep points behind reference face
  PHY_NS::uint32 cp = 0; // clipped points behind reference face
  PHY_NS::real separation = PHY_NS::Dot( refFaceNormal, incidentFace[0] ) - refC;
  if(separation <= 0.0f)
  {
    m->contacts[cp] = incidentFace[0];
    m->penetration = -separation;
    ++cp;
  }
  else
    m->penetration = 0;

  separation = PHY_NS::Dot( refFaceNormal, incidentFace[1] ) - refC;
  if(separation <= 0.0f)
  {
    m->contacts[cp] = incidentFace[1];

    m->penetration += -separation;
    ++cp;

    // Average penetration
    m->penetration /= (PHY_NS::real)cp;
  }

  m->contact_count = cp;
}
