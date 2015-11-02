//---------------------------------------------------------------------------
// Copyright (C) 2013-2015 Krzysztof Grochocki
//
// This file is part of Blabler
//
// Blabler is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3, or (at your option)
// any later version.
//
// Blabler is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GNU Radio. If not, see <http://www.gnu.org/licenses/>.
//---------------------------------------------------------------------------

#include <vcl.h>
#include <windows.h>
#include "BlablerFrm.h"
#include <inifiles.hpp>
#include <IdHashMessageDigest.hpp>
#include <fstream>
#include <XMLDoc.hpp>
#include <PluginAPI.h>
#include <LangAPI.hpp>
#pragma hdrstop

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
	return 1;
}
//---------------------------------------------------------------------------

//Uchwyt-do-formy-ustawien---------------------------------------------------
TSettingsForm* hSettingsForm;
//Struktury-glowne-----------------------------------------------------------
TPluginLink PluginLink;
TPluginInfo PluginInfo;
//---------------------------------------------------------------------------
//Informacje o kontakcie z botem
UnicodeString BotJID;
UnicodeString BotRes;
int BotUsrIdx;
//Styl zalacznika
UnicodeString AttachmentStyle;
//Awatary
TStringList *GetAvatarsList = new TStringList;
int AvatarType;
UnicodeString AvatarStyle;
UnicodeString AvatarsDir;
UnicodeString AvatarsDirW;
//Sciezka pliku cache
UnicodeString CacheDir;
//Blokowanie/zezwalanie na sprawdzanie danych w OnPerformCopyData
bool BlockPerformCopyData;
//Dane z notyfikacji OnPerformCopyData
UnicodeString ItemCopyData;
 //Uchwyt do okna rozmowy
