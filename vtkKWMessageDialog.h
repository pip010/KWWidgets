/*=========================================================================

Copyright (c) 1998-2003 Kitware Inc. 469 Clifton Corporate Parkway,
Clifton Park, NY, 12065, USA.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * Neither the name of Kitware nor the names of any contributors may be used
   to endorse or promote products derived from this software without specific
   prior written permission.

 * Modified source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
// .NAME vtkKWMessageDialog - a message dialog superclass
// .SECTION Description
// A generic superclass for MessageDialog boxes.

#ifndef __vtkKWMessageDialog_h
#define __vtkKWMessageDialog_h

#include "vtkKWDialog.h"

class vtkKWApplication;
class vtkKWCheckButton;
class vtkKWLabel;

class VTK_EXPORT vtkKWMessageDialog : public vtkKWDialog
{
public:
  static vtkKWMessageDialog* New();
  vtkTypeRevisionMacro(vtkKWMessageDialog,vtkKWDialog);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Create a Tk widget
  virtual void Create(vtkKWApplication *app, const char *args);

  // Description:
  // Set the text of the message (and the width of a line, in pixels)
  virtual void SetText(const char *);
  virtual void SetTextWidth(int);
  virtual int GetTextWidth();

  // Description:
  // Invoke the dialog and display it in a modal manner. 
  // This method returns a zero if the dilaog was killed or 
  // canceled, nonzero otherwise.
  virtual int Invoke();

  // Description:
  // Set the style of the message box
  //BTX
  enum 
  {
    Message = 0,
    YesNo,
    OkCancel,
    OkOtherCancel
  };
  //ETX
  vtkSetMacro(Style,int);
  vtkGetMacro(Style,int);
  void SetStyleToMessage() {this->SetStyle(vtkKWMessageDialog::Message);};
  void SetStyleToYesNo() {this->SetStyle(vtkKWMessageDialog::YesNo);};
  void SetStyleToOkCancel() {this->SetStyle(vtkKWMessageDialog::OkCancel);};
  void SetStyleToOkOtherCancel() 
    {this->SetStyle(vtkKWMessageDialog::OkOtherCancel);};

  // Description:
  // Set or get the message dialog name
  vtkSetStringMacro(DialogName);
  vtkGetStringMacro(DialogName);

  // Description:
  // Set different options for the dialog.
  //BTX
  enum 
  {
    RememberYes     = 0x00002,
    RememberNo      = 0x00004,
    ErrorIcon       = 0x00008,
    WarningIcon     = 0x00010,
    QuestionIcon    = 0x00020,
    YesDefault      = 0x00040,
    NoDefault       = 0x00080,
    OkDefault       = 0x00100,
    CancelDefault   = 0x00200,
    Beep            = 0x00400,
    PackVertically  = 0x00800,
    InvokeAtPointer = 0x01000
  };
  //ETX
  vtkSetMacro(Options, int);
  vtkGetMacro(Options, int);

  // Description:
  // The label displayed on the OK button. Only used when
  // the style is OkCancel.
  vtkSetStringMacro(OKButtonText);
  vtkGetStringMacro(OKButtonText);

  // Description:
  // The label displayed on the cancel button. Only used when
  // the style is OkCancel.
  vtkSetStringMacro(CancelButtonText);
  vtkGetStringMacro(CancelButtonText);

  // Description:
  // The label displayed on the other button. Only used when
  // the style is OkOtherCancel.
  vtkSetStringMacro(OtherButtonText);
  vtkGetStringMacro(OtherButtonText);

  // Description:
  // Utility methods to create various dialog windows.
  // icon is a enumerated icon type described in vtkKWIcon.
  // title is a title string of the dialog. name is the dialog name
  // used for the registery. message is the text message displayed
  // in the dialog.
  static void PopupMessage(vtkKWApplication *app, vtkKWWindow *masterWin,
                           const char* title, 
                           const char* message, int options = 0);
  static int PopupYesNo(vtkKWApplication *app,  vtkKWWindow *masterWin,
                        const char* title, 
                        const char* message, int options = 0);
  static int PopupYesNo(vtkKWApplication *app,  vtkKWWindow *masterWin, 
                        const char* name, 
                        const char* title, const char* message, 
                        int options = 0);
  static int PopupOkCancel(vtkKWApplication *app, vtkKWWindow *masterWin,
                           const char* title, 
                           const char* message, int options = 0);

  // Description:
  // Retrieve the frame where the message is.
  vtkGetObjectMacro(TopFrame, vtkKWFrame);
  vtkGetObjectMacro(MessageDialogFrame, vtkKWFrame);
  vtkGetObjectMacro(BottomFrame, vtkKWFrame);

  // Description:
  // Set the icon on the message dialog.
  virtual void SetIcon();

  // Description:
  // Accessor for OK and cancel button
  vtkGetObjectMacro(OKButton, vtkKWWidget);
  vtkGetObjectMacro(CancelButton, vtkKWWidget);
  vtkGetObjectMacro(OtherButton, vtkKWWidget);

  // Description::
  // Close this Dialog (for the third button)
  virtual void Other();

  // Description:
  // Convenience method to guess the width/height of the dialog.
  virtual int GetWidth();
  virtual int GetHeight();

protected:
  vtkKWMessageDialog();
  ~vtkKWMessageDialog();

  int             Style;
  int             Default;
  int             Options;
  char            *DialogName;
  char            *DialogText;

  vtkSetStringMacro(DialogText);
  vtkGetStringMacro(DialogText);

  vtkKWFrame       *TopFrame;
  vtkKWFrame       *MessageDialogFrame;
  vtkKWFrame       *BottomFrame;
  vtkKWLabel       *Label;
  vtkKWFrame       *ButtonFrame;
  vtkKWWidget      *OKButton;
  vtkKWWidget      *CancelButton;  
  vtkKWWidget      *OtherButton;  
  vtkKWLabel       *Icon;
  vtkKWFrame       *OKFrame;
  vtkKWFrame       *CancelFrame;
  vtkKWFrame       *OtherFrame;
  vtkKWCheckButton *CheckButton;

  // Description:
  // Get the value of the check box for remembering the answer from
  // the user.
  int GetRememberMessage();
  
  char* OKButtonText;
  char* CancelButtonText;
  char* OtherButtonText;

private:
  vtkKWMessageDialog(const vtkKWMessageDialog&); // Not implemented
  void operator=(const vtkKWMessageDialog&); // Not implemented
};


#endif



