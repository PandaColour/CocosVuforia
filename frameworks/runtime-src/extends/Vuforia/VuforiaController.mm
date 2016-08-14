//
//  VuforiaController.m
//  template
//
//  Created by jiaxu zhang on 16/7/25.
//
//

#import <Foundation/Foundation.h>
#import "VuforiaController.h"
#import <Vuforia/Vuforia.h>
#import <Vuforia/TrackerManager.h>
#import <Vuforia/ObjectTracker.h>
#import <Vuforia/Trackable.h>
#import <Vuforia/DataSet.h>
#import <Vuforia/CameraDevice.h>
#include "cocos2d.h"

namespace {
    VuforiaController* instance = nil;
}

@interface VuforiaController ()

@end

@implementation VuforiaController

@synthesize vapp, eaglView;

+ (VuforiaController*) getVuforiaController
{
    return instance;
}

- (void) showAR
{
    [vapp initAR:Vuforia::GL_20 orientation:UIInterfaceOrientation::UIInterfaceOrientationLandscapeRight];
}

- (void) stopAR
{
    [vapp stopAR:nil];
    [eaglView stopAR];
}

- (id)initWithEaglView:(CocosGameView*) glview
{
    self = [super init];
    if (self.vapp == nil)
    {
        VuforiaApplicationSession* app = [[VuforiaApplicationSession alloc]initWithDelegate:self];
        self.vapp = app;
    }
    self.eaglView = glview;
    instance = self;
    return self;
}

- (Vuforia::DataSet *)loadObjectTrackerDataSet:(NSString*)dataFile
{
    NSLog(@"loadObjectTrackerDataSet (%@)", dataFile);
    Vuforia::DataSet * dataSet = NULL;
    
    // Get the Vuforia tracker manager image tracker
    Vuforia::TrackerManager& trackerManager = Vuforia::TrackerManager::getInstance();
    Vuforia::ObjectTracker* objectTracker = static_cast<Vuforia::ObjectTracker*>(trackerManager.getTracker(Vuforia::ObjectTracker::getClassType()));
    
    if (NULL == objectTracker) {
        NSLog(@"ERROR: failed to get the ObjectTracker from the tracker manager");
        return NULL;
    } else {
        dataSet = objectTracker->createDataSet();
        
        if (NULL != dataSet) {
            NSLog(@"INFO: successfully loaded data set");
            if (!dataSet->load([dataFile cStringUsingEncoding:NSASCIIStringEncoding], Vuforia::STORAGE_APPRESOURCE)) {
                NSLog(@"ERROR: failed to load data set");
                objectTracker->destroyDataSet(dataSet);
                dataSet = NULL;
            }
        }
        else {
            NSLog(@"ERROR: failed to create data set");
        }
    }
    
    if (objectTracker->activateDataSet(dataSet)) {
        NSLog(@"INFO: successfully activated data set");
    }
    
    return dataSet;
}

#pragma mark - VuforiaApplicationControl

// Initialize the application trackers
- (bool) doInitTrackers {
    // Initialize the object tracker
    Vuforia::TrackerManager& trackerManager = Vuforia::TrackerManager::getInstance();
    Vuforia::Tracker* trackerBase = trackerManager.initTracker(Vuforia::ObjectTracker::getClassType());
    if (trackerBase == NULL)
    {
        NSLog(@"Failed to initialize ObjectTracker.");
        return false;
    }
    return true;
}

// load the data associated to the trackers
- (bool) doLoadTrackersData {
    dataSetCurrent = [self loadObjectTrackerDataSet:@"res/StonesAndChips.xml"];
    return dataSetCurrent != nil;
}

// start the application trackers
- (bool) doStartTrackers {
    Vuforia::TrackerManager& trackerManager = Vuforia::TrackerManager::getInstance();
    Vuforia::Tracker* tracker = trackerManager.getTracker(Vuforia::ObjectTracker::getClassType());
    if(tracker == 0) {
        return false;
    }
    [eaglView showAR];
    tracker->start();
    return true;
}

// callback called when the initailization of the AR is done
- (void) onInitARDone:(NSError *)initError {
    UIActivityIndicatorView *loadingIndicator = (UIActivityIndicatorView *)[eaglView viewWithTag:1];
    [loadingIndicator removeFromSuperview];
    
    if (initError == nil) {
        NSError * error = nil;
        [vapp startAR:Vuforia::CameraDevice::CAMERA_DIRECTION_BACK error:&error];
        
        
    } else {
        NSLog(@"Error initializing AR:%@", [initError description]);
        dispatch_async( dispatch_get_main_queue(), ^{
            
            UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Error"
                                                            message:[initError localizedDescription]
                                                           delegate:self
                                                  cancelButtonTitle:@"OK"
                                                  otherButtonTitles:nil];
            [alert show];
        });
    }
}


- (bool) doStopTrackers {
    // Stop the tracker
    Vuforia::TrackerManager& trackerManager = Vuforia::TrackerManager::getInstance();
    Vuforia::Tracker* tracker = trackerManager.getTracker(Vuforia::ObjectTracker::getClassType());
    
    if (NULL != tracker) {
        tracker->stop();
        NSLog(@"INFO: successfully stopped tracker");
        return YES;
    }
    else {
        NSLog(@"ERROR: failed to get the tracker from the tracker manager");
        return NO;
    }
}

- (bool) doUnloadTrackersData {
    if (dataSetCurrent != NULL) {
        // Get the object tracker:
        Vuforia::TrackerManager& trackerManager = Vuforia::TrackerManager::getInstance();
        Vuforia::ObjectTracker* objectTracker = static_cast<Vuforia::ObjectTracker*>(trackerManager.getTracker(Vuforia::ObjectTracker::getClassType()));
        
        if (objectTracker == NULL)
        {
            NSLog(@"Failed to unload tracking data set because the ObjectTracker has not been initialized.");
            return false;
        }
        // Activate the data set:
        if (!objectTracker->deactivateDataSet(dataSetCurrent))
        {
            NSLog(@"Failed to deactivate data set.");
            return false;
        }
        // Activate the data set:
        if (!objectTracker->destroyDataSet(dataSetCurrent))
        {
            NSLog(@"Failed to destroy data set.");
            return false;
        }
        dataSetCurrent = NULL;
    }
    return true;
}

- (bool) doDeinitTrackers {
    Vuforia::TrackerManager& trackerManager = Vuforia::TrackerManager::getInstance();
    trackerManager.deinitTracker(Vuforia::ObjectTracker::getClassType());
    return YES;
}

- (void) onVuforiaUpdate: (Vuforia::State *) state {
}


@end