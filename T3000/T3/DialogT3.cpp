// DialogT3.cpp : implementation file
//

#include "stdafx.h"
#include "DialogT3.h"
#include "../global_function.h"
#include "../T3000.h"
#include "../MainFrm.h"


#define   T3_MODEL   2000

static BOOL ift3db = TRUE;
extern BOOL bPauseMultiRead;
// CDialogT3
DWORD WINAPI _BackFreshing_4IAO(LPVOID pParam)
{
	CDialogT3* dlg = (CDialogT3*)(pParam);

	Sleep(10 * 1000);


	while (1)
	{
		if (!is_connect())
		{
			continue;
		}
		Sleep(3 * 1000);

		for (int i = 0; i < 3; i++) //Modify by Fance , tstat 6 has more register than 512;
		{
			if (g_bPauseMultiRead)
			{
				return 0;
			}
			Read_Multi(g_tstat_id, &multi_register_value[i * 100], i * 100, 100);
		}
		memcpy_s(product_register_value, sizeof(product_register_value), multi_register_value, sizeof(multi_register_value));
		dlg->ShowDialogData();
	}


	return 1;
}

IMPLEMENT_DYNCREATE(CDialogT3, CFormView)

CDialogT3::CDialogT3()
	: CFormView(CDialogT3::IDD)
	  , m_address(0)
	  , m_firmware(0)
	  , m_serial(0)
	  , m_hardware(0)
	  , m_model(_T(""))
	  , m_picVersion(0)
{
}

CDialogT3::~CDialogT3()
{
}

void CDialogT3::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_T3ADDRESS, m_address);
	DDX_Text(pDX, IDC_EDIT_T3FIRMWARE, m_firmware);
	DDX_Text(pDX, IDC_EDIT_T3SERIAL, m_serial);
	DDX_Text(pDX, IDC_EDIT_T3HARDWARE, m_hardware);
	DDX_Text(pDX, IDC_EDIT_T3MODEL, m_model);
	DDX_Text(pDX, IDC_EDIT_T3PICVERSION, m_picVersion);
	DDX_Control(pDX, IDC_MSFLEXGRID_INPUT, m_msflexgrid_input);
	DDX_Control(pDX, IDC_MSFLEXGRID_OUTPUT, m_msflexgrid_output);

	DDX_Control(pDX, IDC_EDIT_NAME, m_inNameEdt);
	DDX_Control(pDX, IDC_RANGECOMBO, m_comboxRange);
}

BEGIN_MESSAGE_MAP(CDialogT3, CFormView)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_RANGECOMBO, &CDialogT3::OnCbnSelchangeType)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CDialogT3::OnBnClickedBtnReset)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CDialogT3, CFormView)
	ON_EVENT(CDialogT3, IDC_MSFLEXGRID_INPUT, DISPID_CLICK, CDialogT3::ClickInputMsflexgrid, VTS_NONE)
	ON_EVENT(CDialogT3, IDC_MSFLEXGRID_OUTPUT, DISPID_CLICK, CDialogT3::ClickOutputMsflexgrid, VTS_NONE)
END_EVENTSINK_MAP()
// CDialogT3 diagnostics

#ifdef _DEBUG
void CDialogT3::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CDialogT3::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}


#endif
#endif //_DEBUG

// CDialogT3 message handlers
void CDialogT3::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

