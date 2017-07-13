#import "parameter.h"
#import "Utils.h"

@implementation Parameter
-(id)init
{
  self = [super init];
  if(self) {
    label = [[UILabel alloc] init];
    [label setBackgroundColor:[UIColor clearColor]];
  }
  return self;
}

-(void)refresh
{
  return;
}

-(NSString *)getName
{
  return name;
}

-(UILabel *)getLabel
{
  return label;
}

-(void)setFrame:(CGRect)frame
{
  return;
}

-(void)setLabelFrame:(CGRect)frame
{
  [label setFrame:frame];
}

-(void)editValue
{
  if(onelab_cb("check") > 0){
    [[NSNotificationCenter defaultCenter] postNotificationName:@"requestRender" object:nil];
    [[NSNotificationCenter defaultCenter] postNotificationName:@"refreshParameters" object:nil];
  }
}

-(bool) isReadOnly {return NO;}

+(double)getHeight
{
  return 60.0f;
}
@end

@implementation ParameterStringList
-(id) initWithString:(onelab::string)string
{
  self = [super init];
  if(self){
    label.alpha = (string.getReadOnly())? 0.439216f : 1.0f;
    [label setText:[Utils getStringFromCString:string.getShortName().c_str()]];
    name = [Utils getStringFromCString:string.getName().c_str()];
    button = [UIButton buttonWithType:UIButtonTypeSystem];
    [button addTarget:self action:@selector(selectValue) forControlEvents:UIControlEventTouchDown];
    [button setTitle:[Utils getStringFromCString:string.getValue().c_str()] forState:UIControlStateNormal];
  }
  return self;
}

-(void)selectValue
{
  std::vector<onelab::string> string;
  onelab::server::instance()->get(string, [name UTF8String]);
  if(string.size() < 1) return;
  UIActionSheet *popupSelectValue =
    [[UIActionSheet alloc] initWithTitle:[Utils getStringFromCString:string[0].getLabel().c_str()]
                                delegate:self cancelButtonTitle:nil destructiveButtonTitle:nil
                       otherButtonTitles:nil];
  std::vector<std::string> choices = string[0].getChoices();
  for(int i=0;i<choices.size();i++)
    [popupSelectValue addButtonWithTitle:[Utils getStringFromCString:choices[i].c_str()]];
  [popupSelectValue addButtonWithTitle:@"Cancel"];
  [popupSelectValue setCancelButtonIndex:popupSelectValue.numberOfButtons - 1];
  [popupSelectValue showInView:button];
}