HWND hFrmSend;
//Uchwyt do pola RichEdit
HWND hRichEdit;
//Uchwyt do okna timera
HWND hTimerFrm;
//SETTINGS-------------------------------------------------------------------
int AvatarSize;
UnicodeString StaticAvatarStyle;
bool HighlightMsgChk;
int HighlightMsgModeChk;
TStringList *HighlightMsgItemsList = new TStringList;
TCustomIniFile* HighlightMsgColorsList = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
//TIMERY---------------------------------------------------------------------
#define TIMER_UPDATE_AVATARS_ONLOAD 10
#define TIMER_UPDATE_AVATARS 20
//FORWARD-AQQ-HOOKS----------------------------------------------------------
INT_PTR __stdcall OnActiveTab(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnAddLine(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnColorChange(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnModulesLoaded(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnPerformCopyData(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnPrimaryTab(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnThemeChanged(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnXMLDebug(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceBlablerFastSettingsItem(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceInsertItem(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceLikeMsgItem(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceInsertNickItem(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceSendMsgItem(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceSendPrivMsgItem(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceWatchUserItem(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceWatchTagItem(WPARAM wParam, LPARAM lParam);
//FORWARD-TIMER--------------------------------------------------------------
LRESULT CALLBACK TimerFrmProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//---------------------------------------------------------------------------

//Otwarcie okna ustawien
void OpenSettingsForm()
{
	//Przypisanie uchwytu do formy ustawien
	if(!hSettingsForm)
	{
		Application->Handle = (HWND)SettingsForm;
		hSettingsForm = new TSettingsForm(Application);
	}
	//Pokaznie okna ustawien
	hSettingsForm->Show();
}
//---------------------------------------------------------------------------

//Szukanie uchwytu do kontrolki TsRichEdit
bool CALLBACK FindRichEdit(HWND hWnd, LPARAM)
{
	//Pobranie klasy okna
	wchar_t WClassName[128];
	GetClassNameW(hWnd, WClassName, sizeof(WClassName));
	//Sprawdzenie klasy okna
	if((UnicodeString)WClassName=="TsRichEdit")
	{
		//Przypisanie uchwytu
		hRichEdit = hWnd;
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------

//Ustawienie fokusa na polu wpisywania wiadomosci
void FocusRichEdit()
{
	//Ustawianie fokusa
	SetFocus(hRichEdit);
	//Pobieranie dlugosci tekstu z RichEdit
	int iLength = GetWindowTextLengthW(hRichEdit)+1;
	//Zmiana pozycji kursora
	CHARRANGE SelPos;
	SendMessage(hRichEdit, EM_EXGETSEL, NULL, (LPARAM)&SelPos);
	SelPos.cpMin = iLength;
	SelPos.cpMax = iLength;
	SendMessage(hRichEdit, EM_EXSETSEL, NULL, (LPARAM)&SelPos);
}
//---------------------------------------------------------------------------

//Pobieranie sciezki katalogu prywatnego wtyczek
UnicodeString GetPluginUserDir()
{
	return StringReplace((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,0,0), "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
}
//---------------------------------------------------------------------------
UnicodeString GetPluginUserDirW()
{
	return (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,0,0);
}
//---------------------------------------------------------------------------

//Pobieranie sciezki do kompozycji
UnicodeString GetThemeDir()
{
	return StringReplace((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETTHEMEDIR,0,0), "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
}
//---------------------------------------------------------------------------

//Pobieranie sciezki do skorki kompozycji
UnicodeString GetThemeSkinDir()
{
	return StringReplace((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETTHEMEDIR,0,0), "\\", "\\\\", TReplaceFlags() << rfReplaceAll) + "\\\\Skin";
}
//---------------------------------------------------------------------------

//Pobieranie sciezki do pliku DLL wtyczki
UnicodeString GetPluginDir()
{
	return StringReplace((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETPLUGINDIR,(WPARAM)(HInstance),0), "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
}
//---------------------------------------------------------------------------

//Pobieranie sciezki do folderu cache z awatarami
UnicodeString GetAvatarsDir()
{
	return AvatarsDir;
}
//---------------------------------------------------------------------------

//Sprawdzanie czy wlaczona jest zaawansowana stylizacja okien
bool ChkSkinEnabled()
{
	TStrings* IniList = new TStringList();
	IniList->SetText((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP,0,0));
	TMemIniFile *Settings = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
	Settings->SetStrings(IniList);
	delete IniList;
	UnicodeString SkinsEnabled = Settings->ReadString("Settings","UseSkin","1");
	delete Settings;
	return StrToBool(SkinsEnabled);
}
//---------------------------------------------------------------------------

//Sprawdzanie ustawien animacji AlphaControls
bool ChkThemeAnimateWindows()
{
	TStrings* IniList = new TStringList();
	IniList->SetText((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP,0,0));
	TMemIniFile *Settings = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
	Settings->SetStrings(IniList);
	delete IniList;
	UnicodeString AnimateWindowsEnabled = Settings->ReadString("Theme","ThemeAnimateWindows","1");
	delete Settings;
	return StrToBool(AnimateWindowsEnabled);
}
//---------------------------------------------------------------------------
bool ChkThemeGlowing()
{
	TStrings* IniList = new TStringList();
	IniList->SetText((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP,0,0));
	TMemIniFile *Settings = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
	Settings->SetStrings(IniList);
	delete IniList;
	UnicodeString GlowingEnabled = Settings->ReadString("Theme","ThemeGlowing","1");
	delete Settings;
	return StrToBool(GlowingEnabled);
}
//---------------------------------------------------------------------------

//Pobieranie ustawien koloru AlphaControls
int GetHUE()
{
	return (int)PluginLink.CallService(AQQ_SYSTEM_COLORGETHUE,0,0);
}
//---------------------------------------------------------------------------
int GetSaturation()
{
	return (int)PluginLink.CallService(AQQ_SYSTEM_COLORGETSATURATION,0,0);
}
//---------------------------------------------------------------------------
int GetBrightness()
{
	return (int)PluginLink.CallService(AQQ_SYSTEM_COLORGETBRIGHTNESS,0,0);
}
//---------------------------------------------------------------------------

//Pobranie stylu labela
TColor GetWarningColor()
{
	//Odczyt pliku
	TStringStream *Stream = new TStringStream;
	Stream->LoadFromFile(GetThemeDir()+"\\\\elements.xml");
	UnicodeString Text = Stream->DataString;
	delete Stream;
	_di_IXMLDocument XMLDoc = LoadXMLData("<content>" + Text + " </content>");
	_di_IXMLNode MainNode = XMLDoc->DocumentElement;
	int MainNodesCount = MainNode->ChildNodes->GetCount();
	//Parsowanie pliku XML
	for(int Count=0;Count<MainNodesCount;Count++)
	{
		_di_IXMLNode MainNodes = MainNode->ChildNodes->GetNode(Count);
		if(MainNodes->NodeName=="colors")
		{
			int ChildNodesCount = MainNodes->ChildNodes->GetCount();
			for(int ChildCount=0;ChildCount<ChildNodesCount;ChildCount++)
			{
				_di_IXMLNode ChildNode = MainNodes->ChildNodes->GetNode(ChildCount);
				if(ChildNode->NodeName=="label-warning")
				{
					//Parsowanie koloru
					UnicodeString Color = ChildNode->Attributes["color"];
					Color.Delete(1,1);
					UnicodeString Red = Color;
					Red.Delete(3,Red.Length());
					UnicodeString Green = Color;
					Green.Delete(1,2);
					Green.Delete(3,Green.Length());
					UnicodeString Blue = Color;
					Blue.Delete(1,4);
					//Konwersja HEX na RGB
					return (TColor)RGB(HexToInt(Red),HexToInt(Green),HexToInt(Blue));
				}
			}
		}
	}
	//Zwrocenie koloru
	return (TColor)RGB(0,0,0);
}
//---------------------------------------------------------------------------

//Pobieranie danych z danego URL
UnicodeString IdHTTPGet(UnicodeString URL)
{
	//Przypisanie uchwytu do formy ustawien
	if(!hSettingsForm)
	{
		Application->Handle = (HWND)SettingsForm;
		hSettingsForm = new TSettingsForm(Application);
	}
	//Zmienna z danymi
	UnicodeString ResponseText;
	//Proba pobrania danych
	try
	{
		//Wywolanie polaczenia
		hSettingsForm->IdHTTP->ConnectTimeout = 10000;
		ResponseText = hSettingsForm->IdHTTP->Get(URL);
	}
	//Blad
	catch(const Exception& e)
	{
		//Hack na wywalanie sie IdHTTP
		if(e.Message=="Connection Closed Gracefully.")
		{
			//Hack
			hSettingsForm->IdHTTP->CheckForGracefulDisconnect(false);
			//Rozlaczenie polaczenia
			hSettingsForm->IdHTTP->Disconnect();
			//Zwrot pustych danych
			return "";
		}
		//Inne bledy
		else
		{
			//Rozlaczenie polaczenia
			hSettingsForm->IdHTTP->Disconnect();
			//Zwrot pustych danych
			return "";
		}
	}
	//Pobranie kodu odpowiedzi
	int Response = hSettingsForm->IdHTTP->ResponseCode;
	//Wszystko ok
	if(Response==200) return ResponseText;
	//Inne bledy
	else return "";
}
//---------------------------------------------------------------------------

//Kodowanie ciagu znakow do Base64
UnicodeString EncodeBase64(UnicodeString Str)
{
	return (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_BASE64,(WPARAM)Str.w_str(),3);
}
//---------------------------------------------------------------------------

//Dekodowanie ciagu znakow z Base64
UnicodeString DecodeBase64(UnicodeString Str)
{
	return (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_BASE64,(WPARAM)Str.w_str(),2);
}
//---------------------------------------------------------------------------

//Konwersja tekstu na liczbe
int Convert(UnicodeString Char)
{
	for(int IntChar=-113;IntChar<=255;IntChar++)
	{
		if(Char==CHAR(IntChar))
			return IntChar;
	}
	return 0;
}
//---------------------------------------------------------------------------
UnicodeString ConvertToInt(UnicodeString Text)
{
	UnicodeString ConvertedText;
	for(int Count=1;Count<=Text.Length();Count++)
	{
		UnicodeString tmpStr = Text.SubString(Count, 1);
		int tmpInt = Convert(tmpStr);
		ConvertedText = ConvertedText + IntToStr(tmpInt);
	}
	return ConvertedText;
}
//---------------------------------------------------------------------------

//Pobieranie danych przez API
bool GetDataFromAPI(UnicodeString URL, UnicodeString ID)
{
	//Przypisanie uchwytu do formy ustawien
	if(!hSettingsForm)
	{
		Application->Handle = (HWND)SettingsForm;
		hSettingsForm = new TSettingsForm(Application);
	}
	//Wlaczenie AntiFreeze
	hSettingsForm->IdAntiFreeze->Active = true;
	//Pobieranie danych przez API
	UnicodeString Data = IdHTTPGet(URL);
	//Wylaczenie AntiFreeze
	hSettingsForm->IdAntiFreeze->Active = false;
	//Nie udalo sie pobrac danych
	if(Data.IsEmpty()) return false;
	//Parsowanie danych
	_di_IXMLDocument XMLDoc = LoadXMLData(Data);
	_di_IXMLNode MainNode = XMLDoc->DocumentElement;
	int MainItemsCount = MainNode->ChildNodes->GetCount();
	for(int MainCount=0;MainCount<MainItemsCount;MainCount++)
	{
		_di_IXMLNode MainChildNodes = MainNode->ChildNodes->GetNode(MainCount);
		//Informacja o wiadomosci
		if(MainChildNodes->NodeName=="body")
		{
			TIniFile *Ini = new TIniFile(CacheDir);
			Ini->WriteString(ID,"Body64",EncodeBase64(MainChildNodes->GetText()));
			delete Ini;
		}
		//Informacja o nadawcy wiadomosci
		if(MainChildNodes->NodeName=="user_path")
		{
			UnicodeString From = MainChildNodes->GetText();
			while(From.Pos("/")) From.Delete(1,From.Pos("/"));
			TIniFile *Ini = new TIniFile(CacheDir);
			Ini->WriteString(ID,"From",From);
			delete Ini;
		}
		//Informacja o zalacznikach
		if(MainChildNodes->NodeName=="attachments")
		{
			//Enumeracja wszystkich zalacznikow
			int MainChildItemsCount = MainChildNodes->ChildNodes->GetCount();
			for(int MainChildCount=0;MainChildCount<MainChildItemsCount;MainChildCount++)
			{
				//Enumeracja zawartosci zalacznika
				_di_IXMLNode ChildNodes = MainChildNodes->ChildNodes->GetNode(MainChildCount);
				int ChildItemsCount = ChildNodes->ChildNodes->GetCount();
				for(int ChildCount=0;ChildCount<ChildItemsCount;ChildCount++)
				{
					_di_IXMLNode ChildNode = ChildNodes->ChildNodes->GetNode(ChildCount);
					//Adres URL zalacznika
					if(ChildNode->NodeName=="url")
					{
						//Zapisanie informacji do cache
						TIniFile *Ini = new TIniFile(CacheDir);
						Ini->WriteString(ID,"Attachment",ChildNode->GetText());
						delete Ini;
					}
				}
			}
		}
	}
	return true;
}
//---------------------------------------------------------------------------

//Pobranie stylu Attachment & Avatars
void GetThemeStyle()
{
	//Reset stylow
	AttachmentStyle = "";
	AvatarStyle = "";
	//URL do aktuanie uzywanej kompozycji
	UnicodeString ThemeURL = GetThemeDir();
	//URL do domyslnej kompozycji
	UnicodeString ThemeURLW = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETAPPPATH,0,0));
	ThemeURLW = StringReplace(ThemeURLW, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
	ThemeURLW = ThemeURLW + "\\\\System\\\\Shared\\\\Themes\\\\Standard";
	//Pobieranie stylu zalacznika
	TStringStream *Stream = new TStringStream;
	if((FileExists(ThemeURL + "\\\\Message\\\\Attachment.htm"))||(FileExists(ThemeURL + "\\\\Message\\\\Attachment.html")))
	{
		if(FileExists(ThemeURL + "\\\\Message\\\\Attachment.htm")) Stream->LoadFromFile(ThemeURL + "\\\\Message\\\\Attachment.htm");
		else Stream->LoadFromFile(ThemeURL + "\\\\Message\\\\Attachment.html");
	}
	else
	{
		if(FileExists(ThemeURLW + "\\\\Message\\\\Attachment.htm")) Stream->LoadFromFile(ThemeURLW + "\\\\Message\\\\Attachment.htm");
		else Stream->LoadFromFile(ThemeURLW + "\\\\Message\\\\Attachment.html");
	}
	TStringList *DataText = new TStringList;
	DataText->Text = Stream->DataString;
	delete Stream;
	//Wyciaganie wlasciwego stylu zalacznika
	for(int Count=0;Count<DataText->Count;Count++)
	{
		if(DataText->Strings[Count].Pos("CC_ATTACH_ICON"))
		{
			//Pobranie stylu zalacznika
			AttachmentStyle = DataText->Strings[Count].Trim();
			//Zakonczenie petli
			Count = DataText->Count;
		}
	}
	delete DataText;
	//Brak wczytanego stylu zalacznika z kompozycji - styl domyslny
	if(AttachmentStyle.IsEmpty()) AttachmentStyle = "<SPAN class=\"attach_icon\">CC_ATTACH_ICON</SPAN><SPAN id=\"attach_caption\">CC_ATTACH_CAPTION</SPAN>: <SPAN class=\"attach_short\">CC_ATTACH_SHORT</SPAN>";
	//Usuwanie zbednego formatowana ze stylu
	else if(AttachmentStyle.LowerCase().Pos("<div")==1)
	{
		AttachmentStyle.Delete(1,AttachmentStyle.Pos(">"));
		AttachmentStyle.Delete(AttachmentStyle.LowerCase().Pos("</div>"),AttachmentStyle.Length());
		AttachmentStyle = AttachmentStyle.Trim();
	}
	//Dodawanie odstepu do stylu zalacznika
	AttachmentStyle = "<div style=\"display:block;padding-top:5px;\">"+AttachmentStyle+"</div>";
	//Pobieranie stylu awatarow
	if((FileExists(ThemeURL + "\\\\Message\\\\BlablerAvatar.htm"))||(FileExists(ThemeURL + "\\\\Message\\\\BlablerAvatar.html")))
	{
		//Pobieranie danych z pliku
		TStringStream *Stream = new TStringStream;
		if(FileExists(ThemeURL + "\\\\Message\\\\BlablerAvatar.htm")) Stream->LoadFromFile(ThemeURL + "\\\\Message\\\\BlablerAvatar.htm");
		else Stream->LoadFromFile(ThemeURL + "\\\\Message\\\\BlablerAvatar.html");
		AvatarStyle = Stream->DataString.Trim();
		delete Stream;
		//Sprawdzanie zawartosci pliku
		if(!AvatarStyle.Pos("CC_AVATAR"))
		{
			if(!StaticAvatarStyle.IsEmpty())
			{
				AvatarStyle = StaticAvatarStyle;
				AvatarType = 1;
			}
			else
			{
				AvatarStyle = "<span style=\"display: inline-block; padding: 2px 4px 0px 1px; vertical-align: middle;\">CC_AVATAR</span>";;
				AvatarType = 2;
			}
		}
		else AvatarType = 0;
	}
	else if(!StaticAvatarStyle.IsEmpty())
	{
		AvatarStyle = StaticAvatarStyle;
		AvatarType = 1;
	}
	else
	{
		AvatarStyle = "<span style=\"display: inline-block; padding: 2px 4px 0px 1px; vertical-align: middle;\">CC_AVATAR</span>";
		AvatarType = 2;
	}
}
//---------------------------------------------------------------------------

//Pobieranie stylu awatarow z rdzenia wtyczki
UnicodeString GetAvatarStyle()
{
	return AvatarStyle;
}
//---------------------------------------------------------------------------

//Ustawianie stylu awatarow w rdzeniu wtyczki
void SetAvatarStyle(UnicodeString Style)
{
	AvatarStyle = Style;
}
//---------------------------------------------------------------------------

//Pobieranie typu stylu awatarow z rdzenia wtyczki
int GetAvatarType()
{
	return AvatarType;
}
//---------------------------------------------------------------------------

//Ustawianie typu stylu awatarow w rdzeniu wtyczki
int SetAvatarType(int Type)
{
	AvatarType = Type;
}
//---------------------------------------------------------------------------

//Automatyczna aktualizacja awatarow
void AutoAvatarsUpdate()
{
	TIniFile *Ini = new TIniFile(GetPluginUserDir()+"\\\\Blabler\\\\Settings.ini");
	//Pobieranie sposobu aktualizacji awatarow
	int AutoAvatarsUpdateMode = Ini->ReadInteger("Avatars","UpdateMode",0);
	//Jezeli awatary maja byc w ogole aktualizowane automatycznie
	if(AutoAvatarsUpdateMode)
	{
		//Pobieranie daty ostatniej aktualizacji
		UnicodeString sLast = Ini->ReadString("Avatars", "LastUpdate", "");
		if(!sLast.IsEmpty())
			sLast.Delete(sLast.Pos(" "),sLast.Length());
		else
			sLast = "1899-12-30";
		//Zmienne dekodowania daty
		double Last;
		double Current;
		int DiffTime;
		//Dekodowanie daty ostatniej aktualizacji
		UnicodeString LastYear = sLast;
		LastYear = LastYear.Delete(LastYear.Pos("-"),LastYear.Length());
		UnicodeString LastMonth= sLast;
		LastMonth = LastMonth.Delete(1,LastMonth.Pos("-"));
		LastMonth = LastMonth.Delete(LastMonth.Pos("-"),LastMonth.Length());
		UnicodeString LastDay = sLast;
		LastDay = LastDay.Delete(1,LastDay.Pos("-"));
		LastDay = LastDay.Delete(1,LastDay.Pos("-"));
		Last = EncodeDate(StrToInt(LastYear),StrToInt(LastMonth),StrToInt(LastDay));
		//Pobieranie obecnej daty
		TDateTime Time = TDateTime::CurrentDateTime();
		Current = EncodeDate(StrToInt(Time.FormatString("yyyy")), StrToInt(Time.FormatString("mm")), StrToInt(Time.FormatString("dd")));
		//Porownanie dat
		DiffTime = difftime(Current,Last);
		//Sprawdzanie czy aktualizacja jest wymagana
		if(((AutoAvatarsUpdateMode==1)&&(DiffTime>=1))||
		((AutoAvatarsUpdateMode==2)&&(DiffTime>=7))||
		((AutoAvatarsUpdateMode==3)&&(DiffTime>=30)))
		{
			//Przypisanie uchwytu do formy ustawien
			if(!hSettingsForm)
			{
				Application->Handle = (HWND)SettingsForm;
				hSettingsForm = new TSettingsForm(Application);
			}
			//Zmiana caption na buttonie
			hSettingsForm->ManualAvatarsUpdateButton->Caption = "Przerwij aktualizacje";
			//Tworzenie katalogu z awatarami
			if(!DirectoryExists(AvatarsDir)) CreateDir(AvatarsDir);
			//Wlaczenie paska postepu
			hSettingsForm->ProgressBar->Position = 0;
			hSettingsForm->ProgressBar->Visible = true;
			hSettingsForm->ProgressLabel->Caption = "Pobieranie danych...";
			hSettingsForm->ProgressLabel->Visible = true;
			//Wlaczenie paska postepu na taskbarze
			hSettingsForm->Taskbar->ProgressValue = 0;
			hSettingsForm->Taskbar->ProgressState = TTaskBarProgressState::Normal;
			//Pobieranie listy plikow
			hSettingsForm->FileListBox->Directory = "";
			hSettingsForm->FileListBox->Directory = GetPluginUserDirW() + "\\Blabler\\Avatars";
			//Ignorowanie plikow *.tmp i plikow ze spacja (np. konflikty stworzone przez Dropbox'a)
			for(int Count=0;Count<hSettingsForm->FileListBox->Items->Count;Count++)
			{
				if(ExtractFileName(hSettingsForm->FileListBox->Items->Strings[Count]).Pos(".tmp")>0)
				{
					DeleteFile(hSettingsForm->FileListBox->Items->Strings[Count]);
					hSettingsForm->FileListBox->Items->Strings[Count] ="TMP_DELETE";
				}
				else if(ExtractFileName(hSettingsForm->FileListBox->Items->Strings[Count]).Pos(" ")>0)
				{
					DeleteFile(hSettingsForm->FileListBox->Items->Strings[Count]);
					hSettingsForm->FileListBox->Items->Strings[Count] = "TMP_DELETE";
				}
			}
			while(hSettingsForm->FileListBox->Items->IndexOf("TMP_DELETE")!=-1)
				hSettingsForm->FileListBox->Items->Delete(hSettingsForm->FileListBox->Items->IndexOf("TMP_DELETE"));
			//Ustawianie maksymalnego paska postepu
			hSettingsForm->ProgressBar->Max = hSettingsForm->FileListBox->Items->Count;
			//Ustawianie maksymalnego paska postepu na taskbarze
			hSettingsForm->Taskbar->ProgressMaxValue = hSettingsForm->FileListBox->Items->Count;
			//Wlacznie aktualizacji
			hSettingsForm->AutoAvatarsUpdateThread->Start();
		}
	}
	delete Ini;
}
//---------------------------------------------------------------------------

bool ChkAvatarsListItem()
{
	if(GetAvatarsList->Count) return true;
	else return false;
}
//---------------------------------------------------------------------------

UnicodeString GetAvatarsListItem()
{
	if(GetAvatarsList->Count)
	{
		UnicodeString Item = GetAvatarsList->Strings[0];
		GetAvatarsList->Delete(0);
		return Item;
	}
	else return "";
}
//---------------------------------------------------------------------------

//Sprawdzanie czy przekazany znak jest dozwolony
bool AllowedTagsCharacters(UnicodeString Text)
{
	UnicodeString Characters[] = {" ","!","@","#","$","%","^","&","*","(",")","-","_","=","+","[","{","]","}",":",";","'",'"',"<",",",">",".","?","/","\\","|","`","~","�"};
	for(int Char=0;Char<34;Char++)
	{
		if(Text.Pos(Characters[Char])>0) return false;
	}
	return true;
}
//---------------------------------------------------------------------------

//Sprawdzanie czy przekazany znak jest dozwolony
bool AllowedUsersCharacters(UnicodeString Text)
{
	UnicodeString Characters[] = {" ","!","@","#","$","%","^","&","*","(",")","-","_","=","+","[","{","]","}",":",";","'",'"',"<",",",">",".","?","/","\\","|","`","~","�"};
	for(int Char=0;Char<34;Char++)
	{
		if(Text.Pos(Characters[Char])>0) return false;
	}
	return true;
}
//---------------------------------------------------------------------------

//Zwracanie pozycji uciecia tagu
int TagsCutPosition(UnicodeString Text)
{
	UnicodeString Characters[] = {" ","!","@","#","$","%","^","&","*","(",")","=","+","[","{","]","}",":",";","'",'"',"<",",",">",".","?","/","\\","|","`","~","�"};
	int xPos = Text.Length()+1;
	int yPos;
	for(int Char=0;Char<32;Char++)
	{
		yPos = Text.Pos(Characters[Char]);
		if((yPos<xPos)&&(yPos!=0)) xPos = yPos;
	}
	return xPos;
}
//---------------------------------------------------------------------------

//Zwracanie pozycji uciecia uzytkownika Twittera
int UsersCutPosition(UnicodeString Text)
{
	UnicodeString Characters[] = {" ","!","@","#","$","%","^","&","*","(",")","-","_","=","+","[","{","]","}",":",";","'",'"',"<",",",">",".","?","/","\\","|","`","~","�"};
	int xPos = Text.Length()+1;
	int yPos;
	for(int Char=0;Char<34;Char++)
	{
		yPos = Text.Pos(Characters[Char]);
		if((yPos<xPos)&&(yPos!=0)) xPos = yPos;
	}
	return xPos;
}
//---------------------------------------------------------------------------

//Usuwanie elementu szybkiego dostepu do ustawien wtyczki
void DestroyBlablerFastSettings()
{
	TPluginAction FastSettingsItem;
	ZeroMemory(&FastSettingsItem,sizeof(TPluginAction));
	FastSettingsItem.cbSize = sizeof(TPluginAction);
	FastSettingsItem.pszName = L"BlablerFastSettingsItemButton";
	PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM,0,(LPARAM)(&FastSettingsItem));
}
//---------------------------------------------------------------------------
//Tworzenie elementu szybkiego dostepu do ustawien wtyczki
void BuildBlablerFastSettings()
{
	TPluginAction FastSettingsItem;
	ZeroMemory(&FastSettingsItem,sizeof(TPluginAction));
	FastSettingsItem.cbSize = sizeof(TPluginAction);
	FastSettingsItem.pszName = L"BlablerFastSettingsItemButton";
	FastSettingsItem.pszCaption = L"Blabler";
	FastSettingsItem.Position = 0;
	FastSettingsItem.IconIndex = 12;
	FastSettingsItem.pszService = L"sBlablerFastSettingsItem";
	FastSettingsItem.pszPopupName = L"popPlugins";
	FastSettingsItem.PopupPosition = 0;
	PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&FastSettingsItem));
}
//---------------------------------------------------------------------------

//Usuwanie elementu cytowania
void DestroyQuoteMsgItem()
{
	TPluginAction QuoteMsgItem;
	ZeroMemory(&QuoteMsgItem,sizeof(TPluginAction));
	QuoteMsgItem.cbSize = sizeof(TPluginAction);
	QuoteMsgItem.pszName = L"BlablerQuoteMsgItem";
	QuoteMsgItem.Handle = (int)hFrmSend;
	PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&QuoteMsgItem));
}
//---------------------------------------------------------------------------
//Tworzenie elementu cytowania
void BuildQuoteMsgItem()
{
	TPluginAction QuoteMsgItem;
	ZeroMemory(&QuoteMsgItem,sizeof(TPluginAction));
	QuoteMsgItem.cbSize = sizeof(TPluginAction);
	QuoteMsgItem.pszName = L"BlablerQuoteMsgItem";
	QuoteMsgItem.pszCaption = L"Cytuj";
	QuoteMsgItem.Position = 0;
	QuoteMsgItem.IconIndex = 8;
	QuoteMsgItem.pszService = L"sBlablerInsertItem";
	QuoteMsgItem.pszPopupName = L"popURL";
	QuoteMsgItem.Handle = (int)hFrmSend;
	PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&QuoteMsgItem));
}
//---------------------------------------------------------------------------

//Usuwanie elementu wstawiania tagu
void DestroyInsertTagItem()
{
	TPluginAction InsertTagItem;
	ZeroMemory(&InsertTagItem,sizeof(TPluginAction));
	InsertTagItem.cbSize = sizeof(TPluginAction);
	InsertTagItem.pszName = L"BlablerInsertTagItem";
	InsertTagItem.Handle = (int)hFrmSend;
	PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&InsertTagItem));
}
//---------------------------------------------------------------------------
//Tworzenie elementu wstawiania tagu
void BuildInsertTagItem()
{
	TPluginAction InsertTagItem;
	ZeroMemory(&InsertTagItem,sizeof(TPluginAction));
	InsertTagItem.cbSize = sizeof(TPluginAction);
	InsertTagItem.pszName = L"BlablerInsertTagItem";
	InsertTagItem.pszCaption = ("Wstaw " + ItemCopyData).w_str();
	InsertTagItem.Position = 0;
	InsertTagItem.IconIndex = 11;
	InsertTagItem.pszService = L"sBlablerInsertItem";
	InsertTagItem.pszPopupName = L"popURL";
	InsertTagItem.Handle = (int)hFrmSend;
	PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&InsertTagItem));
}
//---------------------------------------------------------------------------

//Usuwanie elementu lajkowania
void DestroyLikeMsgItem()
{
	TPluginAction LikeMsgItem;
	ZeroMemory(&LikeMsgItem,sizeof(TPluginAction));
	LikeMsgItem.cbSize = sizeof(TPluginAction);
	LikeMsgItem.pszName = L"BlablerLikeMsgItem";
	LikeMsgItem.Handle = (int)hFrmSend;
	PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&LikeMsgItem));
}
//---------------------------------------------------------------------------
//Tworzenie elementu lajkowania
void BuildLikeMsgItem()
{
	//Pobieranie informacji o zalajkowaniu wiadomosci
	TIniFile *Ini = new TIniFile(CacheDir);
	bool Like = Ini->ReadBool("Loves",ItemCopyData,false);
	delete Ini;
	//Tworzenie elementu
	TPluginAction LikeMsgItem;
	ZeroMemory(&LikeMsgItem,sizeof(TPluginAction));
	LikeMsgItem.cbSize = sizeof(TPluginAction);
	LikeMsgItem.pszName = L"BlablerLikeMsgItem";
	if(Like) LikeMsgItem.pszCaption = L"Przesta� lubi�";
	else LikeMsgItem.pszCaption = L"Polub";
	LikeMsgItem.Position = 0;
	if(Like) LikeMsgItem.IconIndex = 158;
	else LikeMsgItem.IconIndex = 157;
	LikeMsgItem.pszService = L"sBlablerLikeMsgItem";
	LikeMsgItem.pszPopupName = L"popURL";
	LikeMsgItem.Handle = (int)hFrmSend;
	PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&LikeMsgItem));
}
//---------------------------------------------------------------------------

//Usuwanie elementu wstawiania nicku
void DestroyInsertNickItem()
{
	TPluginAction InsertNickItem;
	ZeroMemory(&InsertNickItem,sizeof(TPluginAction));
	InsertNickItem.cbSize = sizeof(TPluginAction);
	InsertNickItem.pszName = L"BlablerInsertNickItem";
	InsertNickItem.Handle = (int)hFrmSend;
	PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&InsertNickItem));
}
//---------------------------------------------------------------------------
//Tworzenie elementu wstawiania nicku
void BuildInsertNickItem()
{
	TPluginAction InsertNickItem;
	ZeroMemory(&InsertNickItem,sizeof(TPluginAction));
	InsertNickItem.cbSize = sizeof(TPluginAction);
	InsertNickItem.pszName = L"BlablerInsertNickItem";
	InsertNickItem.pszCaption = ("Wstaw ^" + ItemCopyData).w_str();
	InsertNickItem.Position = 0;
	InsertNickItem.IconIndex = 11;
	InsertNickItem.pszService = L"sBlablerInsertNickItem";
	InsertNickItem.pszPopupName = L"popURL";
	InsertNickItem.Handle = (int)hFrmSend;
	PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&InsertNickItem));
}
//---------------------------------------------------------------------------

