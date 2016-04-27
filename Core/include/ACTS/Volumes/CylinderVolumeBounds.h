///////////////////////////////////////////////////////////////////
// CylinderVolumeBounds.h, ACTS project
///////////////////////////////////////////////////////////////////

#ifndef ACTS_VOLUMES_CYLINDERVOLUMESBOUNDS_H
#define ACTS_VOLUMES_CYLINDERVOLUMESBOUNDS_H 1

#include "ACTS/Utilities/Definitions.h"
#include "ACTS/Volumes/VolumeBounds.h"

namespace Acts {

   class Surface;
   class RadialBounds;
   class CylinderBounds;
   class RectangleBounds;

  /**
   @class CylinderVolumeBounds

   Bounds for a cylindrical Volume, the decomposeToSurfaces method creates a
   vector of up to 6 surfaces:

   case A) 3 Surfaces (full cylindrical tube):
    BoundarySurfaceFace [index]:
        - negativeFaceXY [0] : Acts::DiscSurface with \f$ r_{inner}=0 \f$,
                               parallel to \f$ xy \f$ plane at negative \f$ z \f$
        - positiveFaceXY [1] : Acts::DiscSurface with \f$ r_{inner}=0 \f$,
                               parallel to \f$ xy \f$ plane at positive \f$ z \f$
        - cylinderCover  [2] : Acts::CylinderSurface confining the Acts::Volume

   case B) 4 Surfaces (tube with inner and outer radius):
    BoundarySurfaceFace [index]:
        - negativeFaceXY [0] : Acts::DiscSurface with \f$ r_{inner}>0 \f$,
                               parallel to \f$ xy \f$ plane at negative \f$ z \f$
        - positiveFaceXY [1] : Acts::DiscSurface with \f$ r_{inner}>0 \f$,
                               parallel to \f$ xy \f$ plane at positive \f$ z \f$
        - tubeOuterCover [2] : Acts::CylinderSurface with \f$ r = r_{outer} \f$
        - tubeInnerCover [3] : Acts::CylinderSurface with \f$ r = r_{inner} \f$

   case C) 6 Surfaces (sectoral tube with inner and outer radius):
    BoundarySurfaceFace [index]:
        - negativeFaceXY        [0] : Acts::DiscSurface with \f$ r_{inner}>0  \f$ and \f$ \phi < \pi \f$,
                                      parallel to \f$ xy \f$ plane at negative \f$ z \f$
        - positiveFaceXY        [1] : Acts::DiscSurface with \f$ r_{inner}>0 \f$ and \f$ \phi < \pi \f$,
                                      parallel to \f$ xy \f$ plane at positive \f$ z \f$
        - tubeSectorOuterCover  [2] : Acts::CylinderSurface with \f$ r = r_{outer} \f$
        - tubeSectorInnerCover  [3] : Acts::CylinderSurface with \f$ r = r_{inner} \f$
        - tubeSectorNegativePhi [4] : Rectangular Acts::PlaneSurface attached to [0] and [1] at negative \f$ \phi \f$
        - tubeSectorNegativePhi [5] : Rectangular Acts::PlaneSurface attached to [0] and [1] at positive \f$ \phi \f$

    @image html CylinderVolumeBounds_decomp.gif

    @author Andreas.Salzburger@cern.ch
    */

 class CylinderVolumeBounds : public VolumeBounds {

  public:

    /** @enum BoundValues for readability */
    enum BoundValues {
        bv_innerRadius    = 0,
        bv_outerRadius    = 1,
        bv_halfPhiSector  = 2,
        bv_halfZ          = 3,
        bv_length         = 4
    };

    /**Default Constructor*/
    CylinderVolumeBounds();

    /**Constructor - full cylinder */
    CylinderVolumeBounds(double radius, double halez);

    /**Constructor - extruded cylinder */
    CylinderVolumeBounds(double rinner, double router, double halez);

    /**Constructor - extruded cylinder segment */
    CylinderVolumeBounds(double rinner, double router, double halfPhiSector, double halez);

    /**Copy Constructor */
    CylinderVolumeBounds(const CylinderVolumeBounds& cylbo);

    /**Destructor */
    virtual ~CylinderVolumeBounds();

    /**Assignment operator*/
    CylinderVolumeBounds& operator=(const CylinderVolumeBounds& cylbo);

    /**Virtual constructor */
    CylinderVolumeBounds* clone() const override;

    /**This method checks if position in the 3D volume frame is inside the cylinder*/
    bool inside(const Vector3D& , double tol=0.) const override;

