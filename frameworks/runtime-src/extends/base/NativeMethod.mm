//
//  Utils.m
//  MyJsGame
//
//  Created by jiaxu zhang on 16/6/28.
//
//


#import "NativeMethod.h"
#import "AppController.h"
#import "cocos2d.h"
#import "platform/ios/CCEAGLView-ios.h"
#import "RootViewController.h"
#import "VuforiaController.h"
#include "ScriptingCore.h"

@implementation NativeMethodInterface

+ (NSString *) callNativeMethod:(NSString *)param {
    NSData *json = [param dataUsingEncoding:NSASCIIStringEncoding];
    NSDictionary *dict = [NSJSONSerialization JSONObjectWithData:json options:NSJSONReadingAllowFragments error:nil];
    NativeMethodDeal *deal = [NativeMethodInterface createNativeMetodDeal:dict];
    return [deal callRealMethod];
}

+ (NativeMethodDeal *)createNativeMetodDeal:(NSDictionary *)dict {
    NSString * methodName = dict[@"methodName"];
    Class className = NSClassFromString(methodName);
    NativeMethodDeal *deal = nil;
    if (className == nil)  deal = [[[NativeMethodDeal alloc] initWithDict:dict] autorelease];
    else deal =  (NativeMethodDeal *)[[[className alloc] initWithDict:dict] autorelease];
    return  deal;
}

@end

@implementation NativeMethodDeal
-(id)initWithDict:(NSDictionary *)dict {
    if (self == [super init]) {
        params = dict;
    }
    return self;
}

-(NSString *)callRealMethod {
    NSString * methodName = params[@"methodName"];
    return [NSString stringWithFormat:@"{\"result\": \"error\", \"error\" : \"native method: %@ hasn't define.\"}", methodName];
}
@end

@implementation turnScreanToHorizontal
-(NSString *)callRealMethod {
    [UIApplication sharedApplication].statusBarOrientation = UIInterfaceOrientationLandscapeRight;
    [UIApplication sharedApplication].keyWindow.transform = CGAffineTransformMakeRotation(0);
    CGRect cgRect = [UIApplication sharedApplication].keyWindow.bounds;
    if (cgRect.size.height > cgRect.size.width) {
        [UIApplication sharedApplication].keyWindow.bounds = CGRectMake(cgRect.origin.x, cgRect.origin.y, cgRect.size.height, cgRect.size.width);
        cocos2d::Size size = cocos2d::Director::getInstance()->getOpenGLView()->getFrameSize();
        cocos2d::Director::getInstance()->getOpenGLView()->setFrameSize(size.height, size.width);
    }
    return [NSString stringWithFormat:@"{\"result\": \"success\"}"];
}
@end

@implementation turnScreanToVertical : NativeMethodDeal
-(NSString *)callRealMethod {
    [UIApplication sharedApplication].statusBarOrientation = UIInterfaceOrientationPortrait;
    [UIApplication sharedApplication].keyWindow.transform=CGAffineTransformMakeRotation(M_PI_2);
    CGRect cgRect = [UIApplication sharedApplication].keyWindow.bounds;
    if (cgRect.size.width > cgRect.size.height) {
        [UIApplication sharedApplication].keyWindow.bounds = CGRectMake(cgRect.origin.x, cgRect.origin.y, cgRect.size.height, cgRect.size.width);
        cocos2d::Size size = cocos2d::Director::getInstance()->getOpenGLView()->getFrameSize();
        cocos2d::Director::getInstance()->getOpenGLView()->setFrameSize(size.height, size.width);
    }
    return [NSString stringWithFormat:@"{\"result\": \"success\"}"];
}
@end

@implementation getStatusBarOrientation : NativeMethodDeal
-(NSString *)callRealMethod {
    NSString * Orientation = @"Unkown";
    UIInterfaceOrientation UIOrientation = [[UIApplication sharedApplication] statusBarOrientation];
    if (UIOrientation == UIInterfaceOrientationLandscapeRight)
    {
        Orientation = @"LandscapeRight";
    }
    else if (UIOrientation == UIInterfaceOrientationLandscapeLeft)
    {
        Orientation = @"LandscapeLeft";
    }
    else if (UIOrientation == UIInterfaceOrientationPortraitUpsideDown)
    {
        Orientation = @"PortraitUpsideDown";
    }
    else
    {
        Orientation = @"Portrait";
    }
    
    return [NSString stringWithFormat:@"{\"result\": \"%@\"}", Orientation];
}
@end

@implementation showAR : NativeMethodDeal
-(NSString *)callRealMethod {
    [[VuforiaController getVuforiaController] showAR];
    return [NSString stringWithFormat:@"{\"result\": \"success\"}"];
}
@end

@implementation stopAR : NativeMethodDeal
-(NSString *)callRealMethod {
    [[VuforiaController getVuforiaController] stopAR];
    return [NSString stringWithFormat:@"{\"result\": \"success\"}"];
}
@end

@implementation decomposeMatrix : NativeMethodDeal
-(NSString *)callRealMethod{
    NSArray * modelViewMatrix = params[@"modelViewMatrix"];
    float matix[16] = {0.0};
    for (int i = 0; i < modelViewMatrix.count; ++i)
    {
        NSNumber *data = modelViewMatrix[i];
        matix[i] = [data floatValue];
    }
    
    cocos2d::Mat4 mat4(&matix[0]);
    cocos2d::Quaternion quat;
    cocos2d::Vec3 scale;
    cocos2d::Vec3 translation;
    mat4.decompose(&scale, &quat, &translation);
    
    NSMutableArray* quaternion = [NSMutableArray arrayWithCapacity:4];
    [quaternion addObject:[NSNumber numberWithFloat:quat.x]];
    [quaternion addObject:[NSNumber numberWithFloat:quat.y]];
    [quaternion addObject:[NSNumber numberWithFloat:quat.z]];
    [quaternion addObject:[NSNumber numberWithFloat:quat.w]];
    
    NSMutableArray* scaleArrary = [NSMutableArray arrayWithCapacity:3];
    [scaleArrary addObject:[NSNumber numberWithFloat:scale.x]];
    [scaleArrary addObject:[NSNumber numberWithFloat:scale.y]];
    [scaleArrary addObject:[NSNumber numberWithFloat:scale.z]];
    
    NSMutableArray* translationArrary = [NSMutableArray arrayWithCapacity:3];
    [translationArrary addObject:[NSNumber numberWithFloat:translation.x]];
    [translationArrary addObject:[NSNumber numberWithFloat:translation.y]];
    [translationArrary addObject:[NSNumber numberWithFloat:translation.z]];
    
    NSMutableDictionary* result = [NSMutableDictionary dictionaryWithObjectsAndKeys:
                                       @"success", @"result",
                                       quaternion, @"quaternion",
                                       scaleArrary, @"scale",
                                       translationArrary, @"translation", nil];
    NSError *error;
    NSData *jsonData = [NSJSONSerialization dataWithJSONObject:result options:NSJSONWritingPrettyPrinted error:&error];
    NSString *jsonString = [[[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding] autorelease];
    return jsonString;
}
@end
