/*
 *
 * This file is part of XForms.
 *
 * XForms is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1, or
 * (at your option) any later version.
 *
 * XForms is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with XForms; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 *
 */


/*
 * $Id: fd_help.c,v 1.0 2002/05/08 05:27:39 zhao Release $
 *
 *.
 *  This file is part of XForms package
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 * This file is part of the Form Designer.
 *
 * It contains the routines for the help feature of the designer.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "forms.h"
#include "fd_main.h"
#include "ui_theforms.h"

/* Loads the help browser with specific information. */

static void
load_it(char str[][80])
{
    int i;

    fl_freeze_form(fd_help->helpform);
    fl_clear_browser(fd_help->browser);
    for (i = 0; str[i][0] != '\0'; i++)
	fl_add_browser_line(fd_help->browser, str[i]);
    fl_unfreeze_form(fd_help->helpform);
}

 /* Shows the help form. */
void
help_cb(FL_OBJECT * obj, long arg)
{
    fl_show_form(fd_help->helpform,
		 FL_PLACE_GEOMETRY, FL_FULLBORDER, "fdesign help");
}

 /* Stop showing the help window */
void
exithelp_cb(FL_OBJECT * obj, long arg)
{
    fl_hide_form(fd_help->helpform);
}

extern char testhelp[][80], mainhelp[][80], generalhelp[][80];
extern char keyhelp[][80], grouphelp[][80], savehelp[][80];
extern char mousehelp[][80];

/* Shows a particular help item. */
void
showhelp_cb(FL_OBJECT * obj, long arg)
{
    switch (arg)
    {
    case HELP_VERSION:
	load_it(mainhelp);
	break;
    case HELP_GENERAL:
	load_it(generalhelp);
	break;
    case HELP_MOUSE:
	load_it(mousehelp);
	break;
    case HELP_KEYBOARD:
	load_it(keyhelp);
	break;
    case HELP_GROUP:
	load_it(grouphelp);
	break;
    case HELP_TEST:
	load_it(testhelp);
	break;
    case HELP_SAVE:
	load_it(savehelp);
	break;
    }
}

#define S(a)  #a
#define LIBVERSION(a,b) " (LibraryVersion " S(a) "." S(b) ")"

char mainhelp[][80] =
{
    "  ",
    "@C4@M@b@cForm Designer",
    "  ",
    "@C4@b@c$Revision: 1.0 $" LIBVERSION(FL_VERSION, FL_REVISION),
    "@C4@c@bWritten by T.C. Zhao & Mark Overmars",
    "@-",
    " ",
    " The Form Designer is a program for interactively designing dialogue",
    " forms for use with the Forms Library. The prgram is very simple",
" to use. Press the different buttons to get help on the different topics.",
    " ",
    "For more info on fdesign and xforms, see the complete documentation or visit",
    " the xforms home page",
    " ",
    "@f@b@chttp://bragg.phys.uwm.edu/xforms         ",
    ""
};

