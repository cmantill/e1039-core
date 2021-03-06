#include "PHG4BlockDetector.h"

#include <phparameter/PHParameters.h>

#include <g4main/PHG4Utils.h>


#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/getClass.h>

#include <Geant4/G4Box.hh>
#include <Geant4/G4Colour.hh>
#include <Geant4/G4LogicalVolume.hh>
#include <Geant4/G4Material.hh>
#include <Geant4/G4PVPlacement.hh>
#include <Geant4/G4SystemOfUnits.hh>
#include <Geant4/G4UserLimits.hh>
#include <Geant4/G4VisAttributes.hh>

#include <sstream>

using namespace std;

//_______________________________________________________________
PHG4BlockDetector::PHG4BlockDetector( PHCompositeNode *Node, PHParameters *parameters, const std::string &dnam, const int lyr):
  PHG4Detector(Node, dnam),
  params(parameters),
  block_physi(nullptr),
  layer(lyr)
{}

//_______________________________________________________________
bool PHG4BlockDetector::IsInBlock(G4VPhysicalVolume * volume) const
{
  if (volume == block_physi)
  {
    return true;
  }
  return false;
}

//_______________________________________________________________
void PHG4BlockDetector::Construct( G4LogicalVolume* logicWorld )
{

  G4Material* TrackerMaterial = G4Material::GetMaterial(params->get_string_param("material"));


  if ( ! TrackerMaterial )
    {
      std::cout << "Error: Can not set material" << std::endl;
      exit(-1);
    }

  G4VSolid *block_solid = new G4Box(G4String(GetName().c_str()),
                          params->get_double_param("size_x")/2.*cm,
                          params->get_double_param("size_y")/2.*cm,
			  params->get_double_param("size_z")/2.*cm);

  double steplimits = params->get_double_param("steplimits") * cm;
  G4UserLimits *g4userlimits = nullptr;
  if (isfinite(steplimits))
  {
    g4userlimits = new G4UserLimits(steplimits);
  }

  G4LogicalVolume *block_logic = new G4LogicalVolume(block_solid,
                                    TrackerMaterial,
                                    G4String(GetName().c_str()),
                                    nullptr, nullptr, g4userlimits);
  G4VisAttributes* matVis = new G4VisAttributes();
  if (params->get_int_param("blackhole"))
    {
      PHG4Utils::SetColour(matVis, "BlackHole");
      matVis->SetVisibility(false);
      matVis->SetForceSolid(false);
    }
  else
    {
      PHG4Utils::SetColour(matVis, params->get_string_param("material"));
      matVis->SetVisibility(true);
      matVis->SetForceSolid(true);
    }
  block_logic->SetVisAttributes(matVis);

  G4RotationMatrix *rotm  = new G4RotationMatrix();
  rotm->rotateZ(params->get_double_param("rot_z")*deg);
  block_physi = new G4PVPlacement(rotm, G4ThreeVector(params->get_double_param("place_x")*cm,
                                                      params->get_double_param("place_y")*cm,
						      params->get_double_param("place_z")*cm),
                                  block_logic,
                                  G4String(GetName().c_str()),
                                  logicWorld, 0, false, overlapcheck);
}
