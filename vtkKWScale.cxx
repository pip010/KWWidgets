/*=========================================================================

  Module:    $RCSfile: vtkKWScale.cxx,v $

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkKWScale.h"

#include "vtkKWEntry.h"
#include "vtkKWApplication.h"
#include "vtkKWLabel.h"
#include "vtkKWPushButton.h"
#include "vtkObjectFactory.h"


// ---------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWScale );
vtkCxxRevisionMacro(vtkKWScale, "$Revision: 1.76 $");

int vtkKWScaleCommand(ClientData cd, Tcl_Interp *interp,
                      int argc, char *argv[]);

/* 
 * This part was generated by ImageConvert from image:
 *    arrow.png (zlib, base64)
 */
#define image_arrow_width         4
#define image_arrow_height        7
#define image_arrow_pixel_size    4
#define image_arrow_buffer_length 40

static unsigned char image_arrow[] = 
  "eNpjYGD4z4AK/jOgiv1HE/uPB+PSDwcAlQUP8Q==";

// ---------------------------------------------------------------------------
vtkKWScale::vtkKWScale()
{
  this->CommandFunction = vtkKWScaleCommand;

  this->Command      = NULL;
  this->StartCommand = NULL;
  this->EndCommand   = NULL;
  this->EntryCommand = NULL;

  this->DisableCommands = 0;
  this->DisableScaleValueCallback = 1;

  this->SmartResize = 0;

  this->Value      = 0;
  this->Resolution = 1;
  this->Range[0]   = 0;
  this->Range[1]   = 1;  

  this->Scale           = vtkKWWidget::New();
  this->Entry           = NULL;
  this->Label           = NULL;
  this->TopLevel        = NULL;
  this->PopupPushButton = NULL;

  this->RangeMinLabel = NULL;
  this->RangeMaxLabel = NULL;
  
  this->DisplayEntryAndLabelOnTop = 1;
  this->PopupScale = 0;
  this->ExpandEntry = 0;

  this->NormalLabel = NULL;
  this->ShortLabel = NULL;

  this->LongWidth = 230;
  this->MediumWidth = 200;
  this->MediumShortWidth = 150;
  this->ShortWidth = 100;
  
  this->RangeMinLabel = NULL;
  this->RangeMaxLabel = NULL;
  this->DisplayRange = 0;
  
  this->PackEntry = 1;
  this->PackRange = 1;

  this->ClampValue = 1;
}

// ---------------------------------------------------------------------------
vtkKWScale::~vtkKWScale()
{
  if (this->IsAlive())
    {
    this->UnBind();
    }
  if (this->Command)
    {
    delete [] this->Command;
    this->Command = NULL;
    }

  if (this->StartCommand)
    {
    delete [] this->StartCommand;
    this->StartCommand = NULL;
    }

  if (this->EndCommand)
    {
    delete [] this->EndCommand;
    this->EndCommand = NULL;
    }

  if (this->EntryCommand)
    {
    delete [] this->EntryCommand;
    this->EntryCommand = NULL;
    }

  if (this->Scale)
    {
    this->Scale->Delete();
    this->Scale = NULL;
    }

  if (this->Entry)
    {
    this->Entry->Delete();
    this->Entry = NULL;
    }

  if (this->Label)
    {
    this->Label->Delete();
    this->Label = NULL;
    }

  if (this->RangeMinLabel)
    {
    this->RangeMinLabel->Delete();
    this->RangeMinLabel = NULL;
    }
  
  if (this->RangeMaxLabel)
    {
    this->RangeMaxLabel->Delete();
    this->RangeMaxLabel = NULL;
    }
  
  if (this->TopLevel)
    {
    this->TopLevel->Delete();
    this->TopLevel = NULL;
    }

  if (this->PopupPushButton)
    {
    this->PopupPushButton->Delete();
    this->PopupPushButton = NULL;
    }
  
  this->SetShortLabel(NULL);
  this->SetNormalLabel(NULL);
}

