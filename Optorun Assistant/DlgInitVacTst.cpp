// DlgInitVacTst.cpp : implementation file
//

#include "stdafx.h"
#include "Optorun Assistant.h"
#include "DlgInitVacTst.h"

#include "OACnfgDoc.h"

// CDlgInitVacTst dialog

IMPLEMENT_DYNAMIC(CDlgInitVacTst, CDialog)

CDlgInitVacTst::CDlgInitVacTst(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInitVacTst::IDD, pParent)
{
	m_unCurrentTimeEvent = 0;
	m_dwMaxWaitingTime = 10;
	m_nRetValue = 0;
}

CDlgInitVacTst::~CDlgInitVacTst()
{

}

void CDlgInitVacTst::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgInitVacTst, CDialog)
	ON_BN_CLICKED(IDCANCEL, &CDlgInitVacTst::OnBnClickedCancel)
	ON_WM_TIMER()
//	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CDlgInitVacTst message handlers

void CDlgInitVacTst::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	m_nRetValue = _DLG_INIT_VAC_TST_RT_CANCEL;
	OnCancel();
}

BOOL CDlgInitVacTst::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	TCHAR buf[1024];

	m_bFirstTimer = true;

	m_unCurrentTimeEvent = SetTimer(1,1000,NULL);
	m_dwInitTick = GetTickCount();

	if( 0 == m_unCurrentTimeEvent)
	{
		AfxMessageBox(TEXT("Error: Failed to setup timer."),MB_OK|MB_ICONSTOP);
		m_nRetValue = 0;
		return false;
	}
	
	_stprintf(buf,TEXT("%02d:%02d:%02d"),m_dwMaxWaitingTime/3600,(m_dwMaxWaitingTime/60)%60 ,m_dwMaxWaitingTime%60);
	SetDlgItemText(IDC_STATIC_MAX_WAITING_TIME,buf);

	_stprintf(buf,TEXT("%.1E Pa"),	m_dStartPressure);
	SetDlgItemText(IDC_STATIC_START_PRESSURE,buf);

	_stprintf(buf,TEXT("%02d:%02d:%02d"),m_dwPressureKeepTime/3600,(m_dwPressureKeepTime/60)%60 ,m_dwPressureKeepTime%60);
	SetDlgItemText(IDC_STATIC_PRESSURE_MIN_KEEP_TIME,buf);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgInitVacTst::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	TCHAR buf[1024];
	if(nIDEvent == m_unCurrentTimeEvent)
	{
		double dPressure;
		bool   b;
		
		static int  iKeepTime = 0;
		if(m_bFirstTimer)
		{
			iKeepTime = 0;
			m_bFirstTimer = false;
		}

		int iTemp = (GetTickCount()-m_dwInitTick)/1000;

		MSG    msg;
		static bool bLbuttonDonw = false;
		CPoint pt;
		CRect  rect;
		int    istart;

		if( g_pVacProThread->fn_GetVGCch3Pressure(dPressure))
		{
			//ͨ�ųɹ�
			if(dPressure<=m_dStartPressure)
			{//��ʱ����������ڹر�״̬���������˳��Ի��򣬡������¼״̬��
				if( g_pVacProThread->fn_CheckMainValveStatus(b))
				{
					if( !b )
					{// bOpen == false. �ر�״̬�� �˳��Ի��򣬽����¼״̬
						m_nRetValue =_DLG_INIT_VAC_TST_RT_OK;
						OnOK();
						return;
					}
					else
					{// bOpen == true.  ����״̬�� 
						iKeepTime++ ;
						if(iKeepTime >= m_dwPressureKeepTime)
						{
							//�ȴ���ɣ�Ӧ��ģ���ֹ����ر������������¼״̬.
							if(m_unCurrentTimeEvent)
							{
								KillTimer(m_unCurrentTimeEvent);
							}
							m_unCurrentTimeEvent = 0;
							//1, ��ʾ�û�������ʼģ�ⰴ���������û���Ӧ����������

							

							//2, ����ȷ����<�ֶ�����>��ģʽ
							if( g_pVacProThread->fn_SetExhaustMode(true) )
							{
							}
							else
							{
								goto LABEL_TIMER_PLC_COMM_ERROR_RETURN;
							}

							Sleep(200);
							//ȷ�ϴ��� <�ֹ�����>ģʽ
							bool bManualMode = false;
							istart = GetTickCount();
							while( (GetTickCount()-istart)/1000 < 10 )
							{

								_stprintf(buf,TEXT("ȷ��<�ֶ�����>��...%d ��"), (GetTickCount()-istart)/1000);
								SetDlgItemText(IDC_STATIC_STATUS,buf);


								if( g_pVacProThread->fn_CheckExhaustMode(bManualMode))
								{
									if(bManualMode)
									{
										break; //���ֹ�ģʽ�����˳�ȷ�ϡ�<�ֹ�����>����
									}
								}
								else
								{
									goto LABEL_TIMER_PLC_COMM_ERROR_RETURN;
								}

								while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
								{
									if( msg.message==WM_TIMER && msg.hwnd==GetSafeHwnd())
									{ //������Ӧ��WM_TIMER

									}
									else if(msg.message==WM_LBUTTONDOWN)
									{
										GetCursorPos(&pt);
										GetDlgItem(IDCANCEL)->GetWindowRect(&rect);
										if(rect.PtInRect(pt))
										{
											bLbuttonDonw = true;
										}
										else
										{
											bLbuttonDonw = false;
										}
									}
									else if(msg.message==WM_LBUTTONUP)
									{
										GetCursorPos(&pt);	
										GetDlgItem(IDCANCEL)->GetWindowRect(&rect);
										if(rect.PtInRect(pt))
										{
											if(bLbuttonDonw)
											{
												m_nRetValue =_DLG_INIT_VAC_TST_RT_CANCEL;
												KillTimer(m_unCurrentTimeEvent);
												m_unCurrentTimeEvent = 0;
												AfxMessageBox(TEXT("Exit: hehehe �˳���"));  //���Էŵ� ���ô�������
												OnCancel();
												return;
											}
										}					
									}
									else
									{
										TranslateMessage(&msg);
										DispatchMessage(&msg);
									}
								}
								Sleep(200);
							}

							if(!bManualMode)
							{//�� 10 ��������û�н��롡<�ֹ�����> ģʽ�������˳�.
								m_nRetValue =_DLG_INIT_VAC_TST_RT_MANUAL_ERR;
								if(m_unCurrentTimeEvent)
									KillTimer(m_unCurrentTimeEvent);
								m_unCurrentTimeEvent = 0;

								AfxMessageBox(g_item2_String[_ITEM2_STR_MANUAL_EXHUAST_MODE_ERR].IsEmpty()?
									TEXT("�����涨ʱ����û�ܽ����ֶ�����ģʽ��\r\n��ʾ��Ĭ�ϣ����룡\r\nError:Cannot get into manual exhuast mode in 10 seconds."):g_item2_String[_ITEM2_STR_MANUAL_EXHUAST_MODE_ERR]);
								OnCancel();	
								return;
							}

							//3, ģ�ⴥ������һ�� <����>.
							//��ʾ��ȷ�ϴ��� <�����ر�>  ��.
							if(g_pVacProThread->fn_SwitchMainValve())
							{
							}
							else
							{
								goto LABEL_TIMER_PLC_COMM_ERROR_RETURN;
							}

							Sleep(200);
							bool bMainValveOpen = true; //����״̬
							istart = GetTickCount();
							while( (GetTickCount()-istart)/1000 < 10 )
							{
								_stprintf(buf,TEXT("ȷ��<�����ر�>��...%d ��"), (GetTickCount()-istart)/1000);
								SetDlgItemText(IDC_STATIC_STATUS,buf);

								if( g_pVacProThread->fn_CheckMainValveStatus(bMainValveOpen) )
								{
									if(!bMainValveOpen)
									{//�����رգ����Խ��м�¼�ˣ�

										m_nRetValue =_DLG_INIT_VAC_TST_RT_OK;
										OnOK();
										return;
									}
								}
								else
								{
									goto LABEL_TIMER_PLC_COMM_ERROR_RETURN;
								}

								while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
								{
									if( msg.message==WM_TIMER && msg.hwnd==GetSafeHwnd())
									{ //������Ӧ��WM_TIMER

									}
									else if(msg.message==WM_LBUTTONDOWN)
									{
										GetCursorPos(&pt);
										GetDlgItem(IDCANCEL)->GetWindowRect(&rect);
										if(rect.PtInRect(pt))
										{
											bLbuttonDonw = true;
										}
										else
										{
											bLbuttonDonw = false;
										}
									}
									else if(msg.message==WM_LBUTTONUP)
									{
										GetCursorPos(&pt);	
										GetDlgItem(IDCANCEL)->GetWindowRect(&rect);
										if(rect.PtInRect(pt))
										{
											if(bLbuttonDonw)
											{
												m_nRetValue =_DLG_INIT_VAC_TST_RT_CANCEL;
												if(m_unCurrentTimeEvent)
													KillTimer(m_unCurrentTimeEvent);
												m_unCurrentTimeEvent = 0;
												AfxMessageBox(TEXT("Exit: hehehe �˳���"));  //���Էŵ� ���ô�������
												OnCancel();
												return;
											}
										}					
									}
									else
									{
										TranslateMessage(&msg);
										DispatchMessage(&msg);
									}
								}
								Sleep(200);
							}

							//������Ȼ���ڿ���״̬
							ASSERT(bMainValveOpen);
							// bMainValveOpen
							//�� 10 ��������û�йرո߷����򱨴��˳���

							m_nRetValue =_DLG_INIT_VAC_TST_RT_MV_ERR;
							if(m_unCurrentTimeEvent)
								KillTimer(m_unCurrentTimeEvent);
							m_unCurrentTimeEvent = 0;
							AfxMessageBox(g_item2_String[_ITEM2_STR_MAIN_VALVE_CLOSE_ERR].IsEmpty()?
									TEXT("�����涨ʱ��������û�йرգ�\r\n��ʾ��Ĭ�ϣ����룡\r\nError: Main Valve didn't close in 10 seconds."):g_item2_String[_ITEM2_STR_MAIN_VALVE_CLOSE_ERR]);
							//AfxMessageBox(TEXT("�����涨ʱ��������û�йرգ�\r\n��ʾ��Ĭ�ϣ����룡"));  //���Էŵ� ���ô�������
							OnCancel();	
							return;

						}				

					}
				}
				else
				{
					goto LABEL_TIMER_PLC_COMM_ERROR_RETURN;
				}					
			}
			else
			{ 
				iKeepTime--;
				if(iKeepTime<0)
				{
					iKeepTime = 0;
				}

				// ��ʼ��ն�û�дﵽ�� ���һ���������״̬����ʱӦ��Ϊ�رգ�˵�����ڳ��������򱨴��˳���
				if( g_pVacProThread->fn_CheckMainValveStatus(b))
				{
					if( !b )
					{// bOpen == false. �ر�״̬����û�дﵽ��ʼ��նȣ���ʱ������ȴ�Ѿ��رգ�����
						if(  0.1 <  ( dPressure - m_dStartPressure ) / m_dStartPressure )
						{
							//��ն����趨��ʼ��նȲ�𳬹���10%���򱨴�
							m_nRetValue =_DLG_INIT_VAC_TST_RT_PRESS_ERR;
							KillTimer(m_unCurrentTimeEvent);
							m_unCurrentTimeEvent = 0;
							AfxMessageBox(g_item2_String[_ITEM2_STR_START_PRESSURE_ERR].IsEmpty()?
								TEXT("Error: ��ʼ��ն�û�дﵽ��\r\n��ʾ����ʼ��ն�û�дﵽ������£������봦�ڿ���״̬���ſɽ��в��ԣ�\r\nError: Main valve is closed unexpectedly."):g_item2_String[_ITEM2_STR_START_PRESSURE_ERR]);
							//AfxMessageBox(TEXT("Exit: ��ʼ��ն�û�дﵽ��\r\n��ʾ����ʼ��ն�û�дﵽ������£������봦�ڿ���״̬���ſɽ��в��ԣ�"));  //���Էŵ� ���ô�������
							OnCancel();	
							return;
						}
					}
					else
					{//�ȴ�

					}

				}
				else
				{
					goto LABEL_TIMER_PLC_COMM_ERROR_RETURN;
				}
			}			
		}
		else
		{
			goto LABEL_TIMER_PLC_COMM_ERROR_RETURN;
		}		

		if((iTemp >= ( m_dwMaxWaitingTime + m_dwPressureKeepTime)) || (iTemp>= m_dwMaxWaitingTime  && iKeepTime <= 0) )
		{//��ʱ���˳�.
			m_nRetValue = _DLG_INIT_VAC_TST_RT_OVERTIME;
			KillTimer(m_unCurrentTimeEvent);
			m_unCurrentTimeEvent = 0;

			AfxMessageBox(g_item2_String[_ITEM2_STR_WAITTING_TIME_OVER].IsEmpty()?
				TEXT("����: �ȴ���ʱ��\r\nError:Waiting time over!"):g_item2_String[_ITEM2_STR_WAITTING_TIME_OVER]);
			//AfxMessageBox(TEXT("Exit: �ȴ���ʱ��"));
			OnCancel();
			return;
		}

		//ʼ����ʾ��ǰ�Ĺ�ȥʱ��
		_stprintf(buf,TEXT("%02d:%02d:%02d"),iTemp/3600,(iTemp/60)%60 ,iTemp%60);
		SetDlgItemText(IDC_STATIC_ELAPSED_TIME,buf);

		_stprintf(buf,TEXT("%02d:%02d:%02d"),iKeepTime/3600,(iKeepTime/60)%60 ,iKeepTime%60);
		SetDlgItemText(IDC_STATIC_PRESSURE_KEEPED_TIME,buf);

		//��ʾ��ǰ��ն�
		_stprintf(buf,TEXT("%.2E Pa"),dPressure);
		SetDlgItemText(IDC_STATIC_CUR_PRESSURE,buf);

	}
	CDialog::OnTimer(nIDEvent);
	return;

LABEL_TIMER_PLC_COMM_ERROR_RETURN:
	//û��ͨ�ţ���ʾ���˳���
	m_nRetValue =_DLG_INIT_VAC_TST_RT_COMM_ERR;
	if(m_unCurrentTimeEvent)
		KillTimer(m_unCurrentTimeEvent);
	m_unCurrentTimeEvent = 0;
	AfxMessageBox(g_item2_String[_ITEM2_STR_PLC_COMM_ERR].IsEmpty()?
				TEXT("plcͨ�Ų�������\r\n plc communication failed."):g_item2_String[_ITEM2_STR_PLC_COMM_ERR]);
	//AfxMessageBox(TEXT("Exit: plcͨ�Ų�������"));  //���Էŵ� ���ô�������
	OnCancel();	
	return;
}