#if 1
	//设置排/行数量
	m_msflexgrid_input.put_Cols(5);
	m_msflexgrid_input.put_Rows(11);//包括标题栏
	//显示横标题
	m_msflexgrid_input.put_TextMatrix(0, 0,_T("Input Name"));
	m_msflexgrid_input.put_TextMatrix(0, 1,_T("Register Value"));
	m_msflexgrid_input.put_TextMatrix(0, 2,_T("Date Stamp"));
	m_msflexgrid_input.put_TextMatrix(0, 3,_T("Range"));
	m_msflexgrid_input.put_TextMatrix(0, 4,_T("Filter"));


	//设置列宽	
	m_msflexgrid_input.put_ColWidth(0, 1000);
	m_msflexgrid_input.put_ColWidth(1, 1500);
	m_msflexgrid_input.put_ColWidth(2, 1500);
	m_msflexgrid_input.put_ColWidth(3, 1500);
	m_msflexgrid_input.put_ColWidth(4, 1000);
	//居中显示
	for (int col = 0; col < 5; col++)
	{
		m_msflexgrid_input.put_ColAlignment(col, 4);
	}

	//彩色显示
	for (int i = 1; i < 11; i++) //排数量
	{
		for (int k = 0; k < 5; k++) //列数量
		{
			if (i % 2 == 1)
			{
				m_msflexgrid_input.put_Row(i);
				m_msflexgrid_input.put_Col(k);
				m_msflexgrid_input.put_CellBackColor(RGB(255,255,255));
			}
			else
			{
				m_msflexgrid_input.put_Row(i);
				m_msflexgrid_input.put_Col(k);
				m_msflexgrid_input.put_CellBackColor(COLOR_CELL);
			}
		}
	}

	//显示纵标题
	CString str;
	for (int i = 1; i < 11; i++)
	{
		str.Format(_T("Input%d"), i);
		m_msflexgrid_input.put_TextMatrix(i, 0, str);
	}
	//============================================================================================================界面Output DO部份列表框

	//设置行/列数量
	m_msflexgrid_output.put_Rows(13);
	m_msflexgrid_output.put_Cols(3);
	//设置列宽	


	//显示横标题
	m_msflexgrid_output.put_TextMatrix(0, 0,_T("Output Name"));
	m_msflexgrid_output.put_TextMatrix(0, 1,_T("Switch Status"));
	m_msflexgrid_output.put_TextMatrix(0, 2,_T("Value"));

	m_msflexgrid_output.put_ColWidth(0, 1600);
	m_msflexgrid_output.put_ColWidth(1, 1500);
	m_msflexgrid_output.put_ColWidth(2, 1500);

	for (int col = 0; col < 3; col++)
	{
		m_msflexgrid_output.put_ColAlignment(col, 3);
	}

	//彩色显示
	for (int i = 1; i < 13; i++) //排数量
	{
		for (int k = 0; k < 3; k++) //列数量
		{
			if (i % 2 == 1)
			{
				m_msflexgrid_output.put_Row(i);
				m_msflexgrid_output.put_Col(k);
				m_msflexgrid_output.put_CellBackColor(RGB(255,255,255));
			}
			else
			{
				m_msflexgrid_output.put_Row(i);
				m_msflexgrid_output.put_Col(k);
				m_msflexgrid_output.put_CellBackColor(COLOR_CELL);
			}
		}
	}
	//显示纵标题
	CString str_output;
	for (int i = 1; i < 9; i++)
	{
		str_output.Format(_T("Digit Output%d"), i);
		m_msflexgrid_output.put_TextMatrix(i, 0, str_output);
	}
	for (int i = 9; i < 13; i++)
	{
		str_output.Format(_T("Analog Output%d"), i);
		m_msflexgrid_output.put_TextMatrix(i, 0, str_output);
	}


#endif
}

void CDialogT3::Fresh()
{
	if (is_connect())
	{
		Initial_RegisterList();
		ShowDialogData();
	}
}

