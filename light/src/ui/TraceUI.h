//
// rayUI.h
//
// The header file for the UI part
//

#ifndef __rayUI_h__
#define __rayUI_h__

// who the hell cares if my identifiers are longer than 255 characters:
#pragma warning(disable : 4786)

#include <math.h>
#include "../vecmath/vec.h"
#include "../vecmath/mat.h"

#include <string>

using std::string;

class RayTracer;

class TraceUI {
public:
	TraceUI()
		: m_nDepth(0), m_nSize(150), m_nAa(1), 
		m_displayDebuggingInfo( false ),
		m_accelerated( true ),
    m_stoc( false ),
    m_cm( false ),
    m_uv( false ),
    m_st( false ),
    m_debug( true ),
    m_factor( 1.0 ),
    objectLimit(5),
    depthLimit(15),
    raytracer(0),
    cutoff(0.0),
    m_cameraU(0.0),
    m_cameraV(0.0),
    m_lf_n(0),
    m_done(false)
	{ }
  bool    m_done;

	virtual int		run() = 0;

	// Send an alert to the user in some manner
	virtual void		alert(const string& msg) = 0;

	// setters
	virtual void		setRayTracer( RayTracer* r )
		{ raytracer = r; }
  virtual void setCutoff(double c) {
    cutoff = c;
  }
  virtual void setAccelerated(bool b) {
    m_accelerated = b;
  }
  virtual void setStoc(bool b) {
    m_stoc = b;
  }
  virtual void setCubeMap(bool b) {
    m_cm = b;
  }
  virtual void setUVInterp(bool b) {
    m_uv = b;
  }
  virtual void setSTInterp(bool b) {
    m_st = b;
  }
  virtual void setFactor(double d) {
    m_factor = d;
  }
  virtual void setObjectLimit(int l) {
    objectLimit = l;
  }
  virtual void setDepthLimit(int l) {
    depthLimit = l;
  }
  virtual void setXPName(string s) {
    xpName = s;
  }
  virtual void setXNName(string s) {
    xnName = s;
  }
  virtual void setYPName(string s) {
    ypName = s;
  }
  virtual void setYNName(string s) {
    ynName = s;
  }
  virtual void setZPName(string s) {
    zpName = s;
  }
  virtual void setZNName(string s) {
    znName = s;
  }
	// accessors:
	double getCameraU() const { return m_cameraU; }
	double getCameraV() const { return m_cameraV; }
  int		getLFN() const { return m_lf_n; }
  int		getSize() const { return m_nSize; }
	int		getDepth() const { return m_nDepth; }
	int		getAa() const { return m_nAa; }
  bool  getAccelerated() const { return m_accelerated; }
  bool  getStoc() const { return m_stoc; }
  bool  getCubeMap() const { return m_cm; }
  bool  getUVInterp() const { return m_uv; }
  bool  getSTInterp() const { return m_st; }
  bool  getDebug() const { return m_debug; }
  double getFactor() const { return m_factor; }
  int getObjectLimit() const { return objectLimit; }
  int getDepthLimit() const { return depthLimit; }
  string getXPName() const { return xpName; }
  string getXNName() const { return xnName; }
  string getYPName() const { return ypName; }
  string getYNName() const { return ynName; }
  string getZPName() const { return zpName; }
  string getZNName() const { return znName; }
  double getCutoff() const { return cutoff; }
protected:
	RayTracer*	raytracer;

	int			m_nSize;				// Size of the traced image
	int			m_nDepth;				// Max depth of recursion
  int     m_nAa;          // AA amount
  bool    m_accelerated;
  bool    m_stoc;
  bool    m_cm;
  bool    m_uv;
  bool    m_st;
  bool    m_debug;
  double m_factor;
  double m_cameraU;
  double m_cameraV;
  int m_lf_n;
  int objectLimit;
  int depthLimit;
  string xpName;
  string xnName;
  string ypName;
  string ynName;
  string zpName;
  string znName;
  double cutoff;

	// Determines whether or not to show debugging information
	// for individual rays.  Disabled by default for efficiency
	// reasons.
	bool		m_displayDebuggingInfo;

};

#endif
