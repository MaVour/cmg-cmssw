//
// Package:         RecoTracker/RoadSearchTrackCandidateMaker
// Class:           RoadSearchTrackCandidateMakerAlgorithm
// 
// Description:     Converts cleaned clouds into
//                  TrackCandidates using the 
//                  TrajectoryBuilder framework
//
// Original Author: Oliver Gutsche, gutsche@fnal.gov
// Created:         Wed Mar 15 13:00:00 UTC 2006
//
// $Author: gutsche $
// $Date: 2006/03/29 20:10:37 $
// $Revision: 1.2 $
//

#include <vector>
#include <iostream>
#include <cmath>

#include "RecoTracker/RoadSearchTrackCandidateMaker/interface/RoadSearchTrackCandidateMakerAlgorithm.h"

#include "DataFormats/RoadSearchCloud/interface/RoadSearchCloud.h"
#include "DataFormats/TrackCandidate/interface/TrackCandidate.h"
#include "DataFormats/Common/interface/OwnVector.h"

#include "RecoTracker/TrackProducer/interface/TrackingRecHitLess.h"

#include "Geometry/CommonDetUnit/interface/TrackingGeometry.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"

#include "FWCore/Framework/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

RoadSearchTrackCandidateMakerAlgorithm::RoadSearchTrackCandidateMakerAlgorithm(const edm::ParameterSet& conf) : conf_(conf) { 
}

RoadSearchTrackCandidateMakerAlgorithm::~RoadSearchTrackCandidateMakerAlgorithm() {
}

void RoadSearchTrackCandidateMakerAlgorithm::run(const RoadSearchCloudCollection* input,
			      const edm::EventSetup& es,
			      TrackCandidateCollection &output)
{

//
// right now, track candidates are just filled from cleaned
// clouds. The trajectory of the seed is taken as the initial
// trajectory for the final fit
//

  edm::LogInfo("RoadSearch") << "Clean Clouds input size: " << input->size();

  for ( RoadSearchCloudCollection::const_iterator cloud = input->begin(); cloud != input->end(); ++cloud ) {

    // fill rechits from cloud into new OwnVector
    edm::OwnVector<TrackingRecHit> recHits;

    RoadSearchCloud::RecHitOwnVector hits = cloud->recHits();
    for ( RoadSearchCloud::RecHitOwnVector::const_iterator rechit = hits.begin(); rechit != hits.end(); ++rechit) {      
      recHits.push_back(rechit->clone());
    }

    // take the first seed to fill in trackcandidate
    RoadSearchCloud::SeedRefs seeds = cloud->seeds();
    RoadSearchCloud::SeedRef ref = *(seeds.begin());

    // sort recHits, done after getting seed because propagationDirection is needed
    // get tracker geometry
    edm::ESHandle<TrackerGeometry> tracker;
    es.get<TrackerDigiGeometryRecord>().get(tracker);

    recHits.sort(TrackingRecHitLess(((TrackingGeometry*)(&(*tracker))),ref->direction()));

    TrajectorySeed seed = *((*ref).clone());
    PTrajectoryStateOnDet state = *((*ref).startingState().clone());
  
    output.push_back(TrackCandidate(recHits,seed,state));
  }

  edm::LogInfo("RoadSearch") << "Found " << output.size() << " track candidates.";

};