//Usuwanie elementu wysylania wiadomosci
void DestroySendMsgItem()
{
	TPluginAction SendMsgItem;
	ZeroMemory(&SendMsgItem,sizeof(TPluginAction));
	SendMsgItem.cbSize = sizeof(TPluginAction);
	SendMsgItem.pszName = L"BlablerSendMsgItem";
	SendMsgItem.Handle = (int)hFrmSend;
	PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&SendMsgItem));
}
//---------------------------------------------------------------------------
//Tworzenie elementu wysylania wiadomosci
void BuildSendMsgItem()
{
	TPluginAction SendMsgItem;
	ZeroMemory(&SendMsgItem,sizeof(TPluginAction));
	SendMsgItem.cbSize = sizeof(TPluginAction);
	SendMsgItem.pszName = L"BlablerSendMsgItem";
	SendMsgItem.pszCaption = L"Wiadomo��";
	SendMsgItem.Position = 0;
	SendMsgItem.IconIndex = 8;
	SendMsgItem.pszService = L"sBlablerSendMsgItem";
	SendMsgItem.pszPopupName = L"popURL";
	SendMsgItem.Handle = (int)hFrmSend;
	PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&SendMsgItem));
}
//---------------------------------------------------------------------------

//Usuwanie elementu wysylania prywatnej wiadomosci
void DestroySendPrivMsgItem()
{
	TPluginAction SendPrivMsgItem;
	ZeroMemory(&SendPrivMsgItem,sizeof(TPluginAction));
	SendPrivMsgItem.cbSize = sizeof(TPluginAction);
	SendPrivMsgItem.pszName = L"BlablerSendPrivMsgItem";
	SendPrivMsgItem.Handle = (int)hFrmSend;
	PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&SendPrivMsgItem));
}
//---------------------------------------------------------------------------
//Tworzenie elementu wysylania prywatnej wiadomosci
void BuildSendPrivMsgItem()
{
	TPluginAction SendPrivMsgItem;
	ZeroMemory(&SendPrivMsgItem,sizeof(TPluginAction));
	SendPrivMsgItem.cbSize = sizeof(TPluginAction);
	SendPrivMsgItem.pszName = L"BlablerSendPrivMsgItem";
	SendPrivMsgItem.pszCaption = L"Wiadomo�� prywatna";
	SendPrivMsgItem.Position = 0;
	SendPrivMsgItem.IconIndex = 8;
	SendPrivMsgItem.pszService = L"sBlablerSendPrivMsgItem";
	SendPrivMsgItem.pszPopupName = L"popURL";
	SendPrivMsgItem.Handle = (int)hFrmSend;
	PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&SendPrivMsgItem));
}
//---------------------------------------------------------------------------

//Usuwanie elementu obserowania uzytkownika
void DestroyWatchUserItem()
{
	TPluginAction WatchUserItem;
	ZeroMemory(&WatchUserItem,sizeof(TPluginAction));
	WatchUserItem.cbSize = sizeof(TPluginAction);
	WatchUserItem.pszName = L"BlablerWatchUserItem";
	WatchUserItem.Handle = (int)hFrmSend;
	PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&WatchUserItem));
}
//---------------------------------------------------------------------------
//Tworzenie elementu obserowania uzytkownika
void BuildWatchUserItem()
{
	TPluginAction WatchUserItem;
	ZeroMemory(&WatchUserItem,sizeof(TPluginAction));
	WatchUserItem.cbSize = sizeof(TPluginAction);
	WatchUserItem.pszName = L"BlablerWatchUserItem";
	WatchUserItem.pszCaption = ("Obserwuj ^" + ItemCopyData).w_str();
	WatchUserItem.Position = 0;
	WatchUserItem.IconIndex = 86;
	WatchUserItem.pszService = L"sBlablerWatchUserItem";
	WatchUserItem.pszPopupName = L"popURL";
	WatchUserItem.Handle = (int)hFrmSend;
	PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&WatchUserItem));
}
//---------------------------------------------------------------------------

//Usuwanie elementu obserowania tagu
void DestroyWatchTagItem()
{
	TPluginAction WatchTagItem;
	ZeroMemory(&WatchTagItem,sizeof(TPluginAction));
	WatchTagItem.cbSize = sizeof(TPluginAction);
	WatchTagItem.pszName = L"BlablerWatchTagItem";
	WatchTagItem.Handle = (int)hFrmSend;
	PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&WatchTagItem));
}
//---------------------------------------------------------------------------
//Tworzenie elementu obserowania tagu
void BuildWatchTagItem()
{
	TPluginAction WatchTagItem;
	ZeroMemory(&WatchTagItem,sizeof(TPluginAction));
	WatchTagItem.cbSize = sizeof(TPluginAction);
	WatchTagItem.pszName = L"BlablerWatchTagItem";
	WatchTagItem.pszCaption = ("Obserwuj " + ItemCopyData).w_str();
	WatchTagItem.Position = 0;
	WatchTagItem.IconIndex = 86;
	WatchTagItem.pszService = L"sBlablerWatchTagItem";
	WatchTagItem.pszPopupName = L"popURL";
	WatchTagItem.Handle = (int)hFrmSend;
	PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&WatchTagItem));
}
//---------------------------------------------------------------------------

//Usuwanie separatora
void DestroySeparatorItem()
{
	TPluginAction SeparatorItem;
	ZeroMemory(&SeparatorItem,sizeof(TPluginAction));
	SeparatorItem.cbSize = sizeof(TPluginAction);
	SeparatorItem.pszName = L"BlablerSeparatorItem";
	SeparatorItem.Handle = (int)hFrmSend;
	PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&SeparatorItem));
}
//---------------------------------------------------------------------------
//Tworzenie separatora
void BuildSeparatorItem()
{
	TPluginAction SeparatorItem;
	ZeroMemory(&SeparatorItem,sizeof(TPluginAction));
	SeparatorItem.cbSize = sizeof(TPluginAction);
	SeparatorItem.pszName = L"BlablerSeparatorItem";
	SeparatorItem.pszCaption = L"-";
	SeparatorItem.Position = 0;
	SeparatorItem.IconIndex = 0;
	SeparatorItem.pszPopupName = L"popURL";
	SeparatorItem.Handle = (int)hFrmSend;
	PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&SeparatorItem));
}
//---------------------------------------------------------------------------

//Serwis szybkiego dostepu do ustawien wtyczki
INT_PTR __stdcall ServiceBlablerFastSettingsItem(WPARAM wParam, LPARAM lParam)
{
	//Otwarcie okna ustawien
	OpenSettingsForm();

	return 0;
}
//---------------------------------------------------------------------------

//Serwis do wstawiania danego tekstu
INT_PTR __stdcall ServiceInsertItem(WPARAM wParam, LPARAM lParam)
{
	//Szukanie pola wiadomosci
	if(!hRichEdit) EnumChildWindows(hFrmSend,(WNDENUMPROC)FindRichEdit,0);
	//Pobieranie tekstu z RichEdit
	int iLength = GetWindowTextLengthW(hRichEdit)+1;
	wchar_t* pBuff = new wchar_t[iLength];
	GetWindowTextW(hRichEdit, pBuff, iLength);
	UnicodeString Text = pBuff;
	delete pBuff;
	//Wiadomosc nie zawiera juz wskazanego tekstu
	if(!Text.Pos(ItemCopyData))
	{
		//Usuwanie znakow spacji z prawej strony
		Text = Text.TrimRight();
		//Ustawianie tekstu
		if(!Text.IsEmpty())
			SetWindowTextW(hRichEdit, (Text + " " + ItemCopyData + " ").w_str());
		else
			SetWindowTextW(hRichEdit, (ItemCopyData + " ").w_str());
	}
	//Kasowanie tekstu
	ItemCopyData = "";
	//Ustawienie fokusa na polu wpisywania wiadomosci
	FocusRichEdit();

	return 0;
}
//---------------------------------------------------------------------------

//Serwis do lajkowania wiadomosci
INT_PTR __stdcall ServiceLikeMsgItem(WPARAM wParam, LPARAM lParam)
{
	//Pobieranie informacji o zalajkowaniu wiadomosci
	TIniFile *Ini = new TIniFile(CacheDir);
	bool Like = Ini->ReadBool("Loves",ItemCopyData,false);
	delete Ini;
	//Struktura kontaktu
	TPluginContact PluginContact;
	ZeroMemory(&PluginContact,sizeof(TPluginContact));
	PluginContact.JID = BotJID.w_str();
	PluginContact.Resource = BotRes.w_str();
	PluginContact.FromPlugin = false;
	PluginContact.UserIdx = BotUsrIdx;
	//Struktura wiadomosci
	TPluginMessage PluginMessage;
	ZeroMemory(&PluginMessage,sizeof(TPluginMessage));
	PluginMessage.cbSize = sizeof(TPluginMessage);
	PluginMessage.JID = BotJID.w_str();
	if(Like) PluginMessage.Body = ("!idontlove " + ItemCopyData).w_str();
	else PluginMessage.Body = ("!ilove " + ItemCopyData).w_str();
	PluginMessage.Store = true;
	PluginMessage.ShowAsOutgoing = true;
	//Wysylanie wiadomosci
	if(PluginLink.CallService(AQQ_CONTACTS_SENDMSG ,(WPARAM)(&PluginContact),(LPARAM)(&PluginMessage)))
	{
		//Zapisywanie informacji o zalajkowaniu wiadomosci
		TIniFile *Ini = new TIniFile(CacheDir);
		Ini->WriteBool("Loves",ItemCopyData,!Like);
		delete Ini;
	}

	return 0;
}
//---------------------------------------------------------------------------

//Serwis do wstawiania nicku
INT_PTR __stdcall ServiceInsertNickItem(WPARAM wParam, LPARAM lParam)
{
	//Szukanie pola wiadomosci
	if(!hRichEdit) EnumChildWindows(hFrmSend,(WNDENUMPROC)FindRichEdit,0);
	//Pobieranie tekstu z RichEdit
	int iLength = GetWindowTextLengthW(hRichEdit)+1;
	wchar_t* pBuff = new wchar_t[iLength];
	GetWindowTextW(hRichEdit, pBuff, iLength);
	UnicodeString Text = pBuff;
	delete pBuff;
	//Wiadomosc nie zawiera wskazanego nicka
	if(!Text.Pos("^"+ItemCopyData))
	{
		//Usuwanie znakow spacji z prawej strony
		Text = Text.TrimRight();
		//Ustawianie tekstu
		if(!Text.IsEmpty())
			SetWindowTextW(hRichEdit, (Text + " ^" + ItemCopyData + " ").w_str());
		else
			SetWindowTextW(hRichEdit, ("^" + ItemCopyData + " ").w_str());
	}
	//Kasowanie nicku
	ItemCopyData = "";
	//Ustawienie fokusa na polu wpisywania wiadomosci
	FocusRichEdit();

	return 0;
}
//---------------------------------------------------------------------------

//Serwis do wysylania wiadomosci
INT_PTR __stdcall ServiceSendMsgItem(WPARAM wParam, LPARAM lParam)
{
	//Szukanie pola wiadomosci
	if(!hRichEdit) EnumChildWindows(hFrmSend,(WNDENUMPROC)FindRichEdit,0);
	//Pobieranie tekstu z RichEdit
	int iLength = GetWindowTextLengthW(hRichEdit)+1;
	wchar_t* pBuff = new wchar_t[iLength];
	GetWindowTextW(hRichEdit, pBuff, iLength);
	UnicodeString Text = pBuff;
	delete pBuff;
	//Wiadomosc zawiera jakis tekst
	if(!Text.IsEmpty())
	{
		if((Text.Pos(">")==1)||(Text.Pos(">>")==1))
		{
			UnicodeString TempStr = Text;
			TempStr.Delete(TempStr.Pos(" "),TempStr.Length());
			if(((TempStr.Pos(">")==1)&&(TempStr.Length()>1))||(TempStr.Pos(">>")==1)&&(TempStr.Length()>2))
				Text = StringReplace(Text, TempStr, "", TReplaceFlags() << rfReplaceAll);
		}
		SetWindowTextW(hRichEdit, (">" + ItemCopyData + " " + Text.TrimLeft()).w_str());
	}
	//Wiadomosc jest pusta
	else SetWindowTextW(hRichEdit, (">" + ItemCopyData + " ").w_str());
	//Kasowanie nicku
	ItemCopyData = "";
	//Ustawienie fokusa na polu wpisywania wiadomosci
	FocusRichEdit();

	return 0;
}
//---------------------------------------------------------------------------

//Serwis do wysylania prywatnej wiadomosci
INT_PTR __stdcall ServiceSendPrivMsgItem(WPARAM wParam, LPARAM lParam)
{
	//Szukanie pola wiadomosci
	if(!hRichEdit) EnumChildWindows(hFrmSend,(WNDENUMPROC)FindRichEdit,0);
	//Pobieranie tekstu z RichEdit
	int iLength = GetWindowTextLengthW(hRichEdit)+1;
	wchar_t* pBuff = new wchar_t[iLength];
	GetWindowTextW(hRichEdit, pBuff, iLength);
	UnicodeString Text = pBuff;
	delete pBuff;
	//Wiadomosc zawiera jakis tekst
	if(!Text.IsEmpty())
	{
		if((Text.Pos(">")==1)||(Text.Pos(">>")==1))
		{
			UnicodeString TempStr = Text;
			TempStr.Delete(TempStr.Pos(" "),TempStr.Length());
			if(((TempStr.Pos(">")==1)&&(TempStr.Length()>1))||(TempStr.Pos(">>")==1)&&(TempStr.Length()>2))
				Text = StringReplace(Text, TempStr, "", TReplaceFlags() << rfReplaceAll);
		}
		SetWindowTextW(hRichEdit, (">>" + ItemCopyData + " " + Text.TrimLeft()).w_str());
	}
	//Wiadomosc jest pusta
	else SetWindowTextW(hRichEdit, (">>" + ItemCopyData + " ").w_str());
	//Kasowanie nicku
	ItemCopyData = "";
	//Ustawienie fokusa na polu wpisywania wiadomosci
	FocusRichEdit();

	return 0;
}
//---------------------------------------------------------------------------

