#include "debugShapeBuilder.h"
#include "vertex.h"
#include "boundingAABB.h"
#include "mathDef.h"
#include "debugAABB.h"
#include "debugShapeFactory.h"
#include "debugShapeTypes.h"
#include "boundingSphere.h"
#include "debugSphere.h"
#include "boundingOBB.h"
#include "debugOBB.h"
#include "gfxLogger.h"
#include "boundingEllipse.h"
#include "debugEllipseDX10.h"
#include "debugAABBDX10.h"
#include "color.h"
#include "delegate.h"
#include "cameraManager.h"
#include "pivotCamera.h"

#include <map>
#include <functional>
#include <xutility>

namespace GFX
{
    DebugShapeBuilder::DebugShapeBuilder()
    {
    }

    DebugShape* DebugShapeBuilder::BuildAABB(const Vertex* vertList,
                                             unsigned int vertCount)
    {
        DebugAABB* shape = new DebugAABB;
        Phy::BoundingAABB* aabb = new Phy::BoundingAABB;
        Math::Vec3F minVec(Math::FLOAT_MAX,Math::FLOAT_MAX,Math::FLOAT_MAX);
        Math::Vec3F maxVec(-Math::FLOAT_MAX,-Math::FLOAT_MAX,-Math::FLOAT_MAX);

        for(unsigned int i = 0; i < vertCount; ++i)
        {
            Math::Vec3F pos(vertList[i].pos_);
            minVec.X(pos.X() < minVec.X() ? pos.X() : minVec.X());
            minVec.Y(pos.Y() < minVec.Y() ? pos.Y() : minVec.Y());
            minVec.Z(pos.Z() < minVec.Z() ? pos.Z() : minVec.Z());

            maxVec.X(pos.X() > maxVec.X() ? pos.X() : maxVec.X());
            maxVec.Y(pos.Y() > maxVec.Y() ? pos.Y() : maxVec.Y());
            maxVec.Z(pos.Z() > maxVec.Z() ? pos.Z() : maxVec.Z());
        }
        aabb->VertCount(vertCount);
        aabb->MinPt(minVec);
        aabb->MaxPt(maxVec);
        aabb->Center((minVec + maxVec) / 2.0f);
        shape->Shape(aabb);
        shape->Load();
        return shape;
    }

    DebugShape* DebugShapeBuilder::BuildCentroidSphere(const Vertex* vertList,
                                                       unsigned int vertCount)
    {
        DebugSphere* shape = new DebugSphere;
        Phy::BoundingSphere* sphere = new Phy::BoundingSphere;
        Math::Vec3F maxVec(-Math::FLOAT_MAX,-Math::FLOAT_MAX,-Math::FLOAT_MAX);
        Math::Vec3F centroid(Math::Vec3F::Zero);

        for(unsigned int i = 0; i < vertCount; ++i)
        {
            Math::Vec3F pos(vertList[i].pos_);
            maxVec.X(pos.X() > maxVec.X() ? pos.X() : maxVec.X());
            maxVec.Y(pos.Y() > maxVec.Y() ? pos.Y() : maxVec.Y());
            maxVec.Z(pos.Z() > maxVec.Z() ? pos.Z() : maxVec.Z());
            centroid += pos;
        }    

        centroid /= static_cast<float>(vertCount);
        sphere->VertCount(vertCount);
        sphere->Center(centroid);
        sphere->Radius((maxVec - centroid).Length());
        shape->Shape(sphere);
        shape->Load();
        return shape;
    }

