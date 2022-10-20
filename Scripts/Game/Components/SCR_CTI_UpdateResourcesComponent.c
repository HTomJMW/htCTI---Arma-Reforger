[ComponentEditorProps(category: "GameScripted/CTI", description: "CTI Update Resources")]
class SCR_CTI_UpdateResourcesComponentClass : ScriptComponentClass
{
};

class SCR_CTI_UpdateResourcesComponent : ScriptComponent
{
	protected float m_timeDelta;
	protected const float TIMESTEP = 45; // economy cycle
	protected const float BASEINCOME = 10;
	
	//protected FactionManager m_factionManager;
	protected SCR_CTI_GameMode m_gameMode;
	
	void init()
	{
		//m_factionManager = GetGame().GetFactionManager();
		m_gameMode = SCR_CTI_GameMode.Cast(GetOwner());
		m_timeDelta = 0;
	}
	
	protected void update()
	{
		if (!m_gameMode.ClientDataArray) return;
		foreach (SCR_CTI_ClientData clientData : m_gameMode.ClientDataArray)
		{
			if (clientData.isCommander())
			{
				clientData.changeFunds(BASEINCOME * 10); // todo faction size related commander income and need commander account
			} else {
				clientData.changeFunds(BASEINCOME);
			}
		}
	}
			
	override void EOnFixedFrame(IEntity owner, float timeSlice)
	{
		m_timeDelta += timeSlice;
		if (m_timeDelta > TIMESTEP)
			{
				update();
				m_timeDelta = 0;
			}
	}
	
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.FIXEDFRAME);
	}

	void SCR_CTI_UpdateResourcesComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
	}
		
	void ~SCR_CTI_UpdateResourcesComponent()
	{
	}	
};