//Serwis do obserowania uzytkownika
INT_PTR __stdcall ServiceWatchUserItem(WPARAM wParam, LPARAM lParam)
{
	//Struktura kontaktu
	TPluginContact PluginContact;
	ZeroMemory(&PluginContact,sizeof(TPluginContact));
	PluginContact.JID = BotJID.w_str();
	PluginContact.Resource = BotRes.w_str();
	PluginContact.FromPlugin = false;
	PluginContact.UserIdx = BotUsrIdx;
	//Struktura wiadomosci
	TPluginMessage PluginMessage;
	ZeroMemory(&PluginMessage,sizeof(TPluginMessage));
	PluginMessage.cbSize = sizeof(TPluginMessage);
	PluginMessage.JID = BotJID.w_str();
	PluginMessage.Body = ("!obserwuj " + ItemCopyData).w_str();
	PluginMessage.Store = true;
	PluginMessage.ShowAsOutgoing = true;
	//Wysylanie wiadomosci
	PluginLink.CallService(AQQ_CONTACTS_SENDMSG ,(WPARAM)(&PluginContact),(LPARAM)(&PluginMessage));

	return 0;
}
//---------------------------------------------------------------------------

//Serwis do obserowania tagu
INT_PTR __stdcall ServiceWatchTagItem(WPARAM wParam, LPARAM lParam)
{
	//Usuniecie hasha
	ItemCopyData.Delete(1,1);
	//Struktura kontaktu
	TPluginContact PluginContact;
	ZeroMemory(&PluginContact,sizeof(TPluginContact));
	PluginContact.JID = BotJID.w_str();
	PluginContact.Resource = BotRes.w_str();
	PluginContact.FromPlugin = false;
	PluginContact.UserIdx = BotUsrIdx;
	//Struktura wiadomosci
	TPluginMessage PluginMessage;
	ZeroMemory(&PluginMessage,sizeof(TPluginMessage));
	PluginMessage.cbSize = sizeof(TPluginMessage);
	PluginMessage.JID = BotJID.w_str();
	PluginMessage.Body = ("!tag +" + ItemCopyData).w_str();
	PluginMessage.Store = true;
	PluginMessage.ShowAsOutgoing = true;
	//Wysylanie wiadomosci
	PluginLink.CallService(AQQ_CONTACTS_SENDMSG ,(WPARAM)(&PluginContact),(LPARAM)(&PluginMessage));

	return 0;
}
//---------------------------------------------------------------------------

