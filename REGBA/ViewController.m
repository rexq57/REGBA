//
//  ViewController.m
//  REGBA
//
//  Created by Viktor Pih on 2020/11/10.
//

#import "ViewController.h"
#import <src/gba.h>

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    // Do any additional setup after loading the view.
    struct REGBA* gba = regba_create();
    regba_init(gba);
    regba_delete(gba);
}


- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}


@end