// ---------------------------------------------------------------------------
void vtkKWScale::Create(vtkKWApplication *app, const char *args)
{
  const char *wname;

  // Set the application

  if (this->IsCreated())
    {
    vtkErrorMacro("Scale already created");
    return;
    }

  this->SetApplication(app);

  // Create the frame that will embend all widgets

  wname = this->GetWidgetName();
  this->Script("frame %s -bd 0", wname);

  // If we need the scale to popup, create the toplevel and the pushbutton

  if (this->PopupScale)
    {
    this->TopLevel = vtkKWWidget::New();
    this->TopLevel->Create(
      app, "toplevel", "-bg black -bd 2 -relief flat");
    this->Script("wm overrideredirect %s 1", 
                 this->TopLevel->GetWidgetName());
    this->Script("wm withdraw %s", 
                 this->TopLevel->GetWidgetName());
    this->PopupPushButton = vtkKWPushButton::New();
    this->PopupPushButton->SetParent(this);
    this->PopupPushButton->Create(app, "-padx 0 -pady 0");

    this->PopupPushButton->SetImageOption(image_arrow, 
                                          image_arrow_width, 
                                          image_arrow_height, 
                                          image_arrow_pixel_size,
                                          image_arrow_buffer_length);

    this->Scale->SetParent(this->TopLevel);
    }
  else
    {
    this->Scale->SetParent(this);
    }

  // Create the scale

  this->Scale->SetApplication(app);
  this->Script(
    "scale %s -orient horizontal -showvalue no -bd 2 -highlightthickness 0"
    " -resolution %g -from %g -to %g %s", 
    this->Scale->GetWidgetName(), 
    this->Resolution,
    this->Range[0], this->Range[1],
    (args ? args : ""));

  this->PackWidget();
  this->Bind();

  // Update enable state

  this->UpdateEnableState();
}

// ---------------------------------------------------------------------------
void vtkKWScale::DisplayEntry()
{
  if (this->Entry && this->Entry->IsCreated())
    {
    return;
    }

  this->Entry = vtkKWEntry::New();
  this->Entry->SetParent(this);
  this->Entry->Create(this->GetApplication(), 0);
  this->Entry->SetWidth(11);
  this->Entry->SetEnabled(this->Enabled);
  this->Entry->SetValue(this->GetValue());

  this->PackWidget();
  this->Bind();
}

// ---------------------------------------------------------------------------
void vtkKWScale::DisplayLabel(const char *label)
{
  this->SetNormalLabel(label);

  if (!this->ShortLabel)
    {
    this->SetShortLabel(label);
    }
  
  if (!this->Label)
    {
    this->Label = vtkKWLabel::New();
    }

  if (!this->Label->IsCreated())
    {
    this->Label->SetParent(this);
    this->Label->Create(this->GetApplication(), "-anchor w");
    this->Label->SetEnabled(this->Enabled);
    }

  this->Label->SetLabel(label);

  this->PackWidget();
}

// ---------------------------------------------------------------------------
void vtkKWScale::SetDisplayEntryAndLabelOnTop(int arg)
{
  if (this->DisplayEntryAndLabelOnTop == arg)
    {
    return;
    }

  this->DisplayEntryAndLabelOnTop = arg;
  this->Modified();

  this->PackWidget();
}

// ---------------------------------------------------------------------------
void vtkKWScale::SetExpandEntry(int arg)
{
  if (this->ExpandEntry == arg)
    {
    return;
    }

  this->ExpandEntry = arg;
  this->Modified();

  this->PackWidget();
}

