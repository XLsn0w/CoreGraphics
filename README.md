
## iOS绘图框架CoreGraphics


由于CoreGraphics框架有太多的API，对于初次接触或者对该框架不是十分了解的人，在绘图时，对API的选择会感到有些迷茫，甚至会觉得iOS的图形绘制有些繁琐。因此，本文主要介绍一下iOS的绘图方法和分析一下CoreGraphics框架的绘图原理。

一、绘图系统简介

iOS的绘图框架有多种，我们平常最常用的就是UIKit，其底层是依赖CoreGraphics实现的，而且绝大多数的图形界面也都是由UIKit完成，并且UIImage、NSString、UIBezierPath、UIColor等都知道如何绘制自己，也提供了一些方法来满足我们常用的绘图需求。除了UIKit，还有CoreGraphics、Core Animation，Core Image，OpenGL ES等多种框架，来满足不同的绘图要求。各个框架的大概介绍如下：

UIKit：最常用的视图框架，封装度最高，都是OC对象

CoreGraphics：主要绘图系统，常用于绘制自定义视图，纯C的API，使用Quartz2D做引擎

CoreAnimation：提供强大的2D和3D动画效果

CoreImage：给图片提供各种滤镜处理，比如高斯模糊、锐化等

OpenGL-ES：主要用于游戏绘制，但它是一套编程规范，具体由设备制造商实现




二、绘图方式

实际的绘图包括两部分：视图绘制和视图布局，它们实现的功能是不同的，在理解这两个概念之前，需要了解一下什么是绘图周期，因为都是在绘图周期中进行绘制的。

绘图周期：

iOS在运行循环中会整合所有的绘图请求，并一次将它们绘制出来

不能在子线程中绘制，也不能进行复杂的操作，否则会造成主线程卡顿

1.视图绘制

调用UIView的drawRect:方法进行绘制。如果调用一个视图的setNeedsDisplay方法，那么该视图就被标记为重新绘制，并且会在下一次绘制周期中重新绘制，自动调用drawRect:方法。

2.视图布局

调用UIView的layoutSubviews方法。如果调用一个视图的setNeedsLayout方法，那么该视图就被标记为需要重新布局，UIKit会自动调用layoutSubviews方法及其子视图的layoutSubviews方法。

在绘图时，我们应该尽量多使用布局，少使用绘制，是因为布局使用的是GPU，而绘制使用的是CPU。GPU对于图形处理有优势，而CPU要处理的事情较多，且不擅长处理图形，所以尽量使用GPU来处理图形。

三、绘图状态切换

iOS的绘图有多种对应的状态切换，比如：pop/push、save/restore、context/imageContext和CGPathRef/UIBezierPath等，下面分别进行介绍：

1.pop / push

设置绘图的上下文环境（context）

push：UIGraphicsPushContext(context)把context压入栈中，并把context设置为当前绘图上下文

pop：UIGraphicsPopContext将栈顶的上下文弹出，恢复先前的上下文，但是绘图状态不变

下面绘制的视图是黑色

- (void)drawRect:(CGRect)rect {
[[UIColor redColor] setFill];
UIGraphicsPushContext(UIGraphicsGetCurrentContext());
[[UIColor blackColor] setFill];
UIGraphicsPopContext();
UIRectFill(CGRectMake(90, 340, 100, 100)); // black color
}
2.save / restore

设置绘图的状态（state）

save：CGContextSaveGState 压栈当前的绘图状态，仅仅是绘图状态，不是绘图上下文

restore：恢复刚才保存的绘图状态

下面绘制的视图是红色


- (void)drawRect:(CGRect)rect {
[[UIColor redColor] setFill];
CGContextSaveGState(UIGraphicsGetCurrentContext());
[[UIColor blackColor] setFill];
CGContextRestoreGState(UIGraphicsGetCurrentContext());
UIRectFill(CGRectMake(90, 200, 100, 100)); // red color
}
3.context / imageContext

iOS的绘图必须在一个上下文中绘制，所以在绘图之前要获取一个上下文。如果是绘制图片，就需要获取一个图片的上下文；如果是绘制其它视图，就需要一个非图片上下文。对于上下文的理解，可以认为就是一张画布，然后在上面进行绘图操作。

context：图形上下文，可以通过UIGraphicsGetCurrentContext:获取当前视图的上下文

imageContext：图片上下文，可以通过UIGraphicsBeginImageContextWithOptions:获取一个图片上下文，然后绘制完成后，调用UIGraphicsGetImageFromCurrentImageContext获取绘制的图片，最后要记得关闭图片上下文UIGraphicsEndImageContext。

4.CGPathRef / UIBezierPath

图形的绘制需要绘制一个路径，然后再把路径渲染出来，而CGPathRef就是CoreGraphics框架中的路径绘制类，UIBezierPath是封装CGPathRef的面向OC的类，使用更加方便，但是一些高级特性还是不及CGPathRef。

四、具体绘图方法

由于iOS常用的绘图框架有UIKit和CoreGraphics两个，所以绘图的方法也有多种，下面介绍一下iOS的几种常用的绘图方法。

1.图片类型的上下文

图片上下文的绘制不需要在drawRect:方法中进行，在一个普通的OC方法中就可以绘制

使用UIKit实现


// 获取图片上下文
UIGraphicsBeginImageContextWithOptions(CGSizeMake(100,100), NO, 0);
// 绘图
UIBezierPath* p = [UIBezierPath bezierPathWithOvalInRect:CGRectMake(0,0,100,100)];
[[UIColor blueColor] setFill];
[p fill];
// 从图片上下文中获取绘制的图片
UIImage* im = UIGraphicsGetImageFromCurrentImageContext();
// 关闭图片上下文
UIGraphicsEndImageContext();
使用CoreGraphics实现