void CDialogT3::Initial_RegisterList()
{
#if 1
	SN_LOW = 0;
	SN_HI = 2;
	EPROM_VER_NUMBER = 4;
	FIRMWARE_VER_NUMBER = 5;
	MODBUS_ID = 6;
	PRODUCT_MODEL = 7;
	HARDWARE_VER_NUMBER = 8;
	PIC_VER_NUMBER = 9;
	CALIBRATION_OUTPUTS = 13;
	BAUDRATE = 15;
	RESPONSE_DELAY = 19;
	OUTPUT1_RELAY = 100;
	OUTPUT2_RELAY = 101;
	OUTPUT3_RELAY = 102;
	OUTPUT4_RELAY = 103;
	OUTPUT5_RELAY = 104;
	OUTPUT6_RELAY = 105;
	OUTPUT7_RELAY = 106;
	OUTPUT8_RELAY = 107;
	OUTPUT1_ANALOG = 108;
	OUTPUT2_ANALOG = 109;
	OUTPUT3_ANALOG = 110;
	OUTPUT4_ANALOG = 111;
	SWITCH1_STATUS = 116;
	SWITCH2_STATUS = 117;
	SWITCH3_BANK = 118;
	INPUT1_PULSE_COUNT_HIGHT = 119;
	INPUT1_PULSE_COUNT_LOW = 120;
	INPUT2_PULSE_COUNT_HIGHT = 121;
	INPUT2_PULSE_COUNT_LOW = 122;
	INPUT3_PULSE_COUNT_HIGHT = 123;
	INPUT3_PULSE_COUNT_LOW = 124;
	INPUT4_PULSE_COUNT_HIGHT = 125;
	INPUT4_PULSE_COUNT_LOW = 126;
	INPUT5_PULSE_COUNT_HIGHT = 127;
	INPUT5_PULSE_COUNT_LOW = 128;
	INPUT6_PULSE_COUNT_HIGHT = 129;
	INPUT6_PULSE_COUNT_LOW = 130;
	INPUT7_PULSE_COUNT_HIGHT = 131;
	INPUT7_PULSE_COUNT_LOW = 132;
	INPUT8_PULSE_COUNT_HIGHT = 133;
	INPUT8_PULSE_COUNT_LOW = 134;
	INPUT9_PULSE_COUNT_HIGHT = 135;
	INPUT9_PULSE_COUNT_LOW = 136;
	INPUT10_PULSE_COUNT_HIGHT = 137;
	INPUT10_PULSE_COUNT_LOW = 138;
	DATE_STAMP_INPUT1_YEAR = 139;
	DATE_STAMP_INPUT1_MONTH = 140;
	DATE_STAMP_INPUT1_DAY = 141;
	DATE_STAMP_INPUT1_HOUR = 142;
	DATE_STAMP_INPUT1_MINUTE = 143;
	DATE_STAMP_INPUT2_YEAR = 144;
	DATE_STAMP_INPUT2_MONTH = 145;
	DATE_STAMP_INPUT2_DAY = 146;
	DATE_STAMP_INPUT2_HOUR = 147;
	DATE_STAMP_INPUT2_MINUTE = 148;
	DATE_STAMP_INPUT3_YEAR = 149;
	DATE_STAMP_INPUT3_MONTH = 150;
	DATE_STAMP_INPUT3_DAY = 151;
	DATE_STAMP_INPUT3_HOUR = 152;
	DATE_STAMP_INPUT3_MINUTE = 153;
	DATE_STAMP_INPUT4_YEAR = 154;
	DATE_STAMP_INPUT4_MONTH = 155;
	DATE_STAMP_INPUT4_DAY = 156;
	DATE_STAMP_INPUT4_HOUR = 157;
	DATE_STAMP_INPUT4_MINUTE = 158;
	DATE_STAMP_INPUT5_YEAR = 159;
	DATE_STAMP_INPUT5_MONTH = 160;
	DATE_STAMP_INPUT5_DAY = 161;
	DATE_STAMP_INPUT5_HOUR = 162;
	DATE_STAMP_INPUT5_MINUTE = 163;
	DATE_STAMP_INPUT6_YEAR = 164;
	DATE_STAMP_INPUT6_MONTH = 165;
	DATE_STAMP_INPUT6_DAY = 166;
	DATE_STAMP_INPUT6_HOUR = 167;
	DATE_STAMP_INPUT6_MINUTE = 168;
	DATE_STAMP_INPUT7_YEAR = 169;
	DATE_STAMP_INPUT7_MONTH = 170;
	DATE_STAMP_INPUT7_DAY = 171;
	DATE_STAMP_INPUT7_HOUR = 172;
	DATE_STAMP_INPUT7_MINUTE = 173;
	DATE_STAMP_INPUT8_YEAR = 174;
	DATE_STAMP_INPUT8_MONTH = 175;
	DATE_STAMP_INPUT8_DAY = 176;
	DATE_STAMP_INPUT8_HOUR = 177;
	DATE_STAMP_INPUT8_MINUTE = 178;
	DATE_STAMP_INPUT9_YEAR = 179;
	DATE_STAMP_INPUT9_MONTH = 180;
	DATE_STAMP_INPUT9_DAY = 181;
	DATE_STAMP_INPUT9_HOUR = 182;
	DATE_STAMP_INPUT9_MINUTE = 183;
	DATE_STAMP_INPUT10_YEAR = 184;
	DATE_STAMP_INPUT10_MONTH = 185;
	DATE_STAMP_INPUT10_DAY = 186;
	DATE_STAMP_INPUT10_HOUR = 187;
	DATE_STAMP_INPUT10_MINUTE = 188;
	AUTO_MANUAL_OUTPUTS = 189;
	ORIGIGINAL_AD_DATA_INPUT1 = 190;
	ORIGIGINAL_AD_DATA_INPUT2 = 191;
	ORIGIGINAL_AD_DATA_INPUT3 = 192;
	ORIGIGINAL_AD_DATA_INPUT4 = 193;
	ORIGIGINAL_AD_DATA_INPUT5 = 194;
	ORIGIGINAL_AD_DATA_INPUT6 = 195;
	ORIGIGINAL_AD_DATA_INPUT7 = 196;
	ORIGIGINAL_AD_DATA_INPUT8 = 197;
	ORIGIGINAL_AD_DATA_INPUT9 = 198;
	ORIGIGINAL_AD_DATA_INPUT10 = 199;
	RANGE_INPUT1 = 200;
	RANGE_INPUT2 = 201;
	RANGE_INPUT3 = 202;
	RANGE_INPUT4 = 203;
	RANGE_INPUT5 = 204;
	RANGE_INPUT6 = 205;
	RANGE_INPUT7 = 206;
	RANGE_INPUT8 = 207;
	RANGE_INPUT9 = 208;
	RANGE_INPUT10 = 209;
	FILTER_INPUT1 = 210;
	FILTER_INPUT2 = 211;
	FILTER_INPUT3 = 212;
	FILTER_INPUT4 = 213;
	FILTER_INPUT5 = 214;
	FILTER_INPUT6 = 215;
	FILTER_INPUT7 = 216;
	FILTER_INPUT8 = 217;
	FILTER_INPUT9 = 218;
	FILTER_INPUT10 = 219;
	LIGHTING_ZONE_TIME_INPUT1 = 220;
	LIGHTING_ZONE_TIME_INPUT2 = 221;
	LIGHTING_ZONE_TIME_INPUT3 = 222;
	LIGHTING_ZONE_TIME_INPUT4 = 223;
	LIGHTING_ZONE_TIME_INPUT5 = 224;
	LIGHTING_ZONE_TIME_INPUT6 = 225;
	LIGHTING_ZONE_TIME_INPUT7 = 226;
	LIGHTING_ZONE_TIME_INPUT8 = 227;
	LIGHTING_ZONE_TIME_INPUT9 = 228;
	LIGHTING_ZONE_TIME_INPUT10 = 229;
	ZONE_TIME_LEFT_INPUT1 = 230;
	ZONE_TIME_LEFT_INPUT2 = 231;
	ZONE_TIME_LEFT_INPUT3 = 232;
	ZONE_TIME_LEFT_INPUT4 = 233;
	ZONE_TIME_LEFT_INPUT5 = 234;
	ZONE_TIME_LEFT_INPUT6 = 235;
	ZONE_TIME_LEFT_INPUT7 = 236;
	ZONE_TIME_LEFT_INPUT8 = 237;
	ZONE_TIME_LEFT_INPUT9 = 238;
	ZONE_TIME_LEFT_INPUT10 = 239;
	LC_DISABLE_ENABLE = 240;
	SELECT_INPUT_AS_TRIGGER_1 = 241;
	SELECT_INPUT_AS_TRIGGER_2 = 242;
	SELECT_INPUT_AS_TRIGGER_3 = 243;
	SELECT_INPUT_AS_TRIGGER_4 = 244;
	SELECT_INPUT_AS_TRIGGER_5 = 245;
	SELECT_INPUT_AS_TRIGGER_6 = 246;
	SELECT_INPUT_AS_TRIGGER_7 = 247;
	SELECT_INPUT_AS_TRIGGER_8 = 248;
#endif
}

