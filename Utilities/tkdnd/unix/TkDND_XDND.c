/*
 * TkDND_XDND.h -- Tk XDND Drag'n'Drop Protocol Implementation
 * 
 *    This file implements the unix portion of the drag&drop mechanism
 *    for the tk toolkit. The protocol in use under unix is the
 *    XDND protocol.
 *
 * This software is copyrighted by:
 * Georgios Petasis, Athens, Greece.
 * e-mail: petasisg@yahoo.gr, petasis@iit.demokritos.gr
 *
 * The following terms apply to all files associated
 * with the software unless explicitly disclaimed in individual files.
 *
 * The authors hereby grant permission to use, copy, modify, distribute,
 * and license this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions. No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 * they apply.
 * 
 * IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY
 * FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
 * ARISING OUT OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY
 * DERIVATIVES THEREOF, EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 * 
 * THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE
 * IS PROVIDED ON AN "AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE
 * NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
 * MODIFICATIONS.
 */
#include "vtkTcl.h"
#include "vtkTk.h"
#include "tcl.h"
#include "tk.h"
#include "tkInt.h"
#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>

#define XDND_VERSION 5

#define TkDND_TkWin(x) \
  (Tk_NameToWindow(interp, Tcl_GetString(x), Tk_MainWindow(interp)))

#define TkDND_Eval(objc) \
  for (i=0; i<objc; ++i) Tcl_IncrRefCount(objv[i]);\
  if (Tcl_EvalObjv(interp, objc, objv, TCL_EVAL_GLOBAL) != TCL_OK) \
      Tk_BackgroundError(interp); \
  for (i=0; i<objc; ++i) Tcl_DecrRefCount(objv[i]);

#define TkDND_Status_Eval(objc) \
  for (i=0; i<objc; ++i) Tcl_IncrRefCount(objv[i]);\
  status = Tcl_EvalObjv(interp, objc, objv, TCL_EVAL_GLOBAL);\
  if (status != TCL_OK) Tk_BackgroundError(interp); \
  for (i=0; i<objc; ++i) Tcl_DecrRefCount(objv[i]);

int TkDND_RegisterTypesObjCmd(ClientData clientData, Tcl_Interp *interp,
                              int objc, Tcl_Obj *CONST objv[]) {
  if (objc != 4) {
    Tcl_WrongNumArgs(interp, 1, objv, "path toplevel types-list");
    return TCL_ERROR;
  }

  /*
   * We must make the toplevel that holds this widget XDND aware. This means
   * that we have to set the XdndAware property on our toplevel.
   */
  Window root_return, parent, *children_return = 0;
  unsigned int nchildren_return;
  int status;
  Atom version       = XDND_VERSION;
  Tk_Window path     = TkDND_TkWin(objv[1]);
  Tk_Window toplevel = TkDND_TkWin(objv[2]);
  if (Tk_Parent(toplevel) != NULL) {
    return TCL_ERROR;
  }
  Tk_MakeWindowExist(toplevel);
  Tk_MapWindow(toplevel);
  status = XQueryTree(Tk_Display(toplevel), Tk_WindowId(toplevel),
                      &root_return, &parent,
                      &children_return, &nchildren_return);
  if (children_return) XFree(children_return);
  XChangeProperty(Tk_Display(toplevel), parent,
                  Tk_InternAtom(toplevel, "XdndAware"),
                  XA_ATOM, 32, PropModeReplace,
                  (unsigned char *) &version, 1);
  return TCL_OK;
}; /* TkDND_RegisterTypesObjCmd */