    DebugShape* DebugShapeBuilder::BuildRitterSphere(const Vertex* vertList,
                                                     unsigned int vertCount)
    {
        // Min point on x,y,z axis
        Math::Vec3F xmin,xmax,ymin,ymax,zmin,zmax;

        xmin = ymin = zmin = Math::Vec3F(Math::FLOAT_MAX,Math::FLOAT_MAX,Math::FLOAT_MAX);
        xmax = ymax = zmax = Math::Vec3F(-Math::FLOAT_MAX,-Math::FLOAT_MAX,-Math::FLOAT_MAX);

        // Get the max and min values
        for(unsigned int i = 0; i < vertCount; ++i)
        {
            Math::Vec3F pos(vertList[i].pos_);
            if(pos.X() < xmin.X())
                xmin = pos;
            if(pos.X() > xmax.X())
                xmax = pos;

            if(pos.Y() < ymin.Y())
                ymin = pos;
            if(pos.Y() > ymax.Y())
                ymax = pos;

            if(pos.Z() < zmin.Z())
                zmin = pos;
            if(pos.Z() > zmax.Z())
                zmax = pos;
        }

        // The square distance of the max and min points on the x,y,z
        float xspan,yspan,zspan,maxspan;

        // The change along the x,y,z axis
        Math::Vec3F delta;

        // Calculating spans
        delta = xmax - xmin;
        xspan = delta.SquareLength();

        delta = ymax - ymin;
        yspan = delta.SquareLength();

        delta = zmax - zmin;
        zspan = delta.SquareLength();

        // Determine the maxspan and diameter
        // Assume the xspan is the largest
        Math::Vec3F dia1 = xmin; 
        Math::Vec3F dia2 = xmax; 
        maxspan = xspan;

        if (yspan > maxspan)
        {
            maxspan = yspan;
            dia1 = ymin; 
            dia2 = ymax;
        }

        if (zspan > maxspan)
        {
            dia1 = zmin; 
            dia2 = zmax;
        }

        // The center of the sphere
        Math::Vec3F center;

        // Use the diameter to calculate the center of the sphere
        center = (dia1 + dia2) / 2.0f;

        // Radius and radius squared
        float radius;
        float radiusSqr;

        // Determine the radius and radius squared
        delta = dia2 - center;
        radiusSqr = delta.SquareLength();
        radius = std::sqrt(radiusSqr);

        // Increment the radius to fit all points;
        for(unsigned int i = 0; i < vertCount; ++i)
        {
            // Distance from a point to the center
            float ptToCenter,ptToCenterSqr;
            Math::Vec3F point(vertList[i].pos_);

            // Get the distance from a point to the center
            delta = center - point;
            ptToCenterSqr = delta.SquareLength();

            // Check if point is outside of the radius
            if(ptToCenterSqr > radiusSqr)
            {
                // Difference of the old radius to the new
                float dRadius;

                ptToCenter = std::sqrt(ptToCenterSqr);

                // The new radius is the average of the new and old radius
                radius = (radius + ptToCenter) / 2.0f;
                radiusSqr = radius * radius;
                dRadius = ptToCenter - radius;

                // Calculate the new center
                center = (radius * center + dRadius * point) / ptToCenter;
            }
        }

        DebugSphere* shape = new DebugSphere;
        Phy::BoundingSphere* sphere = new Phy::BoundingSphere;
        sphere->VertCount(vertCount);
        sphere->Center(center);
        sphere->Radius(radius);
        shape->Shape(sphere);
        shape->Load();
        return shape;
    }