int CDialogT3::Get_RegID(CString Name)
{
	Name.TrimLeft();
	Name.TrimRight();
	int regid = -1;
	vector<T3Register>::iterator iter;
	for (iter = m_vecT3Register.begin(); iter != m_vecT3Register.end(); iter++)
	{
		if (iter->regName == Name)
		{
			regid = iter->regID;
			break;
		}
	}
	return regid;
}

void CDialogT3::ShowDialogData()
{
	m_model = _T("T3-4AO");
	m_address = product_register_value[MODBUS_ID];
	m_firmware = product_register_value[FIRMWARE_VER_NUMBER];
	m_serial = product_register_value[SN_LOW] + product_register_value[SN_LOW + 1] * 256 + product_register_value[SN_HI] * 256 * 256;
	+product_register_value[SN_HI + 1] * 256 * 256 * 256;//get_serialnumber(); //Address 0-3
	m_hardware = product_register_value[HARDWARE_VER_NUMBER];

	m_picVersion = product_register_value[PIC_VER_NUMBER];


	//==================================================Input============================================================
	//T3-4AO
	//address: input 119~138  INPUT1-10 119(high word) 120(low word)
	//address:Switch 
	//Range Setting for each input  200
	/*
	m_comboxRange.AddString(_T("RAW DATA"));
	m_comboxRange.AddString(_T("10K C"));
	m_comboxRange.AddString(_T("10K F"));
	m_comboxRange.AddString(_T("0-100%"));
	m_comboxRange.AddString(_T("ON/OFF"));
	m_comboxRange.AddString(_T("OFF/ON"));
	m_comboxRange.AddString(_T("Pulse Input"));
	m_comboxRange.AddString(_T("Lighting Control"));
	m_comboxRange.AddString(_T("TYPE3 10K C"));
	m_comboxRange.AddString(_T("TYPE3 10K F"));
	m_comboxRange.AddString(_T("NO USE"));
	m_comboxRange.AddString(_T("0-5V"));
	m_comboxRange.AddString(_T("0-10V"));
	m_comboxRange.AddString(_T("0-20I"));
	*/
	CString strresult;
	int regValue;
	for (int i = 1; i <= 10; i++)
	{
		regValue = product_register_value[INPUT1_PULSE_COUNT_HIGHT + 2 * (i - 1)] * 65535 + product_register_value[INPUT1_PULSE_COUNT_LOW + 2 * (i - 1)];


		if (0 == product_register_value[RANGE_INPUT1 + i - 1])
		{
			strresult.Format(_T("%d"), regValue);
		}
		else if (1 == product_register_value[RANGE_INPUT1 + i - 1])
		{
			strresult.Format(_T("%.1f C"), (float)regValue / 10.0);
		}
		else if (2 == product_register_value[RANGE_INPUT1 + i - 1])
		{
			strresult = _T("10K F");
			strresult.Format(_T("%.1f F"), (float)regValue / 10.0);
		}
		else if (3 == product_register_value[RANGE_INPUT1 + i - 1])
		{
			strresult.Format(_T("%.1f"), (float)regValue);
			strresult += _T("%");
		}
		else if (4 == product_register_value[RANGE_INPUT1 + i - 1])
		{
			//strresult.Format(_T("%.1f F"),(float)regValue/10.0);
			if (regValue == 0)
			{
				strresult = _T("OFF");
			}
			else
			{
				strresult = _T("ON");
			}
		}
		else if (5 == product_register_value[RANGE_INPUT1 + i - 1])
		{
			if (regValue == 0)
			{
				strresult = _T("ON");
			}
			else
			{
				strresult = _T("OFF");
			}
		}
		else if (6 == product_register_value[RANGE_INPUT1 + i - 1])
		{
			strresult.Format(_T("%d"), regValue);
		}
		else if (7 == product_register_value[RANGE_INPUT1 + i - 1])
		{
			strresult.Format(_T("%d"), regValue);
		}
		else if (8 == product_register_value[RANGE_INPUT1 + i - 1])
		{
			//strresult=_T("TYPE3 10K C");
			strresult.Format(_T("%0.1f C"), (float)regValue / 10.0);
		}
		else if (9 == product_register_value[RANGE_INPUT1 + i - 1])
		{
			/*strresult=_T("TYPE3 10K F");*/
			strresult.Format(_T("%0.1f F"), (float)regValue / 10.0);
		}
		else if (10 == product_register_value[RANGE_INPUT1 + i - 1])
		{
			strresult = _T("0");
			//strresult.Format(_T("%0.1f C"),(float)regValue/10.0);
		}
		else if (11 == product_register_value[RANGE_INPUT1 + i - 1])
		{
			//strresult=_T("0-5V");
			strresult.Format(_T("%0.1f V"), (float)regValue / 1000.0);
		}
		else if (12 == product_register_value[RANGE_INPUT1 + i - 1])
		{
			//strresult=_T("0-10V");
			strresult.Format(_T("%0.1f V"), (float)regValue / 1000.0);
		}
		else if (13 == product_register_value[RANGE_INPUT1 + i - 1])
		{
			//strresult=_T("0-20I");
			strresult.Format(_T("%0.1f ma"), (float)regValue / 1000.0);
		}
		m_msflexgrid_input.put_TextMatrix(i, 1, strresult);

		strresult.Format(_T("%4d/%4d/%4d  %2d:%2d"), product_register_value[DATE_STAMP_INPUT1_YEAR + 3 * (i - 1)], product_register_value[DATE_STAMP_INPUT1_MONTH + 3 * (i - 1)], product_register_value[DATE_STAMP_INPUT1_DAY + 3 * (i - 1)], product_register_value[DATE_STAMP_INPUT1_HOUR + 3 * (i - 1)], product_register_value[DATE_STAMP_INPUT1_MINUTE + 3 * (i - 1)]);
		m_msflexgrid_input.put_TextMatrix(i, 2, strresult);
		strresult.Format(_T("%d"), product_register_value[RANGE_INPUT1 + i - 1]);
		if (0 == product_register_value[RANGE_INPUT1 + i - 1])
		{
			strresult = _T("RAW DATA");
		}
		else if (1 == product_register_value[RANGE_INPUT1 + i - 1])
		{
			strresult = _T("T10K C");
		}
		else if (2 == product_register_value[RANGE_INPUT1 + i - 1])
		{
			strresult = _T("10K F");
		}
		else if (3 == product_register_value[RANGE_INPUT1 + i - 1])
		{
			strresult = _T("0-100%");
		}
		else if (4 == product_register_value[RANGE_INPUT1 + i - 1])
		{
			strresult = _T("ON/OFF");
		}
		else if (5 == product_register_value[RANGE_INPUT1 + i - 1])
		{
			strresult = _T("OFF/ON");
		}
		else if (6 == product_register_value[RANGE_INPUT1 + i - 1])
		{
			strresult = _T("Pulse Input");
		}
		else if (7 == product_register_value[RANGE_INPUT1 + i - 1])
		{
			strresult = _T("Lighting Control");
		}
		else if (8 == product_register_value[RANGE_INPUT1 + i - 1])
		{
			strresult = _T("TYPE3 10K C");
		}
		else if (9 == product_register_value[RANGE_INPUT1 + i - 1])
		{
			strresult = _T("TYPE3 10K F");
		}
		else if (10 == product_register_value[RANGE_INPUT1 + i - 1])
		{
			strresult = _T("NO USE");
		}
		else if (11 == product_register_value[RANGE_INPUT1 + i - 1])
		{
			strresult = _T("0-5V");
		}
		else if (12 == product_register_value[RANGE_INPUT1 + i - 1])
		{
			strresult = _T("0-10V");
		}
		else if (13 == product_register_value[RANGE_INPUT1 + i - 1])
		{
			strresult = _T("4-20 ma");
		}

		m_msflexgrid_input.put_TextMatrix(i, 3, strresult);
		strresult.Format(_T("%d"), product_register_value[FILTER_INPUT1 + i - 1]);
		m_msflexgrid_input.put_TextMatrix(i, 4, strresult);
	}


	//====================================================Output============================================================	
	//T3-4AO OUTPUT
	//address: input 100~110   OUTPUT1-12
	//100~111


	CString CstresultDO;
	for (int i = 1; i <= 8; i++)
	{
		CstresultDO.Format(_T("%d"), product_register_value[OUTPUT1_RELAY + i - 1]);


		m_msflexgrid_output.put_TextMatrix(i, 2, CstresultDO);
	}


	for (int i = 9; i < 13; i++)
	{
		CstresultDO.Format(_T("%d"), product_register_value[OUTPUT1_ANALOG + (i - 8) - 1]);
		m_msflexgrid_output.put_TextMatrix(i, 2, CstresultDO);
	}

	//long switch_long=(product_register_value[SWITCH3_BANK]<<16+product_register_value[SWITCH2_STATUS]<<8+product_register_value[SWITCH1_STATUS]);
	bitset<16> BitSwitchValue(product_register_value[SWITCH1_STATUS]);
	int SwitchValue[12];
	for (int i = 0; i < 4; i++)
	{
		SwitchValue[i] = BitSwitchValue[2 * i] + BitSwitchValue[2 * i + 1] * 2;
	}
	bitset<16> BitSwitchValue1(product_register_value[SWITCH2_STATUS]);
	for (int i = 4; i < 8; i++)
	{
		SwitchValue[i] = BitSwitchValue1[2 * (i - 4)] + BitSwitchValue1[2 * (i - 4) + 1] * 2;
	}
	bitset<16> BitSwitchValue2(product_register_value[SWITCH3_BANK]);
	for (int i = 8; i < 12; i++)
	{
		SwitchValue[i] = BitSwitchValue2[2 * (i - 8)] + BitSwitchValue2[2 * (i - 8) + 1] * 2;
	}
	for (int i = 1; i < 13; i++)
	{
		m_msflexgrid_output.put_TextMatrix(i, 1, STRING_SWITCH_STATUS[SwitchValue[i - 1]]);
	}
	UpdateData(FALSE);
}

