class SCR_CTI_ConfirmMenu : SCR_InfoDisplayExtended
{
	protected SCR_CTI_GameMode m_gameMode;
	protected PlayerController m_pc;
	protected ChimeraCharacter m_ch;
	
	protected SCR_CTI_PlacingDefenseComponent m_pdc;
	protected SCR_CTI_PlacingStructureComponent m_psc;

	protected float m_timeDelta;
	protected const float TIMESTEP = 0.05;

	protected ButtonWidget m_confirm;
	protected ButtonWidget m_cancel;
	
	protected RichTextWidget m_confirmText;
	protected RichTextWidget m_cancelText;

	//------------------------------------------------------------------------------------------------
	protected void CreateHud(IEntity owner)
	{
		m_confirm = ButtonWidget.Cast(m_wRoot.FindAnyWidget("ConfirmButton"));
		m_cancel = ButtonWidget.Cast(m_wRoot.FindAnyWidget("CancelButton"));
		
		m_confirmText = RichTextWidget.Cast(m_wRoot.FindAnyWidget("ConfirmText"));
		m_cancelText = RichTextWidget.Cast(m_wRoot.FindAnyWidget("CancelText"));

		m_confirm.SetColor(Color.Orange);
		m_confirm.SetState(false);
		
		m_cancel.SetColor(Color.White);
		m_cancel.SetState(true);
		
		m_cancelText.SetText("[ - Cancel - ]");
		
		m_wRoot.SetVisible(false);
	}

	//------------------------------------------------------------------------------------------------
	protected void DestroyHud()
	{		
		if (!m_wRoot)
			return;		

		if (m_wRoot)
			m_wRoot.RemoveFromHierarchy();
		
		m_wRoot = null;		
	}

	//------------------------------------------------------------------------------------------------
	override event void DisplayUpdate(IEntity owner, float timeSlice)
	{
		m_timeDelta += timeSlice;
		if (m_timeDelta > TIMESTEP)
		{
			if (!m_wRoot)
				return;

			bool menuOpen = GetGame().GetMenuManager().IsAnyMenuOpen();
			if (!menuOpen && (m_pdc.getStartPlacing() || m_psc.getStartPlacing()))
			{
				m_wRoot.SetVisible(true);
			} else {
				m_wRoot.SetVisible(false);
				
				m_pdc.cancelPlacement(true);
				m_psc.cancelPlacement(true);
			}
			
			m_ch = ChimeraCharacter.Cast(m_pc.GetControlledEntity());
			if (m_ch.IsInVehicle())
			{
				m_pdc.cancelPlacement(true);
				m_psc.cancelPlacement(true);
			}

			if (m_wRoot.IsVisible())
			{
				bool mw = GetGame().GetInputManager().GetActionValue("MouseWheel"); // need disable moving speed change on scroll
				if (mw)
				{
					if (m_cancel.GetState())
					{
						m_confirm.SetState(true); m_confirm.SetColor(Color.White); m_confirmText.SetText("[ - Confirm - ]");
						m_cancel.SetState(false); m_cancel.SetColor(Color.Orange); m_cancelText.SetText("Cancel");
					} else {
						m_confirm.SetState(false); m_confirm.SetColor(Color.Orange); m_confirmText.SetText("Confirm");
						m_cancel.SetState(true); m_cancel.SetColor(Color.White); m_cancelText.SetText("[ - Cancel - ]");
					}
				}

				bool wheeldown = GetGame().GetInputManager().GetActionValue("MouseMiddle");
				if (wheeldown)
				{
					if (m_confirm.GetState())
					{
						m_pdc.confirmPlacement(true);
						m_psc.confirmPlacement(true);
					} else {
						m_pdc.cancelPlacement(true);
						m_psc.cancelPlacement(true);
					}
				}
			}
			m_timeDelta = 0;
		}
	}

	//------------------------------------------------------------------------------------------------
	override bool DisplayStartDrawInit(IEntity owner)
	{
		if (m_LayoutPath == "") m_LayoutPath = "{8C4B0F3E158814B4}UI/layouts/ConfirmMenu.layout";
		
		m_gameMode = SCR_CTI_GameMode.Cast(GetGame().GetGameMode());
		m_pc = GetGame().GetPlayerController();
		
		m_pdc = SCR_CTI_PlacingDefenseComponent.Cast(m_pc.FindComponent(SCR_CTI_PlacingDefenseComponent));
		m_psc = SCR_CTI_PlacingStructureComponent.Cast(m_pc.FindComponent(SCR_CTI_PlacingStructureComponent));

		m_timeDelta = 0;

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override void DisplayStartDraw(IEntity owner)
	{
		if (!m_wRoot)
		{
			return;
		}

		CreateHud(owner);
	}

	//------------------------------------------------------------------------------------------------
	override void DisplayInit(IEntity owner)
	{
		if (m_wRoot)
			m_wRoot.RemoveFromHierarchy();
	}

	//------------------------------------------------------------------------------------------------
	override void DisplayStopDraw(IEntity owner)
	{
		DestroyHud();
	}
};