    DebugShape* DebugShapeBuilder::BuildLarssonSphere(const Vertex* vertList,
                                                      unsigned int vertCount)
    {
        unsigned int normalCount = 12;
        Math::Vec3F* normalArray = new Math::Vec3F[normalCount];
        normalArray[0]  = Math::Vec3F(2,2,1);
        normalArray[1]  = Math::Vec3F(1,2,2);
        normalArray[2]  = Math::Vec3F(2,1,2);
        normalArray[3]  = Math::Vec3F(2,-2,1);
        normalArray[4]  = Math::Vec3F(2,2,-1);
        normalArray[5]  = Math::Vec3F(2,-2,-1);
        normalArray[6]  = Math::Vec3F(1,-2,2);
        normalArray[7]  = Math::Vec3F(1,2,-2);
        normalArray[8]  = Math::Vec3F(1,-2,-2);
        normalArray[9]  = Math::Vec3F(2,-1,2);
        normalArray[10] = Math::Vec3F(2,1,-2);
        normalArray[11] = Math::Vec3F(2,-1,-2);

        // Initialize extents array
        unsigned int extentCount = normalCount * 2;
        std::pair<float,const Math::Vec3F*>* extents = new std::pair<float,const Math::Vec3F*>[extentCount];
        for(unsigned int extentIndex = 0; extentIndex < extentCount; ++extentIndex)
        {
            extents[extentIndex].first  = (extentIndex % 2 == 0) ? Math::FLOAT_MAX : -Math::FLOAT_MAX;
            extents[extentIndex].second = NULL;
        }

        // Project sample points along each normal and get initial extents
        for(unsigned int normIndex = 0, extentIndex = 0; normIndex < normalCount; ++normIndex, ++extentIndex)
        {
            const Math::Vec3F& normal = normalArray[normIndex];
            std::pair<float,const Math::Vec3F*>* minExtent = &extents[extentIndex];
            std::pair<float,const Math::Vec3F*>* maxExtent = &extents[++extentIndex];
            for(unsigned int vertIndex = 0; vertIndex < vertCount; ++vertIndex)
            {
                float dot = vertList[vertIndex].pos_.Dot(normal);
                if(dot < minExtent->first)
                {
                    minExtent->first  = dot;
                    minExtent->second = &vertList[vertIndex].pos_; 
                }
                else if(dot > maxExtent->first)
                {
                    maxExtent->first  = dot;
                    maxExtent->second = &vertList[vertIndex].pos_; 
                }
            }
        }

        unsigned int maxExtentIndex = 0;
        unsigned int minExtentIndex = 1;

        // Determine axis that has the largest spread
        for(unsigned int extentIndex = 2; extentIndex < extentCount; extentIndex += 2)
        {
            float currSpread = (*extents[maxExtentIndex].second - *extents[minExtentIndex].second).SquareLength();
            float newSpread = (*extents[extentIndex].second - *extents[extentIndex + 1].second).SquareLength();

            if(currSpread < newSpread)
            {
                maxExtentIndex = extentIndex;
                minExtentIndex = extentIndex + 1;
            }
        }

        // Loop through rest of the points and increase sphere to include each
        // point outside the BV
        // Do not need to check first 2 and last 2 points as they were the 
        // sample points
        float radiusSqr = std::pow((*extents[maxExtentIndex].second - *extents[minExtentIndex].second).Length() / 2.0f,2);
        Math::Vec3F center = (*extents[maxExtentIndex].second + *extents[minExtentIndex].second) / 2.0f;
        for(unsigned int i = 0; i < vertCount; ++i)
        {
            float distSqr = (vertList[i].pos_ - center).SquareLength();
            if(distSqr > radiusSqr)
            {
                float ptToCenter = std::sqrt(distSqr);
                float radius = std::sqrt(radiusSqr);

                radius = (radius + ptToCenter) / 2.0f;
                radiusSqr = radius * radius;

                float dRadius = ptToCenter - radius;

                center = (radius * center + dRadius * vertList[i].pos_) / ptToCenter;
            }
        }

        DebugSphere* shape = new DebugSphere;
        Phy::BoundingSphere* sphere = new Phy::BoundingSphere;
        sphere->VertCount(vertCount);
        sphere->Center(center);
        sphere->Radius(std::sqrt(radiusSqr));
        shape->Shape(sphere);
        shape->Load();
        return shape;
    }

    DebugShape* DebugShapeBuilder::BuildPCASphere(const Vertex* vertList,
                                                  unsigned int vertCount)
    {
        Math::Vec3F centroid = Math::Vec3F::Zero;
        for(unsigned int i = 0; i < vertCount; ++i)
            centroid += vertList[i].pos_;
        centroid /= static_cast<float>(vertCount);

        Math::Mtx4F m, v;

        // Compute the covariance matrix m
        calculateCovarianceMatrix(vertList,vertCount,centroid,m);

        // Decompose it into eigenvectors (in v) and eigenvalues (in m)
        jacobi(m, v);

        // Find the component with largest magnitude eigenvalue (largest spread)
        Math::Vec3F e;
        int maxc = 0;
        float maxf = std::abs(m[1][1]), maxe = std::abs(m[0][0]);
        if(maxf > maxe)
        {
            maxc = 1;
            maxe = maxf;
        }

        maxf = std::abs(m[2][2]);

        if (maxf > maxe)
        {
            maxc = 2;
            maxe = maxf;
        }

        e.X(v[maxc][0]);
        e.Y(v[maxc][1]);
        e.Z(v[maxc][2]);

        int emin, emax;
        extremePoints(e, vertList, vertCount, &emin, &emax);
        float dist = (vertList[emax].pos_ - vertList[emin].pos_).Length() / 2.0f;

        DebugSphere* shape = new DebugSphere;
        Phy::BoundingSphere* sphere = new Phy::BoundingSphere;
        sphere->VertCount(vertCount);
        sphere->Center(centroid);
        sphere->Radius(dist);
        shape->Shape(sphere);
        shape->Load();
        return shape;
    }