void CDialogT3::OnTimer(UINT_PTR nIDEvent)
{
	CFormView::OnTimer(nIDEvent);
}

void CDialogT3::ClickInputMsflexgrid()
{
	KillTimer(T3_MODEL);
	m_comboxRange.ShowWindow(FALSE);
	UpdateData(FALSE);

	long lRow, lCol;
	lRow = m_msflexgrid_input.get_RowSel();//获取点击的行号	
	lCol = m_msflexgrid_input.get_ColSel(); //获取点击的列号
	TRACE(_T("Click input grid!\n"));

	CRect rect;
	m_msflexgrid_input.GetWindowRect(rect); //获取表格控件的窗口矩形
	ScreenToClient(rect); //转换为客户区矩形	
	CDC* pDC = GetDC();

	int nTwipsPerDotX = 1440 / pDC->GetDeviceCaps(LOGPIXELSX);
	int nTwipsPerDotY = 1440 / pDC->GetDeviceCaps(LOGPIXELSY);
	//计算选中格的左上角的坐标(象素为单位)
	long y = m_msflexgrid_input.get_RowPos(lRow) / nTwipsPerDotY;
	long x = m_msflexgrid_input.get_ColPos(lCol) / nTwipsPerDotX;
	//计算选中格的尺寸(象素为单位)。加1是实际调试中，发现加1后效果更好
	long width = m_msflexgrid_input.get_ColWidth(lCol) / nTwipsPerDotX + 1;
	long height = m_msflexgrid_input.get_RowHeight(lRow) / nTwipsPerDotY + 1;
	//形成选中个所在的矩形区域
	CRect rcCell(x, y, x + width, y + height);
	//转换成相对对话框的坐标
	rcCell.OffsetRect(rect.left + 1, rect.top + 1);
	ReleaseDC(pDC);
	CString strValue = m_msflexgrid_input.get_TextMatrix(lRow, lCol);


	if ((0 == lCol || 4 == lCol) && lRow != 0)
	{
		m_inNameEdt.MoveWindow(&rcCell, 1);
		m_inNameEdt.ShowWindow(SW_SHOW);
		m_inNameEdt.SetWindowText(strValue);
		m_inNameEdt.SetFocus();
		m_inNameEdt.SetCapture();//LSC
		int nLenth = strValue.GetLength();
		m_inNameEdt.SetSel(nLenth, nLenth); //全选//
	}
	if ((3 == lCol) && lRow != 0)
	{
		m_comboxRange.MoveWindow(&rcCell, 1);
		m_comboxRange.ResetContent();
		m_comboxRange.AddString(_T("RAW DATA"));
		m_comboxRange.AddString(_T("10K C"));
		m_comboxRange.AddString(_T("10K F"));
		m_comboxRange.AddString(_T("0-100%"));
		m_comboxRange.AddString(_T("ON/OFF"));
		m_comboxRange.AddString(_T("OFF/ON"));
		m_comboxRange.AddString(_T("Pulse Input"));
		m_comboxRange.AddString(_T("Lighting Control"));
		m_comboxRange.AddString(_T("TYPE3 10K C"));
		m_comboxRange.AddString(_T("TYPE3 10K F"));
		m_comboxRange.AddString(_T("NO USE"));
		m_comboxRange.AddString(_T("0-5V"));
		m_comboxRange.AddString(_T("0-10V"));
		m_comboxRange.AddString(_T("4-20 ma"));

		m_comboxRange.ShowWindow(SW_SHOW);


		m_comboxRange.BringWindowToTop();

		m_comboxRange.SetFocus(); //获取焦点
		m_comboxRange.SetWindowText(strValue);

		//m_comboxRange.SetWindowText(strValue);
		//m_comboxRange.SetFocus();
		//m_comboxRange.SetCapture();//LSC
		//int nLenth=strValue.GetLength();
		//		m_comboxRange.SetSel(nLenth,nLenth); //全选//
	}
}