char generalhelp[][80] =
{
    "  ",
    "@l@b@cGeneral Information",
    "  ",
    "@bAdd a form",
 "To add a form, press the button labeled 'New Form'. You will be prompted",
    "for a name.  After the form is created, you can start adding various objects",
    "such as buttons etc., to this form.",
    " ",
    "@bAdding Objects",
 "To add an object to the newly created form, indicate what kind of object",
"to add by clicking on that object under Objects in the control panel. Next",
 "drag your left mouse on the form, a rubber band will appear. The size of",
  "the rubber band is the size of the object.  Once a satifactory size and",
  "location is found, release left mouse. Multiple objects can be added in",
"a similar fashion. After objects are added, you can move, resize and clone",
    "them as needed. See Mouse section",
    " ",
    "@bChanging the form size",
    "It is possible to change the size of a form later. To this end select",
 "the bottom face, using the right mouse button. Next grab the lower-right",
    "corner, scale it to its new size. Changing the window size via window",
    "manager's frame will also work.",
    " ",
    "@bDeleting a form",
    "Use the button labeled 'Delete Form' to delete the current form. You",
    "will have to acknowledge the deletion.",
    " ",

    "@bChanging the name of the form",
    "This can be achieved by pressing the button labeled 'Form Name' below",
    "the list of forms.",
    " ",

    "@bAligning objects",
 "When pressing this button a form appears at the top right of the screen.",
  "Now make a selection of the objects and press one of the buttons in the",
    "form to align them at the left, horizontal centers, right, etc. It is",
 "also possible to put them with equal gaps between them. In this form you",
    "can also indicating the snapping value which indicates the number of",
    "pixels to which values are rounded when putting objects in your form.",
    " ",
    "@bConfiguration",
 "The following resources can be used to select where to place the various",
    "forms that control the operation of fdesign",
    " ",
    "@fformDesgin.geometry",
    "This resource controls the placement and size of the working area",
    " ",
    "@fcontrol.geometry",
    "This resource controls the placement of the control panel",
    " ",
    "@fattributes.geometry",
    "This resource controls the placement of Attributes panel, activated by",
    "double-clicking on the object or by pressing F1",
    " ",
    "@fattributes.background",
    "This resource controls the background color of the Attributes panel",
    " ",
    "@falign.geometry",
    "This resource controls the placement of the align dialog",
    " ",
    "@fxformsHeader",
    "This resource informs the fdesign to emit a header other than forms.h",
    " ",
    "@fcompensate",
"This resource (bool) informs the fdesign to emit a font size compensation",
    "code",
    " ",
    "The most useful xforms resource are",
    "@ffdesign.XForm*FontSize",
    "and",
    "@ffdesign.XForm*PupFontSize",
    " ",
    ""
};

char mousehelp[][80] =
{
    "  ",
    "@l@b@cUsing the Mouse",
    "  ",
    "The mouse is used to add objects, select objects and move them around",
    "or scale them. Sometimes, holding down the <Shift> key influences the",
    "action taken.",
    " ",
    "@bLeft Mouse Button",
    "When adding object is indicated in the Object Browser, dragging the",
"left mouse button is used to add an object to a form. In normal operation",
    "i.e., not adding objects, left mouse is used to select, move or scale",
  "objects. When you click on an object, the object is selected, indicated",
  "by a red outline with 4 small knobs on the corners. To select more than",
"one object, draw a box by dragging the mouse. All objects that fall within",
    "the box are selected. To move objects, select them first then drag the",
    "selected object. To scale an object, drag the scale knobs on the red",
    "selection outline. To delete an object, double click on the object.",
    " ",
"When the <shift> key is down, selecting an object adds the newly selected",
  "object to the existing selection, and moving an object causes a copy of",
  "the object made (cloned) and moved. The cloned object will have exactly",
    "the same attributes as the original object except for object name and shortcut",

    " ",
    "@bRight Mouse Button",
    "The right mouse button works the same way as left mouse button, except",
    "for adding and deleting objects. Double click an object to change the",
    "attributes of the object.",
    ""
};

