//
//  VuforiaController.m
//  template
//
//  Created by jiaxu zhang on 16/7/25.
//
//
#ifndef extends_Vuforia_VuforiaController_h
#define extends_Vuforia_VuforiaController_h
#import <Foundation/Foundation.h>
#import "VuforiaApplicationSession.h"
#import <Vuforia/DataSet.h>
#import "CocosGameView.h"

@interface VuforiaController : NSObject <VuforiaApplicationControl> {
    Vuforia::DataSet*  dataSetCurrent;
}
@property (nonatomic, strong) VuforiaApplicationSession * vapp;
@property (nonatomic, strong) CocosGameView* eaglView;
+ (VuforiaController*) getVuforiaController;
- (id)initWithEaglView: (CocosGameView*) glview;
- (void) showAR;
- (void) stopAR;
@end


#endif