// ---------------------------------------------------------------------------
void vtkKWScale::PackWidget()
{
  if (!this->IsCreated())
    {
    return;
    }

  if (this->Scale && this->Scale->IsCreated())
    {
    if (this->DisplayEntryAndLabelOnTop && !this->PopupScale)
      {
      this->Script("pack %s -side bottom -fill x -expand yes -pady 0 -padx 0",
                   this->Scale->GetWidgetName());
      }
    else
      {
      this->Script("pack %s -side left -fill x -expand yes -pady 0 -padx 0",
                   this->Scale->GetWidgetName());
      }
    }

  int is_label_empty = 1;

  if (this->Label && this->Label->IsCreated())
    {
    this->Script("pack forget %s", this->Label->GetWidgetName());
    const char *res = this->Label->GetLabel();
    is_label_empty = (!res || !*res);
    if (!is_label_empty)
      {
      if (this->DisplayEntryAndLabelOnTop || this->PopupScale)
        {
        this->Script("pack %s -side left -padx 0 -fill y -anchor w", 
                     this->Label->GetWidgetName());
        }
      else
        {
        this->Script("pack %s -side left -padx 0 -fill y -anchor w -before %s", 
                     this->Label->GetWidgetName(), 
                     this->Scale->GetWidgetName());
        }
      }
    }

  if (this->DisplayRange)
    {
    this->Script("pack forget %s %s",
                 this->RangeMinLabel->GetWidgetName(),
                 this->RangeMaxLabel->GetWidgetName());
    if (this->PackRange)
      {
      this->Script("pack %s -side left -padx 0 -fill y -before %s",
                   this->RangeMinLabel->GetWidgetName(),
                   this->Scale->GetWidgetName());
      this->Script("pack %s -side left -padx 0 -fill y -after %s",
                   this->RangeMaxLabel->GetWidgetName(),
                   this->Scale->GetWidgetName());
      }
    }
  
  if (this->Entry && this->Entry->IsCreated())
    {
    this->Script("pack forget %s", this->Entry->GetWidgetName());
    if (this->PackEntry)
      {
      if (this->PopupScale)
        {
        this->Script("pack %s -side left -padx 0 %s", 
                     this->Entry->GetWidgetName(),
                     (this->ExpandEntry ? "-fill both -expand t" : "-fill y"));
        }
      else
        {
        if (this->DisplayEntryAndLabelOnTop)
          {
          this->Script("pack %s -side right -padx 0 -fill y", 
                       this->Entry->GetWidgetName());
          }
        else
          {
          this->Script("pack %s -side right -padx 0 -fill y -after %s", 
                       this->Entry->GetWidgetName(), 
                       this->Scale->GetWidgetName());
          }
        }
      }
    }

  if (this->PopupScale && 
      this->PopupPushButton && this->PopupPushButton->IsCreated())
    {
    this->Script("pack forget %s", this->PopupPushButton->GetWidgetName());
    if (this->Entry || (this->Label && !is_label_empty))
      {
      this->Script("pack %s -side left -padx 1 -fill y -ipadx 1 -after %s", 
                   this->PopupPushButton->GetWidgetName(),
                   (this->Entry ? this->Entry->GetWidgetName() 
                                : this->Label->GetWidgetName()));
      }
    else
      {
      this->Script("pack %s -side left -padx 1 -fill y -ipadx 1", 
                   this->PopupPushButton->GetWidgetName());
      }
    }
}

// ---------------------------------------------------------------------------
void vtkKWScale::Bind()
{
  this->Script("bind %s <Configure> {%s Resize}",
               this->GetWidgetName(), this->GetTclName());
  
  if (this->Scale && this->Scale->IsCreated())
    {
    this->Script("bind %s <ButtonPress> {%s InvokeStartCommand}",
                 this->Scale->GetWidgetName(), this->GetTclName());
    this->Script("bind %s <ButtonRelease> {%s InvokeEndCommand}",
                 this->Scale->GetWidgetName(), this->GetTclName());

    this->Script("bind %s <ButtonPress> {+%s DisableScaleValueCallbackOff}",
                 this->Scale->GetWidgetName(), this->GetTclName());
    this->Script("bind %s <ButtonRelease> {+%s DisableScaleValueCallbackOn}",
                 this->Scale->GetWidgetName(), this->GetTclName());

    // If in popup mode, leaving the toplevel will withdraw it, unless 
    // the user is interacting with the scale.

    if (this->PopupScale &&
        this->TopLevel && this->TopLevel->IsCreated())
      {
      this->Script("bind %s <Leave> {%s WithdrawPopupScaleCallback}",
                   this->TopLevel->GetWidgetName(), this->GetTclName());
      this->Script("bind %s <ButtonPress> {+bind %s <Leave> {}}",
                   this->Scale->GetWidgetName(), 
                   this->TopLevel->GetWidgetName());
      this->Script("bind %s <ButtonRelease> "
                   "{+bind %s <Leave> {%s WithdrawPopupScaleCallback}}",
                   this->Scale->GetWidgetName(), 
                   this->TopLevel->GetWidgetName(), 
                   this->GetTclName());
      }
    
    this->Scale->SetCommand(this, "ScaleValueCallback");
    }

  if (this->Entry && this->Entry->IsCreated())
    {
    this->Script("bind %s <Return> {%s EntryValueCallback}",
                 this->Entry->GetWidgetName(), this->GetTclName());
    this->Script("bind %s <FocusOut> {%s EntryValueCallback}",
                 this->Entry->GetWidgetName(), this->GetTclName());
    }

  if (this->PopupScale && 
      this->PopupPushButton && this->PopupPushButton->IsCreated())
    {
    this->Script("bind %s <ButtonPress> {%s DisplayPopupScaleCallback}",
                 this->PopupPushButton->GetWidgetName(), this->GetTclName());
    }
}

