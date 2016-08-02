#include "myevic.h"
#include "display.h"
#include "screens.h"
#include "menus.h"
#include "miscs.h"
#include "myrtc.h"
#include "dataflash.h"
#include "atomizer.h"
#include "flappy.h"

#include "events.h"

//=============================================================================
//----- (00003738) ------------------------------------------------------------
__myevic__ void KeyRepeat()
{
	if ( !PE0 )
		return;

	if ( dfStatus & 2 && !EditModeTimer )
	{
		if (( Screen != 59 ) && ( Screen != 101 ))
			return;
	}

	if ( dfStatus << 31 )
	{
		if ( Screen != 59 )
			return;
	}
	else if ( EditModeTimer )
	{
		if ( EditItemIndex != 2 )
			return;

		if ( ( dfTCPower == MaxTCPower && !PD2 ) || ( dfTCPower == 10 && !PD3 ) )
		{
			Flags64 |= 0x40000;
			return;
		}
	}
	else if ( ISMODETC(dfMode) )
	{
		if ( dfIsCelsius == 1 )
		{
			if (( dfTemp == 315 && !PD2 ) || ( dfTemp == 100 && !PD3 ))
				return;
		}
		else if ( dfIsCelsius == 0 )
		{
			if (( dfTemp == 600 && !PD2 ) || ( dfTemp == 200 && !PD3 ))
				return;
		}
	}

	if ( PD2 == PD3 )
	{
		if ( KeyUpTimer )
		{
			if ( !--KeyUpTimer && Screen == 1 && ( dfTCPower >= 1000 || dfMode == 6 ) )
				MainView();
		}
		word_20000054 = 0;
		byte_20000080 = 0;
	}
	else if ( byte_20000080 >= 3 || ++byte_20000080 >= 3u )
	{
		if ( word_20000054 >= 105u )
		{
			byte_20000080 = 0;
		}
		else
		{
			++word_20000054;
		}

		if ( PD2 )
		{
			if ( PD3 )
				return;
			Event = 3;
		}
		else
		{
			Event = 2;
		}
	}
}