//Procka okna timera
LRESULT CALLBACK TimerFrmProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg==WM_TIMER)
	{
		//Aktualizacja awatarow po wlaczeniu wtyczki
		if(wParam==TIMER_UPDATE_AVATARS_ONLOAD)
		{
			//Zatrzymanie timera
			KillTimer(hTimerFrm,TIMER_UPDATE_AVATARS_ONLOAD);
			//Wlaczenie timera regularnej aktualizacji
			SetTimer(hTimerFrm,TIMER_UPDATE_AVATARS,3600000,(TIMERPROC)TimerFrmProc);
			//Sprawdzenie aktualizacji awatarow
			AutoAvatarsUpdate();
		}
		//Regularna aktualizacja awatarow
		else if(wParam==TIMER_UPDATE_AVATARS)
		{
			//Sprawdzenie aktualizacji awatarow
			AutoAvatarsUpdate();
		}

		return 0;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
//---------------------------------------------------------------------------

//Hook na aktwyna zakladke lub okno rozmowy (pokazywanie menu do cytowania, tworzenie buttonow)
INT_PTR __stdcall OnActiveTab(WPARAM wParam, LPARAM lParam)
{
	//Pobranie uchwytu do okna rozmowy
	if(!hFrmSend) hFrmSend = (HWND)wParam;
	//Szukanie pola wiadomosci
	if(!hRichEdit) EnumChildWindows(hFrmSend,(WNDENUMPROC)FindRichEdit,0);
	//Pobieranie danych kontatku
	TPluginContact ActiveTabContact = *(PPluginContact)lParam;
	//Pobieranie identyfikatora kontatku
	UnicodeString ContactJID = (wchar_t*)ActiveTabContact.JID;
	//Kontakt jest botem Blabler
	if((ContactJID=="blabler.k2t.eu")||(ContactJID=="blabler@blabler.pl")||(ContactJID.Pos("48263287@plugin.gg")==1))
	{
		//"Wlaczenie" procki zbierajacej adresu URL
		BlockPerformCopyData = false;
		//Zapamietanie danych kontatku
		BotJID = (wchar_t*)ActiveTabContact.JID;
		BotRes = (wchar_t*)ActiveTabContact.Resource;
		BotUsrIdx = ActiveTabContact.UserIdx;
	}
	else
	{
		//"Wylaczenie" procki zbierajacej adresu URL
		BlockPerformCopyData = true;
	}

	return 0;
}
//---------------------------------------------------------------------------

//Hook na pokazywane wiadomosci
INT_PTR __stdcall OnAddLine(WPARAM wParam, LPARAM lParam)
{
	//Pobieranie danych kontatku
	TPluginContact AddLineContact = *(PPluginContact)wParam;
	//Pobieranie identyfikatora kontatku
	UnicodeString ContactJID = (wchar_t*)AddLineContact.JID;
	//Kontakt jest botem Blabler
	if((ContactJID=="blabler.k2t.eu")||(ContactJID=="blabler@blabler.pl")||(ContactJID.Pos("48263287@plugin.gg")==1))
	{
		//Nadawca i odbiorca wiadomosci
		UnicodeString BlablerSender = "";
		UnicodeString BlablerReceiver = "";
		//Pobieranie informacji o wiadomosci
		TPluginMessage AddLineMessage = *(PPluginMessage)lParam;
		UnicodeString MessageDate = (double)AddLineMessage.Date;
		UnicodeString MessageJID = (wchar_t*)AddLineMessage.JID;
		if(MessageJID.Pos("/")) MessageJID.Delete(MessageJID.Pos("/"),MessageJID.Length());
		UnicodeString Body = (wchar_t*)AddLineMessage.Body;
		Body = Body.Trim();

		//Znaki HTMLowe
		if(Body.Pos("&amp;")) Body = StringReplace(Body, "&amp;", "&", TReplaceFlags() << rfReplaceAll);
		if(Body.Pos("&#12288;")) Body = StringReplace(Body, "&#12288;", " ", TReplaceFlags() << rfReplaceAll);
		if(Body.Pos("&#039;")) { Body = StringReplace(Body, "&#039;", "'", TReplaceFlags() << rfReplaceAll); }

		//Formatowanie tagow
		while(Body.Pos("#"))
		{
			//Zmienne tymczasowe
			UnicodeString TempStr = Body;
			UnicodeString TempStr2 = Body;
			//Sprawdzanie poprzedniego znaku/frazy
			TempStr.Delete(TempStr.Pos("#"),TempStr.Length());
			while(TempStr.Pos(" ")) TempStr.Delete(1,TempStr.Pos(" "));
			//Sprawdzanie kolejnego znaku
			TempStr2.Delete(1,TempStr2.Pos("#"));
			TempStr2.Delete(2,TempStr2.Length());
			//Jezeli fraza jest tagiem
			if((AllowedTagsCharacters(TempStr2))
			&&(TempStr.LowerCase().Pos("href=\"")==0)
			&&(TempStr.LowerCase().Pos("title=\"http://")==0)
			&&(TempStr.LowerCase().Pos("title=\"https://")==0)
			&&(TempStr.LowerCase().Pos("title=\"www")==0))
			{
				//Usuwanie hasha z tagu
				UnicodeString TagWithOutHash = Body;
				TagWithOutHash.Delete(1,TagWithOutHash.Pos("#"));
				TagWithOutHash.Delete(TagsCutPosition(TagWithOutHash),TagWithOutHash.Length());
				//Usuwanie polskich znakow
				UnicodeString TagWithOutHashW = TagWithOutHash;
				TagWithOutHashW = TagWithOutHashW.LowerCase();
				TagWithOutHashW = StringReplace(TagWithOutHashW, "�", "e", TReplaceFlags() << rfReplaceAll);
				TagWithOutHashW = StringReplace(TagWithOutHashW, "�", "o", TReplaceFlags() << rfReplaceAll);
				TagWithOutHashW = StringReplace(TagWithOutHashW, "�", "a", TReplaceFlags() << rfReplaceAll);
				TagWithOutHashW = StringReplace(TagWithOutHashW, "�", "s", TReplaceFlags() << rfReplaceAll);
				TagWithOutHashW = StringReplace(TagWithOutHashW, "�", "l", TReplaceFlags() << rfReplaceAll);
				TagWithOutHashW = StringReplace(TagWithOutHashW, "�", "z", TReplaceFlags() << rfReplaceAll);
				TagWithOutHashW = StringReplace(TagWithOutHashW, "�", "z", TReplaceFlags() << rfReplaceAll);
				TagWithOutHashW = StringReplace(TagWithOutHashW, "�", "c", TReplaceFlags() << rfReplaceAll);
				TagWithOutHashW = StringReplace(TagWithOutHashW, "�", "n", TReplaceFlags() << rfReplaceAll);
				//Tworzenie tagu
				UnicodeString TagWithHash = "#" + TagWithOutHash;
				//Formatowanie tagu
				if((TagWithHash.Pos("-")==0)&&(TagWithHash.Pos("_")==0))
				{
					TagWithOutHashW = "[CC_TAGS_LINK]" + TagWithOutHashW + "[CC_TAGS_LINK2][CC_TAGS]" + TagWithOutHash + "[CC_TAGS_LINK3]";
					Body = StringReplace(Body, TagWithHash, TagWithOutHashW, TReplaceFlags());
				}
				else
				{
					TagWithOutHashW = StringReplace(TagWithOutHashW, "-", "", TReplaceFlags() << rfReplaceAll);
					TagWithOutHashW = StringReplace(TagWithOutHashW, "_", "", TReplaceFlags() << rfReplaceAll);
					TagWithOutHashW = "[CC_TAGS_LINK]" + TagWithOutHashW + "[CC_TAGS_LINK2][CC_TAGS]" + TagWithOutHash + "[CC_TAGS_LINK3]";
					Body = StringReplace(Body, TagWithHash, TagWithOutHashW, TReplaceFlags());
				}
			}
			else Body = StringReplace(Body, "#", "[CC_TAGS]", TReplaceFlags());
		}
		Body = StringReplace(Body, "[CC_TAGS_LINK]", "<A HREF=\"http://aqq-link/?url=https://blabler.pl/tag/", TReplaceFlags() << rfReplaceAll);
		Body = StringReplace(Body, "[CC_TAGS_LINK2]", ".html\">", TReplaceFlags() << rfReplaceAll);
		Body = StringReplace(Body, "[CC_TAGS_LINK3]", "</A>", TReplaceFlags() << rfReplaceAll);
		Body = StringReplace(Body, "[CC_TAGS]", "#", TReplaceFlags() << rfReplaceAll);

		//Formatowanie u�ytkownikow
		while(Body.Pos("^"))
		{
			//Zmienne tymczasowe
			UnicodeString TempStr = Body;
			UnicodeString TempStr2 = Body;
			//Sprawdzanie poprzedniego znaku/frazy
			TempStr.Delete(TempStr.Pos("^"),TempStr.Length());
			while(TempStr.Pos(" ")) TempStr.Delete(1,TempStr.Pos(" "));
			//Sprawdzanie kolejnego znaku
			TempStr2.Delete(1,TempStr2.Pos("^"));
			TempStr2.Delete(2,TempStr2.Length());
			//Jezeli fraza jest uzytkownikiem
			if((AllowedUsersCharacters(TempStr2))
			&&(TempStr.LowerCase().Pos("href=\"")==0)
			&&(TempStr.LowerCase().Pos("title=\"http://")==0)
			&&(TempStr.LowerCase().Pos("title=\"https://")==0)
			&&(TempStr.LowerCase().Pos("title=\"www")==0))
			{
				//Usuwanie karety z frazy
				UnicodeString UserWithOutCaret = Body;
				UserWithOutCaret.Delete(1,UserWithOutCaret.Pos("^"));
				UserWithOutCaret.Delete(UsersCutPosition(UserWithOutCaret),UserWithOutCaret.Length());
				//Fraza z kareta
				UnicodeString UserWithCaret = "^" + UserWithOutCaret;
				//Poprawna forma uzytkownika
				if(UserWithCaret.Length()>2)
				{
					//Formatowanie linku
					UserWithOutCaret = "[CC_USERS_LINK]" + UserWithOutCaret + "[CC_USERS_LINK2][CC_USERS]" + UserWithOutCaret + "[CC_USERS_LINK3]";
					Body = StringReplace(Body, UserWithCaret, UserWithOutCaret, TReplaceFlags());
				}
				else
				{
					UserWithOutCaret = "[CC_USERS]" + UserWithOutCaret;
					Body = StringReplace(Body, UserWithCaret, UserWithOutCaret, TReplaceFlags());
				}
			}
			else Body = StringReplace(Body, "^", "[CC_USERS]", TReplaceFlags());
		}
		Body = StringReplace(Body, "[CC_USERS_LINK]", "<A HREF=\"http://aqq-link/?url=https://blabler.pl/u/", TReplaceFlags() << rfReplaceAll);
		Body = StringReplace(Body, "[CC_USERS_LINK2]", ".html\">", TReplaceFlags() << rfReplaceAll);
		Body = StringReplace(Body, "[CC_USERS_LINK3]", "</A>", TReplaceFlags() << rfReplaceAll);
		Body = StringReplace(Body, "[CC_USERS]", "^", TReplaceFlags() << rfReplaceAll);

		//Tworzenie odnosnika dla nadawcy wiadomosci
		if(ContactJID==MessageJID)
		{
			//Wyciaganie pierwszej frazy wiadomosci
			UnicodeString TempStr = Body;
			while(TempStr.Pos(" ")) TempStr.Delete(TempStr.Pos(" "),TempStr.Length());
			//Fraza jest nadawca wiadomosci
			if(TempStr.Pos(":")==TempStr.Length())
			{
				//Tworzenie odnosnika
				TempStr.SetLength(TempStr.Length()-1);
				Body = StringReplace(Body, TempStr + ":", "<B><A HREF=\"http://aqq-link/?url=https://blabler.pl/u/" + TempStr + ".html\">" + TempStr + "</A></B>:", TReplaceFlags());
				//Zapamietywanie nadawcy wiadomosci
				BlablerSender = TempStr;
				//Skip
				goto SkipFormatSender;
			}
		}
		//Tworzenie odnosnika dla nadawcy wiadomosci prywatnej
		if((Body.Pos(" &gt;&gt; "))
		&&(Body.Pos(": "))
		&&(Body.Pos(" &gt;&gt; ")<Body.Pos(": "))
		&&(ContactJID==MessageJID))
		{
			//Pobieranie nadawacy i odbiorcy wiadomosci
			UnicodeString TempStr = Body;
			TempStr.Delete(TempStr.Pos(": "),TempStr.Length());
			TempStr = StringReplace(TempStr, " &gt;&gt; " , "", TReplaceFlags());
			//Fraza jest nadawca i odbiorca wiadomosci
			if(!TempStr.Pos(" "))
			{
				//Tworzenie odnosnika nadawcy wiadomosci
				TempStr = Body;
				TempStr.Delete(TempStr.Pos(" &gt;&gt; "),TempStr.Length());
				Body = StringReplace(Body, TempStr + " &gt;&gt; ", "<B><A HREF=\"http://aqq-link/?url=https://blabler.pl/u/" + TempStr + ".html\">" + TempStr + "</A></B> &gt;&gt; ", TReplaceFlags());
				//Zapamietywanie nadawcy wiadomosci
				BlablerSender = TempStr;
				//Tworzenie odnosnika odbiorcy wiadomosci
				TempStr = Body;
				TempStr.Delete(1,TempStr.Pos(" &gt;&gt; ")+9);
				TempStr.Delete(TempStr.Pos(": "),TempStr.Length());
				Body = StringReplace(Body, TempStr + ": ", "<B><A HREF=\"http://aqq-link/?url=https://blabler.pl/u/" + TempStr + ".html\">" + TempStr + "</A></B>: ", TReplaceFlags());
				//Zapamietywanie odbiorcy wiadomosci
				BlablerReceiver = TempStr;
			}
		}
		//Tworzenie odnosnika dla odbiorcy wiadomosci prywatnej
		else if((Body.Pos("&gt;&gt;")==1)&&(ContactJID!=MessageJID))
		{
			//Wyciaganie odbiorcy wiadomosci prywatnej
			UnicodeString TempStr = Body;
			while(TempStr.Pos(" ")) TempStr.Delete(TempStr.Pos(" "),TempStr.Length());
			if(TempStr.Pos(":")==TempStr.Length())
				TempStr.SetLength(TempStr.Length()-1);
			TempStr.Delete(1,8);
			//Tworzenie odnosnika
			Body = StringReplace(Body, TempStr, "<B><A HREF=\"http://aqq-link/?url=https://blabler.pl/u/" + TempStr + ".html\">" + TempStr + "</A></B>", TReplaceFlags());
		}
		//Tworzenie odnosnika dla nadawcy wiadomosci kierowanej
		else if((Body.Pos(" &gt; "))
		&&(Body.Pos(": "))
		&&(Body.Pos(" &gt; ")<Body.Pos(": "))
		&&(ContactJID==MessageJID))
		{
			//Pobieranie nadawacy i odbiorcy wiadomosci
			UnicodeString TempStr = Body;
			TempStr.Delete(TempStr.Pos(": "),TempStr.Length());
			TempStr = StringReplace(TempStr, " &gt; " , "", TReplaceFlags());
			//Fraza jest nadawca i odbiorca wiadomosci
			if(!TempStr.Pos(" "))
			{
				//Tworzenie odnosnika nadawcy wiadomosci
				TempStr = Body;
				TempStr.Delete(TempStr.Pos(" &gt; "),TempStr.Length());
				Body = StringReplace(Body, TempStr + " &gt; ", "<B><A HREF=\"http://aqq-link/?url=https://blabler.pl/u/" + TempStr + ".html\">" + TempStr + "</A></B> &gt; ", TReplaceFlags());
				//Zapamietywanie nadawcy wiadomosci
				BlablerSender = TempStr;
				//Tworzenie odnosnika odbiorcy wiadomosci
				TempStr = Body;
				TempStr.Delete(1,TempStr.Pos(" &gt; ")+5);
				TempStr.Delete(TempStr.Pos(": "),TempStr.Length());
				Body = StringReplace(Body, TempStr + ": ", "<B><A HREF=\"http://aqq-link/?url=https://blabler.pl/u/" + TempStr + ".html\">" + TempStr + "</A></B>: ", TReplaceFlags());
				//Zapamietywanie odbiorcy wiadomosci
				BlablerReceiver = TempStr;
			}
		}
		//Tworzenie odnosnika dla odbiorcy wiadomosci kierowanej
		else if((Body.Pos("&gt;")==1)&&(ContactJID!=MessageJID))
		{
			//Wyciaganie odbiorcy wiadomosci prywatnej
			UnicodeString TempStr = Body;
			while(TempStr.Pos(" ")) TempStr.Delete(TempStr.Pos(" "),TempStr.Length());
			if(TempStr.Pos(":")==TempStr.Length())
				TempStr.SetLength(TempStr.Length()-1);
			TempStr.Delete(1,4);
			//Tworzenie odnosnika
			Body = StringReplace(Body, TempStr, "<B><A HREF=\"http://aqq-link/?url=https://blabler.pl/u/" + TempStr + ".html\">" + TempStr + "</A></B>", TReplaceFlags());
		}
		//"Wiadomo�� wys�ana do XXX
		else if((Body.Pos("Wiadomo�� wys�ana do ")==1)&&(ContactJID==MessageJID))
		{
			//Wyciaganie nicku odbiorcy wiadomosci
			UnicodeString TempStr = Body;
			TempStr.Delete(1,TempStr.Pos("Wiadomo�� wys�ana do ")+20);
			while(TempStr.Pos(" ")) TempStr.Delete(TempStr.Pos(" "),TempStr.Length());
			//Tworzenie odnosnika
			Body = StringReplace(Body, TempStr, "<B><A HREF=\"http://aqq-link/?url=https://blabler.pl/u/" + TempStr + ".html\">" + TempStr + "</A></B>", TReplaceFlags());
		}
		//Skip
		SkipFormatSender: { /* Only Skip */ }

		//Linkowanie obrazkow i dodawanie zalacznika
		if(Body.Pos("[FOTO]"))
		{
			//Pobieranie ID wiadomosci
			UnicodeString ID = Body;
			ID.Delete(1,ID.Pos(" | <A HREF=\"")+2);
			ID.Delete(ID.Pos("\">"),ID.Length());
			while(ID.Pos("/")) ID.Delete(1,ID.Pos("/"));
			//Szukanie obrazka w cache
			TIniFile *Ini = new TIniFile(CacheDir);
			UnicodeString PhotoURL = Ini->ReadString(ConvertToInt(ID),"Attachment","");
			delete Ini;
			//Adres obrazka nie zapisany w cache
			if(PhotoURL.IsEmpty())
			{
				//Pobieranie danych przez API
				if(GetDataFromAPI("http://api.blabler.pl/updates/"+ID,ConvertToInt(ID)))
				{
					//Ponowne szukanie obrazka w cache
					TIniFile *Ini = new TIniFile(CacheDir);
					PhotoURL = Ini->ReadString(ConvertToInt(ID),"Attachment","");
					delete Ini;
				}
			}
			//Adres obrazka zapisany w cache
			if(!PhotoURL.IsEmpty())
			{
				//Tworzenie odnosnika dla [FOTO]
				Body = StringReplace(Body, "[FOTO]", "<A HREF=\"http://aqq-link/?url=" + PhotoURL + "\" title=\"" + PhotoURL + "\">[FOTO]</A>", TReplaceFlags());
				//Emulacja zalacznika
				if(!AttachmentStyle.IsEmpty())
				{
					//Styl zalacznika
					UnicodeString Attachment = AttachmentStyle;
					//Generowanie ID sesji
					UnicodeString Session = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETSTRID,0,0));
					//Nazwa obrazka
					UnicodeString PhotoFileName = PhotoURL;
					while(PhotoFileName.Pos("/")) PhotoFileName.Delete(1,PhotoFileName.Pos("/"));
					//Pobieranie sciezki URL do grafiki zalacznika
					UnicodeString ThemePNGPath = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPNG_FILEPATH,40,0));
					//Zamiana tagow stylu na dane
					Attachment = StringReplace(Attachment, "CC_ATTACH_ICON", "<IMG border=0 src=\"" + ThemePNGPath + "\">", TReplaceFlags() << rfReplaceAll);
					Attachment = StringReplace(Attachment, "CC_ATTACH_CAPTION", "<SPAN id=id_cctext>Za��cznik</SPAN>", TReplaceFlags() << rfReplaceAll);
					Attachment = StringReplace(Attachment, "CC_ATTACH_SHORT", "<SPAN id=id_cctext><SPAN id=" + Session + "><A HREF=\"image:" + Session + ":" + PhotoURL + "\">" + PhotoFileName + "</A></SPAN></SPAN>", TReplaceFlags() << rfReplaceAll);
					//Dodanie emulacji zalacznika
					Body = Body + Attachment;
				}
			}
		}

		//Wyroznianie wiadomosci
		if((HighlightMsgChk)&&(ContactJID==MessageJID))
		{
			//Jezeli sa jakies elemnty do wyrozniania
			if(HighlightMsgItemsList->Count)
			{
				//Petla wyrozniania wszystkich dodanych fraz
				for(int Count=0;Count<HighlightMsgItemsList->Count;Count++)
				{
					//Pobieranie danych odnosnie
					UnicodeString Item = HighlightMsgItemsList->Strings[Count];
					UnicodeString Color = HighlightMsgColorsList->ReadString("Color",Item,"");
					//Zmiana koloru tekstu (tryb I)
					if(HighlightMsgModeChk==0)
					{
						//Wyrozanie tagow
						if((Item.Pos("#")==1)&&(Body.LowerCase().Pos(Item.LowerCase())))
						{
							UnicodeString ItemBody = Body;
							ItemBody.Delete(1,ItemBody.LowerCase().Pos(Item.LowerCase())+Item.Length()-1);
							ItemBody.SetLength(1);
							if((ItemBody.IsEmpty())||(ItemBody==" ")||(ItemBody=="<"))
							{
								Body = "<div style=\"color: CC_COLOR;\">" + Body + "</div>";
								Body = StringReplace(Body, "CC_COLOR", Color, TReplaceFlags() << rfReplaceAll);
							}
						}
						//Wyrozanianie uzytkownikow
						else if(Item.Pos("^")==1)
						{
							UnicodeString ItemWithOutCaret = StringReplace(Item, "^", "", TReplaceFlags());
							if((Body.LowerCase().Pos(Item.LowerCase()))
							||(ItemWithOutCaret.LowerCase()==BlablerSender.LowerCase())
							||(ItemWithOutCaret.LowerCase()==BlablerReceiver.LowerCase()))
							{
								Body = "<div style=\"color: CC_COLOR;\">" + Body + "</div>";
								Body = StringReplace(Body, "CC_COLOR", Color, TReplaceFlags() << rfReplaceAll);
							}
						}
						//Wyrozanie dowolnych fraz
						else if(Body.LowerCase().Pos(Item.LowerCase()))
						{
							Body = "<div style=\"color: CC_COLOR;\">" + Body + "</div>";
							Body = StringReplace(Body, "CC_COLOR", Color, TReplaceFlags() << rfReplaceAll);
						}
					}
					//Zmiana koloru tekstu (tryb II)
					else if(HighlightMsgModeChk==1)
					{
						//Wyrozanie tagow
						if((Item.Pos("#")==1)&&(Body.LowerCase().Pos(Item.LowerCase())))
						{
							UnicodeString ItemBody = Body;
							ItemBody.Delete(1,ItemBody.LowerCase().Pos(Item.LowerCase())+Item.Length()-1);
							ItemBody.SetLength(1);
							if((ItemBody.IsEmpty())||(ItemBody==" ")||(ItemBody=="<"))
							{
								UnicodeString SelectorID = "MSG" + MessageDate;
								SelectorID = StringReplace(SelectorID, ",", "", TReplaceFlags() << rfReplaceAll);
								Body = "<style>#" + SelectorID + " a { color: CC_COLOR; }</style><div id=\"" + SelectorID + "\" style=\"color: CC_COLOR;\">" + Body + "</div>";
								Body = StringReplace(Body, "CC_COLOR", Color, TReplaceFlags() << rfReplaceAll);
							}
						}
						//Wyrozanianie uzytkownikow
						else if(Item.Pos("^")==1)
						{
							UnicodeString ItemWithOutCaret = StringReplace(Item, "^", "", TReplaceFlags());
							if((Body.LowerCase().Pos(Item.LowerCase()))
							||(ItemWithOutCaret.LowerCase()==BlablerSender.LowerCase())
							||(ItemWithOutCaret.LowerCase()==BlablerReceiver.LowerCase()))
							{
								UnicodeString SelectorID = "MSG" + MessageDate;
								SelectorID = StringReplace(SelectorID, ",", "", TReplaceFlags() << rfReplaceAll);
								Body = "<style>#" + SelectorID + " a { color: CC_COLOR; }</style><div id=\"" + SelectorID + "\" style=\"color: CC_COLOR;\">" + Body + "</div>";
								Body = StringReplace(Body, "CC_COLOR", Color, TReplaceFlags() << rfReplaceAll);
							}
						}
						//Wyrozanie dowolnych fraz
						else if(Body.LowerCase().Pos(Item.LowerCase()))
						{
							UnicodeString SelectorID = "MSG" + MessageDate;
							SelectorID = StringReplace(SelectorID, ",", "", TReplaceFlags() << rfReplaceAll);
							Body = "<style>#" + SelectorID + " a { color: CC_COLOR; }</style><div id=\"" + SelectorID + "\" style=\"color: CC_COLOR;\">" + Body + "</div>";
							Body = StringReplace(Body, "CC_COLOR", Color, TReplaceFlags() << rfReplaceAll);
						}
					}
					//Zmiana koloru frazy
					else if(HighlightMsgModeChk==2)
					{
						//Wyrozanie tagow
						if((Item.Pos("#")==1)&&(Body.LowerCase().Pos(Item.LowerCase())))
						{
							UnicodeString ItemBody = Body;
							ItemBody.Delete(1,ItemBody.LowerCase().Pos(Item.LowerCase())+Item.Length()-1);
							ItemBody.SetLength(1);
							if((ItemBody.IsEmpty())||(ItemBody==" ")||(ItemBody=="<"))
							{
								UnicodeString SelectorID = "MSG" + MessageDate;
								SelectorID = StringReplace(SelectorID, ",", "", TReplaceFlags() << rfReplaceAll);
								Body = "<style>#" + SelectorID + " a { color: CC_COLOR; }</style>" + Body;
								int Pos = Body.LowerCase().Pos(Item.LowerCase());
								Body = Body.Insert("</span>",Pos+Item.Length());
								Body = Body.Insert("<span id=\"" + SelectorID + "\" style=\"color: CC_COLOR;\">",Pos);
								Body = StringReplace(Body, "CC_COLOR", Color, TReplaceFlags() << rfReplaceAll);
							}
						}
						//Wyrozanianie uzytkownikow
						else if(Item.Pos("^")==1)
						{
							//Wyrozanie nadawcow wiadomosci
							UnicodeString ItemWithOutCaret = StringReplace(Item, "^", "", TReplaceFlags());
							if((ItemWithOutCaret.LowerCase()==BlablerSender.LowerCase())||(ItemWithOutCaret.LowerCase()==BlablerReceiver.LowerCase()))
							{
								UnicodeString SelectorID = "MSG" + MessageDate;
								SelectorID = StringReplace(SelectorID, ",", "", TReplaceFlags() << rfReplaceAll);
								Body = "<style>#" + SelectorID + " a { color: CC_COLOR; }</style>" + Body;
								int Pos = Body.LowerCase().Pos((ItemWithOutCaret.LowerCase()+"</a>"));
								Body = Body.Insert("</span>",Pos+ItemWithOutCaret.Length());
								Body = Body.Insert("<span id=\"" + SelectorID + "\" style=\"color: CC_COLOR;\">",Pos);
								Body = StringReplace(Body, "CC_COLOR", Color, TReplaceFlags() << rfReplaceAll);
							}
							//Wyroznianie zacytowanych uzytkownikow
							if(Body.LowerCase().Pos(Item.LowerCase()))
							{
								UnicodeString SelectorID = "MSG" + MessageDate;
								SelectorID = StringReplace(SelectorID, ",", "", TReplaceFlags() << rfReplaceAll);
								Body = "<style>#" + SelectorID + " a { color: CC_COLOR; }</style>" + Body;
								int Pos = Body.LowerCase().Pos(Item.LowerCase());
								Body = Body.Insert("</span>",Pos+Item.Length());
								Body = Body.Insert("<span id=\"" + SelectorID + "\" style=\"color: CC_COLOR;\">",Pos);
								Body = StringReplace(Body, "CC_COLOR", Color, TReplaceFlags() << rfReplaceAll);
							}
						}
					}
					//Zmiana koloru pola wiadomosci
					else if(HighlightMsgModeChk==3)
					{
						//Wyrozanie tagow
						if((Item.Pos("#")==1)&&(Body.LowerCase().Pos(Item.LowerCase())))
						{
							UnicodeString ItemBody = Body;
							ItemBody.Delete(1,ItemBody.LowerCase().Pos(Item.LowerCase())+Item.Length()-1);
							ItemBody.SetLength(1);
							if((ItemBody.IsEmpty())||(ItemBody==" ")||(ItemBody=="<"))
							{
								Body = "<div style=\"padding-bottom: 4px; margin: -4px; border: 4px solid CC_COLOR; border-bottom: 0px; background: none repeat scroll 0 0 CC_COLOR;\">" + Body + "</div>";
								Body = StringReplace(Body, "CC_COLOR", Color, TReplaceFlags() << rfReplaceAll);
							}
						}
						//Wyrozanianie uzytkownikow
						else if(Item.Pos("^")==1)
						{
							UnicodeString ItemWithOutCaret = StringReplace(Item, "^", "", TReplaceFlags());
							if((Body.LowerCase().Pos(Item.LowerCase()))
							||(ItemWithOutCaret.LowerCase()==BlablerSender.LowerCase())
							||(ItemWithOutCaret.LowerCase()==BlablerReceiver.LowerCase()))
							{
								Body = "<div style=\"padding-bottom: 4px; margin: -4px; border: 4px solid CC_COLOR; border-bottom: 0px; background: none repeat scroll 0 0 CC_COLOR;\">" + Body + "</div>";
								Body = StringReplace(Body, "CC_COLOR", Color, TReplaceFlags() << rfReplaceAll);
							}
						}
						//Wyrozanie dowolnych fraz
						else if(Body.LowerCase().Pos(Item.LowerCase()))
						{
							Body = "<div style=\"padding-bottom: 4px; margin: -4px; border: 4px solid CC_COLOR; border-bottom: 0px; background: none repeat scroll 0 0 CC_COLOR;\">" + Body + "</div>";
							Body = StringReplace(Body, "CC_COLOR", Color, TReplaceFlags() << rfReplaceAll);
						}
					}
				}
			}
		}

		//Zacytowane wiadomosci
		if((Body.Pos("http://blabler.pl/s/"))||(Body.Pos("http://blabler.pl/dm/"))||(Body.Pos("http://blabler.pl/pm/"))
		||(Body.Pos("https://blabler.pl/s/"))||(Body.Pos("https://blabler.pl/dm/"))||(Body.Pos("https://blabler.pl/pm/")))
		{
			//Dodawanie specjalnych tagow do wszystkich linkow
			Body = StringReplace(Body, "<A HREF", "[CC_LINK_START]<A HREF", TReplaceFlags() << rfReplaceAll);
			Body = StringReplace(Body, "</A>", "</A>[CC_LINK_END]", TReplaceFlags() << rfReplaceAll);
			while(Body.Pos("[CC_LINK_END]"))
			{
				//Wyciagniecie kodu HTML odnosnika
				UnicodeString URL = Body;
				URL.Delete(1,URL.Pos("[CC_LINK_START]")+14);
				URL.Delete(URL.Pos("[CC_LINK_END]"),URL.Length());
				//Zacytowane wiadomosci
				if(((URL.Pos("http://blabler.pl/s/"))||(URL.Pos("http://blabler.pl/dm/"))||(URL.Pos("http://blabler.pl/pm/"))||(URL.Pos("https://blabler.pl/s/"))||(URL.Pos("https://blabler.pl/dm/"))||(URL.Pos("https://blabler.pl/pm/")))&&(Body.Pos(" | [CC_LINK_START]"+URL)==0))
				{
					//Odnosnik z parametrem title
					if(URL.Pos("title="))
					{
						//Wyciaganie zawartosci title
						UnicodeString Title = URL;
						Title.Delete(1,Title.Pos("title=\"")+6);
						Title.Delete(Title.Pos("\""),Title.Length());
						//Wyciaganie ID wiadomosci
						UnicodeString ID = Title;
						while(ID.Pos("/")) ID.Delete(1,ID.Pos("/"));
						//Szukanie wiadomosci w cache
						TIniFile *Ini = new TIniFile(CacheDir);
						UnicodeString QuoteBody = DecodeBase64(Ini->ReadString(ConvertToInt(ID),"Body64",""));
						UnicodeString From = Ini->ReadString(ConvertToInt(ID),"From","");
						delete Ini;
						//Wiadomosc nie zapisana w cache
						if((QuoteBody.IsEmpty())||(From.IsEmpty()))
						{
							//Pobieranie danych przez API
							if(GetDataFromAPI("http://api.blabler.pl/updates/"+ID,ConvertToInt(ID)))
							{
								//Ponowne szukanie wiadomosci w cache
								TIniFile *Ini = new TIniFile(CacheDir);
								QuoteBody = DecodeBase64(Ini->ReadString(ConvertToInt(ID),"Body64",""));
								From = Ini->ReadString(ConvertToInt(ID),"From","");
								delete Ini;
							}
						}
						//Wiadomosc zapisana w cache
						if((!QuoteBody.IsEmpty())&&(!From.IsEmpty()))
						{
							//Formatowanie tresci zacytowanej wiadomosci
							QuoteBody = StringReplace(QuoteBody, '"', "&quot;", TReplaceFlags() << rfReplaceAll);
							QuoteBody = StringReplace(QuoteBody, ">", "&gt;", TReplaceFlags() << rfReplaceAll);
							QuoteBody = StringReplace(QuoteBody, "<", "&lt;", TReplaceFlags() << rfReplaceAll);
							//Podmiana danych w odnosniku
							UnicodeString tmpURL = URL;
							tmpURL = StringReplace(tmpURL, "title=\""+Title+"\"", "title=\""+QuoteBody+"\"", TReplaceFlags());
							tmpURL = StringReplace(tmpURL, "[blabler.pl]", "[^"+From+"]", TReplaceFlags());
							Body = StringReplace(Body, URL, tmpURL, TReplaceFlags());
						}
					}
					//Odnosnik bez parametru title
					else
					{
						//Wyciaganie ID wiadomosci
						UnicodeString ID = URL;
						ID.Delete(1,ID.Pos(">"));
						ID.Delete(ID.Pos("<"),ID.Length());
						URL = ID;
						while(ID.Pos("/")) ID.Delete(1,ID.Pos("/"));
						//Szukanie wiadomosci w cache
						TIniFile *Ini = new TIniFile(CacheDir);
						UnicodeString QuoteBody = DecodeBase64(Ini->ReadString(ConvertToInt(ID),"Body64",""));;
						UnicodeString From = Ini->ReadString(ConvertToInt(ID),"From","");
						delete Ini;
						//Wiadomosc nie zapisana w cache
						if((QuoteBody.IsEmpty())||(From.IsEmpty()))
						{
							//Pobieranie danych przez API
							if(GetDataFromAPI("http://api.blabler.pl/updates/"+ID,ConvertToInt(ID)))
							{
								//Ponowne szukanie wiadomosci w cache
								TIniFile *Ini = new TIniFile(CacheDir);
								QuoteBody = DecodeBase64(Ini->ReadString(ConvertToInt(ID),"Body64",""));
								From = Ini->ReadString(ConvertToInt(ID),"From","");
								delete Ini;
							}
						}
						//Wiadomosc zapisana w cache
						if((!QuoteBody.IsEmpty())&&(!From.IsEmpty()))
						{
							//Formatowanie tresci zacytowanej wiadomosci
							QuoteBody = StringReplace(QuoteBody, '"', "&quot;", TReplaceFlags() << rfReplaceAll);
							QuoteBody = StringReplace(QuoteBody, ">", "&gt;", TReplaceFlags() << rfReplaceAll);
							QuoteBody = StringReplace(QuoteBody, "<", "&lt;", TReplaceFlags() << rfReplaceAll);
							//Podmiana danych w odnosniku
							Body = StringReplace(Body, URL+"\">"+URL, URL+"\" title=\""+QuoteBody+"\">[^"+From+"]", TReplaceFlags());
						}
						//Pernamentny brak zapisanej wiadomosci w cache
						else
						{
							//Podmiana danych w odnosniku
							Body = StringReplace(Body, URL+"\">"+URL, URL+"\" title=\""+URL+"\">[blabler.pl]", TReplaceFlags());
						}
					}
				}
				//Skracanie koncowki wiadomosci
				else if((Body.Pos(" | [CC_LINK_START]"+URL))&&(URL.Pos("title=")==0))
				{
					//Wyciaganie adresu URL
					URL.Delete(1,URL.Pos(">"));
					URL.Delete(URL.Pos("<"),URL.Length());
					//Podmiana danych w odnosniku
					Body = StringReplace(Body, URL+"\">"+URL, URL+"\" title=\""+URL+"\">[blabler.pl]", TReplaceFlags());
				}
				//Usuwanie wczesniej dodanych tagow
				Body = StringReplace(Body, "[CC_LINK_START]", "", TReplaceFlags());
				Body = StringReplace(Body, "[CC_LINK_END]", "", TReplaceFlags());
			}
		}

		//Dodawanie awatarow
		if((ContactJID==MessageJID)&&(!BlablerSender.IsEmpty()))
		{
			//Tworzenie katalogu z awatarami
			if(!DirectoryExists(AvatarsDir))
				CreateDir(AvatarsDir);
			//Zmienna awataru
			UnicodeString Avatars;
			//Awatara nie ma w folderze cache
			if(!FileExists(AvatarsDir + "\\\\" + BlablerSender))
			{
				//Wstawienie online'owego awatara
				Avatars = StringReplace(AvatarStyle, "CC_AVATAR", "<a href=\"http://aqq-link/?url=https://blabler.pl/u/" + BlablerSender + ".html\"><img class=\"blabler-avatar\" border=\"0px\" src=\"http://api.blabler.pl/avatar/" + BlablerSender + "/standard\" width=\"" + IntToStr(AvatarSize) + "px\" height=\"" + IntToStr(AvatarSize) + "px\"></a>", TReplaceFlags() << rfReplaceAll);
				//Dodanie awatara do pobrania
				GetAvatarsList->Add(BlablerSender);
				//Przypisanie uchwytu do formy ustawien
				if(!hSettingsForm)
				{
					Application->Handle = (HWND)SettingsForm;
					hSettingsForm = new TSettingsForm(Application);
				}
				//Wlaczenie watku
				if(!hSettingsForm->GetAvatarsThread->Active) hSettingsForm->GetAvatarsThread->Start();
			}
			//Awatar znajduje sie w folderze cache
			else Avatars = StringReplace(AvatarStyle, "CC_AVATAR", "<a href=\"http://aqq-link/?url=https://blabler.pl/u/" + BlablerSender + ".html\"><img class=\"blabler-avatar\" border=\"0px\" src=\"file:///" + AvatarsDirW + "/" + BlablerSender + "\" width=\"" + IntToStr(AvatarSize) + "px\" height=\"" + IntToStr(AvatarSize) + "px\"></a>", TReplaceFlags() << rfReplaceAll);
			//Dodanie awatar(a/ow) do tresci wiadomosci
			Body = Avatars + Body;
			//Awatar dla odbiorcy wiadomosci
			if(!BlablerReceiver.IsEmpty())
			{
				//Awatara nie ma w folderze cache
				if(!FileExists(AvatarsDir + "\\\\" + BlablerReceiver))
				{
					//Wstawienie online'owego awatara
					Avatars = StringReplace(AvatarStyle, "CC_AVATAR", "<a href=\"http://aqq-link/?url=https://blabler.pl/u/" + BlablerReceiver + ".html\"><img class=\"blabler-avatar\" border=\"0px\" src=\"http://api.blabler.pl/avatar/" + BlablerReceiver + "/standard\" width=\"" + IntToStr(AvatarSize) + "px\" height=\"" + IntToStr(AvatarSize) + "px\"></a>", TReplaceFlags() << rfReplaceAll);
					//Dodanie awatara do pobrania
					GetAvatarsList->Add(BlablerReceiver);
					//Przypisanie uchwytu do formy ustawien
					if(!hSettingsForm)
					{
						Application->Handle = (HWND)SettingsForm;
						hSettingsForm = new TSettingsForm(Application);
					}
					//Wlaczenie watku
					if(!hSettingsForm->GetAvatarsThread->Active) hSettingsForm->GetAvatarsThread->Start();
				}
				//Awatar znajduje sie w folderze cache
				else Avatars = StringReplace(AvatarStyle, "CC_AVATAR", "<a href=\"http://aqq-link/?url=https://blabler.pl/u/" + BlablerReceiver + ".html\"><img class=\"blabler-avatar\" border=\"0px\" src=\"file:///" + AvatarsDirW + "/" + BlablerReceiver + "\" width=\"" + IntToStr(AvatarSize) + "px\" height=\"" + IntToStr(AvatarSize) + "px\"></a>", TReplaceFlags() << rfReplaceAll);
				//Wstawienie awataru do tresci wiadomosci
				Body = StringReplace(Body, "&gt; ", "&gt;<span style=\"padding-left: 2px;\"></span>"+Avatars, TReplaceFlags());
			}
		}

		//Zwrocenie zmodyfikowanej wiadomosci;
		AddLineMessage.Body = Body.w_str();
		memcpy((PPluginMessage)lParam,&AddLineMessage, sizeof(TPluginMessage));
		return 2;
	}
	//Zacytowane wiadomosci poza botem Blablera
	else
	{
		//Pobieranie informacji o wiadomosci
		TPluginMessage AddLineMessage = *(PPluginMessage)lParam;
		UnicodeString Body = (wchar_t*)AddLineMessage.Body;
		Body = Body.Trim();
		//Tekst zawiera zacytowane wiadomosci
		if((Body.Pos("http://blabler.pl/s/"))||(Body.Pos("http://blabler.pl/dm/"))||(Body.Pos("http://blabler.pl/pm/"))
		||(Body.Pos("https://blabler.pl/s/"))||(Body.Pos("https://blabler.pl/dm/"))||(Body.Pos("https://blabler.pl/pm/")))
		{
			//Dodawanie specjalnych tagow do wszystkich linkow
			Body = StringReplace(Body, "<A HREF", "[CC_LINK_START]<A HREF", TReplaceFlags() << rfReplaceAll);
			Body = StringReplace(Body, "</A>", "</A>[CC_LINK_END]", TReplaceFlags() << rfReplaceAll);
			while(Body.Pos("[CC_LINK_END]"))
			{
				//Wyciagniecie kodu HTML odnosnika
				UnicodeString URL = Body;
				URL.Delete(1,URL.Pos("[CC_LINK_START]")+14);
				URL.Delete(URL.Pos("[CC_LINK_END]"),URL.Length());
				//Zacytowane wiadomosci
				if((URL.Pos("http://blabler.pl/s/"))||(URL.Pos("http://blabler.pl/dm/"))||(URL.Pos("http://blabler.pl/pm/"))
				||(URL.Pos("https://blabler.pl/s/"))||(URL.Pos("https://blabler.pl/dm/"))||(URL.Pos("https://blabler.pl/pm/")))
				{
					//Odnosnik z parametrem title
					if(URL.Pos("title="))
					{
						//Wyciaganie zawartosci title
						UnicodeString Title = URL;
						Title.Delete(1,Title.Pos("title=\"")+6);
						Title.Delete(Title.Pos("\""),Title.Length());
						//Wyciaganie ID wiadomosci
						UnicodeString ID = Title;
						while(ID.Pos("/")) ID.Delete(1,ID.Pos("/"));
						//Szukanie wiadomosci w cache
						TIniFile *Ini = new TIniFile(CacheDir);
						UnicodeString QuoteBody = DecodeBase64(Ini->ReadString(ConvertToInt(ID),"Body64",""));
						UnicodeString From = Ini->ReadString(ConvertToInt(ID),"From","");
						delete Ini;
						//Wiadomosc nie zapisana w cache
						if((QuoteBody.IsEmpty())||(From.IsEmpty()))
						{
							//Pobieranie danych przez API
							if(GetDataFromAPI("http://api.blabler.pl/updates/"+ID,ConvertToInt(ID)))
							{
								//Ponowne szukanie wiadomosci w cache
								TIniFile *Ini = new TIniFile(CacheDir);
								QuoteBody = DecodeBase64(Ini->ReadString(ConvertToInt(ID),"Body64",""));
								From = Ini->ReadString(ConvertToInt(ID),"From","");
								delete Ini;
							}
						}
						//Wiadomosc zapisana w cache
						if((!QuoteBody.IsEmpty())&&(!From.IsEmpty()))
						{
							//Formatowanie tresci zacytowanej wiadomosci
							QuoteBody = StringReplace(QuoteBody, '"', "&quot;", TReplaceFlags() << rfReplaceAll);
							QuoteBody = StringReplace(QuoteBody, ">", "&gt;", TReplaceFlags() << rfReplaceAll);
							QuoteBody = StringReplace(QuoteBody, "<", "&lt;", TReplaceFlags() << rfReplaceAll);
							//Podmiana danych w odnosniku
							UnicodeString tmpURL = URL;
							tmpURL = StringReplace(tmpURL, "title=\""+Title+"\"", "title=\""+From+": "+QuoteBody+"\"", TReplaceFlags());
							Body = StringReplace(Body, URL, tmpURL, TReplaceFlags());
						}
					}
					//Odnosnik bez parametru title
					else
					{
						//Wyciaganie ID wiadomosci
						UnicodeString ID = URL;
						ID.Delete(1,ID.Pos(">"));
						ID.Delete(ID.Pos("<"),ID.Length());
						URL = ID;
						while(ID.Pos("/")) ID.Delete(1,ID.Pos("/"));
						//Szukanie wiadomosci w cache
						TIniFile *Ini = new TIniFile(CacheDir);
						UnicodeString QuoteBody = DecodeBase64(Ini->ReadString(ConvertToInt(ID),"Body64",""));
						UnicodeString From = Ini->ReadString(ConvertToInt(ID),"From","");
						delete Ini;
						//Wiadomosc nie zapisana w cache
						if((QuoteBody.IsEmpty())||(From.IsEmpty()))
						{
							//Pobieranie danych przez API
							if(GetDataFromAPI("http://api.blabler.pl/updates/"+ID,ConvertToInt(ID)))
							{
								//Ponowne szukanie wiadomosci w cache
								TIniFile *Ini = new TIniFile(CacheDir);
								QuoteBody = DecodeBase64(Ini->ReadString(ConvertToInt(ID),"Body64",""));
								From = Ini->ReadString(ConvertToInt(ID),"From","");
								delete Ini;
							}
						}
						//Wiadomosc zapisana w cache
						if((!QuoteBody.IsEmpty())&&(!From.IsEmpty()))
						{
							//Formatowanie tresci zacytowanej wiadomosci
							QuoteBody = StringReplace(QuoteBody, '"', "&quot;", TReplaceFlags() << rfReplaceAll);
							QuoteBody = StringReplace(QuoteBody, ">", "&gt;", TReplaceFlags() << rfReplaceAll);
							QuoteBody = StringReplace(QuoteBody, "<", "&lt;", TReplaceFlags() << rfReplaceAll);
							//Podmiana danych w odnosniku
							Body = StringReplace(Body, URL+"\">"+URL, URL+"\" title=\""+From+": "+QuoteBody+"\">"+URL, TReplaceFlags());
						}
					}
				}
				//Usuwanie wczesniej dodanych tagow
				Body = StringReplace(Body, "[CC_LINK_START]", "", TReplaceFlags());
				Body = StringReplace(Body, "[CC_LINK_END]", "", TReplaceFlags());
			}
			//Zwrocenie zmodyfikowanej wiadomosci;
			AddLineMessage.Body = Body.w_str();
			memcpy((PPluginMessage)lParam,&AddLineMessage, sizeof(TPluginMessage));
			return 2;
		}
	}

	return 0;
}
//---------------------------------------------------------------------------