// ---------------------------------------------------------------------------
void vtkKWScale::UnBind()
{
  this->Script("bind %s <Configure> {}",
               this->GetWidgetName());
  
  if (this->Scale && this->Scale->IsCreated())
    {
    this->Script("bind %s <ButtonPress>", 
                 this->Scale->GetWidgetName());
    this->Script("bind %s <ButtonRelease>", 
                 this->Scale->GetWidgetName());
    this->Script("%s configure -command {}",
                 this->Scale->GetWidgetName());
    }

  if (this->Entry && this->Entry->IsCreated())
    {
    this->Script("bind %s <Return>",
                 this->Entry->GetWidgetName());
    this->Script("bind %s <FocusOut>",
                 this->Entry->GetWidgetName());
    }

  if (this->PopupScale && 
      this->PopupPushButton && this->PopupPushButton->IsCreated())
    {
    this->Script("bind %s <ButtonPress>",
                 this->PopupPushButton->GetWidgetName());
    }
}

// ---------------------------------------------------------------------------
void vtkKWScale::DisplayPopupScaleCallback()
{
  if (!this->PopupScale ||
      !this->PopupPushButton || !this->PopupPushButton->IsCreated() ||
      !this->TopLevel || !this->TopLevel->IsCreated() ||
      !this->Scale || !this->Scale->IsCreated())
    {
    return;
    }

  // Get the position of the mouse, the position and size of the push button,
  // the size of the scale.

  this->Script("concat "
               " [winfo pointerx %s] [winfo pointery %s]" 
               " [winfo rooty %s] [winfo height %s]"
               " [%s coords %g]"
               " [winfo x %s] [winfo y %s]",
               this->GetWidgetName(), 
               this->GetWidgetName(),
               this->PopupPushButton->GetWidgetName(), 
               this->PopupPushButton->GetWidgetName(),
               this->Scale->GetWidgetName(), this->Value,
               this->Scale->GetWidgetName(), this->Scale->GetWidgetName());
  
  int x, y, py, ph, scx, scy, sx, sy;
  sscanf(this->GetApplication()->GetMainInterp()->result, 
         "%d %d %d %d %d %d %d %d", 
         &x, &y, &py, &ph, &scx, &scy, &sx, &sy);
 
  // Place the scale so that the slider is coincident with the x mouse position
  // and just below the push button
  
  x -= sx + scx;

  if (py <= y && y <= (py + ph -1))
    {
    y = py + ph - 3;
    }
  else
    {
    y -= sy + scy;
    }

  this->Script("wm geometry %s +%d+%d",
               this->TopLevel->GetWidgetName(), x, y);
  this->Script("update");
  this->Script("wm deiconify %s", 
               this->TopLevel->GetWidgetName());
  this->Script("raise %s", 
               this->TopLevel->GetWidgetName());
  this->Script("wm transient %s %s", 
               this->TopLevel->GetWidgetName(),
               this->GetWidgetName());

  this->RefreshValue();
}

// ---------------------------------------------------------------------------
void vtkKWScale::WithdrawPopupScaleCallback()
{
  if (!this->PopupScale ||
      !this->TopLevel || !this->TopLevel->IsCreated() ||
      !this->Scale || !this->Scale->IsCreated())
    {
    return;
    }

  // Withdraw the popup

  this->Script("wm withdraw %s",
               this->TopLevel->GetWidgetName());

  this->RefreshValue();
}

// ---------------------------------------------------------------------------
void vtkKWScale::SetResolution(double r)
{
  if (this->Resolution == r)
    {
    return;
    }

  this->Resolution = r;
  this->Modified();

  if (this->Scale && this->Scale->IsCreated())
    {
    this->Script("%s configure -resolution %g",
                 this->Scale->GetWidgetName(), r);
    }
}