-(void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex
{
  std::vector<onelab::string> string;
  onelab::server::instance()->get(string, [name UTF8String]);
  if(string.size() < 1) return;
  if(buttonIndex > string[0].getChoices().size() - 1) return; // cancel
  std::string selected = string[0].getChoices()[buttonIndex];
  string[0].setValue(selected);
  onelab::server::instance()->set(string[0]);
  [super editValue];
}

-(void)refresh
{
  std::vector<onelab::string> string;
  onelab::server::instance()->get(string, [name UTF8String]);
  [button setTitle:[Utils getStringFromCString:string[0].getValue().c_str()] forState:UIControlStateNormal];
}

-(void)setFrame:(CGRect)frame
{
  [button setFrame:frame];
}

-(UIButton *)getUIView
{
  return button;
}

-(bool)isReadOnly
{
  std::vector<onelab::string> string;
  onelab::server::instance()->get(string, [name UTF8String]);
  if(string.size() < 1) return YES;
  return string[0].getReadOnly();
}

+(double)getHeight
{
  return 60.f;
}
@end

@implementation ParameterNumberList
-(id) initWithNumber:(onelab::number) number
{
  self = [super init];
  if(self) {
    label.alpha = (number.getReadOnly())? 0.439216f : 1.0f;
    [label setText:[Utils getStringFromCString:number.getShortName().c_str()]];
    name = [Utils getStringFromCString:number.getName().c_str()];
    button = [UIButton buttonWithType:UIButtonTypeSystem];
    [button addTarget:self action:@selector(selectValue) forControlEvents:UIControlEventTouchDown];
    [button setTitle:[Utils getStringFromCString:number.getValueLabel(number.getValue()).c_str()]
            forState:UIControlStateNormal];
  }
  return self;
}

-(void)selectValue
{
  std::vector<onelab::number> numbers;
  onelab::server::instance()->get(numbers, [name UTF8String]);
  if(numbers.size() < 1) return;
  UIAlertController *alertController;
  UIAlertAction *destroyAction;
  alertController = [UIAlertController alertControllerWithTitle:nil message:nil
                                                 preferredStyle:UIAlertControllerStyleActionSheet];
  std::vector<double> choices = numbers[0].getChoices();
  for(unsigned int i = 0; i < choices.size(); i++)
    [alertController
      addAction:[UIAlertAction
                  actionWithTitle:[Utils getStringFromCString:numbers[0].getValueLabel(choices[i]).c_str()]
                            style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {
          [self updateNumber:numbers[0] withValue:choices[i]];
          [button setTitle:[Utils getStringFromCString:numbers[0].getValueLabel(i).c_str()]
                  forState:UIControlStateNormal];
	}]];

  destroyAction = [UIAlertAction actionWithTitle:@"Cancel"
                                           style:UIAlertActionStyleDestructive
                                         handler:^(UIAlertAction *action) {
      // do nothing
    }];

  [alertController addAction:destroyAction];
  [alertController setModalPresentationStyle:UIModalPresentationPopover];

  UIPopoverPresentationController *popPresenter = [alertController popoverPresentationController];
  popPresenter.sourceView = button;
  popPresenter.sourceRect = button.bounds;
  // FIXME: is traverseResponderChainForUIViewController a good idea?
  [[Utils traverseResponderChainForUIViewController:button] presentViewController:alertController
                                                                         animated:YES completion:nil];
}

-(void) updateNumber: (onelab::number)n withValue:(double)v
{
  n.setValue(v);
  onelab::server::instance()->set(n);

  [super editValue];
}

-(void)refresh
{
  std::vector<onelab::number> number;
  onelab::server::instance()->get(number, [name UTF8String]);
  [button setTitle:[Utils getStringFromCString:number[0].getValueLabel(number[0].getValue()).c_str()]
          forState:UIControlStateNormal];
}

-(void)setFrame:(CGRect)frame
{
  [button setFrame:frame];
}

-(UIButton *)getUIView
{
  return button;
}

-(bool)isReadOnly
{
  std::vector<onelab::number> number;
  onelab::server::instance()->get(number, [name UTF8String]);
  if(number.size() < 1) return YES;
  return number[0].getReadOnly();
}

+(double)getHeight
{
  return 60.f;
}
@end

@implementation ParameterNumberCheckbox
-(id) initWithNumber:(onelab::number) number
{
  self = [super init];
  if(self) {
    label.alpha = (number.getReadOnly())? 0.439216f : 1.0f;
    [label setText:[Utils getStringFromCString:number.getShortName().c_str()]];
    name = [Utils getStringFromCString:number.getName().c_str()];
    checkbox = [[UISwitch alloc] init];
    [checkbox setOn:(number.getValue() == 1)];
    [checkbox addTarget:self action:@selector(valueChange:) forControlEvents:UIControlEventValueChanged];
  }
  return self;
}

-(void)refresh
{
  std::vector<onelab::number> number;
  onelab::server::instance()->get(number, [name UTF8String]);
  [checkbox setSelected:(number[0].getValue() == 1)];
}

-(void) valueChange:(UISwitch *)sender
{
  std::vector<onelab::number> number;
  onelab::server::instance()->get(number, [name UTF8String]);
  if(number.size() < 1) return;
  number[0].setValue(([sender isOn])? 1 : 0);
  onelab::server::instance()->set(number[0]);
  [super editValue];
}

-(void)setFrame:(CGRect)frame
{
  [checkbox setFrame:frame];
}

-(UISwitch *)getCheckbox
{
  return checkbox;
}

-(bool)isReadOnly
{
  std::vector<onelab::number> number;
  onelab::server::instance()->get(number, [name UTF8String]);
  if(number.size() < 1) return YES;
  return number[0].getReadOnly();
}

+(double)getHeight
{
  return 40.0f;
}
@end

@implementation ParameterNumberStepper
-(id) initWithNumber:(onelab::number) number
{
  self = [super init];
  if(self) {
    name = [Utils getStringFromCString:number.getName().c_str()];
    label.alpha = (number.getReadOnly())? 0.439216f : 1.0f;
    stepper = [[UIStepper alloc] init];
    [stepper setValue:number.getValue()];
    [stepper setStepValue:1];
    [stepper setMaximumValue:number.getMax()];
    [stepper setMinimumValue:number.getMin()];
    [stepper addTarget:self action:@selector(stepperValueChanged:) forControlEvents:UIControlEventValueChanged];
    [label setText:[NSString stringWithFormat:@"%@ %d", [Utils getStringFromCString:number.getShortName().c_str()],
                             (int)number.getValue()]];
  }
  return self;
}

-(void)stepperValueChanged:(UIStepper *)sender
{
  std::vector<onelab::number> number;
  onelab::server::instance()->get(number, [name UTF8String]);
  if(number.size() < 1) return;
  number[0].setValue(sender.value);
  onelab::server::instance()->set(number[0]);
  [label setText:[NSString stringWithFormat:@"%@ %d", [Utils getStringFromCString:number[0].getShortName().c_str()],
                           (int)number[0].getValue()]];
  [super editValue];
}

-(void)refresh
{
  std::vector<onelab::number> number;
  onelab::server::instance()->get(number, [name UTF8String]);
  if(number.size() < 1) return;
  [stepper setValue:number[0].getValue()];
  [label setText:[NSString stringWithFormat:@"%@ %d", [Utils getStringFromCString:number[0].getShortName().c_str()],
                           (int)number[0].getValue()]];
}

-(void)setFrame:(CGRect)frame
{
  [stepper setFrame:frame];
}

-(UIStepper *)getStepper
{
  return stepper;
}

+(double)getHeight
{
  return 60.0f;
}
@end

@implementation ParameterNumberRange
-(id) initWithNumber:(onelab::number) number
{
  self = [super init];
  if(self) {
    label.alpha = (number.getReadOnly())? 0.439216f : 1.0f;
    name = [Utils getStringFromCString:number.getName().c_str()];
    slider = [[UISlider alloc] init];
    [slider setMaximumValue:number.getMax()];
    [slider setMinimumValue:number.getMin()];
    [slider setValue:number.getValue()];
    //TODO add step ?
    [slider addTarget:self action:@selector(sliderValueChanged:) forControlEvents:UIControlEventTouchUpOutside];
    [slider addTarget:self action:@selector(sliderValueChanged:) forControlEvents:UIControlEventTouchUpInside];
    [label setText:[NSString stringWithFormat:@"%@ %g", [Utils getStringFromCString:number.getShortName().c_str()],
                             number.getValue()]];
  }
  return self;
}

-(void)refresh
{
  std::vector<onelab::number> number;
  onelab::server::instance()->get(number, [name UTF8String]);
  if(number.size() < 1) return;
  [slider setMaximumValue:number[0].getMax()];
  [slider setMinimumValue:number[0].getMin()];
  [slider setValue:number[0].getValue()];
  [label setText:[NSString stringWithFormat:@"%@ %g", [Utils getStringFromCString:number[0].getShortName().c_str()],
                           number[0].getValue()]];
}

-(void)sliderValueChanged:(UISlider *)sender
{
  std::vector<onelab::number> number;
  onelab::server::instance()->get(number, [name UTF8String]);
  if(number.size() < 1) return;
  number[0].setValue(sender.value);
  onelab::server::instance()->set(number[0]);
  [label setText:[NSString stringWithFormat:@"%@ %g", [Utils getStringFromCString:number[0].getShortName().c_str()],
                           number[0].getValue()]];
  [super editValue];
}

-(void)setFrame:(CGRect)frame
{
  [slider setFrame:frame];
}

-(UISlider *)getSlider
{
  return slider;
}

-(bool)isReadOnly
{
  std::vector<onelab::number> number;
  onelab::server::instance()->get(number, [name UTF8String]);
  if(number.size() < 1) return YES;
  return number[0].getReadOnly();
}

+(double)getHeight
{
  return 65.0f;
}
@end

@implementation ParameterNumberTextbox
-(id)initWithNumber:(onelab::number)number
{
  self = [super init];
  if(self) {
    label.alpha = (number.getReadOnly())? 0.439216f : 1.0f;
    [label setText:[Utils getStringFromCString:number.getShortName().c_str()]];
    name = [Utils getStringFromCString:number.getName().c_str()];
    textbox = [[UITextField alloc] init];
    [textbox setBorderStyle:UITextBorderStyleRoundedRect];
    [textbox setText:[NSString stringWithFormat:@"%g", number.getValue()]];
    [textbox setDelegate:self];
    UIToolbar* numberToolbar = [[UIToolbar alloc]initWithFrame:CGRectMake(0, 0, 320, 50)];
    numberToolbar.items = [NSArray arrayWithObjects:
                                     [[UIBarButtonItem alloc]initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:nil action:nil],
                                   [[UIBarButtonItem alloc]initWithTitle:@"Apply" style:UIBarButtonItemStyleDone target:self action:@selector(doneWithNumberPad)],
                                   nil];
    [numberToolbar sizeToFit];
    textbox.inputAccessoryView = numberToolbar;
  }
  return self;
}

-(void)refresh
{
  std::vector<onelab::number> number;
  onelab::server::instance()->get(number, [name UTF8String]);
  if(number.size() < 1) return;
  [textbox setText:[NSString stringWithFormat:@"%g", number[0].getValue()]];
  [textbox reloadInputViews];
}

-(BOOL)textFieldShouldEndEditing:(UITextField *)textField
{
  std::vector<onelab::number> number;
  onelab::server::instance()->get(number, [name UTF8String]);
  if(number.size() < 1) return YES;
  number[0].setValue([textField.text doubleValue]);
  onelab::server::instance()->set(number[0]);
  [textField setText:[NSString stringWithFormat:@"%g", number[0].getValue()]];
  [super editValue];
  return YES;
}

-(BOOL)textFieldShouldReturn:(UITextField *)textField
{
  return [textField endEditing:YES];
}

-(void)doneWithNumberPad
{
  [textbox endEditing:YES];
}

-(void)setFrame:(CGRect)frame
{
  [textbox setFrame:frame];
}

-(UITextField *)getTextbox
{
  return textbox;
}

-(bool)isReadOnly
{
  std::vector<onelab::number> number;
  onelab::server::instance()->get(number, [name UTF8String]);
  if(number.size() < 1) return YES;
  return number[0].getReadOnly();
}

+(double)getHeight
{
  return 60.f;
}
@end
