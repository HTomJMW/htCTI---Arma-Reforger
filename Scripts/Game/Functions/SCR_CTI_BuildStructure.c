class SCR_CTI_BuildStructure
{
	protected SCR_CTI_GameMode m_gameMode;
	protected SCR_CTI_BaseComponent m_baseComp;

	//------------------------------------------------------------------------------------------------
	void build(FactionKey factionkey, ResourceName resourcename, vector mat[4])
	{
		EntitySpawnParams params = new EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform = mat;
		
		IEntity structure = null;
		Resource resource;
		SCR_CTI_FactoryData factoryData;
		int index = -1;

		switch (factionkey)
		{
			case "USSR":
			{
				index = m_gameMode.FactoriesUSSR.findIndexFromResourcename(resourcename);
				factoryData = m_gameMode.FactoriesUSSR.g_USSR_Factories[index];
				resource = Resource.Load(factoryData.getResName());

				structure = GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params);
				structure.Update();
				//m_baseComp.getBase(factionkey, basecount).addStructure(structure); //TODO get base and add struct 

				break;
			}
			case "US":
			{
				index = m_gameMode.FactoriesUS.findIndexFromResourcename(resourcename);
				factoryData = m_gameMode.FactoriesUS.g_US_Factories[index];
				resource = Resource.Load(factoryData.getResName());

				structure = GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params);
				structure.Update();
				//m_baseComp.getBase(factionkey, basecount).addStructure(structure);

				break;
			}
		}

		if (structure)
		{
			// set faction of building
			FactionAffiliationComponent faffcomp = FactionAffiliationComponent.Cast(structure.FindComponent(FactionAffiliationComponent));
			faffcomp.SetAffiliatedFactionByKey(factionkey);

			// store structure IDs for searching
			RplComponent rplComp = RplComponent.Cast(structure.FindComponent(RplComponent));
			RplId rplid = rplComp.Id();
			m_baseComp.addStuctureRplId(factionkey, rplid);

			// create spawn point
			ResourceName resname = "{987991DCED3DC197}PrefabsEditable/SpawnPoints/E_SpawnPoint.et";
			Resource res = Resource.Load(resname);
			IEntity sp = GetGame().SpawnEntityPrefab(res, GetGame().GetWorld(), params);
			SCR_SpawnPoint spawn = SCR_SpawnPoint.Cast(sp);
			spawn.SetFactionKey(factionkey);

			// Notification
			FactionManager fm = GetGame().GetFactionManager();
			int factionIndex = fm.GetFactionIndex(fm.GetFactionByKey(factionkey));
			m_gameMode.sendFactionNotifF(factionkey, ENotification.CTI_NOTIF_FACTORY_CONSTRUCTED, index, factionIndex, mat[3][0], mat[3][1], mat[3][2]);
		} else {
			PrintFormat("CTI :: Side %1 Structure build Failed!", factionkey);
		}
	}

	//------------------------------------------------------------------------------------------------
	void SCR_CTI_BuildStructure()
	{
		m_gameMode = SCR_CTI_GameMode.Cast(GetGame().GetGameMode());
		m_baseComp = SCR_CTI_BaseComponent.Cast(m_gameMode.FindComponent(SCR_CTI_BaseComponent));
	}

	//------------------------------------------------------------------------------------------------
	void ~SCR_CTI_BuildStructure()
	{
	}
};