//Hook na zmiane kolorystyki AlphaControls
INT_PTR __stdcall OnColorChange(WPARAM wParam, LPARAM lParam)
{
	//Okno ustawien zostalo juz stworzone
	if(hSettingsForm)
	{
		//Wlaczona zaawansowana stylizacja okien
		if(ChkSkinEnabled())
		{
			TPluginColorChange ColorChange = *(PPluginColorChange)wParam;
			hSettingsForm->sSkinManager->HueOffset = ColorChange.Hue;
			hSettingsForm->sSkinManager->Saturation = ColorChange.Saturation;
			hSettingsForm->sSkinManager->Brightness = ColorChange.Brightness;
		}
	}

	return 0;
}
//---------------------------------------------------------------------------

//Hook na zaladowanie wszystkich modulow w AQQ (autoupdate awatarow)
INT_PTR __stdcall OnModulesLoaded(WPARAM wParam, LPARAM lParam)
{
	//Timer aktualizacji awatarow po wlaczeniu wtyczki
	SetTimer(hTimerFrm,TIMER_UPDATE_AVATARS_ONLOAD,300000,(TIMERPROC)TimerFrmProc);
	
	return 0;
}
//---------------------------------------------------------------------------

//Hook na pobieranie adresow URL z roznych popup
INT_PTR __stdcall OnPerformCopyData(WPARAM wParam, LPARAM lParam)
{
	//Domyslne usuwanie elementow
	DestroyQuoteMsgItem();
	DestroyInsertTagItem();
	DestroyLikeMsgItem();
	DestroyInsertNickItem();
	DestroySendMsgItem();
	DestroySendPrivMsgItem();
	DestroyWatchUserItem();
	DestroyWatchTagItem();
	DestroySeparatorItem();
	//Kasowanie zapamietanych danych
	ItemCopyData = "";
	//Jezeli zezwolono na sprawdzanie danych
	if(!BlockPerformCopyData)
	{
		//Pobranie danych
		UnicodeString CopyData = (wchar_t*)lParam;
		//Cytowanie
		if((CopyData.Pos("http://blabler.pl/s/")==1)||(CopyData.Pos("http://blabler.pl/dm/")==1)
		||(CopyData.Pos("https://blabler.pl/s/")==1)||(CopyData.Pos("https://blabler.pl/dm/")==1))
		{
			//Kopiowanie URL
			ItemCopyData = CopyData;
			//Tworzenie separatora
			BuildSeparatorItem();
			//Tworzenie elementu lajkowania
			BuildLikeMsgItem();
			//Tworzenie elementu cytowania
			BuildQuoteMsgItem();
		}
		//Tagi
		else if((CopyData.Pos("http://blabler.pl/tag/")==1)||(CopyData.Pos("https://blabler.pl/tag/")==1))
		{
			//Wyciaganie tag'u
			CopyData.Delete(1,CopyData.Pos("/tag/")+4);
			CopyData.Delete(CopyData.Pos(".html"),CopyData.Length());
			if(!CopyData.IsEmpty())
			{
				//Kopiowanie tag'u
				ItemCopyData = "#" + CopyData;
				//Tworzenie separatora
				BuildSeparatorItem();
				//Tworzenie elementu obserwowania tagu
				BuildWatchTagItem();
				//Tworzenie elementu wstawiania tagu
				BuildInsertTagItem();
			}
		}
		//Uzytkownicy
		else if((CopyData.Pos("http://blabler.pl/u/")==1)||(CopyData.Pos("https://blabler.pl/u/")==1))
		{
			//Wyciaganie nick'a
			CopyData.Delete(1,CopyData.Pos("/u/")+2);
			CopyData.Delete(CopyData.Pos(".html"),CopyData.Length());
			if(!CopyData.IsEmpty())
			{
				//Kopiowanie nick'a
				ItemCopyData = CopyData;
				//Tworzenie separatora
				BuildSeparatorItem();
				//Tworzenie elementu obserowania uzytkownika
				BuildWatchUserItem();
				//Tworzenie elementu wstawiania nicku
				BuildInsertNickItem();
				//Tworzenie elementu wysylania prywatnej wiadomosci
				BuildSendPrivMsgItem();
				//Tworzenie elementu wysylania wiadomosci
				BuildSendMsgItem();
			}
		}
	}

	return 0;
}
//---------------------------------------------------------------------------

