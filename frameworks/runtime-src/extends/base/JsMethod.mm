//
//  JsMethod.m
//  template
//
//  Created by jiaxu zhang on 16/7/27.
//
//

#import "JsMethod.h"
#include "ScriptingCore.h"

@implementation JsMethodInterface

+ (void) callJsMethod:(NSString*)param
{
    const char * jsJsonParam =[param UTF8String];
    std::string jsMethod = std::string("globle.callJsMedthod('") + std::string(jsJsonParam) + "')";
    ScriptingCore::getInstance()->evalString(jsMethod.c_str(), nullptr);
}
@end