int TkDND_HandleXdndEnter(Tk_Window tkwin, XClientMessageEvent cm) {
  TkWindow *winPtr = (TkWindow *) tkwin;
  Tcl_Interp *interp = NULL;
  Atom *typelist;
  const long *l = cm.data.l;
  int i, version = (int)(((unsigned long)(l[1])) >> 24);
  if (winPtr->mainPtr != NULL) interp = winPtr->mainPtr->interp;
  if (interp == NULL) return False; 
  Window drag_source = l[0];
  if (version > XDND_VERSION) return False;
  if (l[1] & 0x1UL) {
    /* Get the types from XdndTypeList property. */
    Atom actualType = None;
    int actualFormat;
    unsigned long itemCount, remainingBytes;
    Atom *data;
    XGetWindowProperty(cm.display, drag_source,
                       Tk_InternAtom(tkwin, "XdndTypeList"), 0,
                       LONG_MAX, False, XA_ATOM, &actualType, &actualFormat,
                       &itemCount, &remainingBytes, (unsigned char **) &data);
    typelist = (Atom *) Tcl_Alloc(sizeof(Atom)*(itemCount+1));
    if (typelist == NULL) return False;
    for (i=0; i<itemCount; i++) { typelist[i] = data[i]; }
    typelist[itemCount] = None;
    if (data) XFree((unsigned char*)data);
  } else {
    typelist = (Atom *) Tcl_Alloc(sizeof(Atom)*4);
    if (typelist == NULL) return False;
    typelist[0] = cm.data.l[2];
    typelist[1] = cm.data.l[3];
    typelist[2] = cm.data.l[4];
    typelist[3] = None;
  }
  /* We have all the information we need. Its time to pass it at the Tcl
   * level.*/
  Tcl_Obj* objv[4], *element;
  objv[0] = Tcl_NewStringObj("tkdnd::xdnd::_HandleXdndEnter", -1);
  objv[1] = Tcl_NewStringObj(Tk_PathName(tkwin), -1);
  objv[2] = Tcl_NewLongObj(drag_source);
  objv[3] = Tcl_NewListObj(0, NULL);
  for (i=0; typelist[i] != None; ++i) {
    element = Tcl_NewStringObj(Tk_GetAtomName(tkwin, typelist[i]), -1);
    Tcl_ListObjAppendElement(NULL, objv[3], element);
  }
  TkDND_Eval(4);
  Tcl_Free((char *) typelist);
}; /* TkDND_HandleXdndEnter */

