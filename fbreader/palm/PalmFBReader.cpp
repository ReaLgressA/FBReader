#include <abstract/ZLOptions.h>
#include <palm/ZLPalmFSManager.h>
#include <abstract/ZLInputStream.h>

#include "PalmFBReader.h"
#include "PalmFBReader-resources.h"

static Boolean MyFormHandleEvent(EventPtr event) {
	switch (event->eType) {
  	case menuCmdBarOpenEvent:	
			MenuCmdBarAddButton(menuCmdBarOnRight, OpenBookBitmap, menuCmdBarResultMenuItem, 5000, NULL);
			MenuCmdBarAddButton(menuCmdBarOnRight, AddBookBitmap, menuCmdBarResultMenuItem, 5001, NULL);
			return false;

   	case penDownEvent:
			{
				char txt[4];
				int x = event->screenX;
				txt[0] = '0' + x / 100;
				txt[1] = '0' + x / 10 % 10;
				txt[2] = '0' + x % 10;
				txt[3] = '\0';
				FrmCustomAlert(GoodnightMoonAlert, txt, 0, 0);
			}
			return true;
  	
  	case frmOpenEvent:	
			{
				RGBColorType bg;
				ZLColorOption BackgroundColor("Color", "Background", ZLColor(255, 255, 204));
				ZLColor bgc = BackgroundColor.value();
				bg.r = bgc.Red;
				bg.g = bgc.Green;
				bg.b = bgc.Blue;
				FrmDrawForm(FrmGetActiveForm());
				WinSetCoordinateSystem(kCoordinatesNative);
				RGBColorType red = { 0x00, 0xff, 0x00, 0xff };
				RGBColorType blue = { 0x00, 0x00, 0x00, 0x80 };
				IndexedColorType iRed = WinRGBToIndex(&red);
				IndexedColorType iBg = WinRGBToIndex(&bg);
				IndexedColorType iBlue = WinRGBToIndex(&blue);
				WinSetTextColor(iRed);
				WinSetBackColor(iBg);
				WinSetForeColor(iBlue);
				RectangleType rectangle;
				WinGetWindowFrameRect(WinGetActiveWindow(), &rectangle);
				WinFillRectangle(&rectangle, 0);

				std::string fileName = "/test1.zip:test1";
				ZLInputStream *istream = ZLPalmFSManager::instance().createInputStream(fileName);
				if (istream != 0) {
					if (istream->open()) {
						char txt[10];
						int size = istream->read(txt, 6);
						WinDrawChars(txt, size, 10, 10);
						istream->close();
					}
					delete istream;
				}

				int barLeft = 0;//rectangle.topLeft.x + 1;
				int barRight = 319;//rectangle.extent.x - 3;
				int barBottom = 319;//rectangle.extent.y - 3;
				int barTop = barBottom - 16;
				WinDrawLine(barLeft, barTop, barLeft, barBottom);
				WinDrawLine(barRight, barTop, barRight, barBottom);
				WinDrawLine(barLeft, barTop, barRight, barTop);
				WinDrawLine(barLeft, barBottom, barRight, barBottom);
				WinSetCoordinateSystem(kCoordinatesStandard);
			}
			return true;

		default:
			return false;
	}
}


static Boolean ApplicationHandleEvent(EventPtr event) {
	FormPtr	frm;
	Int		formId;

	if (event->eType == frmLoadEvent) {
		// Load the form resource specified in the event then activate the form.
		formId = event->data.frmLoad.formID;
		frm = FrmInitForm(formId);
		FrmSetActiveForm(frm);

		// Set the event handler for the form.  The handler of the currently 
		// active form is called by FrmDispatchEvent each time it receives an event.
		switch (formId) {
			case MainFBReaderForm:
				FrmSetEventHandler(frm, MyFormHandleEvent);
				break;
		}
		return true;
	}
	
	return false;
}


void EventLoop(void) {
	EventType event;
	Word error;
	
	do {
		EvtGetEvent(&event, evtWaitForever);
		if (!SysHandleEvent(&event) && !MenuHandleEvent(0, &event, &error) && !ApplicationHandleEvent(&event)) {
			FrmDispatchEvent(&event);
		}
	} while (event.eType != appStopEvent);
}