void CDialogT3::ClickOutputMsflexgrid()
{
	long lRow, lCol;
	lRow = m_msflexgrid_output.get_RowSel();//获取点击的行号	
	lCol = m_msflexgrid_output.get_ColSel(); //获取点击的列号
	TRACE(_T("Click input grid!\n"));

	CRect rect;
	m_msflexgrid_output.GetWindowRect(rect); //获取表格控件的窗口矩形
	ScreenToClient(rect); //转换为客户区矩形	
	CDC* pDC = GetDC();

	int nTwipsPerDotX = 1440 / pDC->GetDeviceCaps(LOGPIXELSX);
	int nTwipsPerDotY = 1440 / pDC->GetDeviceCaps(LOGPIXELSY);
	//计算选中格的左上角的坐标(象素为单位)
	long y = m_msflexgrid_output.get_RowPos(lRow) / nTwipsPerDotY;
	long x = m_msflexgrid_output.get_ColPos(lCol) / nTwipsPerDotX;
	//计算选中格的尺寸(象素为单位)。加1是实际调试中，发现加1后效果更好
	long width = m_msflexgrid_output.get_ColWidth(lCol) / nTwipsPerDotX + 1;
	long height = m_msflexgrid_output.get_RowHeight(lRow) / nTwipsPerDotY + 1;
	//形成选中个所在的矩形区域
	CRect rcCell(x, y, x + width, y + height);
	//转换成相对对话框的坐标
	rcCell.OffsetRect(rect.left + 1, rect.top + 1);
	ReleaseDC(pDC);
	CString strValue = m_msflexgrid_output.get_TextMatrix(lRow, lCol);


	if ((1 == lCol && lRow != 0))
	{
		return; // 2012.2.7老毛说不允许修改
		m_inNameEdt.MoveWindow(&rcCell, 1);
		m_inNameEdt.ShowWindow(SW_SHOW);
		m_inNameEdt.SetWindowText(strValue);
		m_inNameEdt.SetFocus();
		m_inNameEdt.SetCapture();//LSC
		int nLenth = strValue.GetLength();
		m_inNameEdt.SetSel(nLenth, nLenth); //全选//
	}
}