//=============================================================================
//----- (00004B34) ------------------------------------------------------------
__myevic__ void GetUserInput()
{
	UserInputs = 14;

	if ( !PE0 && PD2 && PD3 )
	{
		UserInputs = 1;
	}
	else
	{
		if ( Flags64 & 0x100 )
		{
			if ( LastInputs == 1 )
				StopFire();
			Flags64 |= 0x4000;
			LastInputs = -1;
			KeyPressTime = 0;
			return;
		}

		if ( !FireClickTimer && FireClicksEvent )
		{
			Event = FireClicksEvent;
			FireClicksEvent = 0;
		}

		if ( !(dfStatus << 31) || Screen == 59 )
		{
			if ( !PD2 ) UserInputs = 2;
			if ( !PD3 ) UserInputs = 3;
		}

		if ( !PD2 && !PD3 ) UserInputs = 4;
		if ( !PE0 && !PD2 ) UserInputs = 5;
		if ( !PE0 && !PD3 ) UserInputs = 6;

		if ( USBD_IS_ATTACHED() )
		{
			if ( !(Flags64 & 0x400) )
			{
				UserInputs = 10;
				byte_20000048 = 0;
			}
		}
		else
		{
			if ( Flags64 & 0x400 )
			{
				UserInputs = 11;
			}
		}

		if ( Flags64 & 0x400 )
		{
			if ( !PD7 && !(Flags64 & 0x1000) )
			{
				if ( !byte_20000048 || byte_20000048 >= 50 )
				{
					UserInputs = 12;
					byte_20000048 = 0;
				}
			}
			else if ( PD7 && Flags64 & 0x1000 )
			{
				UserInputs = 13;
				byte_20000048 = 1;
			}
		}
	}

	if ( UserInputs >= 14u )
	{
		if ( LastInputs == 1 )
			StopFire();
		Flags64 |= 0x4000;
		LastInputs = -1;
		KeyPressTime = 0;
		return;
	}

	if ( UserInputs != LastInputs )
	{
		LastInputs = UserInputs;
		KeyPressTime = 0;
		Flags64 &= ~0x4000;
		return;
	}

	++KeyPressTime;

	if ( KeyPressTime == 6 )
	{
		Flags64 &= ~0x4000u;

		if (   UserInputs == 4
			|| UserInputs == 5
			|| UserInputs == 6
			|| UserInputs == 7
			|| UserInputs == 8
			|| UserInputs == 9
		)
			return;

		Event = UserInputs;

		if ( UserInputs == 1 )
		{
			FireClickTimer = 40;
			++FireClickCount;
			FireClicksEvent = 0;
			Event = 0;

			if ( FireClickCount == 1 )
			{
				FireClicksEvent = 15;	// single click

				if ( !EditModeTimer || EditItemIndex != 4 )
				{
					Event = 1;	// fire
				}
			}
			else if ( FireClickCount == 2 )
			{
				FireClicksEvent = EVENT_DOUBLE_FIRE;	// show date
			}
			else if ( FireClickCount == 3 )
			{
				FireClicksEvent = 16;	// edit mode
			}
			else if ( FireClickCount == 4 )
			{
				FireClicksEvent = EVENT_QUAD_FIRE;	// debug mode
			}
			else if ( FireClickCount == 5 )
			{
				FireClicksEvent = 17;	// Switch On/Off
			}
			else if ( FireClickCount == 10 )
			{
				FireClicksEvent = 31;	// board temp screen
			}
			else if ( FireClickCount == 20 )
			{
				Event = 29;	// firmware version screen
			}
		}
		else if ( UserInputs == 2 )
		{
			if ( dfStatus & 2 && !EditModeTimer
				&& Screen != 51 && Screen != 59 && Screen != 82 && Screen != 83
				&& Screen != 101 && Screen != 102 && Screen != 103 )
			{
				Event = 30;	// key lock violation
			}
			else
			{
				Event = 2;	// + button
			}
		}
		else if ( UserInputs == 3 )
		{
			if ( dfStatus & 2 && !EditModeTimer
				&& Screen != 51 && Screen != 59 && Screen != 82 && Screen != 83
				&& Screen != 101 && Screen != 102 && Screen != 103 )
			{
				Event = 30;	// key lock violation
			}
			else
			{
				Event = 3;	// - button
			}
		}
	}
	else if ( KeyPressTime == 20 )
	{
		if ( UserInputs == 1 )
		{
			FireClicksEvent = 0;
		}
	}
	else if ( KeyPressTime == 200 )
	{
		if ( UserInputs == 1 )
		{
			if ( Screen == 59 )
			{
				UpdateDataFlash();
				Flags64 |= 0x20000u;
				Screen = 0;
			}
		}
		else if ( UserInputs == 4 )
		{
			if ( !EditModeTimer )
			{
				if ( dfStatus << 31 )
					Event = 18;	// flip display
				else
					Event = 4;	// key (un)lock
			}
			else
			{
				EditModeTimer = 1000;
				if ( EditItemIndex == 4 )
				{
					if ( dfAPT == 1 )
						Event = 22;	// puff reset
					if ( dfAPT == 2 )
						Event = 23;	// time reset
				}
			}
		}
		else if ( UserInputs == 5 )
		{
			MenuPage = 1;
			if ( !(dfStatus << 31) )
			{
				if ( !(Flags68 & 0x200) )
				{
					Event = EVENT_ENTER_MENUS;	// new menus
				//	Event =  40;	// old menus
				}
				else
				{
					Flags68 &= ~0x200u;
					Event = -1;
					fbInitTimeouts();
					MainView();
				}
			}
		}
		else if ( UserInputs == 6 )
		{
			if ( !(dfStatus << 31) )
			{
				Event = 6;	// stealth on/off
			}
		}
	}
	else if ( KeyPressTime == 500 )
	{
		if ( UserInputs == 5 )
		{
			if ( dfStatus << 31 )
			{
				Event = 39;	// tcr set menu
			}
		}
		else if ( UserInputs == 6 )
		{
			if ( dfStatus << 31 )
			{
				Event = 34;	// battery voltage screen
			}
		}
	}
	else if ( ( KeyPressTime & 0x8000 ) ||  ( KeyPressTime & 0x7fff ) >= 210 )
	{
		if ( UserInputs == 1 )
		{
			if ( KeyPressTime > 1100 )
			{
				KeyPressTime = 1100;
				Flags64 |= 0x4000;
			}
			else if ( FireDuration > 99 )
			{
				Event = 24;	// 10s protection
			}
		}
		else if ( KeyPressTime & 0x8000 )
		{
			KeyPressTime = 6;
		}
	}
}


//=============================================================================
// Event Handling
//-----------------------------------------------------------------------------

unsigned char myDbgFlag = 0;

//-----------------------------------------------------------------------------

__myevic__ int EvtFire()
{
	int vret = 0;

	switch ( Screen )
	{
		case 101:
		{
			Flags68 ^= 0x10;
			if ( Flags68 & 0x10 )
			{
				Flags64 |= 0x20000;
				ScreenDuration = 10;
			}
			else
			{
				UpdateDFTimer = 1;
				UpdateDataFlash();
				MainView();
			}
			vret = 1;
		}
		break;
		
		case 102:
		{
			vret = MenuEvent( LastEvent );
		}
		break;

		case 103:
		{
			MainView();
			vret = 1;
		}
		break;
	}

	return vret;
}

//-----------------------------------------------------------------------------

__myevic__ int EvtSingleFire()
{
	int vret = 0;

	switch ( Screen )
	{
		case 100:
		{
			MainView();
			vret = 1;
		}
		break;

		case 101:
		{
			vret = 1;
		}
		break;

		case 102:
		{
			vret = MenuEvent( LastEvent );
		}
		break;

		case 103:
		{
			vret = 1;
		}
		break;
	}

	return vret;
}

