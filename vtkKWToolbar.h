/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkKWToolbar.h,v $
  Language:  C++
  Date:      $Date: 2002-02-01 14:55:34 $
  Version:   $Revision: 1.5 $

Copyright (c) 2000-2001 Kitware Inc. 469 Clifton Corporate Parkway,
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
// .NAME vtkKWToolbar - a frame that holds tool buttons
// .SECTION Description
// Simply a frame to hold a bunch of tools.  It uses bindings to control
// the height of the frame.
// In the future we could use the object to move toolbars groups around.

#ifndef __vtkKWToolbar_h
#define __vtkKWToolbar_h

#include "vtkKWWidget.h"
class vtkKWApplication;


class VTK_EXPORT vtkKWToolbar : public vtkKWWidget
{
public:
  vtkKWToolbar();
  ~vtkKWToolbar();
  static vtkKWToolbar* New();
  const char *GetClassName() {return "vtkKWToolbar";};

  // Description:
  // Create a Tk widget
  void Create(vtkKWApplication *app);

  // Description:
  // All toolbars should be the same height.
  vtkSetMacro(Height, int);
  vtkGetMacro(Height, int);

  // Description:
  // Callbacks to ensure height stays the same.
  void ScheduleResize();
  void Resize();


protected:

  // Height stuf is not working (ask ken)
  int Height;
  int Expanding;

  vtkKWWidget *Bar1;
  vtkKWWidget *Bar2;

private:
  vtkKWToolbar(const vtkKWToolbar&); // Not implemented
  void operator=(const vtkKWToolbar&); // Not implemented
};


#endif