void CDialogT3::OnCbnSelchangeType()
{
	BeginWaitCursor();
	CString strTemp;
	int lRow = m_msflexgrid_input.get_RowSel();
	int lCol = m_msflexgrid_input.get_ColSel();
	//if ((strText.GetLength()<=0)||strText.CompareNoCase(m_tempGridString)==0)
	//{
	//	return;
	//}
	int sel = m_comboxRange.GetCurSel();
	m_comboxRange.ShowWindow(FALSE);
	if (product_register_value[RANGE_INPUT1 + lRow - 1] == sel)
	{
		return;
	}
	int ret = write_one(g_tstat_id, RANGE_INPUT1 + lRow - 1, sel);
	if (ret > 0)
	{
		product_register_value[RANGE_INPUT1 + lRow - 1] = sel;
	}
	Sleep(1000);

	for (int i = 0; i < 3; i++)
	{
		Read_Multi(g_tstat_id, &product_register_value[i * 100], i * 100, 100);
	}
	ShowDialogData();
	EndWaitCursor();
}


void CDialogT3::OnBnClickedBtnReset()
{
	if (AfxMessageBox(_T(" This will reset the module to the factory defaults,Are you sure to reset it ?")) == IDOK)
	{
		//  write_one(g_tstat_id,299,1);
		write_one(g_tstat_id, 300, 1);
	}
}
