#include "PHG4ParticleGun.h"
#include "PHG4Particlev1.h"

#include "PHG4InEvent.h"

#include <phool/getClass.h>

#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHRandomSeed.h>

#include <Geant4/G4ParticleTable.hh>
#include <Geant4/G4ParticleDefinition.hh>

#include <TRandom.h>

#include <iostream>
//Abi
#include <TGeoMaterial.h>
#include <phgeom/PHGeomUtility.h>
#include <TGeoManager.h>

using namespace std;

PHG4ParticleGun::PHG4ParticleGun(const string &name): 
  PHG4ParticleGeneratorBase(name),
  _beam_profile(nullptr)
 // _legacy_vertexgenerator(nullptr)
{ 
//  _vertexGen = new SQPrimaryVertexGen();
  
  return;
}

PHG4ParticleGun::~PHG4ParticleGun()
{
	delete _beam_profile;
	//delete _vertexGen;
  return;
}

int PHG4ParticleGun::InitRun(PHCompositeNode* topNode){
	PHG4ParticleGeneratorBase::InitRun(topNode);
	gRandom->SetSeed(PHRandomSeed());
	return 0;
}

int
PHG4ParticleGun::process_event(PHCompositeNode *topNode)
{
	double vx = vtx_x, vy = vtx_y;
	if(_beam_profile) {
		_beam_profile->GetRandom2(vx, vy);
	}
	
	


  PHG4InEvent *ineve = findNode::getClass<PHG4InEvent>(topNode,"PHG4INEVENT");
  ReuseExistingVertex(topNode); // checks if we should reuse existing vertex


  // cout<<" Here I am in particle gun...."<<endl;
  //cout << "PHG4ParticleGun: {" << vx << ", " << vy << ", "<<vtx_z<< "}" << endl;
  int vtxindex = ineve->AddVtx(vx,vy,vtx_z,t0);
  vector<PHG4Particle *>::const_iterator iter;
  for (iter = particlelist.begin(); iter != particlelist.end(); ++iter)
    {
      PHG4Particle *particle = new PHG4Particlev1(*iter);
      SetParticleId(particle,ineve);
      ineve->AddParticle(vtxindex, particle);
    }
  if (verbosity > 0)
    {
      ineve->identify();
    }
  return 0;
}