    DebugShape* DebugShapeBuilder::BuildPCAEllipse( const Vertex* vertList, 
                                                    unsigned int vertCount )
    {
        Math::Vec3F centroid = Math::Vec3F::Zero;
        for(unsigned int i = 0; i < vertCount; ++i)
            centroid += vertList[i].pos_;
        centroid /= static_cast<float>(vertCount);

        Math::Mtx4F m, v;

        // Compute the covariance matrix m
        calculateCovarianceMatrix(vertList,vertCount,centroid,m);

        // Decompose it into eigenvectors (in v) and eigenvalues (in m)
        jacobi(m, v);

        // Find the component with largest magnitude eigenvalue (largest spread)
        Math::Vec3F iVec,jVec,kVec;
        int indexMin, indexMax;
        float k;

        iVec.X(v[0][0]);
        iVec.Y(v[0][1]);
        iVec.Z(v[0][2]);
        extremePoints(iVec, vertList, vertCount, &indexMin, &indexMax);
        k = (vertList[indexMax].pos_ - vertList[indexMin].pos_).Length() / 2.0f;
        iVec *= k;

        jVec.X(v[1][0]);
        jVec.Y(v[1][1]);
        jVec.Z(v[1][2]);
        extremePoints(jVec, vertList, vertCount, &indexMin, &indexMax);
        k = (vertList[indexMax].pos_ - vertList[indexMin].pos_).Length() / 2.0f;
        jVec *= k;

        kVec.X(v[2][0]);
        kVec.Y(v[2][1]);
        kVec.Z(v[2][2]);
        extremePoints(kVec, vertList, vertCount, &indexMin, &indexMax);
        k = (vertList[indexMax].pos_ - vertList[indexMin].pos_).Length() / 2.0f;
        kVec *= k;

        DebugEllipse* shape = new DebugEllipse;
        Phy::BoundingEllipse* ellipse = new Phy::BoundingEllipse;
        ellipse->VertCount(vertCount);
        ellipse->Center(centroid);
        ellipse->I(iVec);
        ellipse->J(jVec);
        ellipse->K(kVec);
        shape->Shape(ellipse);
        shape->Load();
        return shape;
    }

    DebugShape* DebugShapeBuilder::BuildPCAOBB( const Vertex* vertList,
                                                unsigned int vertCount )
    {
        Math::Vec3F centroid = Math::Vec3F::Zero;
        for(unsigned int vertIndex = 0; vertIndex < vertCount; ++vertIndex)
            centroid += vertList[vertIndex].pos_;
        centroid /= static_cast<float>(vertCount);

        Math::Mtx4F m, v;

        // Compute the covariance matrix m
        calculateCovarianceMatrix(vertList,vertCount,centroid,m);

        // Decompose it into eigenvectors (in v) and eigenvalues (in m)
        jacobi(m, v);

        // Find the component with largest magnitude eigenvalue (largest spread)
        Math::Vec3F i,j,k;
        std::multimap<float,unsigned int,std::greater<float> > axes;
        axes.insert(std::pair<float,unsigned int>(std::abs(m[0][0]),0));
        axes.insert(std::pair<float,unsigned int>(std::abs(m[1][1]),1));
        axes.insert(std::pair<float,unsigned int>(std::abs(m[2][2]),2));
        unsigned int col = axes.begin()->second;

        i.X(v[col][0]);
        i.Y(v[col][1]);
        i.Z(v[col][2]);

        col = (++axes.begin())->second;
        j.X(v[col][0]);
        j.Y(v[col][1]);
        j.Z(v[col][2]);
        k = i.Cross(j);
        
        int imin, imax, jmin, jmax, kmin, kmax;
        extremePoints(i, vertList, vertCount, &imin, &imax);
        i = i * (i.ProjectOnto(vertList[imax].pos_) - centroid).Length();

        extremePoints(j, vertList, vertCount, &jmin, &jmax);
        j = j * (j.ProjectOnto(vertList[jmax].pos_) - centroid).Length();

        extremePoints(k, vertList, vertCount, &kmin, &kmax);
        k = k *(k.ProjectOnto(vertList[kmax].pos_) - centroid).Length();

        DebugOBB* shape = new DebugOBB;
        Phy::BoundingOBB* box = new Phy::BoundingOBB;
        box->VertCount(vertCount);
        box->Center(centroid);
        box->Radius(i,j,k);
        return shape;
    }