char keyhelp[][80] =
{
    " ",
    "@l@b@cCursor Keys",
    " ",
    "The arrow keys can be used to move the current selection. If <shift>",
    "is down, the selection will be scaled instead. To adjust the number",
    "of pixels to move (or increment), 0-9 can be used with 0 meaning",
    "10 pixels.",

    "  ",
    "@l@b@cFunction Keys",
    "  ",
    "The function keys are used to perform certain actions on the current",
    "selection. You can either use the function keys on the keyboard or",
    "the corresponding buttons on the screen.",
    " ",
    "@bF1  Attributes",
    "Change the attributes of the current selection. This only works if the",
 "objects in the selection are all of the same class. When only one object",
    "is selected, also label and name can be changed.",
    " ",
    "@bF2  Lower selection",
 "Lowers the objects in the current selection, i.e., places them below all",
    "other objects on the form.",
    " ",
    "@bF3  Raise selection",
"Raises all objects in the selection, i.e., places them on top of all other",
    "objects in the form.",
    " ",
    "@bF4  Select all",
    "Puts all objects in a selection.",
    " ",
    "@bF5  Show selection",
    "Makes the all objects in the current selection visible. You can first",
    "use F4 to select all objects.",
    " ",
    "@bF6  Hide selection",
    "Hides all objects in the current selection. Use F5 to make them",
    "visible again.",
    " ",
    "@bF7  Group selection",
    "Places all objects in the selection in a group. If the objects are",
    "already part of a group, they are removed from the old group. Groups",
 "that become empty this way are discarded. You will be asked to provide a",
    "name for the group. This will be the C-name under which the group is",
    "known to the application program.",
    " ",
    "@bF8  Ungroup selection",
    "Takes all objects in the current selection out of their groups. Empty",
    "groups will be discarded.",
    " ",
    "@bF9  Copy selection to buffer",
    "Copies the objects in the current selection to the buffer. They can",
    "later be added to the same or another form using the operation paste.",
    " ",
    "@bF10 Paste buffer into form",
    "Pastes the objects in the buffer (placed there by the last copy or cut",
    "operation) into the form. You can indicate the required position. The",
    "pasted objects will form the new selection.",
    " ",
    "@bF11",
"Selects next object and wraps around if the current selection is the last",
    "object. <TAB> has the same functionality",
    " ",
    "@bF12 Cut selection into the buffer",
    "Deletes the current selection. The deleted objects will be placed in",
 "the buffer and can be pasted back into the form (or another form) later.",
    " ",
    "@l@b@c Other Keys",
    " ",
    "@btstart/stop Testing",
 "Pressing t in the main window is the same as clicking on the Test button",
    "If the form is already being tested, pressing t stops testing",
    " ",
    "@bs Save the forms",
  "Depending on the options selected, pressing s will save the current set",
    "of forms to disk.",
    " ",
    "@bEsc Exit",
    "Quit form designer",
    ""
};

char grouphelp[][80] =
{
    "  ",
    "@l@b@cGroups",
    "  ",
  "The Form Designer has a simple mechanism for dealing with groups inside",
 "forms. To create a group of objects, select all of them, using the right",
    "mouse button, and press key <F7>. The group name you indicate is added",
    "to the list of groups. Whenever you select all elements in a group the",
 "group name is shown in red. You can also select or deselect all elements",
    "in a group by pressing the mouse on the group name. Groups whose",
    "elements get deleted automatically disappear.",
    " ",
    "@bAdding an object to a group",
    "It is not possible to directly add an object to an existing group. You",
  "should first select the group and the new object and next press <F7> to",
    "create a new group. The old group will be removed and a new group,",
  "including the new object, will be created. You will have to type in the",
    "group name again.",
    " ",
    "@bChanging the name of a group",
    "To change the name of a group, select it (and deselect the others) and",
    "press the button labeled 'Group Name' below the list of groups.",
    ""};

char savehelp[][80] =
{
    "  ",
    "@l@b@cSaving and Loading",
    "  ",
 "To save the current set of forms, press the button labeled 'Save Forms'.",
    "The file selector will appear in which you can indicate the name under",
  "which to save the form. This name should end with .fd. Three files will",
    "be generated: <name>.c, <name>.h and <name>.fd. See the",
    "documentation for their purpose.",
    " ",
  "A new set of forms can be loaded using the button labeled 'Load Forms'.",
    "It is also possible to merge the new forms with the current set using",
    "the button labeled 'Merge Forms'.",
    "  ",
    ""};

char testhelp[][80] =
{
    "  ",
    "@l@b@cTesting Forms",
    "  ",
  "To test the current form press the button labeled 'Test Form'. The form",
    "will be centered on the screen and in the top right corner a new form",
  "will appear. Now you can play with the form. In the browser the objects",
"are shown that will be returned to the application program or the callback",
    "routines that will be called. In this way you can verify whether each",
    "object has got a name or callback routine associated with it. To stop",
    "testing press the button labeled 'Stop Testing'.",
    " ",
"Note that changes you make to the objects, e.g. filling in an input field,",
    "will not be saved when saving the forms.",
    ""
};
