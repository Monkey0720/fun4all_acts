// This file is part of the Acts project.
//
// Copyright (C) 2018 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

#include "Acts/Geometry/GeometryContext.hpp"
#include "Acts/Geometry/ProtoLayer.hpp"
#include "Acts/Geometry/ProtoLayerHelper.hpp"
#include "Acts/Surfaces/Surface.hpp"
#include "Acts/Tests/CommonHelpers/CylindricalTrackingGeometry.hpp"
#include "Acts/Tests/CommonHelpers/FloatComparisons.hpp"
#include "Acts/Visualization/GeometryVisualization.hpp"
#include "Acts/Visualization/IVisualization.hpp"
#include "Acts/Visualization/ObjVisualization.hpp"

#include <cmath>

namespace Acts {

namespace Test {
namespace Layers {
BOOST_AUTO_TEST_SUITE(Geometry)

BOOST_AUTO_TEST_CASE(ProtoLayerHelperTests) {
  ProtoLayerHelper::Config plhConfig;
  ProtoLayerHelper plHelper(plhConfig);

  GeometryContext tgContext = GeometryContext();

  ObjVisualization objVis;

  CylindricalTrackingGeometry ctGeometry(tgContext);
  CylindricalTrackingGeometry::DetectorStore dStore;

  /// Helper method to prepare the streams & helpers
  /// @param path is the file path
  /// @param clear ist he indicator to clear the helper
  auto write = [&](IVisualization& helper, const std::string& path,
                   bool clear = true) -> void {
    helper.write(path);
    if (clear) {
      helper.clear();
    }
  };

  /// Cylindrical section ---------------------------------------------------

  std::vector<double> layerRadii = {32., 72., 116., 172.};
  std::vector<std::pair<int, int>> layerBinning = {
      {16, 14}, {32, 14}, {52, 14}, {78, 14}};
  std::vector<double> moduleTiltPhi = {0.145, 0.145, 0.145, 0.145};
  std::vector<double> moduleHalfX = {8.4, 8.4, 8.4, 8.4};
  std::vector<double> moduleHalfY = {36., 36., 36., 36.};
  std::vector<double> moduleThickness = {0.15, 0.15, 0.15, 0.15};

  std::vector<const Surface*> cylinderSurfaces;
  for (size_t ilp = 0; ilp < layerRadii.size(); ++ilp) {
    std::vector<const Surface*> layerSurfaces = ctGeometry.surfacesCylinder(
        dStore, moduleHalfX[ilp], moduleHalfY[ilp], moduleThickness[ilp],
        moduleTiltPhi[ilp], layerRadii[ilp], 2., 5., layerBinning[ilp]);
    cylinderSurfaces.insert(cylinderSurfaces.begin(), layerSurfaces.begin(),
                            layerSurfaces.end());
  }

  IVisualization::ColorType unsortedColor = {252, 160, 0};
  for (auto& sf : cylinderSurfaces) {
    Visualization::drawSurface(objVis, *sf, tgContext, Transform3D::Identity(),
                               1, false, unsortedColor);
  }
  // Draw the all surfaces
  write(objVis, "ProtoLayerHelper_CylinderLayers_unsorted", true);

  // Sort into ProtoLayers
  auto radialLayers =
      plHelper.protoLayers(tgContext, cylinderSurfaces, binR, 5.);

  BOOST_CHECK(radialLayers.size() == 4);

  std::vector<IVisualization::ColorType> sortedColors = {{102, 204, 255},
                                                         {102, 255, 153},
                                                         {255, 204, 102},
                                                         {204, 102, 0},
                                                         {278, 123, 55}};

  size_t il = 0;
  for (auto& layer : radialLayers) {
    for (auto& sf : layer.surfaces()) {
      auto color = sortedColors[il];
      Visualization::drawSurface(objVis, *sf, tgContext,
                                 Transform3D::Identity(), 1, false, color);
    }
    ++il;
  }

  // Draw the sorted surfaces
  write(objVis, "ProtoLayerHelper_CylinderLayers_radially", true);

  /// Disc section ---------------------------------------------------
  std::vector<const Surface*> discSurfaces;

  std::vector<double> discZ = {-350., -250., -150., -100.};
  std::vector<double> discRadii = {55., 55., 55., 55.};
  std::vector<int> discModules = {22, 22, 22, 22};

  std::vector<double> dModuleHalfXMinY = {6.4, 6.4, 6.4, 6.4};
  std::vector<double> dModuleHalfXMaxY = {12.4, 12.4, 12.4, 12.4};
  std::vector<double> dModuleHalfY = {36., 36., 36., 36.};
  std::vector<double> dModuleTilt = {0.075, 0.075, 0.075, 0.075};
  std::vector<double> dModuleThickness = {0.15, 0.15, 0.15, 0.15};

  for (size_t ilp = 0; ilp < discZ.size(); ++ilp) {
    std::vector<const Surface*> layerSurfaces = ctGeometry.surfacesRing(
        dStore, dModuleHalfXMinY[ilp], dModuleHalfXMaxY[ilp], dModuleHalfY[ilp],
        dModuleThickness[ilp], dModuleTilt[ilp], discRadii[ilp], discZ[ilp], 2.,
        discModules[ilp]);
    discSurfaces.insert(discSurfaces.begin(), layerSurfaces.begin(),
                        layerSurfaces.end());
  }

  for (auto& sf : discSurfaces) {
    Visualization::drawSurface(objVis, *sf, tgContext, Transform3D::Identity(),
                               1, false, unsortedColor);
  }
  // Draw the all surfaces
  write(objVis, "ProtoLayerHelper_DiscLayers_unsorted", true);

  // Sort into ProtoLayers
  auto discLayersZ = plHelper.protoLayers(tgContext, discSurfaces, binZ, 5.);

  BOOST_CHECK(discLayersZ.size() == 4);

  il = 0;
  for (auto& layer : discLayersZ) {
    for (auto& sf : layer.surfaces()) {
      Visualization::drawSurface(objVis, *sf, tgContext,
                                 Transform3D::Identity(), 1, false,
                                 sortedColors[il]);
    }
    ++il;
  }

  // Draw the sorted surfaces
  write(objVis, "ProtoLayerHelper_DiscLayers_longitudinally", true);

  /// Ring layout section ---------------------------------------------------
  std::vector<const Surface*> ringSurfaces;

  std::vector<double> ringZ = {-350., -250., -150., -100., -360., -255.,
                               -120., -330., -260., -150., -95.};
  std::vector<double> ringRadii = {32., 32., 32., 32., 58., 58.,
                                   58., 84., 84., 84., 84.};
  std::vector<int> ringModules = {22, 22, 22, 22, 32, 32, 32, 44, 44, 44, 44};

  std::vector<double> rModuleHalfXMinY(11, 6.4);
  std::vector<double> rModuleHalfXMaxY(11, 6.4);
  std::vector<double> rModuleHalfY(11, 10.);
  std::vector<double> rModuleTilt(11, 0.075);
  std::vector<double> rModuleThickness(11, 0.15);

  for (size_t ilp = 0; ilp < ringZ.size(); ++ilp) {
    std::vector<const Surface*> layerSurfaces = ctGeometry.surfacesRing(
        dStore, rModuleHalfXMinY[ilp], rModuleHalfXMaxY[ilp], rModuleHalfY[ilp],
        rModuleThickness[ilp], rModuleTilt[ilp], ringRadii[ilp], ringZ[ilp], 2.,
        ringModules[ilp]);
    ringSurfaces.insert(ringSurfaces.begin(), layerSurfaces.begin(),
                        layerSurfaces.end());
  }

  for (auto& sf : ringSurfaces) {
    Visualization::drawSurface(objVis, *sf, tgContext, Transform3D::Identity(),
                               1, false, unsortedColor);
  }
  // Draw the all surfaces
  write(objVis, "ProtoLayerHelper_RingLayers_unsorted", true);

  // First: Sort into ProtoLayers radially
  auto rSorted = plHelper.protoLayers(tgContext, ringSurfaces, binR, 1.);
  BOOST_CHECK(rSorted.size() == 3);

  IVisualization::ColorType dColor = {0, 0, 0};

  int ir = 0;
  for (auto& rBatch : rSorted) {
    auto lSorted = plHelper.protoLayers(tgContext, rBatch.surfaces(), binZ, 5.);
    il = 0;
    dColor[ir] = 256;
    for (auto& layer : lSorted) {
      dColor[ir] -= il * 50;
      for (auto& sf : layer.surfaces()) {
        Visualization::drawSurface(objVis, *sf, tgContext,
                                   Transform3D::Identity(), 1, false, dColor);
      }
      ++il;
    }
    ++ir;
  }
  // Draw the all surfaces
  write(objVis, "ProtoLayerHelper_RingLayers_sorted", true);
}

BOOST_AUTO_TEST_SUITE_END()
}  // namespace Layers
}  // namespace Test

}  // namespace Acts