    DebugShape* DebugShapeBuilder::BuildPCAAABB( const Vertex* vertList,
                                                 unsigned int vertCount )
    {
        Math::Vec3F centroid = Math::Vec3F::Zero;
        for(unsigned int vertIndex = 0; vertIndex < vertCount; ++vertIndex)
            centroid += vertList[vertIndex].pos_;
        centroid /= static_cast<float>(vertCount);

        Math::Mtx4F m, v;

        // Compute the covariance matrix m
        calculateCovarianceMatrix(vertList,vertCount,centroid,m);

        // Decompose it into eigenvectors (in v) and eigenvalues (in m)
        jacobi(m, v);

        // Find the component with largest magnitude eigenvalue (largest spread)
        Math::Vec3F i,j,k;
        i.X(v[0][0]);
        i.Y(v[0][1]);
        i.Z(v[0][2]);

        j.X(v[1][0]);
        j.Y(v[1][1]);
        j.Z(v[1][2]);

        k.X(v[2][0]);
        k.Y(v[2][1]);
        k.Z(v[2][2]);

        // Find the most extreme points along direction 'e'
        int imin, imax, jmin, jmax, kmin, kmax;
        extremePoints(i, vertList, vertCount, &imin, &imax);
        extremePoints(j, vertList, vertCount, &jmin, &jmax);
        extremePoints(k, vertList, vertCount, &kmin, &kmax);

        Math::Vec3F minRadius(min(vertList[imin].pos_.X(),min(vertList[jmin].pos_.X(),vertList[kmin].pos_.X())),
                                min(vertList[imin].pos_.Y(),min(vertList[jmin].pos_.Y(),vertList[kmin].pos_.Y())),
                                min(vertList[imin].pos_.Z(),min(vertList[jmin].pos_.Z(),vertList[kmin].pos_.Z())));
        
        Math::Vec3F maxRadius(max(vertList[imax].pos_.X(),max(vertList[jmax].pos_.X(),vertList[kmax].pos_.X())),
                                max(vertList[imax].pos_.Y(),max(vertList[jmax].pos_.Y(),vertList[kmax].pos_.Y())),
                                max(vertList[imax].pos_.Z(),max(vertList[jmax].pos_.Z(),vertList[kmax].pos_.Z())));

        DebugAABB* shape =  new DebugAABB;
        Phy::BoundingAABB* box = new Phy::BoundingAABB;
        box->VertCount(vertCount);
        box->MinPt(minRadius);
        box->MaxPt(maxRadius);
        shape->Shape(box);
        shape->Load();
        return shape;
    }

    //--------------------------------------------------------------------------

    void DebugShapeBuilder::calculateCovarianceMatrix(const Vertex* vertList,
                                                      unsigned int vertCount,
                                                      const Math::Vec3F& centroid,
                                                      Math::Mtx4F& result)
    {
        result = Math::Mtx4F::Identity;

        // Translate points so center of mass is at the origin
        for(unsigned int i = 0; i < vertCount; ++i)
        {
            Math::Vec3F p = vertList[i].pos_ - centroid;
            result[0][0] += p.X() * p.X();
            result[1][1] += p.Y() * p.Y();
            result[2][2] += p.Z() * p.Z();
            result[0][1] += p.X() * p.Y();
            result[0][2] += p.X() * p.Z();
            result[1][2] += p.Y() * p.Z();
        }

        float k = 1.0f/static_cast<float>(vertCount);
        result[0][0] *= k;
        result[1][1] *= k;
        result[2][2] *= k;
        result[0][1] = result[1][0] *= k;
        result[0][2] = result[2][0] *= k;
        result[1][2] = result[2][1] *= k;
    }