    /** Method to decompose the Bounds into boundarySurfaces */
    const std::vector<const Acts::Surface*>* decomposeToSurfaces(std::shared_ptr<Transform3D> transformPtr) const override;

    /** Binning offset - overloaded for some R-binning types */
    virtual Vector3D binningOffset(BinningValue bValue) const override;

    /** Binning borders in double */
    virtual double binningBorder(BinningValue bValue) const override;


    /**This method returns the inner radius*/
    double  innerRadius() const;

    /**This method returns the outer radius*/
    double  outerRadius() const;

    /**This method returns the medium radius*/
    double  mediumRadius() const;

    /**This method returns the delta radius*/
    double  deltaRadius() const;

    /**This method returns the halfPhiSector angle*/
    double  halfPhiSector() const;

    /**This method returns the halflengthZ*/
    double  halflengthZ() const;

    /** Output Method for std::ostream */
    std::ostream& dump(std::ostream& sl) const override;

  private:
    /** templated dumpT method */
    template <class T> T& dumpT(T& t) const;

    /** This method returns the associated CylinderBounds of the inner CylinderSurfaces. */
    CylinderBounds* innerCylinderBounds() const;

    /** This method returns the associated CylinderBounds of the outer CylinderSurfaces. */
    CylinderBounds* outerCylinderBounds() const;

    /** This method returns the associated RadialBounds for the bottom/top DiscSurface. */
    RadialBounds* discBounds() const;

    /** This method returns the associated PlaneBounds limiting a sectoral CylinderVolume. */
    RectangleBounds* sectorPlaneBounds() const;

    /** The internal version of the bounds can be float/double*/
    std::vector<TDD_real_t>   m_boundValues;

    /** numerical stability */
    static double s_numericalStable;

 };

 inline CylinderVolumeBounds* CylinderVolumeBounds::clone() const
 { return new CylinderVolumeBounds(*this); }

 inline bool CylinderVolumeBounds::inside(const Vector3D &pos, double tol) const
 {
   double ros = pos.perp();
   bool insidePhi =  cos(pos.phi()) >= cos(m_boundValues[bv_halfPhiSector]) - tol;
   bool insideR = insidePhi ? ((ros >=  m_boundValues[bv_innerRadius] - tol ) && (ros <= m_boundValues[bv_outerRadius] + tol)) : false;
   bool insideZ = insideR ? (fabs(pos.z()) <= m_boundValues[bv_halfZ] + tol ) : false ;
   return (insideZ && insideR && insidePhi);
 }

 inline Vector3D CylinderVolumeBounds::binningOffset(BinningValue bValue) const
 {  // the medium radius is taken for r-type binning
    if (bValue == Acts::binR || bValue == Acts::binRPhi)
      return Vector3D(mediumRadius(),0.,0.);
    return VolumeBounds::binningOffset(bValue);
 }

 inline double CylinderVolumeBounds::binningBorder(BinningValue bValue) const
 {  // the medium radius is taken for r-type binning
    if (bValue == Acts::binR)
        return 0.5*deltaRadius();
    if (bValue == Acts::binZ)
        return halflengthZ();
    return VolumeBounds::binningBorder(bValue);
 }

 inline double CylinderVolumeBounds::innerRadius() const { return m_boundValues[bv_innerRadius]; }

 inline double CylinderVolumeBounds::outerRadius() const { return m_boundValues[bv_outerRadius]; }

 inline double CylinderVolumeBounds::mediumRadius() const { return 0.5*(m_boundValues[bv_innerRadius]+m_boundValues[bv_outerRadius]); }

 inline double CylinderVolumeBounds::deltaRadius() const { return (m_boundValues[bv_outerRadius]-m_boundValues[bv_innerRadius]); }

 inline double CylinderVolumeBounds::halfPhiSector() const { return m_boundValues[bv_halfPhiSector]; }

 inline double CylinderVolumeBounds::halflengthZ() const { return m_boundValues[bv_halfZ]; }

 template <class T> T& CylinderVolumeBounds::dumpT(T& tstream) const
 {
     tstream << std::setiosflags(std::ios::fixed);
     tstream << std::setprecision(2);
     tstream << "Acts::CylinderVolumeBounds: (rMin, rMax, halfPhi, halfZ) = ";
     tstream <<  m_boundValues[bv_innerRadius] << ", " << m_boundValues[bv_outerRadius] << ", " << m_boundValues[bv_halfPhiSector] << ", " << m_boundValues[bv_halfZ];
     return tstream;
 }

}

#endif // ACTS_VOLUMES_CYLINDERVOLUMESBOUNDS_H