INT_PTR __stdcall OnPrimaryTab (WPARAM wParam, LPARAM lParam)
{
	//Pobranie uchwytu do okna rozmowy
	if(!hFrmSend) hFrmSend = (HWND)wParam;
	//Szukanie pola wiadomosci
	if(!hRichEdit) EnumChildWindows(hFrmSend,(WNDENUMPROC)FindRichEdit,0);
	//Pobieranie danych kontatku
	TPluginContact PrimaryTabContact = *(PPluginContact)lParam;
	//Pobieranie identyfikatora kontatku
	UnicodeString ContactJID = (wchar_t*)PrimaryTabContact.JID;
	//Kontakt jest botem Blabler
	if((ContactJID=="blabler.k2t.eu")||(ContactJID=="blabler@blabler.pl")||(ContactJID.Pos("48263287@plugin.gg")==1))
	{
		//"Wlaczenie" procki zbierajacej adresu URL
		BlockPerformCopyData = false;
		//Zapamietanie danych kontatku
		BotJID = (wchar_t*)PrimaryTabContact.JID;
		BotRes = (wchar_t*)PrimaryTabContact.Resource;
		BotUsrIdx = PrimaryTabContact.UserIdx;
	}
	else
	{
		//"Wylaczenie" procki zbierajacej adresu URL
		BlockPerformCopyData = true;
	}

	return 0;
}
//---------------------------------------------------------------------------

//Hook na zmiane kompozycji (pobranie stylu zalacznikow oraz zmiana skorkowania wtyczki)
INT_PTR __stdcall OnThemeChanged(WPARAM wParam, LPARAM lParam)
{
	//Okno ustawien zostalo juz stworzone
	if(hSettingsForm)
	{
		//Wlaczona zaawansowana stylizacja okien
		if(ChkSkinEnabled())
		{
			//Pobieranie sciezki nowej aktywnej kompozycji
			UnicodeString ThemeSkinDir = StringReplace((wchar_t*)lParam, "\\", "\\\\", TReplaceFlags() << rfReplaceAll) + "\\\\Skin";
			//Plik zaawansowanej stylizacji okien istnieje
			if(FileExists(ThemeSkinDir + "\\\\Skin.asz"))
			{
				//Dane pliku zaawansowanej stylizacji okien
				ThemeSkinDir = StringReplace(ThemeSkinDir, "\\\\", "\\", TReplaceFlags() << rfReplaceAll);
				hSettingsForm->sSkinManager->SkinDirectory = ThemeSkinDir;
				hSettingsForm->sSkinManager->SkinName = "Skin.asz";
				//Ustawianie animacji AlphaControls
				if(ChkThemeAnimateWindows()) hSettingsForm->sSkinManager->AnimEffects->FormShow->Time = 200;
				else hSettingsForm->sSkinManager->AnimEffects->FormShow->Time = 0;
				hSettingsForm->sSkinManager->Effects->AllowGlowing = ChkThemeGlowing();
				//Zmiana kolorystyki AlphaControls
				hSettingsForm->sSkinManager->HueOffset = GetHUE();
				hSettingsForm->sSkinManager->Saturation = GetSaturation();
				hSettingsForm->sSkinManager->Brightness = GetBrightness();
				//Aktywacja skorkowania AlphaControls
				hSettingsForm->sSkinManager->Active = true;
			}
			//Brak pliku zaawansowanej stylizacji okien
			else hSettingsForm->sSkinManager->Active = false;
		}
		//Zaawansowana stylizacja okien wylaczona
		else hSettingsForm->sSkinManager->Active = false;
		//Kolor labelow
		if(hSettingsForm->sSkinManager->Active)
		{
			hSettingsForm->UsedAvatarsStyleLabel->Font->Color = GetWarningColor();
			hSettingsForm->LastAvatarsUpdateLabel->Font->Color = hSettingsForm->UsedAvatarsStyleLabel->Font->Color;
		}
		else
		{
			hSettingsForm->UsedAvatarsStyleLabel->Font->Color = clGreen;
			hSettingsForm->LastAvatarsUpdateLabel->Font->Color = clGreen;
		}
	}
	//Pobranie stylu Attachment & Avatars
	GetThemeStyle();

	return 0;
}
//---------------------------------------------------------------------------

//Hook na odbieranie pakietow XML
INT_PTR __stdcall OnXMLDebug(WPARAM wParam, LPARAM lParam)
{
	//Pobranie informacji nt. pakietu XML
	TPluginXMLChunk XMLChunk = *(PPluginXMLChunk)lParam ;
	//Pobranie nadawcy pakietu XML
	UnicodeString XMLSender = (wchar_t*)XMLChunk.From;
	//Pakiet wyslany od bota Blablera
	if((XMLSender=="blabler.k2t.eu")||(XMLSender=="blabler@blabler.pl"))
	{
		//Zmienna ID wiadomosci
		UnicodeString MsgID;
		//Pobranie pakieru XML
		UnicodeString XML = (wchar_t*)wParam;
		//Kodowanie pakietu
		XML = UTF8ToUnicodeString(XML.w_str());
		//Parsowanie pakietu
		_di_IXMLDocument XMLDoc = LoadXMLData(XML);
		_di_IXMLNode MainNode = XMLDoc->DocumentElement;
		int ItemsCount = MainNode->ChildNodes->GetCount();
		for(int Count=0;Count<ItemsCount;Count++)
		{
			_di_IXMLNode ChildNodes = MainNode->ChildNodes->GetNode(Count);
			//Pobieranie ID wiadomosci
			if(ChildNodes->NodeName=="body")
			{
				MsgID = ChildNodes->GetText();
				while(MsgID.Pos("/")) MsgID.Delete(1,MsgID.Pos("/"));
			}
			//Wiadomosc zawiera dodatkowe informacje
			if(ChildNodes->NodeName=="x")
			{
				//Pobieranie typu dodatkoweg pakietu
				UnicodeString xmlns = ChildNodes->Attributes["xmlns"];
				//Informacja o zacytowanych wiadomosciach
				if(xmlns=="urn:k2t:blabler:quote:0")
				{
					int ChildItemsCount = ChildNodes->ChildNodes->GetCount();
					for(int ChildCount=0;ChildCount<ChildItemsCount;ChildCount++)
					{
						_di_IXMLNode ChildNode = ChildNodes->ChildNodes->GetNode(ChildCount);
						if(ChildNode->NodeName=="quote")
						{
							//Pobieranie informacji nt. cytowanej wiadomosci
							UnicodeString ID = ChildNode->Attributes["id"];
							UnicodeString From = ChildNode->Attributes["from"];
							UnicodeString Text = ChildNode->GetText();
							//Zapisywanie informacji do cache
							TIniFile *Ini = new TIniFile(CacheDir);
							Ini->WriteString(ConvertToInt(ID),"Body64",EncodeBase64(Text));
							Ini->WriteString(ConvertToInt(ID),"From",From);
							delete Ini;
						}
					}
				}
				//Informacja o zalaczonych obrazkach
				else if(xmlns=="urn:k2t:blabler:attachment:0")
				{
					int ChildItemsCount = ChildNodes->ChildNodes->GetCount();
					for(int ChildCount=0;ChildCount<ChildItemsCount;ChildCount++)
					{
						_di_IXMLNode ChildNode = ChildNodes->ChildNodes->GetNode(ChildCount);
						if(ChildNode->NodeName=="attachment")
						{
							//Pobieranie informacji zalaczonego obrazka
							UnicodeString URL = ChildNode->GetText();
							//Zapisywanie informacji do cache
							TIniFile *Ini = new TIniFile(CacheDir);
							Ini->WriteString(ConvertToInt(MsgID),"Attachment",URL);
							delete Ini;
						}
					}
				}
			}
		}
	}

	return 0;
}
//---------------------------------------------------------------------------

//Zapisywanie zasob�w
void ExtractRes(wchar_t* FileName, wchar_t* ResName, wchar_t* ResType)
{
	TPluginTwoFlagParams PluginTwoFlagParams;
	PluginTwoFlagParams.cbSize = sizeof(TPluginTwoFlagParams);
	PluginTwoFlagParams.Param1 = ResName;
	PluginTwoFlagParams.Param2 = ResType;
	PluginTwoFlagParams.Flag1 = (int)HInstance;
	PluginLink.CallService(AQQ_FUNCTION_SAVERESOURCE,(WPARAM)&PluginTwoFlagParams,(LPARAM)FileName);
}
//---------------------------------------------------------------------------