    void DebugShapeBuilder::realSchurDecomposition(Math::Mtx4F& covariance,
                                                   unsigned int p,
                                                   unsigned int q,
                                                   float& c,
                                                   float& s)
    {
        if(std::abs(covariance[p][q]) > Math::EPSILON)
        {
            float r = (covariance[q][q] - covariance[p][p]) / (2.0f * covariance[p][q]); 
            float t;

            if (r >= 0.0f)
                t = 1.0f / (r + std::sqrt(1.0f + r*r));
            else
                t = -1.0f / (-r + std::sqrt(1.0f + r*r));
            c = 1.0f / std::sqrt(1.0f + t*t);
            s = t * c;
        } 
        else 
        {
            c = 1.0f;
            s = 0.0f;
        }
    }

    void DebugShapeBuilder::jacobi( Math::Mtx4F& a,
                                    Math::Mtx4F& v )
    {
        int r, c, n, p, q;
        float prevoff = 0.f;
        
        // Initialize v to identity matrix
        v = Math::Mtx4F::Identity;

        // Repeat for some maximum number of iterations
        const int MAX_ITERATIONS = 50;
        for (n = 0; n < MAX_ITERATIONS; n++) 
        {
            // Find largest off-diagonal absolute element a[p][q]
            p = 0; q = 1;
            for (c = 0; c < 3; c++) 
            {
                for (r = 0; r < 3; r++) 
                {
                    if (c == r)
                        continue;

                    if (std::abs(a[c][r]) > std::abs(a[p][q])) 
                    {
                        p = c;
                        q = r;
                    }
                }
            }

            // Compute the Jacobi rotation matrix J(p, q, theta)
            // (This code can be optimized for the three different cases of rotation)
            float cosine, sine;
            realSchurDecomposition(a, p, q, cosine, sine);
            Math::Mtx4F jacobiMtx = Math::Mtx4F::Identity;

            jacobiMtx[p][p] = cosine; 
            jacobiMtx[p][q] = sine;
            jacobiMtx[q][p] = -sine; 
            jacobiMtx[q][q] = cosine;

            // Cumulate rotations into what will contain the eigenvectors
            v = v * jacobiMtx;

            // Make 'a' more diagonal, until just eigenvalues remain on diagonal
            a = (jacobiMtx.Transposed() * a) * jacobiMtx;

            // Compute "norm" of off-diagonal elements
            float off = 0.0f;
            for (c = 0; c < 3; c++) 
            {
                for (r = 0; r < 3; r++) 
                {
                    if (c == r) 
                        continue; 
                    off += a[c][r] * a[c][r];
                }
            }

            // off = sqrt(off); not needed for norm comparison 
            // Stop when norm no longer decreasing
            if (n > 2 && off >= prevoff)
                return;

            prevoff = off;
        }
    }

    void DebugShapeBuilder::extremePoints( const Math::Vec3F& dir,
                                           const Vertex* vertList,
                                           int n,
                                           int* imin,
                                           int* imax )
    {
        float minproj = Math::FLOAT_MAX, maxproj = -Math::FLOAT_MAX;
        for (int i = 0; i < n; i++) 
        {
            // Project vector from origin to point onto direction vector
            float proj = vertList[i].pos_.Dot(dir);

            // Keep track of least distant point along direction vector
            if (proj < minproj) 
            {
                minproj = proj;
                *imin = i;
            }

            // Keep track of most distant point along direction vector
            if (proj > maxproj) 
            {
                maxproj = proj;
                *imax = i;
            }
        }
    }
    
    Util::BinaryTree<DebugShape*>* DebugShapeBuilder::BuildTree( const Util::BinaryTree<Phy::BoundingVolume*>& tree)
    {
        // Generate level colors
        unsigned int depth = tree.MaxDepth() + 1;
        Color* colorArray = new Color[depth];
        HSV hsvColor(0,1,1,1);
        float hueDelta = 270.f/(float)depth;

        for(unsigned int i = 0; i < depth; ++i)
        {
            Color hsvToRGB(ConvertHSVToRGB(hsvColor));
            colorArray[i] = hsvToRGB;
            hsvColor.H(hsvColor.H() + hueDelta);
        }

        Util::BinaryTree<DebugShape*>* root = new Util::BinaryTree<DebugShape*>;

        DebugShape* shape = 0;
        switch(tree.Data()->Type())
        {
        case Phy::BV_AABB:
            shape = new DebugAABB;
            break;
        case Phy::BV_OBB:
            shape = new DebugOBB;
            break;
        case Phy::BV_SPHERE:
            shape = new DebugSphere;
            break;
        case Phy::BV_ELLIPSE:
            shape = new DebugEllipse;
            break;
        }
        shape->Shape(tree.Data());
        shape->ShapeColor(colorArray[0]);
        shape->Load();
        root->Data(shape);

        if(tree.Left())
            root->Left(buildTreeRecurrive(*tree.Left(),1,colorArray));
        if(tree.Right())
            root->Right(buildTreeRecurrive(*tree.Right(),1,colorArray));

        delete colorArray;
        return root;
    }

