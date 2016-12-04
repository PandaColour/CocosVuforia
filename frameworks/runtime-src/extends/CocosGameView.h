//
//  CocosGameView.h
//  template
//
//  Created by jiaxu zhang on 16/7/25.
//
//

#import "CCEAGLView-ios.h"
#import <Vuforia/UIGLViewProtocol.h>
#import "Vuforia/VuforiaApplicationSession.h"
#include "cocos2d.h"

class ARDrawer : public cocos2d::Drawer
{
public:
    virtual void draw();
    virtual void customProject();
    virtual cocos2d::Mat4 getCustomProjectMat4();
    virtual cocos2d::Vec3 getCustomPoint(POINT_TYPE type);
    cocos2d::Mat4 getCustomCameraMat4();
private:
    float _fieldOfView;
    float _aspectRatio;
};

@interface CocosGameView : CCEAGLView <UIGLViewProtocol> {
@private
    ARDrawer drawer;
}
- (void) showAR;
- (void) stopAR;
@end


