
#import "UIImage+swizzle.h"
#import "SwizzleClassMethodHeader.h"

@implementation UIImage (swizzle)

+ (void)load {
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        swizzleIntanceMethod(self, @selector(imageNamed:), @selector(xlsn0w_imageNamed:));
    });
}

+ (nullable UIImage *)xlsn0w_imageNamed:(NSString *)name {
    UIImage *image;
    
    return image;
}


@end