//-----------------------------------------------------------------------------

__myevic__ int EvtPlusButton()
{
	int vret = 0;

	switch ( Screen )
	{
		case 1:
		{
			if ( EditModeTimer )
			{
				if ( EditItemIndex == 1 )
				{
					if ( dfMode < 3 )
					{
						KeyUpTimer = 5;
						EditModeTimer = 1000;

						do
						{
							if ( ++dfMode > 2 ) dfMode = 0;
						}
						while ( dfModesSel & ( 1 << dfMode ) );
						dfLastTCMode = dfMode;
						ModeChange();

						UpdateDFTimer = 50;
						Flags64 |= 0x20000;
						vret = 1;
					}
				}
				else if ( EditItemIndex == 4 )
				{
					KeyUpTimer = 5;
					EditModeTimer = 1000;

					if ( ++dfAPT > 6 ) dfAPT = 0;
					
					UpdateDFTimer = 50;
					Flags64 |= 0x20000;
					vret = 1;
				}
			}
		}
		break;

		case 83:
		{
			Event = EVENT_EDIT_CONTRAST;
		//	Screen = 101;
		//	ScreenDuration = 10;
		//	Flags64 |= 0x20000;
			vret = 1;
		}
		break;

		case 101:
		{
			if ( Flags68 &= 0x10 )
			{
				if ( dfContrast <= 240 ) dfContrast += 15;
				else dfContrast = 255;
				UpdateDFTimer = 50;
				DisplaySendCommand( 0x81 );
				DisplaySendCommand( dfContrast );
				Flags64 |= 0x20000;
				ScreenDuration = 10;
				vret = 1;
			}
			else
			{
				Event = 40;
			}
		}
		break;

		case 102:
		{
			vret = MenuEvent( LastEvent );
		}
		break;

		case 103:
		{
			unsigned int cs = RTCGetClockSpeed();
			if ( cs < 50000 ) ++cs;
			RTCSetClockSpeed( cs );
			Flags64 |= 0x20000;
			ScreenDuration = 120;
			vret = 1;
		}
		break;
	}

	return vret;
}

//-----------------------------------------------------------------------------

__myevic__ int EvtMinusButton()
{
	int vret = 0;

	switch ( Screen )
	{
		case 101:
		{
			Flags68 |= 0x10;
			if ( dfContrast >= 15 ) dfContrast -= 15;
			else dfContrast = 0;
			UpdateDFTimer = 50;
			DisplaySendCommand( 0x81 );
			DisplaySendCommand( dfContrast );
			Flags64 |= 0x20000;
			ScreenDuration = 10;
			vret = 1;
		}
		break;

		case 102:
		{
			vret = MenuEvent( LastEvent );
		}
		break;

		case 103:
		{
			unsigned int cs = RTCGetClockSpeed();
			if ( cs > 10000 ) --cs;
			RTCSetClockSpeed( cs );
			Flags64 |= 0x20000;
			ScreenDuration = 120;
			vret = 1;
		}
		break;
	}

	return vret;
}

//-----------------------------------------------------------------------------

__myevic__ int EvtDoubleFire()
{
	ShowDateFlag = 3;
	Flags64 |= 0x20000;
	return 1;
}

//-----------------------------------------------------------------------------

__myevic__ int EvtQuadFire()
{
	myDbgFlag ^= 1;
	Flags64 |= 0x20000;
	return 1;
}

//-----------------------------------------------------------------------------

__myevic__ int EvtContrastMenu()
{
	Screen = 101;
	ScreenDuration = 10;
	Flags64 |= 0x20000;
	return 1;
}

//-----------------------------------------------------------------------------

__myevic__ int EvtEnterMenus()
{
	CurrentMenu = 0;
	CurrentMenuItem = 0;
	Screen = 102;
	ScreenDuration = 10;
	Flags64 |= 0x20000;
	return 1;
}

//-----------------------------------------------------------------------------

__myevic__ int CustomEvents()
{
	int vret;
	
	vret = 0;

	switch ( LastEvent )
	{
		case   1:	// Fire button
			vret = EvtFire();
			break;

		case   2:	// + button
			vret = EvtPlusButton();
			break;

		case   3:	// - button
			vret = EvtMinusButton();
			break;

		case  15:	//Single Fire
			vret = EvtSingleFire();
			break;

	//	case 40:	// Enter menus
	//		vret = EvtEnterMenus();
	//		break;

		case EVENT_DOUBLE_FIRE:	// Double Fire
			vret = EvtDoubleFire();
			break;

		case EVENT_EDIT_CONTRAST:	// Contrast screen
			vret = EvtContrastMenu();
			break;

		case EVENT_ENTER_MENUS:	// Menus screen
			vret = EvtEnterMenus();
			break;

		case EVENT_QUAD_FIRE:
			vret = EvtQuadFire();
			break;

		default:
			break;
	}
	return vret;
}

//=============================================================================