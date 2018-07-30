//
//  SwizzleClassMethodHeader.h
//  Hoolink_IoT
//
//  Created by HL on 2018/7/30.
//  Copyright © 2018年 hoolink_IoT. All rights reserved.
//

#ifndef SwizzleClassMethodHeader_h
#define SwizzleClassMethodHeader_h
//#import <objc/runtime.h>

static inline void swizzleMethod(Class class, SEL oldSel, SEL newSel){
    
    //class_getInstanceMethod既可以获取实例方法,又可以获取类方法
    //class_getClassMethod只能获取类方法,不能获取实例方法
    Method oldMethod = class_getInstanceMethod(class, oldSel);
    Method newMethod = class_getInstanceMethod(class, newSel);
    
    
    // Method newMethod = class_getClassMethod(class, newSel);
    // Method oldMethod = class_getClassMethod(class, oldSel);
    
    
    IMP oldIMP = class_getMethodImplementation(class, oldSel);
    IMP newIMP = class_getMethodImplementation(class, newSel);
    
    //oldsel 未实现(当判断类方法时,需要传入的类为object_getClass((id)self),否则即使原来的方法已经实现,也会执行一下的第一段代码,并且造成交换方法失败
    if (class_addMethod(class, oldSel, newIMP, method_getTypeEncoding(newMethod))) {
        class_replaceMethod(class, newSel, oldIMP, method_getTypeEncoding(oldMethod));
    } else {
        //oldsel 已经实现
        method_exchangeImplementations(oldMethod, newMethod);
    }
    
}
static void swizzleClassMethod(Class class, SEL oldSel, SEL newSel){
    //要特别注意你替换的方法到底是哪个性质的方法
    // When swizzling a Instance method, use the following:
    // Class class = [self class];
    
    // When swizzling a class method, use the following:
    
    // Class class = object_getClass((id)self);//获取到的是类的地址(0x00000001ace88090)
    // Class cs = [[[UIImage alloc] init] class];//获取到的是类名(UIImage)
    //
    // Class cl = [self class];//获取到的是类名(UIImage)
    
    Class relclass = object_getClass(class);
    swizzleMethod(relclass, oldSel, newSel);
}
static void swizzleIntanceMethod(Class class, SEL oldSel, SEL newSel){
    swizzleMethod(class, oldSel, newSel);
}

static NSMutableDictionary * imageForKeyDict(){
    static dispatch_once_t onceToken;
    static NSMutableDictionary *imageForKeyDict = nil;
    dispatch_once(&onceToken, ^{
        imageForKeyDict = [NSMutableDictionary dictionary];
    });
    return imageForKeyDict;
}
static NSMutableDictionary * scaleForKeyDict(){
    static dispatch_once_t onceToken;
    static NSMutableDictionary *scaleForKeyDict = nil;
    dispatch_once(&onceToken, ^{
        scaleForKeyDict = [NSMutableDictionary dictionary];
    });
    return scaleForKeyDict;
}
//static pthread_mutex_t pthread_lock(){
//
// static pthread_mutexattr_t attr;
// static pthread_mutex_t pthread_lock;
// static dispatch_once_t onceToken;
// dispatch_once(&;onceToken, ^{
// pthread_mutexattr_init(&;attr);
// pthread_mutexattr_settype(&;attr, PTHREAD_MUTEX_RECURSIVE);
// pthread_mutex_init(&;pthread_lock, &;attr);
// });
// return pthread_lock;
//}
static void dictionarySetWeakRefrenceObjectForKey(NSMutableDictionary *dictM, id object, NSString *key){
    
    NSValue *value = [NSValue valueWithNonretainedObject:object];
    [dictM setValue:value forKey:key];
    // [dictM setObject:object forKey:key];
}
static id dictionaryGetWeakRefrenceObjectForKey(NSMutableDictionary *dictM, NSString *key){
    NSValue *value = [dictM valueForKey:key];
    return value.nonretainedObjectValue;
    // return [dictM objectForKey:key];
}
static void dictionarySetObjectForKey(NSMutableDictionary *dictM, id object, NSString *key){
    
    [dictM setObject:object forKey:key];
    // [dictM setObject:object forKey:key];
}
static id dictionaryGetObjectForKey(NSMutableDictionary *dictM, NSString *key){
    return [dictM objectForKey:key];
    // return [dictM objectForKey:key];
}
static void dictionaryRemoveObjectForKey(NSMutableDictionary *dictM, NSString *key){
    [dictM removeObjectForKey:key];
}
#pragma clang diagnostic pop
static NSArray *dictionaryAllKeysForObject(NSMutableDictionary *dictM,id object){
    return [dictM allKeysForObject:object];
}


#endif /* SwizzleClassMethodHeader_h */