// 获取图片上下文
UIGraphicsBeginImageContextWithOptions(CGSizeMake(100,100), NO, 0);
// 绘图
CGContextRef con = UIGraphicsGetCurrentContext();
CGContextAddEllipseInRect(con, CGRectMake(0,0,100,100));
CGContextSetFillColorWithColor(con, [UIColor blueColor].CGColor);
CGContextFillPath(con);
// 从图片上下文中获取绘制的图片
UIImage* im = UIGraphicsGetImageFromCurrentImageContext();
// 关闭图片上下文
UIGraphicsEndImageContext();
2.drawRect:

在UIView子类的drawRect:方法中实现图形重新绘制，绘图步骤如下：

获取上下文

绘制图形

渲染图形

UIKit方法

- (void) drawRect: (CGRect) rect {
UIBezierPath* p = [UIBezierPathbezierPathWithOvalInRect:CGRectMake(0,0,100,100)];
[[UIColor blueColor] setFill];
[p fill];
}
CoreGraphics


- (void) drawRect: (CGRect) rect {
CGContextRef con = UIGraphicsGetCurrentContext();
CGContextAddEllipseInRect(con, CGRectMake(0,0,100,100));
CGContextSetFillColorWithColor(con, [UIColor blueColor].CGColor);
CGContextFillPath(con);
}

3.drawLayer:inContext:

在UIView子类的drawLayer:inContext:方法中也可以实现绘图任务，它是一个图层的代理方法，而为了能够调用该方法，需要给图层的delegate设置代理对象，其中代理对象不能是UIView对象，因为UIView对象已经是它内部根层（隐式层）的代理对象，再将它设置为另一个层的代理对象就会出问题。

一个view被添加到其它view上时，图层的变化如下：

先隐式地把此view的layer的CALayerDelegate设置成此view

调用此view的self.layer的drawInContext方法

由于drawLayer方法的注释：If defined, called by the default implementation of -drawInContext:说明了drawInContext里if([self.delegate responseToSelector:@selector(drawLayer:inContext:)])就执行drawLayer:inContext:方法，这里我们因为实现了drawLayer:inContext:所以会执行

[super drawLayer:layer inContext:ctx]会让系统自动调用此view的drawRect:方法，至此self.layer画出来了

在self.layer上再加一个子layer，当调用[layer setNeedsDisplay];时会自动调用此layer的drawInContext方法

如果drawRect不重写，就不会调用其layer的drawInContext方法，也就不会调用drawLayer:inContext方法

调用内部根层的drawLayer:inContext:


//如果drawRect不重写，就不会调用其layer的drawInContext方法，也就不会调用drawLayer:inContext方法
-(void)drawRect:(CGRect)rect{
NSLog(@"2-drawRect:");
NSLog(@"drawRect里的CGContext:%@",UIGraphicsGetCurrentContext());
//得到的当前图形上下文正是drawLayer中传递过来的
[super drawRect:rect];
}
#pragma mark - CALayerDelegate
-(void)drawLayer:(CALayer *)layer inContext:(CGContextRef)ctx{
NSLog(@"1-drawLayer:inContext:");
NSLog(@"drawLayer里的CGContext:%@",ctx);
// 如果去掉此句就不会执行drawRect!!!!!!!!
[super drawLayer:layer inContext:ctx];
}
调用外部代理对象的drawLayer:inContext:

由于不能把UIView对象设置为CALayerDelegate的代理，所以我们需要创建一个NSObject对象，然后实现drawLayer:inContext:方法，这样就可以在代理对象里绘制所需图形。另外，在设置代理时，不需要遵守CALayerDelegate的代理协议，即这个方法是NSObject的，不需要显式地指定协议。


// MyLayerDelegate.m
- (void)drawLayer:(CALayer *)layer inContext:(CGContextRef)ctx
{
CGContextAddEllipseInRect(ctx, CGRectMake(100,100,100,100));
CGContextSetFillColorWithColor(ctx, [UIColor blueColor].CGColor);
CGContextFillPath(ctx);
}
// ViewController.m
@interface ViewController () @property (nonatomic, strong) id myLayerDelegate;
@end
@implementation ViewController
- (void)viewDidLoad {
// 设置layer的delegate为NSObject子类对象
_myLayerDelegate = [[MyLayerDelegate alloc] init];
MyView *myView = [[MyView alloc] initWithFrame:self.view.bounds];
[self.view addSubview:myView];
CALayer *layer = [CALayer layer];
layer.backgroundColor = [UIColor magentaColor].CGColor;
layer.bounds = CGRectMake(0, 0, 300, 500);
layer.anchorPoint = CGPointZero;
layer.delegate = _myLayerDelegate;
[layer setNeedsDisplay];
[myView.layer addSublayer:layer];
}
详细实现过程

当UIView需要显示时，它内部的层会准备好一个CGContextRef(图形上下文)，然后调用delegate(这里就是UIView)的drawLayer:inContext:方法，并且传入已经准备好的CGContextRef对象。而UIView在drawLayer:inContext:方法中又会调用自己的drawRect:方法。平时在drawRect:中通过UIGraphicsGetCurrentContext()获取的就是由层传入的CGContextRef对象，在drawRect:中完成的所有绘图都会填入层的CGContextRef中，然后被拷贝至屏幕。
