/*
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway,
University of London 2001 - 2024.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSPlasmaHDF5.hh"
#include "BDSUtilities.hh"

BDSPlasmaHDF5::BDSPlasmaHDF5(G4String mainPathIn,
                             G4String particlePathIn,
                             G4String fieldPathIn):
mainPath(mainPathIn),
particlePath(particlePathIn),
fieldPath(fieldPathIn)
{
  //fileName = "/Users/marindeniaud/PhD/PhD-git/bdsim-develop/pwfa/diags/hdf5/data00000000.h5";
  //G4String fileName = BDS::GetFullPath(fn);
}

BDSPlasmaHDF5::~BDSPlasmaHDF5()
{;}

void BDSPlasmaHDF5::LoadHDF5(const G4String filename)
{
  G4cout << "BDSPlasmaHDF5::LoadHDF5" << G4endl;

  CheckHDF5(filename);
  H5::H5File *file= new H5::H5File(filename, H5F_ACC_RDWR);

  particles = file->openGroup(mainPath + particlePath);
  fields = file->openGroup(mainPath + fieldPath);
}

void BDSPlasmaHDF5::CheckHDF5(const G4String filename)
{
  G4cout << "BDSPlasmaHDF5::CheckHDF5" << G4endl;
  try
  {
    H5::H5File *file= new H5::H5File(filename, H5F_ACC_RDWR);
  }
  catch (H5::FileIException& e)
  {
    std::string msg = "Error on checking file";
    throw BDSException(__METHOD_NAME__, msg);
  }
}

std::vector<double> BDSPlasmaHDF5::Get1DVector(H5::Group group,
                                               G4String setName)
{
  H5::DataSet set = group.openDataSet(setName);

  H5::DataSpace dataspace = set.getSpace();
  int rank = dataspace.getSimpleExtentNdims();
  hsize_t dims[rank];
  dataspace.getSimpleExtentDims(dims, nullptr);
  std::vector<double> data(dims[0]);

  set.read(data.data(), H5::PredType::NATIVE_DOUBLE);

  return data;
}

std::vector<double> BDSPlasmaHDF5::Get3DVector(H5::Group group,
                                               G4String setName)
{
  H5::DataSet set = group.openDataSet(setName);

  H5::DataSpace dataspace = set.getSpace();
  int rank = dataspace.getSimpleExtentNdims();
  hsize_t dims[rank];
  dataspace.getSimpleExtentDims(dims, nullptr);
  std::vector<double> data(dims[0] * dims[1] * dims[2]);

  set.read(data.data(), H5::PredType::NATIVE_DOUBLE);

  return data;
}

std::vector<double> BDSPlasmaHDF5::GetX()
{
  H5::Group position = particles.openGroup("position");
  return Get1DVector(position, "x");
}

std::vector<double> BDSPlasmaHDF5::GetY()
{
  H5::Group position = particles.openGroup("position");
  return Get1DVector(position, "y");
}

std::vector<double> BDSPlasmaHDF5::GetZ()
{
  H5::Group position = particles.openGroup("position");
  return Get1DVector(position, "z");
}

std::vector<double> BDSPlasmaHDF5::GetPX()
{
  H5::Group momentum = particles.openGroup("momentum");
  return Get1DVector(momentum, "x");
}

std::vector<double> BDSPlasmaHDF5::GetPY()
{
  H5::Group momentum = particles.openGroup("momentum");
  return Get1DVector(momentum, "y");
}

std::vector<double> BDSPlasmaHDF5::GetPZ()
{
  H5::Group momentum = particles.openGroup("momentum");
  return Get1DVector(momentum, "z");
}

std::vector<double> BDSPlasmaHDF5::GetEr()
{
  H5::Group E = fields.openGroup("E");
  return Get3DVector(E, "r");
}

std::vector<double> BDSPlasmaHDF5::GetEz()
{
  H5::Group E = fields.openGroup("B");
  return Get3DVector(E, "z");
}

std::vector<double> BDSPlasmaHDF5::GetBt()
{
  H5::Group E = fields.openGroup("B");
  return Get3DVector(E, "t");
}