//Obliczanie sumy kontrolnej pliku
UnicodeString MD5File(UnicodeString FileName)
{
	if(FileExists(FileName))
	{
		UnicodeString Result;
		TFileStream *fs;
		fs = new TFileStream(FileName, fmOpenRead | fmShareDenyWrite);
		try
		{
			TIdHashMessageDigest5 *idmd5= new TIdHashMessageDigest5();
			try
			{
				Result = idmd5->HashStreamAsHex(fs);
			}
			__finally
			{
				delete idmd5;
			}
		}
		__finally
		{
			delete fs;
		}
		return Result;
	}
	else return 0;
}
//---------------------------------------------------------------------------

//Odczyt ustawien
void LoadSettings()
{
	TIniFile *Ini = new TIniFile(GetPluginUserDir()+"\\\\Blabler\\\\Settings.ini");
	//Awatary
	AvatarSize = Ini->ReadInteger("Avatars","Size",25);
	StaticAvatarStyle = DecodeBase64(Ini->ReadString("Avatars","Style64","")).Trim();
	if(StaticAvatarStyle=="<span style=\"display: inline-block; padding: 2px 4px 0px 1px; vertical-align: middle;\">CC_AVATAR</span>")
		StaticAvatarStyle = "";
	//Wyroznianie
	HighlightMsgChk = Ini->ReadBool("HighlightMsg","Enabled",false);
	HighlightMsgModeChk= Ini->ReadInteger("HighlightMsg","Mode",0);
	HighlightMsgItemsList->Clear();
	HighlightMsgColorsList->EraseSection("Color");
	TStringList *HighlightItems = new TStringList;
	Ini->ReadSection("HighlightItems",HighlightItems);
	if(HighlightItems->Count)
	{
		for(int Count=0;Count<HighlightItems->Count;Count++)
		{
			UnicodeString ItemName = HighlightItems->Strings[Count];
			UnicodeString HighlightItem = Ini->ReadString("HighlightItems",ItemName,"");
			if(!HighlightItem.IsEmpty())
			{
				UnicodeString Item = HighlightItem;
				Item = Item.Delete(Item.Pos(";"),Item.Length());
				UnicodeString Color = HighlightItem;
				Color = Color.Delete(1,Color.Pos(";"));
				HighlightMsgItemsList->Add(Item);
				HighlightMsgColorsList->WriteString("Color",Item,Color);
			}
		}
	}
	delete HighlightItems;
	delete Ini;
}
//---------------------------------------------------------------------------

extern "C" INT_PTR __declspec(dllexport) __stdcall Load(PPluginLink Link)
{
	//Linkowanie wtyczki z komunikatorem
	PluginLink = *Link;
	//Sciezka folderu prywatnego wtyczek
	UnicodeString PluginUserDir = GetPluginUserDir();
	//Tworzenie katalogow lokalizacji
	if(!DirectoryExists(PluginUserDir+"\\\\Languages"))
		CreateDir(PluginUserDir+"\\\\Languages");
	if(!DirectoryExists(PluginUserDir+"\\\\Languages\\\\Blabler"))
		CreateDir(PluginUserDir+"\\\\Languages\\\\Blabler");
	if(!DirectoryExists(PluginUserDir+"\\\\Languages\\\\Blabler\\\\EN"))
		CreateDir(PluginUserDir+"\\\\Languages\\\\Blabler\\\\EN");
	if(!DirectoryExists(PluginUserDir+"\\\\Languages\\\\Blabler\\\\PL"))
		CreateDir(PluginUserDir+"\\\\Languages\\\\Blabler\\\\PL");
	//Wypakowanie plikow lokalizacji
	//C8F1D0F4BFD75C1D38751836B649A6C6
	if(!FileExists(PluginUserDir+"\\\\Languages\\\\Blabler\\\\EN\\\\Const.lng"))
		ExtractRes((PluginUserDir+"\\\\Languages\\\\Blabler\\\\EN\\\\Const.lng").w_str(),L"EN_CONST",L"DATA");
	else if(MD5File(PluginUserDir+"\\\\Languages\\\\Blabler\\\\EN\\\\Const.lng")!="C8F1D0F4BFD75C1D38751836B649A6C6")
		ExtractRes((PluginUserDir+"\\\\Languages\\\\Blabler\\\\EN\\\\Const.lng").w_str(),L"EN_CONST",L"DATA");
	//B8BD774791826418DE8FF1F512F32875
	if(!FileExists(PluginUserDir+"\\\\Languages\\\\Blabler\\\\EN\\\\TSettingsForm.lng"))
		ExtractRes((PluginUserDir+"\\\\Languages\\\\Blabler\\\\EN\\\\TSettingsForm.lng").w_str(),L"EN_SETTINGSFRM",L"DATA");
	else if(MD5File(PluginUserDir+"\\\\Languages\\\\Blabler\\\\EN\\\\TSettingsForm.lng")!="B8BD774791826418DE8FF1F512F32875")
		ExtractRes((PluginUserDir+"\\\\Languages\\\\Blabler\\\\EN\\\\TSettingsForm.lng").w_str(),L"EN_SETTINGSFRM",L"DATA");
	//05B6674FE6241E679362FF4DB9278E16
	if(!FileExists(PluginUserDir+"\\\\Languages\\\\Blabler\\\\PL\\\\Const.lng"))
		ExtractRes((PluginUserDir+"\\\\Languages\\\\Blabler\\\\PL\\\\Const.lng").w_str(),L"PL_CONST",L"DATA");
	else if(MD5File(PluginUserDir+"\\\\Languages\\\\Blabler\\\\PL\\\\Const.lng")!="05B6674FE6241E679362FF4DB9278E16")
		ExtractRes((PluginUserDir+"\\\\Languages\\\\Blabler\\\\PL\\\\Const.lng").w_str(),L"PL_CONST",L"DATA");
	//257B0F961AB2E1117167CF83E510D471
	if(!FileExists(PluginUserDir+"\\\\Languages\\\\Blabler\\\\PL\\\\TSettingsForm.lng"))
		ExtractRes((PluginUserDir+"\\\\Languages\\\\Blabler\\\\PL\\\\TSettingsForm.lng").w_str(),L"PL_SETTINGSFRM",L"DATA");
	else if(MD5File(PluginUserDir+"\\\\Languages\\\\Blabler\\\\PL\\\\TSettingsForm.lng")!="257B0F961AB2E1117167CF83E510D471")
		ExtractRes((PluginUserDir+"\\\\Languages\\\\Blabler\\\\PL\\\\TSettingsForm.lng").w_str(),L"PL_SETTINGSFRM",L"DATA");
	//Ustawienie sciezki lokalizacji wtyczki
	UnicodeString LangCode = (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETLANGCODE,0,0);
	LangPath = PluginUserDir + "\\\\Languages\\\\Blabler\\\\" + LangCode + "\\\\";
	if(!DirectoryExists(LangPath))
	{
		LangCode = (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETDEFLANGCODE,0,0);
		LangPath = PluginUserDir + "\\\\Languages\\\\Blabler\\\\" + LangCode + "\\\\";
	}
	//Wypakiwanie ikonki Blabler.dll.png
	//984BE6E674B8DD8C48B4C20EC0913586
	/*if(!DirectoryExists(PluginUserDir+"\\\\Shared"))
		CreateDir(PluginUserDir+"\\\\Shared");
	if(!FileExists(PluginUserDir+"\\\\Shared\\\\Blabler.dll.png"))
		ExtractRes((PluginUserDir+"\\\\Shared\\\\Blabler.dll.png").w_str(),L"SHARED",L"DATA");
	else if(MD5File(PluginUserDir+"\\\\Shared\\\\Blabler.dll.png")!="984BE6E674B8DD8C48B4C20EC0913586")
		ExtractRes((PluginUserDir+"\\\\Shared\\\\Blabler.dll.png").w_str(),L"SHARED",L"DATA");*/
	//Tworzeniu katalogu z ustawieniami wtyczki
	if(!DirectoryExists(PluginUserDir+"\\\\Blabler"))
		CreateDir(PluginUserDir+"\\\\Blabler");
	//Sciezka katalogu z awatarami
	AvatarsDir = PluginUserDir+"\\\\Blabler\\\\Avatars";
	AvatarsDirW = StringReplace(AvatarsDir, "\\\\", "/", TReplaceFlags() << rfReplaceAll);
	//Tworzenie katalogu z awatarami
	if(!DirectoryExists(AvatarsDir))
		CreateDir(AvatarsDir);
	//Sciezka do pliku cache
	CacheDir = PluginUserDir+"\\\\Blabler\\\\Cache.ini";
	//Tworzenie serwisow
	//Szybki dostep do ustawien wtyczki
	PluginLink.CreateServiceFunction(L"sBlablerFastSettingsItem",ServiceBlablerFastSettingsItem);
	//Serwis do wstawiania danego tekstu
	PluginLink.CreateServiceFunction(L"sBlablerInsertItem",ServiceInsertItem);
	//Serwis do lajkowania wiadomosci
	PluginLink.CreateServiceFunction(L"sBlablerLikeMsgItem",ServiceLikeMsgItem);
	//Serwis do wstawiania nicku
	PluginLink.CreateServiceFunction(L"sBlablerInsertNickItem",ServiceInsertNickItem);
	//Serwis do wysylania wiadomosci prywatnej
	PluginLink.CreateServiceFunction(L"sBlablerSendMsgItem",ServiceSendMsgItem);
	//Serwis do wysylania wiadomosci prywatnej
	PluginLink.CreateServiceFunction(L"sBlablerSendPrivMsgItem",ServiceSendPrivMsgItem);
	//Serwis do obserowania uzytownika
	PluginLink.CreateServiceFunction(L"sBlablerWatchUserItem",ServiceWatchUserItem);
	//Serwis do obserowania tagu
	PluginLink.CreateServiceFunction(L"sBlablerWatchTagItem",ServiceWatchTagItem);
	//Tworzenie interfejsu szybkiego dostepu do ustawien wtyczki
	BuildBlablerFastSettings();
	//Hook na aktwyna zakladke lub okno rozmowy (pokazywanie menu do cytowania, tworzenie buttonow)
	PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_ACTIVETAB,OnActiveTab);
	//Hook na pokazywane wiadomosci
	PluginLink.HookEvent(AQQ_CONTACTS_ADDLINE,OnAddLine);
	//Hook na zmiane kolorystyki AlphaControls
	PluginLink.HookEvent(AQQ_SYSTEM_COLORCHANGEV2,OnColorChange);
	//Hook na zaladowanie wszystkich modulow w AQQ (autoupdate awatarow)
	PluginLink.HookEvent(AQQ_SYSTEM_MODULESLOADED,OnModulesLoaded);
	//Hook na pobieranie adresow URL z roznych popup
	PluginLink.HookEvent(AQQ_SYSTEM_PERFORM_COPYDATA,OnPerformCopyData);
	//Hook na zmiane kompozycji (pobranie stylu zalacznikow oraz zmiana skorkowania wtyczki)
	PluginLink.HookEvent(AQQ_SYSTEM_THEMECHANGED,OnThemeChanged);
	//Hook na odbieranie pakietow XML
	PluginLink.HookEvent(AQQ_SYSTEM_XMLDEBUG,OnXMLDebug);
	//Odczyt ustawien
	LoadSettings();
	//Pobranie stylu Attachment & Avatars
	GetThemeStyle();
	//Rejestowanie klasy okna timera
	WNDCLASSEX wincl;
	wincl.cbSize = sizeof (WNDCLASSEX);
	wincl.style = 0;
	wincl.lpfnWndProc = TimerFrmProc;
	wincl.cbClsExtra = 0;
	wincl.cbWndExtra = 0;
	wincl.hInstance = HInstance;
	wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wincl.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
	wincl.lpszMenuName = NULL;
	wincl.lpszClassName = L"TBlablerTimer";
	wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&wincl);
	//Tworzenie okna timera
	hTimerFrm = CreateWindowEx(0, L"TBlablerTimer", L"",	0, 0, 0, 0, 0, NULL, NULL, HInstance, NULL);
	//Jezeli wszystkie moduly w AQQ zostaly juz zaladowany przed wczytaniem wtyczki
	if(PluginLink.CallService(AQQ_SYSTEM_MODULESLOADED,0,0))
	{
		//Timer aktualizacji awatarow po wlaczeniu wtyczki
		SetTimer(hTimerFrm,TIMER_UPDATE_AVATARS_ONLOAD,300000,(TIMERPROC)TimerFrmProc);
		//Hook na pobieranie aktywnych zakladek
		PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_PRIMARYTAB,OnPrimaryTab);
		PluginLink.CallService(AQQ_CONTACTS_BUDDY_FETCHALLTABS,0,0);
		PluginLink.UnhookEvent(OnPrimaryTab);
	}
	
	return 0;
}
//---------------------------------------------------------------------------

extern "C" INT_PTR __declspec(dllexport) __stdcall Unload()
{
	//Anty "Abnormal program termination"
	if(hSettingsForm) hSettingsForm->aForceDisconnect->Execute();
	//Zatrzymanie timerow
	for(int TimerID=10;TimerID<=20;TimerID=TimerID+10) KillTimer(hTimerFrm,TimerID);
	//Usuwanie okna timera
	DestroyWindow(hTimerFrm);
	//Wyrejestowanie klasy okna timera
	UnregisterClass(L"TBlablerTimer",HInstance);
	//Wyladowanie hookow
	PluginLink.UnhookEvent(OnActiveTab);
	PluginLink.UnhookEvent(OnAddLine);
	PluginLink.UnhookEvent(OnColorChange);
	PluginLink.UnhookEvent(OnModulesLoaded);
	PluginLink.UnhookEvent(OnPerformCopyData);
	PluginLink.UnhookEvent(OnThemeChanged);
	PluginLink.UnhookEvent(OnXMLDebug);
	//Usuwanie elementow z interfejsu AQQ
	DestroyBlablerFastSettings();
	DestroyQuoteMsgItem();
	DestroyInsertTagItem();
	DestroyLikeMsgItem();
	DestroyInsertNickItem();
	DestroySendMsgItem();
	DestroySendPrivMsgItem();
	DestroySeparatorItem();
	//Usuwanie serwisow
	PluginLink.DestroyServiceFunction(ServiceBlablerFastSettingsItem);
	PluginLink.DestroyServiceFunction(ServiceInsertItem);
	PluginLink.DestroyServiceFunction(ServiceLikeMsgItem);
	PluginLink.DestroyServiceFunction(ServiceInsertNickItem);
	PluginLink.DestroyServiceFunction(ServiceSendMsgItem);
	PluginLink.DestroyServiceFunction(ServiceSendPrivMsgItem);
	PluginLink.DestroyServiceFunction(ServiceWatchUserItem);
	PluginLink.DestroyServiceFunction(ServiceWatchTagItem);

	return 0;
}
//---------------------------------------------------------------------------

//Ustawienia wtyczki
extern "C" INT_PTR __declspec(dllexport)__stdcall Settings()
{
	//Otwarcie okna ustawien
	OpenSettingsForm();

	return 0;
}
//---------------------------------------------------------------------------

//Informacje o wtyczce
extern "C" __declspec(dllexport) PPluginInfo __stdcall AQQPluginInfo(DWORD AQQVersion)
{
	PluginInfo.cbSize = sizeof(TPluginInfo);
	PluginInfo.ShortName = L"Blabler";
	PluginInfo.Version = PLUGIN_MAKE_VERSION(1,2,8,0);
	PluginInfo.Description = L"Wtyczka przeznaczona dla os�b u�ywaj�cych mikrobloga Blabler (nast�pcy serwisu Blip.pl). Formatuje ona wszystkie wiadomo�ci przychodz�ce jak i wychodz�ce dla bota, kt�rego serwis udost�pnia zar�wno dla sieci Jabber jak i Gadu-Gadu.";
	PluginInfo.Author = L"Krzysztof Grochocki";
	PluginInfo.AuthorMail = L"kontakt@beherit.pl";
	PluginInfo.Copyright = L"Krzysztof Grochocki";
	PluginInfo.Homepage = L"http://beherit.pl";
	PluginInfo.Flag = 0;
	PluginInfo.ReplaceDefaultModule = 0;

	return &PluginInfo;
}
//---------------------------------------------------------------------------