// ---------------------------------------------------------------------------
void vtkKWScale::SetValue(double num)
{
  if (this->ClampValue)
    {
    if (this->Range[1] > this->Range[0])
      {
      if (num > this->Range[1]) 
        { 
        num = this->Range[1]; 
        }
      else if (num < this->Range[0])
        {
        num = this->Range[0];
        }
      }
    else
      {
      if (num < this->Range[1]) 
        { 
        num = this->Range[1]; 
        }
      else if (num > this->Range[0])
        {
        num = this->Range[0];
        }
      }
    }

  if (this->Value == num)
    {
    // Pass the value to the entry to keep it in sync with the scale
    this->RefreshValue();
    return;
    }

  this->Value = num;
  this->Modified();

  this->RefreshValue();

  if (this->Command && !this->DisableCommands)
    {
    this->Script("eval %s", this->Command);
    }
}

// ---------------------------------------------------------------------------
void vtkKWScale::RefreshValue()
{
  if (this->Scale && this->Scale->IsCreated())
    {
    int was_disabled = !this->Scale->GetEnabled();
    if (was_disabled)
      {
      this->Scale->SetStateOption(1);
      this->Scale->SetEnabled(1);
      }
    this->Script("%s set %g", 
                 this->Scale->GetWidgetName(), this->Value);
    if (was_disabled)
      {
      this->Scale->SetStateOption(0);
      this->Scale->SetEnabled(0);
      }
    }
  
  if (this->Entry && this->Entry->IsCreated())
    {
    this->Entry->SetValue(this->Value);
    }
}

// ---------------------------------------------------------------------------
void vtkKWScale::SetRange(double min, double max)
{
  if (this->Range[0] == min && this->Range[1] == max)
    {
    return;
    }

  this->Range[0] = min;
  this->Range[1] = max;
  this->Modified();

  if (this->Scale && this->Scale->IsCreated())
    {
    this->Script("%s configure -from %g -to %g",
                 this->Scale->GetWidgetName(), min, max);
    }
  if (this->RangeMinLabel && this->RangeMinLabel->IsCreated())
    {
    this->Script("%s configure -text %g",
                 this->RangeMinLabel->GetWidgetName(), min);
    }
  if (this->RangeMaxLabel && this->RangeMaxLabel->IsCreated())
    {
    this->Script("%s configure -text %g",
                 this->RangeMaxLabel->GetWidgetName(), max);
    }
}

// ---------------------------------------------------------------------------
void vtkKWScale::ScaleValueCallback(double num)
{
  if (this->DisableScaleValueCallback)
    {
    return;
    }

  this->SetValue(num);
}

// ---------------------------------------------------------------------------
void vtkKWScale::EntryValueCallback()
{
  double value = this->Entry->GetValueAsFloat();
  double old_value = this->GetValue();
  this->SetValue(value);

  if (value != old_value && this->EntryCommand && !this->DisableCommands)
    {
    this->Script("eval %s", this->EntryCommand);
    }
}

// ---------------------------------------------------------------------------
void vtkKWScale::InvokeStartCommand()
{
  if (this->StartCommand && !this->DisableCommands)
    {
    this->Script("eval %s", this->StartCommand);
    }
}

// ---------------------------------------------------------------------------
void vtkKWScale::InvokeEndCommand()
{
  if (this->EndCommand && !this->DisableCommands)
    {
    this->Script("eval %s", this->EndCommand);
    }
}

// ---------------------------------------------------------------------------
void vtkKWScale::InvokeEntryCommand()
{
  if (this->EntryCommand && !this->DisableCommands)
    {
    this->Script("eval %s",this->EntryCommand);
    }
}

// ---------------------------------------------------------------------------
void vtkKWScale::SetStartCommand(vtkKWObject* Object, 
                                 const char * MethodAndArgString)
{
  if (this->StartCommand)
    {
    delete [] this->StartCommand;
    }

  if (!Object)
    {
    return;
    }

  ostrstream command;
  command << Object->GetTclName() << " " << MethodAndArgString << ends;
  this->StartCommand = command.str();
}

// ---------------------------------------------------------------------------
void vtkKWScale::SetEndCommand(vtkKWObject* Object, 
                               const char * MethodAndArgString)
{
  if (this->EndCommand)
    {
    delete [] this->EndCommand;
    }

  if (!Object)
    {
    return;
    }

  ostrstream command;
  command << Object->GetTclName() << " " << MethodAndArgString << ends;
  this->EndCommand = command.str();
}

