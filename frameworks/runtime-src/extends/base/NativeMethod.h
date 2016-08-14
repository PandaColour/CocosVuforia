#ifndef NativeMethod_h
#define NativeMethod_h

#import <Foundation/Foundation.h>

@interface NativeMethodDeal : NSObject {
    NSDictionary* params;
}
-(NSString *)callRealMethod;
-(id)initWithDict:(NSDictionary *)dict;
@end

@interface NativeMethodInterface : NSObject
+ (NSString *) callNativeMethod:(NSString *)param;
+ (NativeMethodDeal *)createNativeMetodDeal:(NSDictionary *)dict;
@end

@interface turnScreanToHorizontal : NativeMethodDeal
-(NSString *)callRealMethod;
@end

@interface turnScreanToVertical : NativeMethodDeal
-(NSString *)callRealMethod;
@end

@interface getStatusBarOrientation : NativeMethodDeal
-(NSString *)callRealMethod;
@end

@interface showAR : NativeMethodDeal
-(NSString *)callRealMethod;
@end

@interface stopAR : NativeMethodDeal
-(NSString *)callRealMethod;
@end

@interface decomposeMatrix : NativeMethodDeal
-(NSString *)callRealMethod;
@end

#endif