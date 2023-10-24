[EntityEditorProps(category: "GameScripted/CTI", description: "CTI Vehicle Spawn")]
class SCR_CTI_VehicleSpawnClass : SCR_BasePrefabSpawnerClass
{
};

class SCR_CTI_VehicleSpawn : SCR_BasePrefabSpawner
{
	protected Vehicle m_spawnedVehicle;
	protected bool m_used = false;
	protected FactionKey m_factionkey;
	protected ref array<IEntity> m_items = {};
	protected SCR_CTI_GameMode m_gameMode;

	//------------------------------------------------------------------------------------------------
	protected override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);

		m_gameMode = SCR_CTI_GameMode.Cast(GetGame().GetGameMode());
	}

	//------------------------------------------------------------------------------------------------
	protected override bool CanSpawn()
	{
		return !m_spawnedVehicle && !m_used;
	}

	//------------------------------------------------------------------------------------------------
	protected override void OnSpawn(IEntity newEnt)
	{
		m_spawnedVehicle = Vehicle.Cast(newEnt);

		if (m_items) insertItem(m_spawnedVehicle);

		ChimeraWorld world = newEnt.GetWorld();
		GarbageSystem garbageSystem = world.GetGarbageSystem();

		switch (m_rnPrefab)
		{
			case SCR_CTI_Constants.USSR_MHQ:
			{
				RplId rplId = Replication.FindId(newEnt);
				m_gameMode.setMHQrplId("USSR", rplId);

				garbageSystem.Insert(newEnt);
				garbageSystem.Withdraw(newEnt); // UpdateVictory Component handles MHQ lifetime

				IEntity child = newEnt.GetChildren();
				while (child)
				{
					SCR_SpawnPoint spawnPoint = SCR_SpawnPoint.Cast(child);
					if (spawnPoint)
					{
						spawnPoint.setDisplayName("USSR MHQ");
						break;
					}
					
					child = child.GetSibling();
				}

				break;
			}
			case SCR_CTI_Constants.US_MHQ:
			{
				RplId rplId = Replication.FindId(newEnt);
				m_gameMode.setMHQrplId("US", rplId);

				garbageSystem.Insert(newEnt);
				garbageSystem.Withdraw(newEnt); // UpdateVictory Component handles MHQ lifetime

				IEntity child = newEnt.GetChildren();
				while (child)
				{
					SCR_SpawnPoint spawnPoint = SCR_SpawnPoint.Cast(child);
					if (spawnPoint)
					{
						spawnPoint.setDisplayName("US MHQ");
						break;
					}
					
					child = child.GetSibling();
				}

				break;
			}
			default:
			{
				garbageSystem.Insert(newEnt, SCR_CTI_Constants.VEHICLECOLLECTIONTIME);
				garbageSystem.Withdraw(newEnt); // First GetIn event will start GM countdown timer
				//PrintFormat("CTI :: Default vehicle: %1", m_rnPrefab);
				break;
			}
		}
		PrintFormat("CTI :: DEBUG :: Vehicle in garbage manager: %1 (%2)", garbageSystem.IsInserted(newEnt).ToString(), m_rnPrefab);
		PrintFormat("CTI :: DEBUG :: Lifetime: %1 (%2)", garbageSystem.GetLifetime(newEnt), m_rnPrefab);

		if (m_items) m_items.Clear();
		m_items = null;

		m_used = true;
		Deactivate(); // Deactivate after vehicle spawned
	}

	//------------------------------------------------------------------------------------------------
	void setPrefab(ResourceName newPrefab)
	{
		m_rnPrefab = newPrefab;
	}

	//------------------------------------------------------------------------------------------------
	void addItemPrefab(ResourceName prefab)
	{
		Resource res = Resource.Load(prefab);
		IEntity item = GetGame().SpawnEntityPrefab(res, GetWorld());
		m_items.Insert(item);
	}

	//------------------------------------------------------------------------------------------------
	void addItemsPrefab(map<ResourceName, int> prefabMap)
	{
		foreach (ResourceName prefab, int piece : prefabMap)
		{
			Resource res = Resource.Load(prefab);
			for (int i = 0; i < piece; i++)
			{
				IEntity item = GetGame().SpawnEntityPrefab(res, GetWorld());
				m_items.Insert(item);
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	protected void insertItem(Vehicle veh)
	{
		InventoryStorageManagerComponent ismc = InventoryStorageManagerComponent.Cast(veh.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
		UniversalInventoryStorageComponent uisc = UniversalInventoryStorageComponent.Cast(veh.FindComponent(UniversalInventoryStorageComponent));

		foreach (IEntity item : m_items)
		{
			//ismc.TryInsertItem(item); // Items may stacked
			ismc.TryInsertItemInStorage(item, uisc);
		}
	}
};