    void DebugShapeBuilder::DestroyTree( Util::BinaryTree<DebugShape*>*& tree )
    {
        tree->DepthFirst(tree,Util::Delegate::CreateDelegate<DebugShapeBuilder,&DebugShapeBuilder::deleteDebugShapeCallback>(this));
    }

    void DebugShapeBuilder::RenderTree( Util::BinaryTree<DebugShape*>*& tree )
    {
        tree->DepthFirst(tree,Util::Delegate::CreateDelegate<DebugShapeBuilder,&DebugShapeBuilder::renderDebugShapeCallback>(this));
    }

    void DebugShapeBuilder::RenderTreeNodesWithDepth(unsigned int depth,
                                                     Util::BinaryTree<DebugShape*>*& tree)
    {
        renderTreeNodeMaxDepth_ = depth;
        tree->DepthFirst(tree,Util::Delegate::CreateDelegate<DebugShapeBuilder,&DebugShapeBuilder::renderDebugShapeWithMaxDepthCallback>(this));
    }

    //--------------------------------------------------------------------------

    Util::BinaryTree<DebugShape*>* DebugShapeBuilder::buildTreeRecurrive( const Util::BinaryTree<Phy::BoundingVolume*>& tree,
                                                                          unsigned int level,
                                                                          Color* colorArray)
    {
        Util::BinaryTree<DebugShape*>* node = new Util::BinaryTree<DebugShape*>;

        DebugShape* shape = 0;
        switch(tree.Data()->Type())
        {
        case Phy::BV_AABB:
            shape = new DebugAABB;
            break;
        case Phy::BV_OBB:
            shape = new DebugOBB;
            break;
        case Phy::BV_SPHERE:
            shape = new DebugSphere;
            break;
        case Phy::BV_ELLIPSE:
            shape = new DebugEllipse;
            break;
        }
        shape->Shape(tree.Data());
        shape->ShapeColor(colorArray[level]);
        shape->Load();
        node->Data(shape);

        if(tree.Left())
            node->Left(buildTreeRecurrive(*tree.Left(),level + 1,colorArray));
        if(tree.Right())
            node->Right(buildTreeRecurrive(*tree.Right(),level + 1,colorArray));
        return node;
    }

    void DebugShapeBuilder::deleteDebugShapeCallback( Util::DelegateParameter& param )
    {
        Util::BinaryTree<DebugShape*>* node = param.As<Util::BinaryTree<DebugShape*>::TreeDelegateParam>().node_;

        if(node->Data())
        {
            delete node->Data();
            node->Data(0);
        }
    }

    void DebugShapeBuilder::renderDebugShapeCallback( Util::DelegateParameter& param )
    {
        Util::BinaryTree<DebugShape*>* node = param.As<Util::BinaryTree<DebugShape*>::TreeDelegateParam>().node_;

        if(node->Data())
        {
            CameraManager* camMgr = CameraManager::InstancePtr();
            PivotCamera* camera = reinterpret_cast<PivotCamera*>(camMgr->GetCamera(camMgr->GetCurrentCamera()));
            node->Data()->RenderSetup();
            node->Data()->Render(camera,0,0);
        }
    }

    void DebugShapeBuilder::renderDebugShapeWithMaxDepthCallback(Util::DelegateParameter& param)
    {
        Util::BinaryTree<DebugShape*>* node = param.As<Util::BinaryTree<DebugShape*>::TreeDelegateParam>().node_;

        if(node->Data() && node->MaxDepth() == renderTreeNodeMaxDepth_)
        {
            CameraManager* camMgr = CameraManager::InstancePtr();
            PivotCamera* camera = reinterpret_cast<PivotCamera*>(camMgr->GetCamera(camMgr->GetCurrentCamera()));
            node->Data()->RenderSetup();
            node->Data()->Render(camera,0,0);
        }
    }
}