// ---------------------------------------------------------------------------
void vtkKWScale::SetEntryCommand(vtkKWObject* Object, 
                                 const char * MethodAndArgString)
{
  if (this->EntryCommand)
    {
    delete [] this->EntryCommand;
    }

  if (!Object)
    {
    return;
    }

  ostrstream command;
  command << Object->GetTclName() << " " << MethodAndArgString << ends;
  this->EntryCommand = command.str();
}

// ---------------------------------------------------------------------------
void vtkKWScale::SetCommand(vtkKWObject* Object, 
                            const char *CommandString)
{
  if (this->Command)
    {
    delete [] this->Command;
    }

  if (!Object)
    {
    return;
    }

  ostrstream command;
  command << Object->GetTclName() << " " << CommandString << ends;
  this->Command = command.str();
}

// ---------------------------------------------------------------------------
void vtkKWScale::SetBalloonHelpString(const char *string)
{
  this->Scale->SetBalloonHelpString(string);

  if (this->Entry)
    {
    this->Entry->SetBalloonHelpString(string);
    }

  if (this->Label)
    {
    this->Label->SetBalloonHelpString(string);
    }

  if (this->PopupScale && this->PopupPushButton)
    {
    ostrstream temp;
    temp << string << " (press this button to display a scale)" << ends;
    this->PopupPushButton->SetBalloonHelpString(temp.str());
    temp.rdbuf()->freeze(0);
    }
}

// ---------------------------------------------------------------------------
void vtkKWScale::SetBalloonHelpJustification(int j)
{
  this->Scale->SetBalloonHelpJustification(j);

  if (this->Entry)
    {
    this->Entry->SetBalloonHelpJustification(j);
    }

  if (this->Label)
    {
    this->Label->SetBalloonHelpJustification(j);
    }

  if (this->PopupScale && this->PopupPushButton)
    {
    this->PopupPushButton->SetBalloonHelpJustification(j);
    }
}

// ---------------------------------------------------------------------------
void vtkKWScale::UpdateEnableState()
{
  this->Superclass::UpdateEnableState();

  if (this->Entry)
    {
    this->Entry->SetEnabled(this->Enabled);
    }

  if (this->Label)
    {
    this->Label->SetEnabled(this->Enabled);
    }

  if (this->Scale)
    {
    this->Scale->SetStateOption(this->Enabled);
    this->Scale->SetEnabled(this->Enabled);
    }

  if (this->PopupPushButton)
    {
    this->PopupPushButton->SetEnabled(this->Enabled);
    }
}

// ---------------------------------------------------------------------------
void vtkKWScale::Resize()
{
  if (!this->SmartResize || 
      !this->IsCreated() || 
      !this->Label || 
      this->PopupScale)
    {
    return;
    }
  
  this->Script("winfo width %s", this->GetWidgetName());
  int width = vtkKWObject::GetIntegerResult(this->GetApplication());
  
  char labelText[100];
  
  if (width < this->ShortWidth)
    {
    this->Label->SetLabel("");
    if (this->RangeMinLabel && this->RangeMinLabel->IsCreated())
      {
      this->Script("%s configure -text {}",
                   this->RangeMinLabel->GetWidgetName());
      }
    if (this->RangeMaxLabel && this->RangeMaxLabel->IsCreated())
      {
      this->Script("%s configure -text {}",
                   this->RangeMaxLabel->GetWidgetName());
      }
    this->Script("%s configure -sliderlength 10",
                 this->Scale->GetWidgetName());
    
    this->PackEntry = 0;
    this->PackRange = 0;
    this->PackWidget();
    }
  else if (width < this->MediumShortWidth)
    {
    this->Script("%s configure -sliderlength 15",
                 this->Scale->GetWidgetName());
    this->PackRange = 0;
    this->PackEntry = 0;
    this->PackWidget();
    }
  else if (width < this->MediumWidth)
    {
    this->Script("%s configure -sliderlength 20",
                 this->Scale->GetWidgetName());
    this->Label->SetLabel(this->ShortLabel);
    this->PackRange = 0;
    this->PackEntry = 1;
    this->PackWidget();
    }
  else if (width < this->LongWidth)
    {
    this->Script("%s configure -sliderlength 25",
                 this->Scale->GetWidgetName());
    this->Label->SetLabel(this->NormalLabel);
    this->PackRange = 0;
    this->PackEntry = 1;
    this->PackWidget();
    }
  else
    {
    this->Script("%s configure -sliderlength 30",
                 this->Scale->GetWidgetName());
    this->Label->SetLabel(this->NormalLabel);
    
    if (this->RangeMinLabel && this->RangeMinLabel->IsCreated())
      {
      sprintf(labelText, "(%g)", this->Range[0]);
      this->Script("%s configure -text %s",
                   this->RangeMinLabel->GetWidgetName(), labelText);
      }
    if (this->RangeMaxLabel && this->RangeMaxLabel->IsCreated())
      {
      sprintf(labelText, "(%g)", this->Range[1]);
      this->Script("%s configure -text %s",
                   this->RangeMaxLabel->GetWidgetName(), labelText);
      }
    this->PackRange = 1;
    this->PackEntry = 1;
    this->PackWidget();
    }
}