int TkDND_HandleXdndPosition(Tk_Window tkwin, XClientMessageEvent cm) {
  TkWindow *winPtr = (TkWindow *) tkwin;
  Tcl_Interp *interp = NULL;
  Tk_Window mouse_tkwin;
  Tcl_Obj* result;
  Tcl_Obj* objv[4];
  const unsigned long *l = (const unsigned long *) cm.data.l;
  int rootX, rootY, i, index, status;
  XClientMessageEvent response;
  int width = 1, height = 1;
  static char *DropActions[] = {
    "copy", "move", "link", "ask",  "private", "refuse_drop", "default",
    (char *) NULL
  };
  enum dropactions {
    ActionCopy, ActionMove, ActionLink, ActionAsk, ActionPrivate,
    refuse_drop, ActionDefault
  };

  if (winPtr->mainPtr != NULL) interp = winPtr->mainPtr->interp;
  if (interp == NULL) return False;

  rootX = (l[2] & 0xffff0000) >> 16;
  rootY =  l[2] & 0x0000ffff;
  mouse_tkwin = Tk_CoordsToWindow(rootX, rootY, tkwin);
  if (mouse_tkwin == NULL) {
    /* We received the client message, but we cannot find a window? Strange...*/
    /* A last attemp: execute wm containing x, y */
    objv[0] = Tcl_NewStringObj("update", -1);
    TkDND_Eval(1);
    objv[0] = Tcl_NewStringObj("winfo", -1);
    objv[1] = Tcl_NewStringObj("containing", -1);
    objv[2] = Tcl_NewIntObj(rootX);
    objv[3] = Tcl_NewIntObj(rootY);
    TkDND_Status_Eval(4);
    if (status == TCL_OK) {
      result = Tcl_GetObjResult(interp); Tcl_IncrRefCount(result);
      mouse_tkwin = Tk_NameToWindow(interp, Tcl_GetString(result),
                                    Tk_MainWindow(interp));
      Tcl_DecrRefCount(result);
    }
  }
  /* Get the drag source. */
  objv[0] = Tcl_NewStringObj("tkdnd::xdnd::_GetDragSource", -1);
  TkDND_Status_Eval(1); if (status != TCL_OK) return False;
  if (Tcl_GetLongFromObj(interp, Tcl_GetObjResult(interp), &response.window) !=
                  TCL_OK) return False;
  /* Now that we have found the containing widget, ask it whether it will accept
   * the drop... */
  index = refuse_drop;
  if (mouse_tkwin != NULL) {
    objv[0] = Tcl_NewStringObj("tkdnd::xdnd::_HandleXdndPosition", -1);
    objv[1] = Tcl_NewStringObj(Tk_PathName(mouse_tkwin), -1);
    objv[2] = Tcl_NewIntObj(rootX);
    objv[3] = Tcl_NewIntObj(rootY);
    TkDND_Status_Eval(4);
    if (status == TCL_OK) {
      /* Get the returned action... */
      result = Tcl_GetObjResult(interp); Tcl_IncrRefCount(result);
      status = Tcl_GetIndexFromObj(interp, result, (const char **) DropActions,
                              "dropactions", 0, &index);
      Tcl_DecrRefCount(result);
      if (status != TCL_OK) index = refuse_drop;
    }
  }
  /* Sent */
  response.type         = ClientMessage;
  response.format       = 32;
  response.message_type = Tk_InternAtom(tkwin, "XdndStatus");
  response.data.l[0]    = (mouse_tkwin!=NULL) ? Tk_WindowId(mouse_tkwin) : 0;
  response.data.l[1]    = 1; // yes
  response.data.l[2]    = ((rootX) << 16) | ((rootY)  & 0xFFFFUL); // x, y
  response.data.l[3]    = ((width) << 16) | ((height) & 0xFFFFUL); // w, h
  response.data.l[4]    = 0; // action
  switch ((enum dropactions) index) {
    case ActionDefault:
    case ActionCopy:
      response.data.l[4] = Tk_InternAtom(tkwin, "XdndActionCopy");    break;
    case ActionMove:
      response.data.l[4] = Tk_InternAtom(tkwin, "XdndActionMove");    break;
    case ActionLink:
      response.data.l[4] = Tk_InternAtom(tkwin, "XdndActionLink");    break;
    case ActionAsk:
      response.data.l[4] = Tk_InternAtom(tkwin, "XdndActionAsk");     break;
    case ActionPrivate: 
      response.data.l[4] = Tk_InternAtom(tkwin, "XdndActionPrivate"); break;
    case refuse_drop: {
      response.data.l[1] = 0; /* Refuse drop. */
    }
  }
  XSendEvent(cm.display, response.window, False, NoEventMask,
             (XEvent*)&response);
}; /* TkDND_HandleXdndPosition */

int TkDND_HandleXdndLeave(Tk_Window tkwin, XClientMessageEvent cm) {
  TkWindow *winPtr = (TkWindow *) tkwin;
  Tcl_Interp *interp = NULL;
  if (winPtr->mainPtr != NULL) interp = winPtr->mainPtr->interp;
  if (interp == NULL) return False; 
  Tcl_Obj* objv[1];
  int i;
  objv[0] = Tcl_NewStringObj("tkdnd::xdnd::_HandleXdndLeave", -1);
  TkDND_Eval(1);
}; /* TkDND_HandleXdndLeave */

