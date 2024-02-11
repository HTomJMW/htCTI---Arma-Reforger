[EntityEditorProps(category: "GameScripted/CTI", description: "CTI Startup Vehicle Spawner")]
class SCR_CTI_VehicleSpawnClass : SCR_BasePrefabSpawnerClass
{
};

class SCR_CTI_VehicleSpawn : SCR_BasePrefabSpawner
{
	protected bool m_used = false;
	protected Vehicle m_spawnedVehicle;
	protected FactionKey m_factionkey;
	protected SCR_CTI_GameMode m_gameMode;
	
	protected ref array<IEntity> m_items = {};

	//------------------------------------------------------------------------------------------------
	protected override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);

		m_gameMode = SCR_CTI_GameMode.GetInstance();
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

		CarControllerComponent_SA carController = CarControllerComponent_SA.Cast(m_spawnedVehicle.FindComponent(CarControllerComponent_SA));
		if (carController) carController.SetPersistentHandBrake(true);
		
		Physics physicsComponent = m_spawnedVehicle.GetPhysics();
		if (physicsComponent) physicsComponent.SetVelocity("0 -1 0");

		if (m_items) insertItem(m_spawnedVehicle);
		
		removeSupplyBoxes(m_spawnedVehicle);

		ChimeraWorld world = newEnt.GetWorld();
		GarbageManager garbagemanager = world.GetGarbageManager();

		switch (m_rnPrefab)
		{
			case SCR_CTI_Constants.USSR_MHQ:
			{
				RplId rplId = Replication.FindId(newEnt);
				m_gameMode.setMHQrplId("USSR", rplId);

				garbagemanager.Insert(newEnt);
				garbagemanager.Withdraw(newEnt); // UpdateVictory Component handles MHQ lifetime

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

				garbagemanager.Insert(newEnt);
				garbagemanager.Withdraw(newEnt); // UpdateVictory Component handles MHQ lifetime

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
				garbagemanager.Insert(newEnt, SCR_CTI_Constants.VEHICLECOLLECTIONTIME);
				garbagemanager.Withdraw(newEnt); // First GetIn event will start GM countdown timer
				//PrintFormat("CTI :: Default vehicle: %1", m_rnPrefab);
				break;
			}
		}
		//PrintFormat("CTI :: DEBUG :: Vehicle in garbage manager: %1 (%2)", garbagemanager.IsInserted(newEnt).ToString(), m_rnPrefab);
		//PrintFormat("CTI :: DEBUG :: Lifetime: %1 (%2)", garbagemanager.GetLifetime(newEnt), m_rnPrefab);

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

	//------------------------------------------------------------------------------------------------
	protected void removeSupplyBoxes(Vehicle veh)
	{
		IEntity child = veh.GetChildren();
		while (child)
		{
		    SCR_ResourceComponent resourceComponent = SCR_ResourceComponent.Cast(child.FindComponent(SCR_ResourceComponent));
		    if (resourceComponent)
		    {
		        array<SCR_ResourceContainer> containers = resourceComponent.GetContainers();
				if (containers)
				{
					foreach(SCR_ResourceContainer rc : containers)
					{
			            // Force change value event
			            rc.SetResourceValueEx(100);
			            rc.SetResourceValueEx(0);
			        }
				}
		    }
		
		    child = child.GetSibling();
		}
	}
};