// ---------------------------------------------------------------------------
void vtkKWScale::SetDisplayRange(int flag)
{
  if (this->DisplayRange == flag)
    {
    return;
    }
  
  this->DisplayRange = flag;
  char labelText[100];
  
  if ( ! this->RangeMinLabel)
    {
    this->RangeMinLabel = vtkKWLabel::New();
    this->RangeMinLabel->SetParent(this);
    sprintf(labelText, "(%g)", this->Range[0]);
    this->RangeMinLabel->Create(this->GetApplication(), "");
    this->RangeMinLabel->SetLabel(labelText);
    this->RangeMinLabel->SetEnabled(this->Enabled);
    }
  if ( ! this->RangeMaxLabel)
    {
    this->RangeMaxLabel = vtkKWLabel::New();
    this->RangeMaxLabel->SetParent(this);
    sprintf(labelText, "(%g)", this->Range[1]);
    this->RangeMaxLabel->Create(this->GetApplication(), "");
    this->RangeMaxLabel->SetLabel(labelText);
    this->RangeMaxLabel->SetEnabled(this->Enabled);
    }
  this->Modified();
  this->PackWidget();
}

//----------------------------------------------------------------------------
void vtkKWScale::SetLabelWidth(int width)
{
  if (this->Label && this->Label->IsCreated())
    {
    this->Label->SetWidth(width);
    }
}

//----------------------------------------------------------------------------
void vtkKWScale::SetEntryWidth(int width)
{
  if (this->Entry)
    {
    this->Entry->SetWidth(width);
    }
}

//----------------------------------------------------------------------------
void vtkKWScale::SetWidth(int width)
{
  if (this->IsCreated())
    {
    this->Script("%s config -width %d",
                 this->Scale->GetWidgetName(), width);
    }
}

//----------------------------------------------------------------------------
void vtkKWScale::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Value: " << this->Value << endl;
  os << indent << "Resolution: " << this->Resolution << endl;
  os << indent << "Range: " << this->Range[0] << "..." <<  this->Range[1] << endl;
  os << indent << "Label: " << this->Label << endl;
  os << indent << "Entry: " << this->Entry << endl;
  os << indent << "Scale: " << this->Scale << endl;
  os << indent << "PopupPushButton: " << this->PopupPushButton << endl;
  os << indent << "DisplayEntryAndLabelOnTop: " 
     << (this->DisplayEntryAndLabelOnTop ? "On" : "Off") << endl;
  os << indent << "SmartResize: " 
     << (this->SmartResize ? "On" : "Off") << endl;
  os << indent << "PopupScale: " 
     << (this->PopupScale ? "On" : "Off") << endl;
  os << indent << "ExpandEntry: " 
     << (this->ExpandEntry ? "On" : "Off") << endl;
  os << indent << "ShortLabel: " 
     << ( this->ShortLabel ? this->ShortLabel : "(None)" ) << endl;
  os << indent << "DisplayRange: "
     << (this->DisplayRange ? "On" : "Off") << endl;
  os << indent << "DisableCommands: "
     << (this->DisableCommands ? "On" : "Off") << endl;
  os << indent << "DisableScaleValueCallback: "
     << (this->DisableScaleValueCallback ? "On" : "Off") << endl;
  os << indent << "ClampValue: " << (this->ClampValue ? "On" : "Off") << endl;
}