int TkDND_HandleXdndDrop(Tk_Window tkwin, XClientMessageEvent cm) {
  XClientMessageEvent finished;
  TkWindow *winPtr = (TkWindow *) tkwin;
  Tcl_Interp *interp = NULL;
  Tcl_Obj* objv[2], *result;
  int status, i, index;
  Time time = cm.data.l[2];
  static char *DropActions[] = {
    "copy", "move", "link", "ask",  "private", "refuse_drop", "default",
    (char *) NULL
  };
  enum dropactions {
    ActionCopy, ActionMove, ActionLink, ActionAsk, ActionPrivate,
    refuse_drop, ActionDefault
  };
    
  if (winPtr->mainPtr != NULL) interp = winPtr->mainPtr->interp;
  if (interp == NULL) return False;

  /* Get the drag source. */
  objv[0] = Tcl_NewStringObj("tkdnd::xdnd::_GetDragSource", -1);
  TkDND_Status_Eval(1); if (status != TCL_OK) return False;
  if (Tcl_GetLongFromObj(interp,
          Tcl_GetObjResult(interp), &finished.window) != TCL_OK) return False;

  /* Get the drop target. */
  objv[0] = Tcl_NewStringObj("tkdnd::xdnd::_GetDropTarget", -1);
  TkDND_Status_Eval(1);
  if (Tcl_GetLongFromObj(interp,
         Tcl_GetObjResult(interp), &finished.data.l[0]) != TCL_OK) {
    finished.data.l[0] = None;
  }

  /* Call out Tcl callback. */
  objv[0] = Tcl_NewStringObj("tkdnd::xdnd::_HandleXdndDrop", -1);
  objv[1] = Tcl_NewLongObj(time);
  TkDND_Status_Eval(2);
  finished.data.l[1] = 1; // Accept drop.
  if (status == TCL_OK) {
    /* Get the returned action... */
    result = Tcl_GetObjResult(interp); Tcl_IncrRefCount(result);
    status = Tcl_GetIndexFromObj(interp, result, (const char **) DropActions,
                            "dropactions", 0, &index);
    Tcl_DecrRefCount(result);
    if (status != TCL_OK) index = refuse_drop;
    switch ((enum dropactions) index) {
      case ActionDefault:
      case ActionCopy:
        finished.data.l[2] = Tk_InternAtom(tkwin, "XdndActionCopy");    break;
      case ActionMove:
        finished.data.l[2] = Tk_InternAtom(tkwin, "XdndActionMove");    break;
      case ActionLink:
        finished.data.l[2] = Tk_InternAtom(tkwin, "XdndActionLink");    break;
      case ActionAsk:
        finished.data.l[2] = Tk_InternAtom(tkwin, "XdndActionAsk");     break;
      case ActionPrivate: 
        finished.data.l[2] = Tk_InternAtom(tkwin, "XdndActionPrivate"); break;
      case refuse_drop: {
        finished.data.l[1] = 0; /* Drop canceled. */
      }
    }
  } else {
    finished.data.l[1] = 0;
  }
  /* Send XdndFinished. */
  finished.type         = ClientMessage;
  finished.format       = 32;
  finished.message_type = Tk_InternAtom(tkwin, "XdndFinished");
  XSendEvent(cm.display, finished.window, False, NoEventMask,
             (XEvent*)&finished);
}; /* TkDND_HandleXdndDrop */

int TkDND_HandleXdndStatus(Tk_Window tkwin, XClientMessageEvent cm) {
}; /* TkDND_HandleXdndStatus */

int TkDND_HandleXdndFinished(Tk_Window tkwin, XClientMessageEvent cm) {
}; /* TkDND_HandleXdndFinished */

static int TkDND_XDNDHandler(Tk_Window tkwin, XEvent *xevent) {
  if (xevent->type != ClientMessage) return False;
  XClientMessageEvent clientMessage = xevent->xclient;

  if (clientMessage.message_type ==
             XInternAtom(clientMessage.display, "XdndEnter", False)) {
#ifdef DEBUG_CLIENTMESSAGE_HANDLER
    printf("XDND_HandleClientMessage: Received XdndEnter\n");
#endif /* DEBUG_CLIENTMESSAGE_HANDLER */
    return TkDND_HandleXdndEnter(tkwin, clientMessage);
  } else if (clientMessage.message_type ==
             XInternAtom(clientMessage.display, "XdndPosition", False)) {
#ifdef DEBUG_CLIENTMESSAGE_HANDLER
    printf("XDND_HandleClientMessage: Received XdndPosition\n");
#endif /* DEBUG_CLIENTMESSAGE_HANDLER */
    return TkDND_HandleXdndPosition(tkwin, clientMessage);
  } else if (clientMessage.message_type ==
             XInternAtom(clientMessage.display, "XdndLeave", False)) {
#ifdef DEBUG_CLIENTMESSAGE_HANDLER
    printf("XDND_HandleClientMessage: Received XdndLeave\n");
#endif /* DEBUG_CLIENTMESSAGE_HANDLER */
    return TkDND_HandleXdndLeave(tkwin, clientMessage);
  } else if (clientMessage.message_type ==
             XInternAtom(clientMessage.display, "XdndDrop", False)) {
#ifdef DEBUG_CLIENTMESSAGE_HANDLER
    printf("XDND_HandleClientMessage: Received XdndDrop\n");
#endif /* DEBUG_CLIENTMESSAGE_HANDLER */
    return TkDND_HandleXdndDrop(tkwin, clientMessage);
  } else if (clientMessage.message_type ==
             XInternAtom(clientMessage.display, "XdndStatus", False)) {
#ifdef DEBUG_CLIENTMESSAGE_HANDLER
    printf("XDND_HandleClientMessage: Received XdndStatus\n");
#endif /* DEBUG_CLIENTMESSAGE_HANDLER */
    return TkDND_HandleXdndStatus(tkwin, clientMessage);
  } else if (clientMessage.message_type ==
             XInternAtom(clientMessage.display, "XdndFinished", False)) {
#ifdef DEBUG_CLIENTMESSAGE_HANDLER
    printf("XDND_HandleClientMessage: Received XdndFinished\n");
#endif /* DEBUG_CLIENTMESSAGE_HANDLER */
    return TkDND_HandleXdndFinished(tkwin, clientMessage);
  } else {
#ifdef TKDND_ENABLE_MOTIF_DROPS
    if (MotifDND_HandleClientMessage(dnd, *xevent)) return True;
#endif /* TKDND_ENABLE_MOTIF_DROPS */
  }
  return False;
} /* TkDND_XDNDHandler */

/*
 * For C++ compilers, use extern "C"
 */
#ifdef __cplusplus
extern "C" {
#endif
DLLEXPORT int Tkdnd_Init(Tcl_Interp *interp);
DLLEXPORT int Tkdnd_SafeInit(Tcl_Interp *interp);
#ifdef __cplusplus
}
#endif

int DLLEXPORT Tkdnd_Init(Tcl_Interp *interp) {
  int major, minor, patchlevel;

  if (
#ifdef USE_TCL_STUBS 
      Tcl_InitStubs(interp, "8.3", 0)
#else
      Tcl_PkgRequire(interp, "Tcl", "8.3", 0)
#endif /* USE_TCL_STUBS */
            == NULL) {
            return TCL_ERROR;
  }
  if (
#ifdef USE_TK_STUBS
       Tk_InitStubs(interp, "8.3", 0)
#else
       Tcl_PkgRequire(interp, "Tk", "8.3", 0)
#endif /* USE_TK_STUBS */
            == NULL) {
            return TCL_ERROR;
  }

  /*
   * Get the version, because we really need 8.3.3+.
   */
  Tcl_GetVersion(&major, &minor, &patchlevel, NULL);
  if ((major == 8) && (minor == 3) && (patchlevel < 3)) {
    Tcl_SetResult(interp, "tkdnd requires Tk 8.3.3 or greater", TCL_STATIC);
    return TCL_ERROR;
  }


  /* Register the various commands */
  if (Tcl_CreateObjCommand(interp, "_register_types",
           (Tcl_ObjCmdProc*) TkDND_RegisterTypesObjCmd,
           (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL) == NULL) {
      return TCL_ERROR;
  }

  /* Finally, register the XDND Handler... */
  Tk_CreateClientMessageHandler(&TkDND_XDNDHandler);

  Tcl_PkgProvide(interp, "tkdnd", "2.0");
  return TCL_OK;
} /* Tkdnd_Init */

int DLLEXPORT Tkdnd_SafeInit(Tcl_Interp *interp) {
  return Tkdnd_Init(interp);
} /* Tkdnd_SafeInit */